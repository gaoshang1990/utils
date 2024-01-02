#ifndef _UCL_TIME_H_
#define _UCL_TIME_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIME_STR_LEN       19 /* "YYYY-MM-DD HH:MM:SS" ²»º¬½áÊø·û */
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
 * \param   setted_ms: unit: ms, if > 0, TMR_USER_FLAG can be used
 */
Timer_t timer_new(uint64_t setted_ms);

void timer_del(Timer_t timer);
void timer_set_ms(Timer_t timer, uint64_t settedMs);

/* call this function in the start of loop */
int timer_running(Timer_t timer);

bool past_setted_ms(Timer_t timer);
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