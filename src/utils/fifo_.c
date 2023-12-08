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
    void* data;
} FifoNode;


struct _Fifo_t_ {
    FifoMutex lock;
    size_t    size;
    size_t    write;
    size_t    read;

    size_t      data_len;
    FifoNode*   nodes;
    FreeNode_cb free_cb;
    CopyNode_cb copy_cb;
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


Fifo_t fifo_init(size_t fifo_size, size_t data_len, FreeNode_cb free_cb, CopyNode_cb copy_cb)
{
    Fifo_t fifo = (Fifo_t)calloc(1, sizeof(struct _Fifo_t_));
    if (fifo == NULL)
        return NULL;

    fifo->nodes = (FifoNode*)calloc(fifo_size, sizeof(FifoNode));
    if (fifo->nodes) {
        fifo->lock     = _semaphore_create(1);
        fifo->size     = fifo_size;
        fifo->data_len = data_len;
        fifo->free_cb  = free_cb ? free_cb : free;
        fifo->copy_cb  = copy_cb ? copy_cb : memcpy;
    }
    else {
        free(fifo);
        fifo = NULL;
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


int fifo_write(Fifo_t fifo, void* src, bool external_allocated)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    if (fifo_full(fifo))
        return -FIFO_FULL; /* caller decide to free data or write again */

    _semaphore_wait(fifo->lock);

    int pos = fifo->write++ % fifo->size;

    if (external_allocated) { /* data is malloc by caller */
        fifo->nodes[pos].data = src;
    }
    else {
        fifo->nodes[pos].data = malloc(fifo->data_len);
        fifo->copy_cb(fifo->nodes[pos].data, src, fifo->data_len);
    }

    _semaphore_post(fifo->lock);

    return FIFO_OK;
}


int fifo_free_data(Fifo_t fifo, void* data)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    fifo->free_cb(data);

    return FIFO_OK;
}


/**
 * \param   dst if NULL, return data pointer, and caller should free data after use
 *              if not NULL, copy data to dst and free data
 */
void* fifo_read(Fifo_t fifo, void* dst)
{
    if (fifo == NULL)
        return NULL;

    if (fifo_empty(fifo))
        return NULL;

    _semaphore_wait(fifo->lock);

    int       pos  = fifo->read++ % fifo->size;
    FifoNode* node = &fifo->nodes[pos];

    if (dst) {
        fifo->copy_cb(dst, node->data, fifo->data_len);
        fifo->free_cb(node->data);
    }
    else {
        dst = node->data; /* caller should free data after use */
    }

    _semaphore_post(fifo->lock);

    return dst;
}


int fifo_clear(Fifo_t fifo)
{
    if (fifo == NULL)
        return -FIFO_NULL;

    while (!fifo_empty(fifo)) {
        void* data = fifo_read(fifo, NULL);
        fifo->free_cb(data);
    }

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
