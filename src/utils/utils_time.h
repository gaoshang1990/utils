#ifndef _UTILS_TIME_H___
#define _UTILS_TIME_H___

#include <stdbool.h>
#include <stdint.h>
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


time_t   time_str_to_sec(const char* str);
int      time_str(char* timestr, time_t sec);
int      time_str_with_ms(char* timestr, uint64_t ms);
int      get_weekday(int year, int month, int day);
bool     check_time_valid(struct tm* pdate);
int      delay_ms(int ms);
uint64_t time_ms(void);
uint64_t cpu_ms(void);

/* timer demo:
    UtilTimer timer = timer_new(100);

    while (1) {
        timer_running(timer);

        if (past_second(timer))
            printf("past second\n");

        if (past_user_define(timer))
            printf("past 100ms\n");

        delay_ms(10);
    }
 */

typedef struct _Timer_t_* UtilTimer;

/**
 * @param set_all_flag 初次运行时是否设置所有标识为真
 * @param user_define  用户自定义的定时器周期, 单位ms
 */
UtilTimer timer_new(bool set_all_flag, uint64_t user_define);

void timer_del(UtilTimer self);
void timer_user_define(UtilTimer self, uint64_t settedMs);

/* call this function in the start of loop */
void timer_running(UtilTimer self);

bool past_user_define(UtilTimer self);
bool past_second(UtilTimer self);
bool past_minute(UtilTimer self);
bool past_hour(UtilTimer self);
bool past_day(UtilTimer self);
bool past_week(UtilTimer self);
bool past_month(UtilTimer self);
bool past_year(UtilTimer self);

int now_year(UtilTimer self);
int now_month(UtilTimer self);
int now_day(UtilTimer self);
int now_hour(UtilTimer self);
int now_minute(UtilTimer self);
int now_second(UtilTimer self);
int now_weekday(UtilTimer self);


#ifdef __cplusplus
}
#endif

#endif /* _UTILS_TIME_H___ */
