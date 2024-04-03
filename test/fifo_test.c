#include <string.h>
#include <stdlib.h>

#include "mlog_.h"
#include "fifo_.h"
#include "debug_.h"


int fifo_simple_test()
{
    SLOG_INFO("-- fifo simple test start --\n");

    char buf[256] = {0};

    Fifo_t fifo = fifo_new(sizeof(buf), NULL, NULL, false);
    ASSERT_(fifo, "fifo_new failed");

    // 写队列1: 静态分配
    for (int i = 0; i < 26; i++) {
        buf[0] = 'a' + i;
        fifo_write(fifo, buf, false);
    }

    for (int i = 0; !fifo_empty(fifo); i++) {
        fifo_read(fifo, buf);
        SLOG_DEBUG("buf = %s", buf);
        ASSERT_(buf[0] == 'a' + i, "buf[0] = %c", buf[0]);
    }

    // 写队列2: 动态分配
    for (int i = 0; i < 26; i++) {
        char* test = (char*)malloc(sizeof(buf));
        memset(test, 0, sizeof(buf));
        test[0] = 'A' + i;
        fifo_write(fifo, test, true);
    }

    for (int i = 0; !fifo_empty(fifo); i++) {
        char* test = (char*)fifo_read(fifo, NULL);
        SLOG_DEBUG("test = %s", test);
        ASSERT_(test[0] == 'A' + i, "test[0] = %c", test[0]);
        free(test); // 调用者传入NULL, fifo直接返回指针，调用者需要释放test
    }

    fifo_del(fifo);

    SLOG_INFO("-- fifo simple test done --\n");

    return 0;
}


typedef struct _FifoTest_t_ {
    char* str;
    int   len;
} FifoTest;


void FifoTest_free(void* data)
{
    FifoTest* test = (FifoTest*)data;
    if (test->str)
        free(test->str);

    free(data);
}


void* FifoTest_copy(void* dst, const void* src, size_t len)
{
    memcpy(dst, src, len);

    FifoTest* dst_ = (FifoTest*)dst;
    FifoTest* src_ = (FifoTest*)src;

    dst_->len = src_->len;
    dst_->str = (char*)malloc(sizeof(char) * dst_->len);
    memcpy(dst_->str, src_->str, sizeof(char) * dst_->len);

    return NULL;
}


/* 复杂节点测试, 如节点含有动态分配内容等 */
int fifo_complex_test()
{
    SLOG_INFO("-- fifo complex test start --\n");

    Fifo_t fifo = fifo_new(sizeof(FifoTest),
                           FifoTest_free, // FifoTest_copy can be NULL, equal to free()
                           FifoTest_copy, // FifoTest_free can be NULL, equal to memcpy()
                           false);
    ASSERT_(fifo, "fifo_new failed");

    /* 写队列1 */
    FifoTest test1;
    test1.len = 2;
    test1.str = (char*)malloc(sizeof(char) * test1.len);
    memset(test1.str, 0, sizeof(char) * test1.len);
    for (int i = 0; i < 26; i++) {
        test1.str[0] = 'a' + i;
        fifo_write(fifo, &test1, false);
    }
    free(test1.str); // fifo will copy test1, we should free arr here

    for (int i = 0; !fifo_empty(fifo); i++) {
        FifoTest test2;
        fifo_read(fifo, &test2);
        SLOG_DEBUG("test2 = %s", test2.str);
        ASSERT_(test2.str[0] == 'a' + i, "test2.str[0] = %c", test2.str[0]);
        free(test2.str); // 调用者传入变量test2, fifo深拷贝后，调用者需要释放arr
    }

    /* 写队列2 */
    for (int i = 0; i < 26; i++) {
        FifoTest* test2 = (FifoTest*)malloc(sizeof(FifoTest));
        test2->len      = 2;
        test2->str      = (char*)malloc(sizeof(char) * test2->len);
        test2->str[0]   = 'A' + i;
        test2->str[1]   = 0;
        fifo_write(fifo, test2, true); // do not free test2, fifo will free it
    }

    for (int i = 0; !fifo_empty(fifo); i++) {
        FifoTest* test3 = (FifoTest*)fifo_read(fifo, NULL);
        SLOG_DEBUG("test3 = %s", test3->str);
        ASSERT_(test3->str[0] == 'A' + i, "test3.str[0] = %c", test3->str[0]);
        FifoTest_free(test3); // 调用者传入NULL, fifo直接返回指针，调用者需要释放test3
    }

    fifo_del(fifo);

    SLOG_INFO("-- fifo complex test done --\n");

    return 0;
}


int fifo_test()
{
    SLOG_INFO("-- fifo test start --\n");

    fifo_simple_test();
    fifo_complex_test();

    SLOG_INFO("-- fifo test done --\n");

    return 0;
}
