#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#  include <unistd.h>
#endif

#include "time_s.h"


const uint8_t _monthTab[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


struct tm second2struct_(time_t sec)
{
    struct tm tms;
    LOCAL_TIME(&sec, &tms);

    return tms;
}


time_t timestr2second_(const char* str)
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


int second2timestr_(char* timestr, time_t sec)
{
    if (timestr == NULL)
        return -1;

    memset(timestr, 0, TIME_STR_LEN + 1);

    struct tm now_tm = second2struct_(sec);
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


int delayMs_(int ms)
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
int getWeekDay_(int year, int month, int day)
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
int checkTime_(struct tm* pdate)
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

    /* ==0xffff 不判断 */
    if ((pdate->tm_wday != 0xffff) &&
        (((pdate->tm_wday > 0) ? (pdate->tm_wday - 1) : 6) != getWeekDay_(year, month, day)))
    {
        return 0;
    }

    c[1] = 28 + IS_LEAP_YEAR(year);
    if (day <= c[month - 1])
        return 0;
    else
        return -1;
}


/* 从UTC1970-1-1 0:0:0开始的毫秒数 */
uint64_t timeMs_(void)
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


/* 从系统启动开始的毫秒数 */
uint64_t cpuMs_(void)
{
    uint64_t nowMs = 0;

#ifdef _WIN32
    long nowTick = clock();
    if (nowTick < 0) {
        nowMs = timeMs_();
    }
    else {
        nowMs = (uint64_t)((nowTick * 1000) / CLOCKS_PER_SEC);
    }
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    nowMs = ((uint64_t)tp.tv_sec) * 1000LL + (tp.tv_nsec / 1000000);
#endif

    return nowMs;
}


#define TMR_SEC_FLAG  0x01 /* 过秒标志 */
#define TMR_MIN_FLAG  0x02 /* 过分标志 */
#define TMR_HOUR_FLAG 0x04 /* 过时标志 */
#define TMR_DAY_FLAG  0x08 /* 过日标志 */
#define TMR_MON_FLAG  0x10 /* 过月标志 */
#define TMR_WEEK_FLAG 0x20 /* 过星期标志 */
#define TMR_YEAR_FLAG 0x40 /* 过年标志 */
#define TMR_USER_FLAG 0x80 /* 自定义标志 */


struct _Timer_t_ {
    uint8_t   flag; /* bit: 0-过秒 1-过分 2-过时 3-过日 4-过月 5-过星期 6-过年 7-过自定义时间 */
    uint64_t  settedMs; /* unit: ms, if > 0, TMR_USER_FLAG can be used */
    uint64_t  lastMs;
    struct tm lastTm;
    struct tm nowTm;
};


/**
 * \param   settedMs: unit: ms, if > 0, TMR_USER_FLAG can be used
 */
Timer_t timerInit_(uint64_t settedMs)
{
    Timer_t timer = (Timer_t)malloc(sizeof(struct _Timer_t_));
    if (timer != NULL) {
        time_t nowSec = time(NULL);
        LOCAL_TIME(&nowSec, &timer->lastTm);

        if (settedMs > 0) {
            timer->settedMs = settedMs;
            timer->lastMs   = cpuMs_();
        }
    }

    return timer;
}


/**
 * \brief   call this function in the start of loop
 */
int timerRunning_(Timer_t timer)
{
    timer->flag = 0; /* clear timer flag */

    time_t nowSec = time(NULL);
    LOCAL_TIME(&nowSec, &timer->nowTm);

    if (timer->settedMs > 0) {
        uint64_t nowMs = cpuMs_();
        if (nowMs - timer->lastMs > timer->settedMs) {
            timer->lastMs = nowMs;
            timer->flag |= TMR_USER_FLAG;
        }
    }
    if (timer->nowTm.tm_sec != timer->lastTm.tm_sec) {
        timer->lastTm.tm_sec = timer->nowTm.tm_sec;
        timer->flag |= TMR_SEC_FLAG;
    }
    if (timer->nowTm.tm_min != timer->lastTm.tm_min) {
        timer->lastTm.tm_min = timer->nowTm.tm_min;
        timer->flag |= TMR_MIN_FLAG;
    }
    if (timer->nowTm.tm_hour != timer->lastTm.tm_hour) {
        timer->lastTm.tm_hour = timer->nowTm.tm_hour;
        timer->flag |= TMR_HOUR_FLAG;
    }
    if (timer->nowTm.tm_mday != timer->lastTm.tm_mday) {
        timer->lastTm.tm_mday = timer->nowTm.tm_mday;
        timer->flag |= TMR_DAY_FLAG;
    }
    if (timer->nowTm.tm_mon != timer->lastTm.tm_mon) {
        timer->lastTm.tm_mon = timer->nowTm.tm_mon;
        timer->flag |= TMR_MON_FLAG;
    }
    if (timer->nowTm.tm_year != timer->lastTm.tm_year) {
        timer->lastTm.tm_year = timer->nowTm.tm_year;
        timer->flag |= TMR_YEAR_FLAG;
    }
    if (timer->nowTm.tm_min != timer->lastTm.tm_min) {
        timer->lastTm.tm_min = timer->nowTm.tm_min;
        timer->flag |= TMR_MIN_FLAG;
    }

    return 0;
}


bool pastSettedMs_(Timer_t timer)
{
    return timer->flag & TMR_USER_FLAG;
}


bool pastSecond_(Timer_t timer)
{
    return timer->flag & TMR_SEC_FLAG;
}


bool pastMinute_(Timer_t timer)
{
    return timer->flag & TMR_MIN_FLAG;
}


bool pastHour_(Timer_t timer)
{
    return timer->flag & TMR_HOUR_FLAG;
}


bool pastDay_(Timer_t timer)
{
    return timer->flag & TMR_DAY_FLAG;
}


bool pastMonth_(Timer_t timer)
{
    return timer->flag & TMR_MON_FLAG;
}


bool pastWeek_(Timer_t timer)
{
    return timer->flag & TMR_WEEK_FLAG;
}


bool pastYear_(Timer_t timer)
{
    return timer->flag & TMR_YEAR_FLAG;
}
