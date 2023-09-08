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


Fifo_t fifoInit_(int QSize);
bool   fifoFull_(Fifo_t fifo);
bool   fifoEmpty_(Fifo_t fifo);
int    fifoWrite_(Fifo_t fifo, void* data);
int    fifoRead_(void* dst, int dstLen, Fifo_t fifo);
int    fifoClear_(Fifo_t fifo);
int    fifoDestroy_(Fifo_t fifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */