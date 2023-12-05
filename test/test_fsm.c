#include <stdio.h>

#include "time_s.h"
#include "fsm_.h"

/* �����û��ź� */
enum Signal { SAY_HELLO = FSM_SIG_USER_START };

Fsm_t _fsm;    /* ��״̬�� */
Fsm_t _fsmSon; /* ��״̬�� */

/* ��״̬��״̬1 */
static void State1(Fsm_t const pFsm, FsmSignal const fsmSignal);
/* ��״̬��״̬2 */
static void State2(Fsm_t const pFsm, FsmSignal const fsmSignal);

/******************************************************************************
 * ����  ����״̬��״̬1
 * ����  ��-pFsm       ��ǰ״̬��
 *         -fsmSignal  ��ǰ�����ź�
 * ����  ����
 ******************************************************************************/
static void State1(Fsm_t const pFsm, FsmSignal const fsmSignal)
{
    switch (fsmSignal) {
    case FSM_SIG_ENTER:
        printf("enter state1\r\n");
        break;

    case FSM_SIG_EXIT:
        printf("exit state1\r\n\r\n");
        break;

    case SAY_HELLO:
        printf("state1 say hello, and want to be state2\r\n");
        /* �л���״̬2 */
        fsmTransferWithEvent_(pFsm, State2);
        break;
    }
}

/******************************************************************************
 * ����  ����״̬��״̬2
 * ����  ��-pFsm       ��ǰ״̬��
 *         -fsmSignal  ��ǰ�����ź�
 * ����  ����
 ******************************************************************************/
static void State2(Fsm_t const pFsm, FsmSignal const fsmSignal)
{
    switch (fsmSignal) {
    case FSM_SIG_ENTER:
        printf("enter state2\r\n");
        break;

    case FSM_SIG_EXIT:
        printf("exit state2\r\n\r\n");
        break;

    case SAY_HELLO:
        printf("state2 say hello, and want to be state1\r\n");
        /* �л���״̬1 */
        fsmTransferWithEvent_(pFsm, State1);
        break;
    }
}

/******************************************************************************
 * ����  ����״̬��״̬
 * ����  ��-pFsm       ��ǰ״̬��
 *         -fsmSignal  ��ǰ�����ź�
 * ����  ����
 ******************************************************************************/
static void SonState(Fsm_t const pFsm, FsmSignal const fsmSignal)
{
    switch (fsmSignal) {
    case SAY_HELLO:
        printf("son say hello only in state2\r\n");
        break;
    }
}

/******************************************************************************
 * ����  �������ʼ��
 * ����  ����
 * ����  ����
 ******************************************************************************/
void App_Fsm_init(void)
{
    /* ������״̬���������ʼ״̬ */
    fsmInit_(&_fsm);
    fsmSetState_(_fsm, State1);

    /* ������״̬���������ʼ״̬ */
    fsmInit_(&_fsmSon);
    fsmSetState_(_fsmSon, SonState);

    /* ������״̬�����ڸ�״̬State2���� */
    fsmSetOwner_(_fsmSon, State2);

    /* ����״̬����ӵ���״̬�� */
    fsmAddChild_(_fsm, _fsmSon);

    /* ����״̬�� */
    fsmRunAll_(_fsm);
}

/******************************************************************************
 * ����  ���������
 * ����  ����
 * ����  ����
 ******************************************************************************/
void App_Fsm_process(void)
{
    delayMs_(3000);

    /* ÿ1000ms����״̬��������SAY_HELLO�ź� */
    fsmDispatch_(_fsm, SAY_HELLO);
}

/******************************** END OF FILE ********************************/