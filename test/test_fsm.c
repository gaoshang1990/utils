#include <stdio.h>

#include "time_s.h"
#include "fsm_.h"

/* 定义用户信号 */
enum Signal { SAY_HELLO = FSM_SIG_USER_START };

Fsm_t _fsm;    /* 父状态机 */
Fsm_t _fsmSon; /* 子状态机 */

/* 父状态机状态1 */
static void State1(Fsm_t const pFsm, FsmSignal const fsmSignal);
/* 父状态机状态2 */
static void State2(Fsm_t const pFsm, FsmSignal const fsmSignal);

/******************************************************************************
 * 描述  ：父状态机状态1
 * 参数  ：-pFsm       当前状态机
 *         -fsmSignal  当前调度信号
 * 返回  ：无
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
        /* 切换到状态2 */
        fsmTransferWithEvent_(pFsm, State2);
        break;
    }
}

/******************************************************************************
 * 描述  ：父状态机状态2
 * 参数  ：-pFsm       当前状态机
 *         -fsmSignal  当前调度信号
 * 返回  ：无
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
        /* 切换到状态1 */
        fsmTransferWithEvent_(pFsm, State1);
        break;
    }
}

/******************************************************************************
 * 描述  ：子状态机状态
 * 参数  ：-pFsm       当前状态机
 *         -fsmSignal  当前调度信号
 * 返回  ：无
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
 * 描述  ：任务初始化
 * 参数  ：无
 * 返回  ：无
 ******************************************************************************/
void App_Fsm_init(void)
{
    /* 创建父状态机，并设初始状态 */
    fsmInit_(&_fsm);
    fsmSetState_(_fsm, State1);

    /* 创建子状态机，并设初始状态 */
    fsmInit_(&_fsmSon);
    fsmSetState_(_fsmSon, SonState);

    /* 设置子状态机仅在父状态State2触发 */
    fsmSetOwner_(_fsmSon, State2);

    /* 把子状态机添加到父状态机 */
    fsmAddChild_(_fsm, _fsmSon);

    /* 运行状态机 */
    fsmRunAll_(_fsm);
}

/******************************************************************************
 * 描述  ：任务程序
 * 参数  ：无
 * 返回  ：无
 ******************************************************************************/
void App_Fsm_process(void)
{
    delayMs_(3000);

    /* 每1000ms调度状态机，发送SAY_HELLO信号 */
    fsmDispatch_(_fsm, SAY_HELLO);
}

/******************************** END OF FILE ********************************/