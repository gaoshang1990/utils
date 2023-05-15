#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#  include <pthread.h>
#  include <semaphore.h>
#endif

#include "fifo_s.h"


typedef void* FifoMutex;

struct _Fifo_t_ {
    FifoMutex lock;
    int       size;
    uint16_t  write;
    uint16_t  read;
    void**    data;
};


#ifndef ffree
#  define ffree(p)            \
      do {                    \
          if (p) {            \
              free((void*)p); \
              p = NULL;       \
          }                   \
      } while (0)
#endif


static FifoMutex s_Semaphore_create(int initialValue)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, initialValue, 1, NULL);
#else
    FifoMutex self = malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, initialValue);
#endif

    return self;
}

static void s_Semaphore_wait(FifoMutex self)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)self, INFINITE);
#else
    sem_wait((sem_t*)self);
#endif
}


static void s_Semaphore_post(FifoMutex self)
{
#ifdef _WIN32
    ReleaseSemaphore((HANDLE)self, 1, NULL);
#else
    sem_post((sem_t*)self);
#endif
}


static void s_Semaphore_destroy(FifoMutex self)
{
#ifdef _WIN32
    CloseHandle((HANDLE)self);
#else
    sem_destroy((sem_t*)self);
    free(self);
#endif
}


/* 环形队列初始化 */
Fifo_t* fifo_init_(int QSize)
{
    Fifo_t* fifo = (Fifo_t*)malloc(sizeof(Fifo_t));
    if (fifo == NULL) {
        printf("fifo_init_: fifo is NULL!\n");
        return NULL;
    }

    memset(fifo, 0, sizeof(Fifo_t));
    fifo->lock = s_Semaphore_create(1);
    fifo->size = QSize;
    fifo->data = (void**)malloc(QSize * sizeof(void*));
    if (fifo->data == NULL) {
        printf("fifo_init_: fifo->data is NULL!\n");
        s_Semaphore_destroy(fifo->lock);
        ffree(fifo);
        return NULL;
    }
    memset(fifo->data, 0, QSize * sizeof(void*));

    return fifo;
}


/* 判断环形队列是否已满 */
bool fifo_full_(Fifo_t* fifo)
{
    if (fifo == NULL) {
        printf("fifo_full_: fifo is NULL!\n");
        return true;
    }

    s_Semaphore_wait(fifo->lock);

    if (fifo->write < fifo->read) { /* 计数器环回处理 */
        fifo->write += fifo->size;
        fifo->read = fifo->read % fifo->size;
    }

    /* 写位置减去读位置等于队列长度，则环形队列已满 */
    bool ret = fifo->size == fifo->write - fifo->read;

    s_Semaphore_post(fifo->lock);
    return ret;
}


/* 判断环形队列为空 */
bool fifo_empty_(Fifo_t* fifo)
{
    if (fifo == NULL) {
        printf("fifo_empty_: fifo is NULL!\n");
        return true;
    }

    s_Semaphore_wait(fifo->lock);

    /* 写位置和读的位置相等，则环形队列为空 */
    bool ret = fifo->write == fifo->read;

    s_Semaphore_post(fifo->lock);
    return ret;
}


/* 插入数据 */
int fifo_write_(Fifo_t* fifo, void* data)
{
    if (fifo == NULL) {
        printf("fifo_write_: fifo is NULL!\n");
        return -1;
    }
    if (fifo_full_(fifo)) {
        // ffree(data); /* 直接释放待增加的数据 */
        /* 调用者根据返回值判断是否释放或重新添加 */
        return -2;
    }
    s_Semaphore_wait(fifo->lock);

    // ffree(fifo->data[fifo->write & (fifo->size - 1)]); /* 已在read里释放 */
    fifo->data[fifo->write % fifo->size] = data;
    fifo->write++;

    s_Semaphore_post(fifo->lock);
    return 0;
}


/* 读取数据 */
int fifo_read_(void* dst, int dstLen, Fifo_t* fifo)
{
    if (fifo == NULL) {
        printf("fifo_read_: fifo is NULL!\n");
        return -1;
    }
    if (fifo_empty_(fifo)) {
        return -1;
    }

    s_Semaphore_wait(fifo->lock);

    void* data = fifo->data[fifo->read % fifo->size];
    if (dst != NULL && data != NULL) {
        memcpy(dst, data, dstLen);
    }
    ffree(data);
    fifo->read++;

    s_Semaphore_post(fifo->lock);

    return 0;
}


/* 清空 */
int fifo_clear_(Fifo_t* fifo)
{
    if (fifo == NULL) {
        printf("fifo is NULL!\n");
        return -1;
    }

    while (fifo_empty_(fifo) == false) {
        fifo_read_(NULL, 0, fifo);
    }

    return 0;
}


/* 销毁 */
int fifo_destroy_(Fifo_t* fifo)
{
    if (fifo == NULL) {
        printf("fifo is NULL!\n");
        return -1;
    }
    s_Semaphore_destroy(fifo->lock);

    uint16_t i;
    for (i = 0; i < fifo->size; i++) {
        ffree(fifo->data[i]);
    }
    ffree(fifo->data);
    ffree(fifo);

    return 0;
}
