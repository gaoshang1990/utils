#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*FreeNode_cb)(void* data);
typedef struct _Fifo_t_* Fifo_t;


enum {
    FIFO_OK,
    FIFO_NULL,
    FIFO_FULL,
    FIFO_EMPTY,
};


Fifo_t fifo_init(uint16_t QSize, FreeNode_cb free_cb);
bool   fifo_full(Fifo_t fifo);
bool   fifo_empty(Fifo_t fifo);
int    fifo_write(Fifo_t fifo, void* src, int src_len, bool caller_owned);
int    fifo_read(Fifo_t fifo, void* dst, int dst_len, bool caller2free);
int    fifo_clear(Fifo_t fifo);
int    fifo_destroy(Fifo_t fifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */