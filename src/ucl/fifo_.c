#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fifo_.h"
#include "public_.h"


/* ���ζ��г�ʼ�� */
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


/* �жϻ��ζ����Ƿ����� */
uint8_t fifoIsFull_(Fifo* pFifo)
{
    if (pFifo == NULL) {
        printf("fifoIsFull_: pFifo is NULL!\n");
        return true;
    }
    if (pFifo->write < pFifo->read) { /* ���������ش��� */
        pFifo->write += pFifo->size;
        pFifo->read = pFifo->read % pFifo->size;
    }

    /* дλ�ü�ȥ��λ�õ��ڶ��г��ȣ����ζ������� */
    if (pFifo->size == pFifo->write - pFifo->read) {
        return TRUE;
    }

    return FALSE;
}


/* �жϻ��ζ���Ϊ�� */
uint8_t fifoIsEmpty_(Fifo* pFifo)
{
    /* дλ�úͶ���λ����ȣ����ζ���Ϊ�� */
    if (pFifo->write == pFifo->read) {
        return TRUE;
    }
    return FALSE;
}


/* �������� */
int fifoWrite_(Fifo* pFifo, void* pData)
{
    if (pFifo == NULL) {
        printf("fifoWrite_: pFifo is NULL!\n");
        return -1;
    }
    if (fifoIsFull_(pFifo)) {
        // ffree(pData); /* ֱ���ͷŴ����ӵ����� */
        /* �����߸��ݷ���ֵ�ж��Ƿ��ͷŻ�������� */
        return -2;
    }
    Semaphore_wait(pFifo->lock);

    // ffree(pFifo->pData[pFifo->write & (pFifo->size - 1)]); /* ����read���ͷ� */
    pFifo->pData[pFifo->write % pFifo->size] = pData;
    pFifo->write++;

    Semaphore_post(pFifo->lock);
    return 0;
}


/* ��ȡ���� */
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


/* ��� */
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


/* ���� */
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
