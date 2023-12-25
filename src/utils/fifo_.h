#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define FIFO_SIZE_MAX (256)
#define FIFO_SIZE_MIN (4)

typedef void (*FreeNode_cb)(void* data);
typedef void* (*CopyNode_cb)(void* dst, const void* src, size_t len);
typedef struct _Fifo_t_* Fifo_t;


enum {
    FIFO_OK,
    FIFO_NULL,
    FIFO_FULL,
    FIFO_EMPTY,
};


Fifo_t fifo_new(size_t node_size, FreeNode_cb free_cb, CopyNode_cb copy_cb, bool thread_safe);
bool   fifo_full(Fifo_t fifo);
bool   fifo_empty(Fifo_t fifo);
int    fifo_write(Fifo_t fifo, void* src, bool dynamic);
void*  fifo_read(Fifo_t fifo, void* dst);
int    fifo_clear(Fifo_t fifo);
int    fifo_del(Fifo_t fifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */