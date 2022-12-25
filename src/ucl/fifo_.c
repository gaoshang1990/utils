#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fifo_.h"


/* ���ζ��г�ʼ�� */
Fifo* fifoInit_(int QSize)
{
    Fifo* pFifo = (Fifo*)malloc(sizeof(Fifo));
    if (pFifo == NULL) {
        printf("fifoInit_: pFifo is NULL!\n");
        return NULL;
    }
    memset(pFifo, 0, sizeof(Fifo));
    pFifo->lock  = Semaphore_create(1);
    pFifo->size  = QSize;
    pFifo->data = (void**)malloc(QSize * sizeof(void*));
    if (pFifo->data == NULL) {
        printf("fifoInit_: pFifo->data is NULL!\n");
        Semaphore_destroy(pFifo->lock);
        ffree(pFifo);
        return NULL;
    }
    memset(pFifo->data, 0, QSize * sizeof(void*));

    return pFifo;
}


/* �жϻ��ζ����Ƿ����� */
bool fifoFull_(Fifo* pFifo)
{
    if (pFifo == NULL) {
        printf("fifoFull_: pFifo is NULL!\n");
        return true;
    }
    if (pFifo->write < pFifo->read) { /* ���������ش��� */
        pFifo->write += pFifo->size;
        pFifo->read = pFifo->read % pFifo->size;
    }

    /* дλ�ü�ȥ��λ�õ��ڶ��г��ȣ����ζ������� */
    if (pFifo->size == pFifo->write - pFifo->read) {
        return true;
    }

    return false;
}


/* �жϻ��ζ���Ϊ�� */
bool fifoEmpty_(Fifo* pFifo)
{
    /* дλ�úͶ���λ����ȣ����ζ���Ϊ�� */
    if (pFifo->write == pFifo->read) {
        return TRUE;
    }
    return FALSE;
}


/* �������� */
int fifoWrite_(Fifo* pFifo, void* data)
{
    if (pFifo == NULL) {
        printf("fifoWrite_: pFifo is NULL!\n");
        return -1;
    }
    if (fifoFull_(pFifo)) {
        // ffree(data); /* ֱ���ͷŴ����ӵ����� */
        /* �����߸��ݷ���ֵ�ж��Ƿ��ͷŻ�������� */
        return -2;
    }
    Semaphore_wait(pFifo->lock);

    // ffree(pFifo->data[pFifo->write & (pFifo->size - 1)]); /* ����read���ͷ� */
    pFifo->data[pFifo->write % pFifo->size] = data;
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
    if (fifoEmpty_(pFifo)) {
        return -1;
    }

    Semaphore_wait(pFifo->lock);

    void* data = pFifo->data[pFifo->read % pFifo->size];
    if (pDst != NULL && data != NULL) {
        memcpy(pDst, data, dstLen);
    }
    ffree(data);
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
    while (fifoEmpty_(pFifo) == FALSE) {
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
        ffree(pFifo->data[i]);
    }
    ffree(pFifo->data);
    ffree(pFifo);

    return 0;
}
