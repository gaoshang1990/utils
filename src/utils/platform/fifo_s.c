#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <semaphore.h>
#endif

#include "fifo_s.h"

/**
 * TODO..
 * 增加队列节点数据的释放函数
 */


#ifndef _ffree
#  define _ffree(p)           \
      do {                    \
          if (p) {            \
              free((void*)p); \
              p = NULL;       \
          }                   \
      } while (0)
#endif


typedef void* FifoMutex;


struct _Fifo_t_ {
    FifoMutex lock;
    int       size;
    uint16_t  write;
    uint16_t  read;
    void**    datas;
};


static FifoMutex _semaphore_create(int initialValue)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, initialValue, 1, NULL);
#else
    FifoMutex self = malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, initialValue);
#endif

    return self;
}


static void _semaphore_wait(FifoMutex self)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)self, INFINITE);
#else
    sem_wait((sem_t*)self);
#endif
}


static void _semaphore_post(FifoMutex self)
{
#ifdef _WIN32
    ReleaseSemaphore((HANDLE)self, 1, NULL);
#else
    sem_post((sem_t*)self);
#endif
}


static void _semaphore_destroy(FifoMutex self)
{
#ifdef _WIN32
    CloseHandle((HANDLE)self);
#else
    sem_destroy((sem_t*)self);
    free(self);
#endif
}


/* 环形队列初始化 */
Fifo_t fifoInit_(int QSize)
{
    Fifo_t fifo = (Fifo_t)malloc(sizeof(struct _Fifo_t_));
    if (fifo == NULL) {
        printf("fifoInit_: fifo is NULL!\n");
        return NULL;
    }

    memset(fifo, 0, sizeof(struct _Fifo_t_));
    fifo->lock  = _semaphore_create(1);
    fifo->size  = QSize;
    fifo->datas = (void**)malloc(QSize * sizeof(void*));
    if (fifo->datas == NULL) {
        printf("fifoInit_: fifo->datas is NULL!\n");
        _semaphore_destroy(fifo->lock);
        _ffree(fifo);
        return NULL;
    }
    memset(fifo->datas, 0, QSize * sizeof(void*));

    return fifo;
}


/* 判断环形队列是否已满 */
bool fifoFull_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifoFull_: fifo is NULL!\n");
        return true;
    }

    _semaphore_wait(fifo->lock);

    if (fifo->write < fifo->read) { /* 计数器环回处理 */
        fifo->write += fifo->size;
        fifo->read = fifo->read % fifo->size;
    }

    /* 写位置减去读位置等于队列长度，则环形队列已满 */
    bool ret = fifo->size == fifo->write - fifo->read;

    _semaphore_post(fifo->lock);
    return ret;
}


/* 判断环形队列为空 */
bool fifoEmpty_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifoEmpty_: fifo is NULL!\n");
        return true;
    }

    _semaphore_wait(fifo->lock);

    /* 写位置和读的位置相等，则环形队列为空 */
    bool ret = fifo->write == fifo->read;

    _semaphore_post(fifo->lock);
    return ret;
}


/* 插入数据 */
int fifoWrite_(Fifo_t fifo, void* data)
{
    if (fifo == NULL) {
        printf("fifoWrite_: fifo is NULL\n");
        return -FIFO_NULL;
    }
    if (fifoFull_(fifo)) {
        // _ffree(data); /* 直接释放待增加的数据 */
        /* 调用者根据返回值判断是否释放或重新添加 */
        printf("fifoWrite_: fifo is full\n");
        return -FIFO_FULL;
    }
    _semaphore_wait(fifo->lock);

    fifo->datas[fifo->write % fifo->size] = data; /* free data in fifoRead_ */
    fifo->write++;

    _semaphore_post(fifo->lock);
    return FIFO_OK;
}


/* 读取数据 */
int fifoRead_(void* dst, int dstLen, Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifoRead_: fifo is NULL!\n");
        return -FIFO_NULL;
    }
    if (fifoEmpty_(fifo)) {
        return -FIFO_EMPTY;
    }

    _semaphore_wait(fifo->lock);

    void* data = fifo->datas[fifo->read % fifo->size];
    if (dst != NULL && data != NULL) {
        memcpy(dst, data, dstLen);
    }
    _ffree(data);
    fifo->read++;

    _semaphore_post(fifo->lock);

    return FIFO_OK;
}


int fifoClear_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifo is NULL!\n");
        return -FIFO_NULL;
    }

    while (fifoEmpty_(fifo) == false) {
        fifoRead_(NULL, 0, fifo);
    }

    return FIFO_OK;
}


int fifoDestroy_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifo is NULL!\n");
        return -FIFO_NULL;
    }

    _semaphore_destroy(fifo->lock);

    for (int i = 0; i < fifo->size; i++) {
        _ffree(fifo->datas[i]);
    }
    _ffree(fifo->datas);
    _ffree(fifo);

    return FIFO_OK;
}
