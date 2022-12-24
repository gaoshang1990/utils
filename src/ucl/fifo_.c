#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fifo_.h"
#include "public_.h"


/* 环形队列初始化 */
Fifo* fifoInit_(uint16_t QSize)
{
    Fifo* pFifo = (Fifo*)malloc(sizeof(Fifo));
    if (pFifo == NULL) {
        printf("fifoInit_: pFifo is NULL!\n");
        return NULL;
    }
    memset(pFifo, 0, sizeof(Fifo));
    pFifo->lock  = Semaphore_create(1);
    pFifo->size  = QSize;
    pFifo->pData = (void**)malloc(QSize * sizeof(void*));
    if (pFifo->pData == NULL) {
        printf("fifoInit_: pFifo->pData is NULL!\n");
        Semaphore_destroy(pFifo->lock);
        ffree(pFifo);
        return NULL;
    }
    memset(pFifo->pData, 0, QSize * sizeof(void*));

    return pFifo;
}


/* 判断环形队列是否已满 */
uint8_t fifoIsFull_(Fifo* pFifo)
{
    if (pFifo == NULL) {
        printf("fifoIsFull_: pFifo is NULL!\n");
        return true;
    }
    if (pFifo->write < pFifo->read) { /* 计数器环回处理 */
        pFifo->write += pFifo->size;
        pFifo->read = pFifo->read % pFifo->size;
    }

    /* 写位置减去读位置等于队列长度，则环形队列已满 */
    if (pFifo->size == pFifo->write - pFifo->read) {
        return TRUE;
    }

    return FALSE;
}


/* 判断环形队列为空 */
uint8_t fifoIsEmpty_(Fifo* pFifo)
{
    /* 写位置和读的位置相等，则环形队列为空 */
    if (pFifo->write == pFifo->read) {
        return TRUE;
    }
    return FALSE;
}


/* 插入数据 */
int fifoWrite_(Fifo* pFifo, void* pData)
{
    if (pFifo == NULL) {
        printf("fifoWrite_: pFifo is NULL!\n");
        return -1;
    }
    if (fifoIsFull_(pFifo)) {
        // ffree(pData); /* 直接释放待增加的数据 */
        /* 调用者根据返回值判断是否释放或重新添加 */
        return -2;
    }
    Semaphore_wait(pFifo->lock);

    // ffree(pFifo->pData[pFifo->write & (pFifo->size - 1)]); /* 已在read里释放 */
    pFifo->pData[pFifo->write % pFifo->size] = pData;
    pFifo->write++;

    Semaphore_post(pFifo->lock);
    return 0;
}


/* 读取数据 */
int fifoRead_(void* pDst, int dstLen, Fifo* pFifo)
{
    if (pFifo == NULL) {
        // printf("fifoRead_: pFifo is NULL!\n");
        return -1;
    }
    if (fifoIsEmpty_(pFifo)) {
        return -1;
    }

    Semaphore_wait(pFifo->lock);

    void* pData = pFifo->pData[pFifo->read % pFifo->size];
    if (pDst != NULL && pData != NULL) {
        memcpy(pDst, pData, dstLen);
    }
    ffree(pData);
    pFifo->read++;

    Semaphore_post(pFifo->lock);

    return 0;
}


/* 清空 */
int fifoClear_(Fifo* pFifo)
{
    if (pFifo == NULL) {
        printf("pFifo is NULL!\n");
        return -1;
    }
    while (fifoIsEmpty_(pFifo) == FALSE) {
        fifoRead_(NULL, 0, pFifo);
    }

    return 0;
}


/* 销毁 */
int fifoDestroy_(Fifo* pFifo)
{
    if (pFifo == NULL) {
        printf("pFifo is NULL!\n");
        return -1;
    }
    Semaphore_destroy(pFifo->lock);

    uint16_t i;
    for (i = 0; i < pFifo->size; i++) {
        ffree(pFifo->pData[i]);
    }
    ffree(pFifo->pData);
    ffree(pFifo);

    return 0;
}
