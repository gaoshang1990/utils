#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdint.h>

#include "hal_thread.h"
#include "public_.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    Semaphore lock;
    int       size;
    uint16_t  write;
    uint16_t  read;
    void**    data;
} Fifo;


extern Fifo* fifoInit_(int QSize);
extern bool  fifoFull_(Fifo* pFifo);
extern bool  fifoEmpty_(Fifo* pFifo);
extern int   fifoWrite_(Fifo* pFifo, void* data);
extern int   fifoRead_(void* pDst, int dstLen, Fifo* pFifo);
extern int   fifoClear_(Fifo* pFifo);
extern int   fifoDestroy_(Fifo* pFifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */