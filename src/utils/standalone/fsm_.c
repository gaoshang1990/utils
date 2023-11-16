/**
 *****************************************************************************
 * \file    fsm_.c
 * \author  xu
 * \version V1.0.0
 * \date    2023-11-13
 * \brief   finite state machine
 * \depend  lstLib_s.c
 *
 * \history
 *
 * 1. Date: 2023-11-13
 *    Author: xu
 *    Modification: modified from https://github.com/54zorb/Zorb-Framework
 *
 *****************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "debug_s.h"
#include "fsm_.h"


struct _Fsm_t {
    NODE node;

    bool isRunning;           /* default off */
    int  depth;               /* Ƕ�ײ�������״̬������Ϊ1����״̬���������� */
                              /* ע���Ͻ��ݹ�Ƕ�׺ͻ���Ƕ�� */
    LIST        children;     /* ��״̬���б� */
    Fsm_t       father;       /* ��״̬�� */
    FsmState_cb ownerTrigger; /* ����״̬��Ϊ�趨״̬ʱ���Ŵ�����ǰ״̬�� */
                              /* �����趨����ִ���길״̬��������������״̬�� */
    FsmState_cb currentState; /* ��ǰ״̬ */
};


Fsm_t fsmInit_(Fsm_t* fsm)
{
    Fsm_t out = (Fsm_t)malloc(sizeof(struct _Fsm_t));
    if (out == NULL) {
        LOG_WARN("malloc fsm error\r\n");
        return false;
    }

    out->depth        = 1;
    out->father       = NULL;
    out->ownerTrigger = NULL;
    out->currentState = NULL;
    out->isRunning    = false;
    lstInit(&out->children);

    if (fsm)
        *fsm = out;

    return out;
}


int fsmSetState_(Fsm_t const fsm, FsmState_cb initialState)
{
    ASSERT_(fsm != NULL);
    ASSERT_(initialState != NULL);

    fsm->currentState = initialState;

    return 0;
}


int fsmRun_(Fsm_t const fsm)
{
    ASSERT_(fsm != NULL);

    if (!fsm->isRunning) {
        fsm->isRunning = true;
        return 0;
    }

    return -1;
}


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


int fsmStop_(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsm->isRunning = false;

    return 0;
}


int fsmStopAll_(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsmStop_(fsm);

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t pChildFsm = (Fsm_t)lstNth(&fsm->children, i);
        fsmStopAll_(pChildFsm);
    }

    return 0;
}


int fsmDestroy_(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t child = (Fsm_t)lstNth(&fsm->children, i);
        fsmDestroy_(child);
    }

    free(fsm);

    return 0;
}


int fsmAddChild_(Fsm_t const fsm, Fsm_t const child)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(child != (Fsm_t)0);

    child->father = fsm;
    child->depth  = fsm->depth + 1;

    lstAdd(&fsm->children, (NODE*)child);

    return 0;
}


int fsmSetOwner_(Fsm_t const fsm, FsmState_cb state)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsm->ownerTrigger = state;

    return 0;
}


int fsmRemoveChild_(Fsm_t const fsm, Fsm_t const child)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(child != (Fsm_t)0);

    lstDelete(&fsm->children, (NODE*)child);

    return 0;
}


int fsmDispatch_(Fsm_t const fsm, FsmSignal const signal)
{
    ASSERT_(fsm != NULL);

    if (!fsm->isRunning)
        return -1;

    if (fsm->currentState != NULL) {
        if (fsm->father == NULL ||                          /* 1: Schedule when it's the root state machine */
            fsm->ownerTrigger == NULL ||                    /* 2: Schedule when no trigger state is set */
            fsm->ownerTrigger == fsm->father->currentState) /* 3: Schedule during the trigger state */
        {
            fsm->currentState(fsm, signal);
        }
    }

    for (int i = 0; i < lstCount(&fsm->children); i++) {
        Fsm_t child = (Fsm_t)lstNth(&fsm->children, i);
        fsmDispatch_(child, signal);
    }


    return 0;
}


void fsmTransfer_(Fsm_t const fsm, FsmState_cb nextState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(nextState != (FsmState_cb)0);

    fsm->currentState = (FsmState_cb)nextState;
}


void fsmTransferWithEvent_(Fsm_t const fsm, FsmState_cb nextState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(nextState != (FsmState_cb)0);

    fsmDispatch_(fsm, FSM_SIG_EXIT);
    fsmTransfer_(fsm, nextState);
    fsmDispatch_(fsm, FSM_SIG_ENTER);
}
