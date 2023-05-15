#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _Fifo_t_ Fifo_t;


extern Fifo_t* fifo_init_(int QSize);
extern bool    fifo_full_(Fifo_t* pFifo);
extern bool    fifo_empty_(Fifo_t* pFifo);
extern int     fifo_write_(Fifo_t* pFifo, void* data);
extern int     fifo_read_(void* pDst, int dstLen, Fifo_t* pFifo);
extern int     fifo_clear_(Fifo_t* pFifo);
extern int     fifo_destroy_(Fifo_t* pFifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */