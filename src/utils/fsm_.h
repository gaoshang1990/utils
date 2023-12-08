#ifndef __ZF_FSM_H__
#define __ZF_FSM_H__

#include <stdint.h>
#include <stdbool.h>

#include "lstLib_.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ״̬���ź�0-31�������û��ź���32�Ժ��� */
enum {
    FSM_SIG_NULL = 0,
    FSM_SIG_ENTER,
    FSM_SIG_EXIT,

    FSM_SIG_USER_START = 32 /* �û��ź������û��ļ�����, �������ڴ˶��� */

};

typedef struct _Fsm_t* Fsm_t;

typedef uint32_t FsmSignal;

typedef void (*FsmState_cb)(Fsm_t const, FsmSignal const);


Fsm_t fsmInit_(Fsm_t* ppFsm); /* ����״̬��(�ڲ�����ռ�) */


/* ���ó�ʼ״̬ */
int fsmSetState_(Fsm_t const pFsm, FsmState_cb initialState);

/* ���е�ǰ״̬�� */
int fsmRun_(Fsm_t const pFsm);

/* ���е�ǰ״̬������״̬�� */
int fsmRunAll_(Fsm_t const pFsm);

/* ֹͣ��ǰ״̬�� */
int fsmStop_(Fsm_t const pFsm);

/* ֹͣ��ǰ״̬������״̬�� */
int fsmStopAll_(Fsm_t const pFsm);

/* �ͷŵ�ǰ״̬�� */
int fsmDestroy_(Fsm_t const pFsm);

/* �����״̬�� */
int fsmAddChild_(Fsm_t const pFsm, Fsm_t const pChildFsm);

int fsmSetOwner_(Fsm_t const fsm, FsmState_cb state);

/* �Ƴ���״̬��(���ͷſռ�) */
int fsmRemoveChild_(Fsm_t const pFsm, Fsm_t const pChildFsm);

/* ����״̬�� */
int fsmDispatch_(Fsm_t const pFsm, FsmSignal const signal);

/* ״̬ת�� */
void fsmTransfer_(Fsm_t const pFsm, FsmState_cb nextState);

/* ״̬ת��(����ת����ת���¼�) */
void fsmTransferWithEvent_(Fsm_t const pFsm, FsmState_cb nextState);


#ifdef __cplusplus
}
#endif

#endif /* __ZF_FSM_H__ */
