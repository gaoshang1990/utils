#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <semaphore.h>
#endif

#include "fifo_.h"


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


typedef struct _FifoNode_t_ {
    void*  data;
    size_t size;
} FifoNode;


struct _Fifo_t_ {
    FifoMutex   lock;
    uint16_t    size;
    uint16_t    write;
    uint16_t    read;
    FifoNode*   nodes;
    FreeNode_cb free_cb;
};


static FifoMutex _semaphore_create(int initialValue)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, initialValue, 1, NULL);
#else
    sem_t* self = (sem_t*)malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, initialValue);
#endif

    return (FifoMutex)self;
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


Fifo_t fifo_init(uint16_t QSize, FreeNode_cb free_cb)
{
    Fifo_t fifo = (Fifo_t)calloc(1, sizeof(struct _Fifo_t_));
    if (fifo == NULL)
        return NULL;

    fifo->nodes = (FifoNode*)calloc(QSize, sizeof(FifoNode));
    if (fifo->nodes) {
        fifo->size    = QSize;
        fifo->free_cb = free_cb;
        fifo->lock    = _semaphore_create(1);
    }
    else {
        free(fifo);
    }

    return fifo;
}


bool fifo_full(Fifo_t fifo)
{
    if (fifo == NULL)
        return true;

    _semaphore_wait(fifo->lock);

    if (fifo->write < fifo->read) {
        fifo->write += fifo->size;
        fifo->read = fifo->read % fifo->size;
    }

    bool ret = (fifo->size == fifo->write - fifo->read); /* if write - read == size, fifo is full */

    _semaphore_post(fifo->lock);

    return ret;
}


bool fifo_empty(Fifo_t fifo)
{
    if (fifo == NULL)
        return true;

    _semaphore_wait(fifo->lock);

    bool ret = (fifo->write == fifo->read); /* if write == read, fifo is empty */

    _semaphore_post(fifo->lock);

    return ret;
}


int fifo_write(Fifo_t fifo, void* src, int src_len, bool caller_owned)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    if (fifo_full(fifo))
        return -FIFO_FULL; /* caller decide to free data or write again */

    _semaphore_wait(fifo->lock);

    int pos = fifo->write++ % fifo->size;

    if (caller_owned) { /* data is malloc by caller */
        fifo->nodes[pos].data = src;
        fifo->nodes[pos].size = src_len;
    }
    else {
        fifo->nodes[pos].data = malloc(src_len);
        memcpy(fifo->nodes[pos].data, src, src_len);
    }

    _semaphore_post(fifo->lock);

    return FIFO_OK;
}


/**
 * \param   caller2free if true, make dst point to data, and caller should free data after use
 *                      if false, copy data to dst and free data
 */
int fifo_read(Fifo_t fifo, void* dst, int dst_len, bool caller2free)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    if (fifo_empty(fifo))
        return -FIFO_EMPTY;

    _semaphore_wait(fifo->lock);

    int       pos  = fifo->read++ % fifo->size;
    FifoNode* node = &fifo->nodes[pos];

    if (caller2free) {
        dst = fifo->nodes[pos].data; /* caller should free data */
    }
    else {
        if (dst != NULL && node->data != NULL)
            memcpy(dst, node->data, dst_len);

        if (fifo->free_cb)
            fifo->free_cb(fifo->nodes[pos].data);
        else
            _ffree(fifo->nodes[pos].data);
    }

    _semaphore_post(fifo->lock);

    return FIFO_OK;
}


int fifo_clear(Fifo_t fifo)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    while (fifo_empty(fifo) == false)
        fifo_read(fifo, NULL, 0, false);

    return FIFO_OK;
}


int fifo_destroy(Fifo_t fifo)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    fifo_clear(fifo);

    _semaphore_destroy(fifo->lock);

    _ffree(fifo->nodes);
    _ffree(fifo);

    return FIFO_OK;
}
