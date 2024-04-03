#ifndef _UCL_FIFO_H_
#define _UCL_FIFO_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define FIFO_SIZE_MAX (256)
#define FIFO_SIZE_MIN (4)

typedef void (*FreeNode_cb)(void* data);
typedef void* (*CopyNode_cb)(void* dst, const void* src, size_t len);
typedef struct _Fifo_t_* Fifo_t;


enum {
    FIFO_OK,
    FIFO_NULL,
    FIFO_FULL,
    FIFO_EMPTY,
};


Fifo_t fifo_new(size_t node_size, FreeNode_cb free_cb, CopyNode_cb copy_cb, bool thread_safe);
void   fifo_lock(Fifo_t fifo);
void   fifo_unlock(Fifo_t fifo);
bool   fifo_full(Fifo_t fifo);
bool   fifo_empty(Fifo_t fifo);

/**
 * @brief   写入数据到 fifo
 * @param   src         源数据指针, 将被直接保存到fifo;
 *                      若src为静态分配的内存, auto_free应为false,
 *                      且在该变量的生命周期结束前保证fifo中的数据已被使用完毕
 * @param   auto_free   是否在fifo_read()中自动释放src
 * @return  FIFO_OK: 写入成功, 其他失败
 */
int fifo_write(Fifo_t fifo, void* src, bool auto_free);

/**
 * @brief   读取数据
 * @param   dst     将数据拷贝到dst, 根据auto_free标识决定是否释放原数据
 * @return  FIFO_OK: 读取成功, 其他失败
 */
int fifo_read(Fifo_t fifo, void* dst);

int fifo_clear(Fifo_t fifo);
int fifo_del(Fifo_t fifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */
