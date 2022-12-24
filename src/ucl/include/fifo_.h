#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdint.h>
// #include <pthread.h>

#include "hal_thread.h"
#include "public_.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    Semaphore lock;
    uint16_t  size;
    uint16_t  write;
    uint16_t  read;
    void**    pData;
} Fifo;


extern Fifo*   fifoInit_(uint16_t QSize);
extern uint8_t fifoIsFull_(Fifo* pFifo);
extern uint8_t fifoIsEmpty_(Fifo* pFifo);
extern int     fifoWrite_(Fifo* pFifo, void* pData);
extern int     fifoRead_(void* pDst, int dstLen, Fifo* pFifo);
extern int     fifoClear_(Fifo* pFifo);
extern int     fifoDestroy_(Fifo* pFifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */