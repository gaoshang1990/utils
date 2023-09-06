#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _Fifo_t_* Fifo_t;


enum {
    FIFO_OK,
    FIFO_NULL,
    FIFO_FULL,
    FIFO_EMPTY,
};


Fifo_t fifo_init_(int QSize);
bool   fifo_full_(Fifo_t fifo);
bool   fifo_empty_(Fifo_t fifo);
int    fifo_write_(Fifo_t fifo, void* data);
int    fifo_read_(void* dst, int dstLen, Fifo_t fifo);
int    fifo_clear_(Fifo_t fifo);
int    fifo_destroy_(Fifo_t fifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */