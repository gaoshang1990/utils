
#include "mlog_.h"
#include "fifo_.h"
#include "debug_.h"


typedef struct _FifoTest_t_ {
    int* arr;
    int  len;
} FifoTest;


void FifoTest_free(void* data)
{
    FifoTest* test = (FifoTest*)data;
    if (test->arr)
        free(test->arr);

    free(data);
}


void* FifoTest_copy(void* dst, const void* src, size_t len)
{
    memcpy(dst, src, len);

    FifoTest* dst_ = (FifoTest*)dst;
    FifoTest* src_ = (FifoTest*)src;

    dst_->len = src_->len;
    dst_->arr = (int*)malloc(sizeof(int) * dst_->len);
    memcpy(dst_->arr, src_->arr, sizeof(int) * dst_->len);

    return NULL;
}


int fifo_test()
{
    SLOG_INFO("-- fifo test start --\n");

    Fifo_t fifo = fifo_new(10,
                           sizeof(FifoTest),
                           FifoTest_free, // FifoTest_copy can be NULL, equal to free()
                           FifoTest_copy, // FifoTest_free can be NULL, equal to memcpy()
                           false);
    ASSERT_(fifo, "fifo_new failed");

    FifoTest test1;
    test1.len    = 2;
    test1.arr    = (int*)malloc(sizeof(int) * test1.len);
    test1.arr[0] = 1;
    test1.arr[1] = 2;
    fifo_write(fifo, &test1, false);
    free(test1.arr); // fifo will copy test1, we should free arr here

    FifoTest* test2 = (FifoTest*)malloc(sizeof(FifoTest));
    test2->len      = 2;
    test2->arr      = (int*)malloc(sizeof(int) * test2->len);
    test2->arr[0]   = 3;
    test2->arr[1]   = 4;
    fifo_write(fifo, test2, true); // do not free test2, fifo will free it

    FifoTest test3;
    fifo_read(fifo, &test3);
    SLOG_DEBUG("test3.len = %d, test3.arr[0] = %d, test3.arr[1] = %d\n", test3.len, test3.arr[0], test3.arr[1]);
    free(test3.arr); // fifo will copy test3, we should free arr here

    FifoTest* test4 = (FifoTest*)fifo_read(fifo, NULL);
    SLOG_DEBUG("test4.len = %d, test4.arr[0] = %d, test4.arr[1] = %d\n", test4->len, test4->arr[0], test4->arr[1]);
    FifoTest_free(test4); // we should free it here

    fifo_del(fifo);

    SLOG_INFO("-- fifo test done --\n");

    return 0;
}