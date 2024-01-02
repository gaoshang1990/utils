#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#  include <unistd.h>
#endif

#include "time_.h"


const uint8_t MONTH_TAB[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


time_t timestr2second(const char* str)
{
    if (str == NULL)
        return 0;

    if (strlen(str) != TIME_STR_LEN) {
        printf("timestr[%s] error!\n", str); /* "2000-01-01 00:00:00" */
        return 0;
    }
    struct tm cur_tm;
    sscanf(str,
           "%04d-%02d-%02d %02d:%02d:%02d",
           &cur_tm.tm_year,
           &cur_tm.tm_mon,
           &cur_tm.tm_mday,
           &cur_tm.tm_hour,
           &cur_tm.tm_min,
           &cur_tm.tm_sec);
    if (0 == cur_tm.tm_year) {
        printf("timeStr[%s] is invalid!\n", str);
        return 0;
    }
    cur_tm.tm_year -= 1900;
    cur_tm.tm_mon -= 1;

    return mktime(&cur_tm);
}


int second2timestr(char* timestr, time_t sec)
{
    if (timestr == NULL)
        return -1;

    memset(timestr, 0, TIME_STR_LEN + 1);

    struct tm now_tm;
    LOCAL_TIME(&sec, &now_tm);

    sprintf(timestr,
            "%04d-%02d-%02d %02d:%02d:%02d",
            now_tm.tm_year + 1900,
            now_tm.tm_mon + 1,
            now_tm.tm_mday,
            now_tm.tm_hour,
            now_tm.tm_min,
            now_tm.tm_sec);

    return 0;
}


int delay_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
    return 0;
}


/**
 * \brief   基姆拉尔森计算星期公式
 * \retval  0-6：星期日~六
 */
int get_weekday(int year, int month, int day)
{
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }

    return (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
}


/**
 * \brief   判定一个时间的合法性，注意该检测包含非法日期检测
 * \retval  0-正确；-1-错误
 */
int check_time(struct tm* pdate)
{
    int c[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (pdate->tm_year < 100 || pdate->tm_year > 200)
        return -1;
    if ((pdate->tm_mon < 0) || (pdate->tm_mon > 11))
        return -1;
    if ((pdate->tm_mday < 0) || (pdate->tm_mday > 31))
        return -1;
    if ((pdate->tm_hour < 0) || (pdate->tm_hour > 23))
        return -1;
    if ((pdate->tm_min < 0) || (pdate->tm_min > 59))
        return -1;
    if ((pdate->tm_sec < 0) || (pdate->tm_sec > 60))
        return -1;

    int year  = pdate->tm_year + 1900;
    int month = pdate->tm_mon + 1;
    int day   = pdate->tm_mday;

    if ((pdate->tm_wday != 0xffff) &&
        (((pdate->tm_wday > 0) ? (pdate->tm_wday - 1) : 6) != get_weekday(year, month, day)))
    {
        return 0;
    }

    c[1] = 28 + IS_LEAP_YEAR(year);
    if (day <= c[month - 1])
        return 0;

    return -1;
}


/* since UTC1970-01-01 00:00:00 */
uint64_t time_ms(void)
{
    uint64_t ret = 0;

#ifdef _WIN32
    FILETIME              ft;
    uint64_t              now;
    static const uint64_t DIFF_TO_UNIXTIME = 11644473600000LL;

    GetSystemTimeAsFileTime(&ft);
    now = (uint64_t)ft.dwLowDateTime + ((uint64_t)(ft.dwHighDateTime) << 32LL);
    ret = (now / 10000LL) - DIFF_TO_UNIXTIME;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    ret = ((uint64_t)now.tv_sec * 1000LL) + (now.tv_usec / 1000);
#endif

    return ret;
}


/* since system start */
uint64_t cpu_ms(void)
{
    uint64_t now_ms = 0;

#ifdef _WIN32
    long now_tick = clock();
    if (now_tick < 0)
        now_ms = time_ms();
    else
        now_ms = (uint64_t)((now_tick * 1000) / CLOCKS_PER_SEC);
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    nowMs = ((uint64_t)tp.tv_sec) * 1000LL + (tp.tv_nsec / 1000000);
#endif

    return now_ms;
}


#define TMR_SEC_FLAG  0x01
#define TMR_MIN_FLAG  0x02
#define TMR_HOUR_FLAG 0x04
#define TMR_DAY_FLAG  0x08
#define TMR_MON_FLAG  0x10
#define TMR_WEEK_FLAG 0x20
#define TMR_YEAR_FLAG 0x40
#define TMR_USER_FLAG 0x80


struct _Timer_t_ {
    uint8_t   flag;
    uint64_t  setted_ms; /* if > 0, TMR_USER_FLAG can be used */
    uint64_t  last_ms;
    struct tm last_tm;
    struct tm curr_tm;
};


/**
 * \param   setted_ms: unit: ms, if > 0, TMR_USER_FLAG can be used
 */
Timer_t timer_new(uint64_t setted_ms)
{
    Timer_t timer = (Timer_t)malloc(sizeof(struct _Timer_t_));
    if (timer != NULL) {
        time_t nowSec = time(NULL);
        LOCAL_TIME(&nowSec, &timer->last_tm);

        if (setted_ms > 0) {
            timer->setted_ms = setted_ms;
            timer->last_ms   = cpu_ms();
        }
    }

    return timer;
}


void timer_del(Timer_t timer)
{
    if (timer != NULL) {
        free(timer);
        timer = NULL;
    }
}


void timer_set_ms(Timer_t timer, uint64_t setted_ms)
{
    timer->setted_ms = setted_ms;
    timer->last_ms   = cpu_ms();
}


/**
 * \brief   call this function in the start of loop
 */
int timer_running(Timer_t timer)
{
    timer->flag = 0; /* clear timer flag */

    time_t nowSec = time(NULL);
    LOCAL_TIME(&nowSec, &timer->curr_tm);

    if (timer->setted_ms > 0) {
        uint64_t nowMs = cpu_ms();
        if (nowMs - timer->last_ms > timer->setted_ms) {
            timer->last_ms = nowMs;
            timer->flag |= TMR_USER_FLAG;
        }
    }
    if (timer->curr_tm.tm_sec != timer->last_tm.tm_sec) {
        timer->last_tm.tm_sec = timer->curr_tm.tm_sec;
        timer->flag |= TMR_SEC_FLAG;
    }
    if (timer->curr_tm.tm_min != timer->last_tm.tm_min) {
        timer->last_tm.tm_min = timer->curr_tm.tm_min;
        timer->flag |= TMR_MIN_FLAG;
    }
    if (timer->curr_tm.tm_hour != timer->last_tm.tm_hour) {
        timer->last_tm.tm_hour = timer->curr_tm.tm_hour;
        timer->flag |= TMR_HOUR_FLAG;
    }
    if (timer->curr_tm.tm_mday != timer->last_tm.tm_mday) {
        timer->last_tm.tm_mday = timer->curr_tm.tm_mday;
        timer->flag |= TMR_DAY_FLAG;
    }
    if (timer->curr_tm.tm_mon != timer->last_tm.tm_mon) {
        timer->last_tm.tm_mon = timer->curr_tm.tm_mon;
        timer->flag |= TMR_MON_FLAG;
    }
    if (timer->curr_tm.tm_year != timer->last_tm.tm_year) {
        timer->last_tm.tm_year = timer->curr_tm.tm_year;
        timer->flag |= TMR_YEAR_FLAG;
    }
    if (timer->curr_tm.tm_min != timer->last_tm.tm_min) {
        timer->last_tm.tm_min = timer->curr_tm.tm_min;
        timer->flag |= TMR_MIN_FLAG;
    }

    return 0;
}


bool past_setted_ms(Timer_t timer)
{
    return timer->flag & TMR_USER_FLAG;
}


bool past_second(Timer_t timer)
{
    return timer->flag & TMR_SEC_FLAG;
}


bool past_minute(Timer_t timer)
{
    return timer->flag & TMR_MIN_FLAG;
}


bool past_hour(Timer_t timer)
{
    return timer->flag & TMR_HOUR_FLAG;
}


bool past_day(Timer_t timer)
{
    return timer->flag & TMR_DAY_FLAG;
}


bool past_month(Timer_t timer)
{
    return timer->flag & TMR_MON_FLAG;
}


bool past_week(Timer_t timer)
{
    return timer->flag & TMR_WEEK_FLAG;
}


bool past_year(Timer_t timer)
{
    return timer->flag & TMR_YEAR_FLAG;
}
