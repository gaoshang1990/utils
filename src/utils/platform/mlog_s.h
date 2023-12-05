#ifndef _UCL_MLOG_H_
#define _UCL_MLOG_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum _E_MlogLevel { M_TRACE = 0, M_DEBUG, M_INFO, M_WARN, M_ERROR } MLogLevel_t;


int  mlogInit_(int logNo, const char* logDir, const char* fileName, MLogLevel_t level);
int  slogInit_(const char* logDir, const char* fileName, MLogLevel_t level);
int  mlog_set_level(int log_no, int level);
int  slog_set_level(int level);
void mlogWrite_(int logNo, int level, bool isRaw, const char* szFunc, int line, const char* fmt, ...);
int  printBuffer_(int logLevel, uint8_t* pBuf, uint16_t bufLen);
int  printAppInfo_(const char* szName, const char* szVersion, const char* szDate, const char* szTime);

#define PRINT_APP_INFO(name, version)    printAppInfo_(name, version, __DATE__, __TIME__)

/*!
 * Multiton mode:
 * mlogInit_(0, "./log", "mlog1.log", M_TRACE);
 * mlogInit_(1, "./log", "mlog2.log", M_DEBUG);
 * MLOG_ERROR(0, "MLOG ERROR TEST");
 * MLOG_WARN(1, "MLOG WARN TEST");
 */
#define MLOG(logNo, level, fmt, ...)     mlogWrite_(logNo, level, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE(logNo, fmt, ...)      MLOG(logNo, M_TRACE, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG(logNo, fmt, ...)      MLOG(logNo, M_DEBUG, fmt, ##__VA_ARGS__)
#define MLOG_INFO(logNo, fmt, ...)       MLOG(logNo, M_INFO, fmt, ##__VA_ARGS__)
#define MLOG_WARN(logNo, fmt, ...)       MLOG(logNo, M_WARN, fmt, ##__VA_ARGS__)
#define MLOG_ERROR(logNo, fmt, ...)      MLOG(logNo, M_ERROR, fmt, ##__VA_ARGS__)

#define MLOG_RAW(logNo, level, fmt, ...) mlogWrite_(logNo, level, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE_RAW(logNo, fmt, ...)  MLOG_RAW(logNo, M_TRACE, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG_RAW(logNo, fmt, ...)  MLOG_RAW(logNo, M_DEBUG, fmt, ##__VA_ARGS__)
#define MLOG_INFO_RAW(logNo, fmt, ...)   MLOG_RAW(logNo, M_INFO, fmt, ##__VA_ARGS__)
#define MLOG_WARN_RAW(logNo, fmt, ...)   MLOG_RAW(logNo, M_WARN, fmt, ##__VA_ARGS__)
#define MLOG_ERROR_RAW(logNo, fmt, ...)  MLOG_RAW(logNo, M_ERROR, fmt, ##__VA_ARGS__)

/*!
 * singleton mode:
 * slogInit_("./log", "mlog.log", M_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 */
#define SLOG(level, fmt, ...)            MLOG(0, level, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...)             SLOG(M_TRACE, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...)             SLOG(M_DEBUG, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)              SLOG(M_INFO, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)              SLOG(M_WARN, fmt, ##__VA_ARGS__)
#define SLOG_ERROR(fmt, ...)             SLOG(M_ERROR, fmt, ##__VA_ARGS__)

#define SLOG_RAW(level, fmt, ...)        MLOG_RAW(0, level, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...)         SLOG_RAW(M_TRACE, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...)         SLOG_RAW(M_DEBUG, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)          SLOG_RAW(M_INFO, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)          SLOG_RAW(M_WARN, fmt, ##__VA_ARGS__)
#define SLOG_ERROR_RAW(fmt, ...)         SLOG_RAW(M_ERROR, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* _UCL_MLOG_H_ */