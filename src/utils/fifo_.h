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
 * @brief   д�����ݵ� fifo
 * @param   src         Դ����ָ��, ����ֱ�ӱ��浽fifo;
 *                      ��srcΪ��̬������ڴ�, auto_freeӦΪfalse,
 *                      ���ڸñ������������ڽ���ǰ��֤fifo�е������ѱ�ʹ�����
 * @param   auto_free   �Ƿ���fifo_read()���Զ��ͷ�src
 * @return  FIFO_OK: д��ɹ�, ����ʧ��
 */
int fifo_write(Fifo_t fifo, void* src, bool auto_free);

/**
 * @brief   ��ȡ����
 * @param   dst     �����ݿ�����dst, ����auto_free��ʶ�����Ƿ��ͷ�ԭ����
 * @return  FIFO_OK: ��ȡ�ɹ�, ����ʧ��
 */
int fifo_read(Fifo_t fifo, void* dst);

int fifo_clear(Fifo_t fifo);
int fifo_del(Fifo_t fifo);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_FIFO_H_ */
