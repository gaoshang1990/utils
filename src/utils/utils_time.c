#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#  include <unistd.h>
#endif

#include "utils_time.h"


time_t time_str_to_sec(const char* str)
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


int time_str(char* timestr, time_t sec)
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


int time_str_with_ms(char* timestr, uint64_t ms)
{
    if (timestr == NULL)
        return -1;

    struct tm now_tm;
    time_t    now_sec = ms / 1000;
    int       now_ms  = ms % 1000;
    LOCAL_TIME(&now_sec, &now_tm);

    sprintf(timestr,
            "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            now_tm.tm_year + 1900,
            now_tm.tm_mon + 1,
            now_tm.tm_mday,
            now_tm.tm_hour,
            now_tm.tm_min,
            now_tm.tm_sec,
            now_ms);

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
 * @brief   获取某年月的天数
 */
int month_days(int year, int month)
{
    const uint8_t MONTH_TAB[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2)
        return 28 + IS_LEAP_YEAR(year);

    return MONTH_TAB[month - 1];
}


/**
 * @brief   基姆拉尔森计算星期公式
 * @retval  0-6：星期日~六
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
 * @brief   判定一个时间的合法性，注意该检测包含非法日期检测
 * @retval  0-正确；-1-错误
 */
bool check_time_valid(struct tm* pdate)
{
    if (pdate->tm_year < 100 || pdate->tm_year > 200)
        return false;
    if ((pdate->tm_mon < 0) || (pdate->tm_mon > 11))
        return false;
    if ((pdate->tm_mday < 0) || (pdate->tm_mday > 31))
        return false;
    if ((pdate->tm_hour < 0) || (pdate->tm_hour > 23))
        return false;
    if ((pdate->tm_min < 0) || (pdate->tm_min > 59))
        return false;
    if ((pdate->tm_sec < 0) || (pdate->tm_sec > 60))
        return false;

    int year  = pdate->tm_year + 1900;
    int month = pdate->tm_mon + 1;
    int day   = pdate->tm_mday;

    if ((pdate->tm_wday != 0xffff) &&
        (((pdate->tm_wday > 0) ? (pdate->tm_wday - 1) : 6) != get_weekday(year, month, day)))
    {
        return true;
    }

    int day_of_month = month_days(year, month);
    if (day <= day_of_month)
        return true;

    return false;
}


/* since UTC1970-01-01 00:00:00 */
uint64_t time_ms(void)
{
    uint64_t ret = 0;

#ifdef _WIN32
    static const uint64_t _DIFF_TO_UNIXTIME = 11644473600000LL;

    FILETIME ft;
    uint64_t now;

    GetSystemTimeAsFileTime(&ft);
    now = (uint64_t)ft.dwLowDateTime + ((uint64_t)(ft.dwHighDateTime) << 32LL);
    ret = (now / 10000LL) - _DIFF_TO_UNIXTIME; /* now的单位为100ns */
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
    LARGE_INTEGER frequency; /* 计时器频率 */
    LARGE_INTEGER now;       /* 当前时间 */

    if (QueryPerformanceFrequency(&frequency)) { /* 获取计时器的频率(每秒的计数) */
        if (QueryPerformanceCounter(&now)) {     /* 获取当前计数 */
            now_ms = (now.QuadPart * 1000) / frequency.QuadPart;
        }
    }

#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    now_ms = ((uint64_t)tp.tv_sec) * 1000LL + (tp.tv_nsec / 1000000);
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
    uint64_t  user_define; /* if > 0, TMR_USER_FLAG can be used */
    uint64_t  last_ms;
    struct tm last_tm;
    struct tm curr_tm;
};

/**
 * @param set_all_flag 初次运行时是否设置所有标识为真
 * @param user_define  用户自定义的定时器周期, 单位ms
 */
UtilTimer timer_new(bool set_all_flag, uint64_t user_define)
{
    UtilTimer tmr = (UtilTimer)malloc(sizeof(struct _Timer_t_));
    if (tmr != NULL) {
        if (set_all_flag)
            memset(&tmr->last_tm, 0xff, sizeof(struct tm)); /* 首次所有标识将置1 */
        else {
            time_t now_sec = time(NULL);
            LOCAL_TIME(&now_sec, &tmr->last_tm);
        }

        if (user_define > 0) {
            tmr->user_define = user_define;
            tmr->last_ms     = cpu_ms();
        }
    }

    return tmr;
}


void timer_del(UtilTimer self)
{
    if (self != NULL) {
        free(self);
        self = NULL;
    }
}


void timer_user_define(UtilTimer self, uint64_t user_define)
{
    self->user_define = user_define;
    self->last_ms     = cpu_ms();
}


/**
 * @brief   call this function in the start of loop
 */
void timer_running(UtilTimer self)
{
    self->flag = 0; /* clear timer flag */

    time_t now_sec = time(NULL);
    LOCAL_TIME(&now_sec, &self->curr_tm);

    if (self->user_define > 0) {
        uint64_t now_ms = cpu_ms();
        if (now_ms - self->last_ms > self->user_define) {
            self->last_ms = now_ms;
            self->flag |= TMR_USER_FLAG;
        }
    }
    if (self->curr_tm.tm_sec != self->last_tm.tm_sec) {
        self->last_tm.tm_sec = self->curr_tm.tm_sec;
        self->flag |= TMR_SEC_FLAG;
    }
    if (self->curr_tm.tm_min != self->last_tm.tm_min) {
        self->last_tm.tm_min = self->curr_tm.tm_min;
        self->flag |= TMR_MIN_FLAG;
    }
    if (self->curr_tm.tm_hour != self->last_tm.tm_hour) {
        self->last_tm.tm_hour = self->curr_tm.tm_hour;
        self->flag |= TMR_HOUR_FLAG;
    }
    if (self->curr_tm.tm_mday != self->last_tm.tm_mday) {
        self->last_tm.tm_mday = self->curr_tm.tm_mday;
        self->flag |= TMR_DAY_FLAG;
    }
    if (self->curr_tm.tm_mon != self->last_tm.tm_mon) {
        self->last_tm.tm_mon = self->curr_tm.tm_mon;
        self->flag |= TMR_MON_FLAG;
    }
    if (self->curr_tm.tm_year != self->last_tm.tm_year) {
        self->last_tm.tm_year = self->curr_tm.tm_year;
        self->flag |= TMR_YEAR_FLAG;
    }
    if (self->curr_tm.tm_min != self->last_tm.tm_min) {
        self->last_tm.tm_min = self->curr_tm.tm_min;
        self->flag |= TMR_MIN_FLAG;
    }

    return;
}


bool past_user_define(UtilTimer self)
{
    return self->flag & TMR_USER_FLAG;
}


bool past_second(UtilTimer self)
{
    return self->flag & TMR_SEC_FLAG;
}


bool past_minute(UtilTimer self)
{
    return self->flag & TMR_MIN_FLAG;
}


bool past_hour(UtilTimer self)
{
    return self->flag & TMR_HOUR_FLAG;
}


bool past_day(UtilTimer self)
{
    return self->flag & TMR_DAY_FLAG;
}


bool past_month(UtilTimer self)
{
    return self->flag & TMR_MON_FLAG;
}


bool past_week(UtilTimer self)
{
    return self->flag & TMR_WEEK_FLAG;
}


bool past_year(UtilTimer self)
{
    return self->flag & TMR_YEAR_FLAG;
}

/**
 * @brief   获取当前年份
 */
int now_year(UtilTimer self)
{
    return self->curr_tm.tm_year + 1900;
}

/**
 * @brief   获取当前月份
 * @retval  1-12
 */
int now_month(UtilTimer self)
{
    return self->curr_tm.tm_mon + 1;
}

/**
 * @brief   获取当前日期
 * @retval  1-31
 */
int now_day(UtilTimer self)
{
    return self->curr_tm.tm_mday;
}

/**
 * @brief   获取当前小时
 * @retval  0-23
 */
int now_hour(UtilTimer self)
{
    return self->curr_tm.tm_hour;
}

/**
 * @brief   获取当前分钟
 * @retval  0-59
 */
int now_minute(UtilTimer self)
{
    return self->curr_tm.tm_min;
}

/**
 * @brief   获取当前秒
 * @retval  0-59
 */
int now_second(UtilTimer self)
{
    return self->curr_tm.tm_sec;
}

/**
 * @brief   获取当前星期
 * @retval  1-7
 */
int now_weekday(UtilTimer self)
{
    return self->curr_tm.tm_wday + 1;
}
