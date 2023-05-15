#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#    include <windows.h>
#else
#    include <sys/time.h>
#    include <unistd.h>
#endif

#include "time_.h"
// #include "math_.h"


const uint8_t g_monthTab[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


/* ��ת��Ϊʱ��ṹ�� */
struct tm second2tmStruct_(time_t sec)
{
    struct tm tms;
    memcpy(&tms, localtime(&sec), sizeof(struct tm ));

    return tms;
}


time_t timestr2second_(const char* str)
{
    if(str == NULL) {
        return 0;
    }

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
    cur_tm.tm_mon  -= 1;

    return mktime(&cur_tm);
}


int second2timestr_(char* timeStr, time_t sec)
{
    if(timeStr == NULL) {
        return -1;
    }
    struct tm cur_tm;

    memset(timeStr, 0, TIME_STR_LEN + 1);
    LOCAL_TIME(&sec, &cur_tm);

    sprintf(timeStr,
            "%04d-%02d-%02d %02d:%02d:%02d",
            cur_tm.tm_year + 1900,
            cur_tm.tm_mon + 1,
            cur_tm.tm_mday,
            cur_tm.tm_hour,
            cur_tm.tm_min,
            cur_tm.tm_sec);

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
 * \brief   ��ķ����ɭ�������ڹ�ʽ
 * \retval  0-6��������~��
 */
int getWeekDay_(int year, int month, int day)
{
    if (month == 1 || month == 2)
    {
        month += 12;
        year--;
    }

    return (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
}

/** 
 * \brief   �ж�һ��ʱ��ĺϷ��ԣ�ע��ü������Ƿ����ڼ��
 * \retval  0-��ȷ��-1-����
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

    /* ==0xffff ���ж� */
    if ((pdate->tm_wday != 0xffff) &&
        (((pdate->tm_wday > 0) ? (pdate->tm_wday - 1) : 6) != getWeekDay_(year, month, day)))
        return 0;

    c[1] = 28 + IS_LEAP_YEAR(year);
    if (day <= c[month - 1])
        return 0;
    else
        return -1;
}


/* ��UTC1970-1-1 0:0:0��ʼ�ĺ����� */
uint64_t timeMs_(void)
{
    uint64_t ret = 0;

#ifdef _WIN32
    FILETIME ft;
    uint64_t now;
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


/* ��ϵͳ������ʼ�ĺ����� */
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


#define TMR_SEC_FLAG  0x01 /* �����־ */
#define TMR_MIN_FLAG  0x02 /* ���ֱ�־ */
#define TMR_HOUR_FLAG 0x04 /* ��ʱ��־ */
#define TMR_DAY_FLAG  0x08 /* ���ձ�־ */
#define TMR_MON_FLAG  0x10 /* ���±�־ */
#define TMR_WEEK_FLAG 0x20 /* �����ڱ�־ */
#define TMR_YEAR_FLAG 0x40 /* �����־ */

static uint8_t   s_timerFlag; /* ʱ���ʶ: bit0-���� 1-���� 2-��ʱ 3-���� 4-���� 5-������ 6-���� */
static struct tm s_lastTm;
static struct tm s_nowTm;
static time_t    s_nowSec = 0;
static uint64_t  s_nowMs  = 0;


/* ÿ�δ�ѭ����ʼ�������ã�����ʱ���ʶ�Ĺ��� */
int timerRunning_(void)
{
    /* ˢ�µ�ǰʱ�� */
    s_nowMs  = cpuMs_();
    s_nowSec = time(NULL);
    LOCAL_TIME(&s_nowSec, &s_nowTm);
    if (s_lastTm.tm_year == 0) { /* ��һ�����е��� */
        s_lastTm = s_nowTm;
    }

    s_timerFlag = 0; /* ��ʱ���ʶ */

    if (s_nowTm.tm_sec != s_lastTm.tm_sec) { /* ���� */
        s_lastTm.tm_sec = s_nowTm.tm_sec;
        s_timerFlag |= TMR_SEC_FLAG;
    }
    if (s_nowTm.tm_min != s_lastTm.tm_min) { /* ���� */
        s_lastTm.tm_min = s_nowTm.tm_min;
        s_timerFlag |= TMR_MIN_FLAG;
    }
    if (s_nowTm.tm_hour != s_lastTm.tm_hour) { /* ��ʱ */
        s_lastTm.tm_hour = s_nowTm.tm_hour;
        s_timerFlag |= TMR_HOUR_FLAG;
    }
    if (s_nowTm.tm_mday != s_lastTm.tm_mday) { /* ���� */
        s_lastTm.tm_mday = s_nowTm.tm_mday;
        s_timerFlag |= TMR_DAY_FLAG;
    }
    if (s_nowTm.tm_mon != s_lastTm.tm_mon) { /* ���� */
        s_lastTm.tm_mon = s_nowTm.tm_mon;
        s_timerFlag |= TMR_MON_FLAG;
    }
    if (s_nowTm.tm_wday != s_lastTm.tm_wday) { /* ������ */
        s_lastTm.tm_wday = s_nowTm.tm_wday;
        s_timerFlag |= TMR_WEEK_FLAG;
    }
    if (s_nowTm.tm_year != s_lastTm.tm_year) { /* ���� */
        s_lastTm.tm_year = s_nowTm.tm_year;
        s_timerFlag |= TMR_YEAR_FLAG;
    }

    return 0;
}


bool pastSecond_(void)
{
    return s_timerFlag & TMR_SEC_FLAG;
}


bool pastMinute_(void)
{
    return s_timerFlag & TMR_MIN_FLAG;
}


bool pastHour_(void)
{
    return s_timerFlag & TMR_HOUR_FLAG;
}


bool pastDay_(void)
{
    return s_timerFlag & TMR_DAY_FLAG;
}


bool pastMonth_(void)
{
    return s_timerFlag & TMR_MON_FLAG;
}


bool pastWeek_(void)
{
    return s_timerFlag & TMR_WEEK_FLAG;
}


bool pastYear_(void)
{
    return s_timerFlag & TMR_YEAR_FLAG;
}



