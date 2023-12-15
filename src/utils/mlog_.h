#ifndef _UCL_MLOG_H_
#define _UCL_MLOG_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum { M_TRACE = 0, M_DEBUG, M_INFO, M_WARN, M_ERROR } E_MLOG_LEVEL;


int  mlog_init(E_MLOG_LEVEL level, int logNo, const char* logDir, const char* fileName);
int  mlog_set_level(int log_no, int level);
void mlog_write(int level, int logNo, bool isRaw, const char* szFunc, int line, const char* fmt, ...);
int  print_buf(int logLevel, uint8_t* pBuf, uint16_t bufLen);
int  print_app_info(const char* szName, const char* szVersion, const char* szDate, const char* szTime);

#define SLOG_INIT(level, dir, file)      mlog_init(0, level, dir, file)
#define SLOG_SET_LEVEL(level)            mlog_set_level(0, level)
#define PRINT_APP_INFO(name, version)    print_app_info(name, version, __DATE__, __TIME__)


/*!
 * Multiton mode:
 * mlogInit_(0, "./log", "mlog1.log", M_TRACE);
 * mlogInit_(1, "./log", "mlog2.log", M_DEBUG);
 * MLOG_ERROR(0, "MLOG ERROR TEST");
 * MLOG_WARN(1, "MLOG WARN TEST");
 */
#define MLOG(level, logNo, fmt, ...)     mlog_write(level, logNo, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE(logNo, fmt, ...)      MLOG(M_TRACE, logNo, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG(logNo, fmt, ...)      MLOG(M_DEBUG, logNo, fmt, ##__VA_ARGS__)
#define MLOG_INFO(logNo, fmt, ...)       MLOG(M_INFO, logNo, fmt, ##__VA_ARGS__)
#define MLOG_WARN(logNo, fmt, ...)       MLOG(M_WARN, logNo, fmt, ##__VA_ARGS__)
#define MLOG_ERROR(logNo, fmt, ...)      MLOG(M_ERROR, logNo, fmt, ##__VA_ARGS__)

#define MLOG_RAW(level, logNo, fmt, ...) mlog_write(level, logNo, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE_RAW(logNo, fmt, ...)  MLOG_RAW(M_TRACE, logNo, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG_RAW(logNo, fmt, ...)  MLOG_RAW(M_DEBUG, logNo, fmt, ##__VA_ARGS__)
#define MLOG_INFO_RAW(logNo, fmt, ...)   MLOG_RAW(M_INFO, logNo, fmt, ##__VA_ARGS__)
#define MLOG_WARN_RAW(logNo, fmt, ...)   MLOG_RAW(M_WARN, logNo, fmt, ##__VA_ARGS__)
#define MLOG_ERROR_RAW(logNo, fmt, ...)  MLOG_RAW(M_ERROR, logNo, fmt, ##__VA_ARGS__)

/*!
 * singleton mode:
 * slogInit_("./log", "mlog.log", M_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 */
#define SLOG(level, fmt, ...)            MLOG(level, 0, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...)             SLOG(M_TRACE, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...)             SLOG(M_DEBUG, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)              SLOG(M_INFO, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)              SLOG(M_WARN, fmt, ##__VA_ARGS__)
#define SLOG_ERROR(fmt, ...)             SLOG(M_ERROR, fmt, ##__VA_ARGS__)

#define SLOG_RAW(level, fmt, ...)        MLOG_RAW(level, 0, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...)         SLOG_RAW(M_TRACE, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...)         SLOG_RAW(M_DEBUG, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)          SLOG_RAW(M_INFO, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)          SLOG_RAW(M_WARN, fmt, ##__VA_ARGS__)
#define SLOG_ERROR_RAW(fmt, ...)         SLOG_RAW(M_ERROR, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* _UCL_MLOG_H_ */