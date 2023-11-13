/**
 *****************************************************************************
 * \file    fsm_.c
 * \author  xu
 * \version V1.0.0
 * \date    2023-11-13
 * \brief   有限状态机的实现
 * \depend  lstLib_s.c
 *
 * \history
 *
 * 1. Date: 2023-11-13
 *    Author: xu
 *    Modification: 修改自https://github.com/54zorb/Zorb-Framework
 *
 *****************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "debug_s.h"
#include "fsm_.h"


/* 状态机结构 */
struct _Fsm_t {
    NODE node;
    int  depth;                  /* 嵌套层数，根状态机层数为1，子状态机层数自增 */
                                 /* 注：严禁递归嵌套和环形嵌套 */
    LIST      children;          /* 子状态机列表 */
    Fsm_t     father;            /* 父状态机 */
    IFsmState OwnerTriggerState; /* 当父状态机为设定状态时，才触发当前状态机 */
                                 /* 若不设定，则当执行完父状态机，立即运行子状态机 */
    IFsmState CurrentState;      /* 当前状态 */
    bool      isRunning;         /* 是否正在运行(默认关) */
};


/******************************************************************************
 * 描述  ：创建状态机(内部分配空间)
 ******************************************************************************/
Fsm_t fsmInit_(Fsm_t* pFsm)
{
    Fsm_t fsm = (Fsm_t)malloc(sizeof(struct _Fsm_t));
    if (fsm == NULL) {
        LOG_WARN("malloc fsm space error\r\n");
        return false;
    }

    fsm->depth             = 1;
    fsm->father            = NULL;
    fsm->OwnerTriggerState = NULL;
    fsm->CurrentState      = NULL;
    fsm->isRunning         = false;
    lstInit(&fsm->children);

    if (pFsm)
        *pFsm = fsm;

    return fsm;
}

/******************************************************************************
 * 描述  ：设置状态机初始状态
 ******************************************************************************/
int fsmSetState_(Fsm_t const fsm, IFsmState initialState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(initialState != (IFsmState)0);

    fsm->CurrentState = initialState;

    return 0;
}

/******************************************************************************
 * 描述  ：运行当前状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmRun_(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    if (!fsm->isRunning) {
        fsm->isRunning = true;
        return 0;
    }

    return -1;
}

/******************************************************************************
 * 描述  ：运行当前状态机和子状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmRunAll_(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsmRun_(fsm);

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t pChildFsm = (Fsm_t)lstNth(&fsm->children, i);
        fsmRunAll_(pChildFsm);
    }

    return 0;
}

/******************************************************************************
 * 描述  ：停止当前状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmStop(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsm->isRunning = false;

    return 0;
}

/******************************************************************************
 * 描述  ：停止当前状态机和子状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmStopAll(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsmStop(fsm);

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t pChildFsm = (Fsm_t)lstNth(&fsm->children, i);
        fsmStopAll(pChildFsm);
    }

    return 0;
}

/******************************************************************************
 * 描述  ：释放当前状态机及子状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmDestroy(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t child = (Fsm_t)lstNth(&fsm->children, i);
        fsmDestroy(child);
    }

    free(fsm);

    return 0;
}


/******************************************************************************
 * 描述  ：添加子状态机
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-pChildFsm      子状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmAddchild(Fsm_t const fsm, Fsm_t const child)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(child != (Fsm_t)0);

    child->father = fsm;
    child->depth  = fsm->depth + 1;

    lstAdd(&fsm->children, (NODE*)child);

    return 0;
}

/******************************************************************************
 * 描述  ：移除子状态机(不释放空间)
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-pChildFsm      子状态机指针
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmRemoveChild(Fsm_t const fsm, Fsm_t const child)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(child != (Fsm_t)0);

    lstDelete(&fsm->children, (NODE*)child);

    return 0;
}

/******************************************************************************
 * 描述  ：调度状态机
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-signal         调度信号
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
int fsmDispatch(Fsm_t const fsm, FsmSignal const signal)
{
    ASSERT_(fsm != (Fsm_t)0);

    if (fsm->isRunning)
        return -1;

    if (fsm->CurrentState != NULL) {
        if (fsm->father == NULL ||                               /* 1:根状态机时调度 */
            fsm->OwnerTriggerState == NULL ||                    /* 2:没设置触发状态时调度 */
            fsm->OwnerTriggerState == fsm->father->CurrentState) /* 3:正在触发状态时调度 */
        {
            fsm->CurrentState(fsm, signal);
        }
    }

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t child = (Fsm_t)lstNth(&fsm->children, i);
        fsmDispatch(child, signal);
    }


    return 0;
}

/******************************************************************************
 * 描述  ：状态转移
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-nextState      转移后状态
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
void fsmTransfer(Fsm_t const fsm, IFsmState nextState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(nextState != (IFsmState)0);

    fsm->CurrentState = (IFsmState)nextState;
}

/******************************************************************************
 * 描述  ：状态转移(触发转出和转入事件)
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-nextState      转移后状态
 * 返回  ：-true               成功
 *         -false              失败
 ******************************************************************************/
void fsmTransferWithEvent(Fsm_t const fsm, IFsmState nextState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(nextState != (IFsmState)0);

    fsmDispatch(fsm, FSM_SIG_EXIT);
    fsmTransfer(fsm, nextState);
    fsmDispatch(fsm, FSM_SIG_ENTER);
}
