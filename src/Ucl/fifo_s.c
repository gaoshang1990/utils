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


/* ���ζ��г�ʼ�� */
Fifo_t fifo_init_(int QSize)
{
    Fifo_t fifo = (Fifo_t)malloc(sizeof(struct _Fifo_t_));
    if (fifo == NULL) {
        printf("fifo_init_: fifo is NULL!\n");
        return NULL;
    }

    memset(fifo, 0, sizeof(struct _Fifo_t_));
    fifo->lock  = s_Semaphore_create(1);
    fifo->size  = QSize;
    fifo->datas = (void**)malloc(QSize * sizeof(void*));
    if (fifo->datas == NULL) {
        printf("fifo_init_: fifo->datas is NULL!\n");
        _semaphore_destroy(fifo->lock);
        _ffree(fifo);
        return NULL;
    }
    memset(fifo->datas, 0, QSize * sizeof(void*));

    return fifo;
}


/* �жϻ��ζ����Ƿ����� */
bool fifo_full_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifo_full_: fifo is NULL!\n");
        return true;
    }

    _semaphore_wait(fifo->lock);

    if (fifo->write < fifo->read) { /* ���������ش��� */
        fifo->write += fifo->size;
        fifo->read = fifo->read % fifo->size;
    }

    /* дλ�ü�ȥ��λ�õ��ڶ��г��ȣ����ζ������� */
    bool ret = fifo->size == fifo->write - fifo->read;

    _semaphore_post(fifo->lock);
    return ret;
}


/* �жϻ��ζ���Ϊ�� */
bool fifo_empty_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifo_empty_: fifo is NULL!\n");
        return true;
    }

    _semaphore_wait(fifo->lock);

    /* дλ�úͶ���λ����ȣ����ζ���Ϊ�� */
    bool ret = fifo->write == fifo->read;

    _semaphore_post(fifo->lock);
    return ret;
}


/* �������� */
int fifo_write_(Fifo_t fifo, void* data)
{
    if (fifo == NULL) {
        printf("fifo_write_: fifo is NULL\n");
        return -FIFO_NULL;
    }
    if (fifo_full_(fifo)) {
        // _ffree(data); /* ֱ���ͷŴ����ӵ����� */
        /* �����߸��ݷ���ֵ�ж��Ƿ��ͷŻ�������� */
        printf("fifo_write_: fifo is full\n");
        return -FIFO_FULL;
    }
    _semaphore_wait(fifo->lock);

    fifo->datas[fifo->write % fifo->size] = data; /* free data in fifo_read_ */
    fifo->write++;

    _semaphore_post(fifo->lock);
    return FIFO_OK;
}


/* ��ȡ���� */
int fifo_read_(void* dst, int dstLen, Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifo_read_: fifo is NULL!\n");
        return -FIFO_NULL;
    }
    if (fifo_empty_(fifo)) {
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


int fifo_clear_(Fifo_t fifo)
{
    if (fifo == NULL) {
        printf("fifo is NULL!\n");
        return -FIFO_NULL;
    }

    while (fifo_empty_(fifo) == false) {
        fifo_read_(NULL, 0, fifo);
    }

    return FIFO_OK;
}


int fifo_destroy_(Fifo_t fifo)
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
