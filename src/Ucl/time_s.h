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

extern const uint8_t _monthTab[];

extern time_t    timestr2second_(const char* str);
extern int       second2timestr_(char* timestr, time_t sec);
extern struct tm second2struct_(time_t sec);
extern int       getWeekDay_(int year, int month, int day);
extern int       checkTime_(struct tm* pdate);
extern int       delayMs_(int ms);
extern uint64_t  timeMs_(void);
extern uint64_t  cpuMs_(void);

/* timer demo:
    Timer_t timer = timerInit_(100);
    while (1) {
        timerRunning_(timer);

        if (pastSecond_(timer))
            printf("past second\n");

        if (pastSettedMs_(timer))
            printf("past 100ms\n");

        delayMs_(10);
    }
 */

/**
 * \param   settedMs: unit: ms, if > 0, TMR_USER_FLAG can be used
 */
Timer_t timerInit_(uint64_t settedMs);

/**
 * \brief   call this function in the start of loop
 */
int     timerRunning_(Timer_t timer);

bool    pastSettedMs_(Timer_t timer);
bool    pastSecond_(Timer_t timer);
bool    pastMinute_(Timer_t timer);
bool    pastHour_(Timer_t timer);
bool    pastDay_(Timer_t timer);
bool    pastWeek_(Timer_t timer);
bool    pastMonth_(Timer_t timer);
bool    pastYear_(Timer_t timer);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_TIME_H_ */