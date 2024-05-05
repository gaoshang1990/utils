#ifndef _UCL_TIME_H_
#define _UCL_TIME_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIME_STR_LEN       19 /* "YYYY-MM-DD HH:MM:SS" 不含结束符 */
#define IS_LEAP_YEAR(year) ((year % 4 == 0 && year % 100) || year % 400 == 0)


#ifdef _WIN32
#  define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#else
#  define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#endif

typedef struct _Timer_t_* Timer_t;

extern const uint8_t MONTH_TAB[];

time_t   timestr2second(const char* str);
int      second2timestr(char* timestr, time_t sec);
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
 * @param set_all_flag 初次运行时是否设置所有标识为真
 * @param user_define  用户自定义的定时器周期, 单位ms
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


#ifdef __cplusplus
}
#endif

#endif /* _UCL_TIME_H_ */
