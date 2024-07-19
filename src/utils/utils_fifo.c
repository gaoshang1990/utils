#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <semaphore.h>
#endif

#include "utils_fifo.h"


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
    bool  auto_free;
} FifoNode;


struct _Fifo_t_ {
    FifoMutex lock;
    size_t    fifo_size;
    size_t    head;
    size_t    tail;

    size_t      node_size;
    FifoNode*   nodes;
    FreeNode_cb free_cb;
    CopyNode_cb copy_cb;
};


static FifoMutex _semaphore_new(int init_value)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, init_value, 1, NULL);
#else
    sem_t* self = (sem_t*)malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, init_value);
#endif

    return (FifoMutex)self;
}


static void _semaphore_wait(FifoMutex self)
{
    if (self == NULL)
        return;

#ifdef _WIN32
    WaitForSingleObject((HANDLE)self, INFINITE);
#else
    sem_wait((sem_t*)self);
#endif
}


static void _semaphore_post(FifoMutex self)
{
    if (self == NULL)
        return;

#ifdef _WIN32
    ReleaseSemaphore((HANDLE)self, 1, NULL);
#else
    sem_post((sem_t*)self);
#endif
}


static void _semaphore_del(FifoMutex self)
{
    if (self == NULL)
        return;

#ifdef _WIN32
    CloseHandle((HANDLE)self);
#else
    sem_destroy((sem_t*)self);
    free(self);
#endif
}


void fifo_lock(Fifo_t fifo)
{
    _semaphore_wait(fifo->lock);
}


void fifo_unlock(Fifo_t fifo)
{
    _semaphore_post(fifo->lock);
}


Fifo_t
fifo_new(size_t node_size, FreeNode_cb free_cb, CopyNode_cb copy_cb, bool thread_safe)
{
    Fifo_t fifo = (Fifo_t)calloc(1, sizeof(struct _Fifo_t_));
    if (fifo == NULL)
        return NULL;

    fifo->nodes = (FifoNode*)calloc(FIFO_SIZE_MIN, sizeof(FifoNode));
    if (fifo->nodes) {
        fifo->lock      = thread_safe ? _semaphore_new(1) : NULL;
        fifo->fifo_size = FIFO_SIZE_MIN;
        fifo->node_size = node_size;
        fifo->free_cb   = free_cb ? free_cb : free;
        fifo->copy_cb   = copy_cb ? copy_cb : memcpy;
    }
    else
        _ffree(fifo);

    return fifo;
}


bool fifo_full(Fifo_t fifo)
{
    bool ret;

    _semaphore_wait(fifo->lock);
    {
        if (fifo->head < fifo->tail) {
            fifo->head += fifo->fifo_size;
            fifo->tail = fifo->tail % fifo->fifo_size;
        }

        ret = (fifo->fifo_size ==
               fifo->head - fifo->tail); /* if head - tail == size, fifo is full */
    }
    _semaphore_post(fifo->lock);

    return ret;
}


bool fifo_empty(Fifo_t fifo)
{
    _semaphore_wait(fifo->lock);
    bool ret = (fifo->head == fifo->tail); /* if head == tail, fifo is empty */
    _semaphore_post(fifo->lock);

    return ret;
}


static int _fifo_grow(Fifo_t fifo)
{
    if (fifo->fifo_size * 2 > FIFO_SIZE_MAX)
        return -FIFO_FULL;

    FifoNode* new_nodes = (FifoNode*)calloc(fifo->fifo_size * 2, sizeof(FifoNode));
    if (new_nodes == NULL)
        return -FIFO_NULL;

    for (size_t i = 0; i < fifo->fifo_size; i++)
        new_nodes[i].data = fifo->nodes[(fifo->tail + i) % fifo->fifo_size].data;

    _ffree(fifo->nodes);

    fifo->nodes     = new_nodes;
    fifo->fifo_size = fifo->fifo_size * 2;
    fifo->head      = fifo->fifo_size / 2;
    fifo->tail      = 0;

    return FIFO_OK;
}

/**
 * @brief   写入数据到 fifo
 * @param   src         源数据指针, 将被直接保存到fifo;
 *                      若src为静态分配的内存, auto_free应为false,
 *                      且在该变量的生命周期结束前保证fifo中的数据已被使用完毕
 * @param   auto_free   是否在fifo_read()中自动释放src
 * @return  FIFO_OK: 写入成功, 其他失败
 */
int fifo_write(Fifo_t fifo, void* src, bool auto_free)
{
    if (fifo_full(fifo) && _fifo_grow(fifo) < 0)
        return -FIFO_FULL; /* caller decide to free data or head again */

    _semaphore_wait(fifo->lock);
    {
        int pos = fifo->head++ % fifo->fifo_size;

        fifo->nodes[pos].data      = src;
        fifo->nodes[pos].auto_free = auto_free;
    }
    _semaphore_post(fifo->lock);

    return FIFO_OK;
}


/**
 * @brief   读取数据
 * @param   dst     将数据拷贝到dst, 根据auto_free标识决定是否释放原数据
 * @return  FIFO_OK: 读取成功, 其他失败
 */
int fifo_read(Fifo_t fifo, void* dst)
{
    if (fifo_empty(fifo))
        return -FIFO_EMPTY;

    _semaphore_wait(fifo->lock);
    {
        int       pos  = fifo->tail++ % fifo->fifo_size;
        FifoNode* node = &fifo->nodes[pos];

        if (dst) {
            fifo->copy_cb(dst, node->data, fifo->node_size);
            if (node->auto_free)
                fifo->free_cb(node->data);
        }
    }
    _semaphore_post(fifo->lock);

    return FIFO_OK;
}


int fifo_clear(Fifo_t fifo)
{
    while (!fifo_empty(fifo))
        fifo_read(fifo, NULL);

    return FIFO_OK;
}


int fifo_del(Fifo_t fifo)
{
    fifo_clear(fifo);

    _semaphore_del(fifo->lock);

    _ffree(fifo->nodes);
    _ffree(fifo);

    return FIFO_OK;
}
