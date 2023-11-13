#ifndef __ZF_FSM_H__
#define __ZF_FSM_H__

#include <stdint.h>
#include <stdbool.h>

#include "lstLib_s.h"

#ifdef __cplusplus
extern "C" {
#endif


/* 状态机信号0-31保留，用户信号在32以后定义 */
enum {
    FSM_SIG_NULL = 0,
    FSM_SIG_ENTER,
    FSM_SIG_EXIT,
    FSM_SIG_USER_START = 32
    /* 用户信号请在用户文件定义，不允许在此定义 */
};

typedef struct _Fsm_t* Fsm_t;

typedef uint32_t FsmSignal;

typedef void (*IFsmState)(Fsm_t const, FsmSignal const);


Fsm_t fsmInit_(Fsm_t* ppFsm); /* 创建状态机(内部分配空间) */


/* 设置初始状态 */
int fsmSetState_(Fsm_t const pFsm, IFsmState initialState);

/* 运行当前状态机 */
int fsmRun_(Fsm_t const pFsm);

/* 运行当前状态机和子状态机 */
int fsmRunAll_(Fsm_t const pFsm);

/* 停止当前状态机 */
int fsmStop(Fsm_t const pFsm);

/* 停止当前状态机和子状态机 */
int fsmStopAll(Fsm_t const pFsm);

/* 释放当前状态机 */
int fsmDestroy(Fsm_t const pFsm);

/* 添加子状态机 */
int fsmAddchild(Fsm_t const pFsm, Fsm_t const pChildFsm);

/* 移除子状态机(不释放空间) */
int fsmRemoveChild(Fsm_t const pFsm, Fsm_t const pChildFsm);

/* 调度状态机 */
int fsmDispatch(Fsm_t const pFsm, FsmSignal const signal);

/* 状态转移 */
void fsmTransfer(Fsm_t const pFsm, IFsmState nextState);

/* 状态转移(触发转出和转入事件) */
void fsmTransferWithEvent(Fsm_t const pFsm, IFsmState nextState);


#ifdef __cplusplus
}
#endif

#endif /* __ZF_FSM_H__ */
