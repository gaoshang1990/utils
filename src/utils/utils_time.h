#ifndef _UTILS_TIME_H___
#define _UTILS_TIME_H___

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIME_STR_LEN       19 /* "YYYY-MM-DD HH:MM:SS" ���������� */
#define IS_LEAP_YEAR(year) ((year % 4 == 0 && year % 100) || year % 400 == 0)


#ifdef _WIN32
#  define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#else
#  define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#endif

typedef struct _Timer_t_* Timer_t;


time_t   time_str_to_sec(const char* str);
int      time_str(char* timestr, time_t sec);
int      get_weekday(int year, int month, int day);
int      check_time(struct tm* pdate);
int      delay_ms(int ms);
uint64_t time_ms(void);
uint64_t cpu_ms(void);

/* timer demo:
    Timer_t timer = timer_new(100);
    while (1) {
        timer_running(timer);

        if (past_second(timer))
            printf("past second\n");

        if (past_setted_ms(timer))
            printf("past 100ms\n");

        delay_ms(10);
    }
 */

/**
 * @param set_all_flag ��������ʱ�Ƿ��������б�ʶΪ��
 * @param user_define  �û��Զ���Ķ�ʱ������, ��λms
 */
Timer_t timer_new(bool set_all_flag, uint64_t user_define);

void timer_del(Timer_t timer);
void timer_set_ms(Timer_t timer, uint64_t settedMs);

/* call this function in the start of loop */
int timer_running(Timer_t timer);

bool past_user_define(Timer_t timer);
bool past_second(Timer_t timer);
bool past_minute(Timer_t timer);
bool past_hour(Timer_t timer);
bool past_day(Timer_t timer);
bool past_week(Timer_t timer);
bool past_month(Timer_t timer);
bool past_year(Timer_t timer);

int now_year(Timer_t self);
int now_month(Timer_t self);
int now_day(Timer_t self);
int now_hour(Timer_t self);
int now_minute(Timer_t self);
int now_second(Timer_t self);
int now_weekday(Timer_t self);
int now_ms(Timer_t self);


#ifdef __cplusplus
}
#endif

#endif /* _UTILS_TIME_H___ */
