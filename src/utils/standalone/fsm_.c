/**
 *****************************************************************************
 * \file    fsm_.c
 * \author  xu
 * \version V1.0.0
 * \date    2023-11-13
 * \brief   ����״̬����ʵ��
 * \depend  lstLib_s.c
 *
 * \history
 *
 * 1. Date: 2023-11-13
 *    Author: xu
 *    Modification: �޸���https://github.com/54zorb/Zorb-Framework
 *
 *****************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "debug_s.h"
#include "fsm_.h"


/* ״̬���ṹ */
struct _Fsm_t {
    NODE node;
    int  depth;                  /* Ƕ�ײ�������״̬������Ϊ1����״̬���������� */
                                 /* ע���Ͻ��ݹ�Ƕ�׺ͻ���Ƕ�� */
    LIST      children;          /* ��״̬���б� */
    Fsm_t     father;            /* ��״̬�� */
    IFsmState OwnerTriggerState; /* ����״̬��Ϊ�趨״̬ʱ���Ŵ�����ǰ״̬�� */
                                 /* �����趨����ִ���길״̬��������������״̬�� */
    IFsmState CurrentState;      /* ��ǰ״̬ */
    bool      isRunning;         /* �Ƿ���������(Ĭ�Ϲ�) */
};


/******************************************************************************
 * ����  ������״̬��(�ڲ�����ռ�)
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
 * ����  ������״̬����ʼ״̬
 ******************************************************************************/
int fsmSetState_(Fsm_t const fsm, IFsmState initialState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(initialState != (IFsmState)0);

    fsm->CurrentState = initialState;

    return 0;
}

/******************************************************************************
 * ����  �����е�ǰ״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
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
 * ����  �����е�ǰ״̬������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
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
 * ����  ��ֹͣ��ǰ״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
 ******************************************************************************/
int fsmStop(Fsm_t const fsm)
{
    ASSERT_(fsm != (Fsm_t)0);

    fsm->isRunning = false;

    return 0;
}

/******************************************************************************
 * ����  ��ֹͣ��ǰ״̬������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
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
 * ����  ���ͷŵ�ǰ״̬������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
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
 * ����  �������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-pChildFsm      ��״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
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
 * ����  ���Ƴ���״̬��(���ͷſռ�)
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-pChildFsm      ��״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
 ******************************************************************************/
int fsmRemoveChild(Fsm_t const fsm, Fsm_t const child)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(child != (Fsm_t)0);

    lstDelete(&fsm->children, (NODE*)child);

    return 0;
}

/******************************************************************************
 * ����  ������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-signal         �����ź�
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
 ******************************************************************************/
int fsmDispatch(Fsm_t const fsm, FsmSignal const signal)
{
    ASSERT_(fsm != (Fsm_t)0);

    if (fsm->isRunning)
        return -1;

    if (fsm->CurrentState != NULL) {
        if (fsm->father == NULL ||                               /* 1:��״̬��ʱ���� */
            fsm->OwnerTriggerState == NULL ||                    /* 2:û���ô���״̬ʱ���� */
            fsm->OwnerTriggerState == fsm->father->CurrentState) /* 3:���ڴ���״̬ʱ���� */
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
 * ����  ��״̬ת��
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-nextState      ת�ƺ�״̬
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
 ******************************************************************************/
void fsmTransfer(Fsm_t const fsm, IFsmState nextState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(nextState != (IFsmState)0);

    fsm->CurrentState = (IFsmState)nextState;
}

/******************************************************************************
 * ����  ��״̬ת��(����ת����ת���¼�)
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-nextState      ת�ƺ�״̬
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
 ******************************************************************************/
void fsmTransferWithEvent(Fsm_t const fsm, IFsmState nextState)
{
    ASSERT_(fsm != (Fsm_t)0);
    ASSERT_(nextState != (IFsmState)0);

    fsmDispatch(fsm, FSM_SIG_EXIT);
    fsmTransfer(fsm, nextState);
    fsmDispatch(fsm, FSM_SIG_ENTER);
}
