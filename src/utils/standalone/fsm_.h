#ifndef __ZF_FSM_H__
#define __ZF_FSM_H__

#include <stdint.h>
#include <stdbool.h>

#include "lstLib_s.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ״̬���ź�0-31�������û��ź���32�Ժ��� */
enum {
    FSM_SIG_NULL = 0,
    FSM_SIG_ENTER,
    FSM_SIG_EXIT,
    FSM_SIG_USER_START = 32
    /* �û��ź������û��ļ����壬�������ڴ˶��� */
};

typedef struct _Fsm_t* Fsm_t;

typedef uint32_t FsmSignal;

typedef void (*IFsmState)(Fsm_t const, FsmSignal const);


Fsm_t fsmInit_(Fsm_t* ppFsm); /* ����״̬��(�ڲ�����ռ�) */


/* ���ó�ʼ״̬ */
int fsmSetState_(Fsm_t const pFsm, IFsmState initialState);

/* ���е�ǰ״̬�� */
int fsmRun_(Fsm_t const pFsm);

/* ���е�ǰ״̬������״̬�� */
int fsmRunAll_(Fsm_t const pFsm);

/* ֹͣ��ǰ״̬�� */
int fsmStop(Fsm_t const pFsm);

/* ֹͣ��ǰ״̬������״̬�� */
int fsmStopAll(Fsm_t const pFsm);

/* �ͷŵ�ǰ״̬�� */
int fsmDestroy(Fsm_t const pFsm);

/* �����״̬�� */
int fsmAddchild(Fsm_t const pFsm, Fsm_t const pChildFsm);

/* �Ƴ���״̬��(���ͷſռ�) */
int fsmRemoveChild(Fsm_t const pFsm, Fsm_t const pChildFsm);

/* ����״̬�� */
int fsmDispatch(Fsm_t const pFsm, FsmSignal const signal);

/* ״̬ת�� */
void fsmTransfer(Fsm_t const pFsm, IFsmState nextState);

/* ״̬ת��(����ת����ת���¼�) */
void fsmTransferWithEvent(Fsm_t const pFsm, IFsmState nextState);


#ifdef __cplusplus
}
#endif

#endif /* __ZF_FSM_H__ */
