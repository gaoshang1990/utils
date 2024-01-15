#ifndef _UCL_MLOG_H_
#define _UCL_MLOG_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define M_TRACE (0)
#define M_DEBUG (1)
#define M_INFO  (2)
#define M_WARN  (3)
#define M_ERROR (4)


int  mlog_init(int level, int log_id, const char* file_dir, const char* file_name);
int  mlog_set_level(int log_no, int level);
void mlog_write(int level, int log_id, bool is_raw, const char* szfunc, int line, const char* fmt, ...);
int  print_buf(int logLevel, uint8_t* pBuf, uint16_t bufLen);
int  print_app_info(const char* name, const char* version, const char* date, const char* time);

#define SLOG_INIT(level, dir, file)       mlog_init(level, 0, dir, file)
#define SLOG_SET_LEVEL(level)             mlog_set_level(level, 0)
#define PRINT_APP_INFO(name, version)     print_app_info(name, version, __DATE__, __TIME__)


/*!
 * Multiton mode:
 * mlogInit_(0, "./log", "mlog1.log", M_TRACE);
 * mlogInit_(1, "./log", "mlog2.log", M_DEBUG);
 * MLOG_ERROR(0, "MLOG ERROR TEST");
 * MLOG_WARN(1, "MLOG WARN TEST");
 */
#define MLOG(level, log_id, fmt, ...)     mlog_write(level, log_id, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE(log_id, fmt, ...)      MLOG(M_TRACE, log_id, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG(log_id, fmt, ...)      MLOG(M_DEBUG, log_id, fmt, ##__VA_ARGS__)
#define MLOG_INFO(log_id, fmt, ...)       MLOG(M_INFO, log_id, fmt, ##__VA_ARGS__)
#define MLOG_WARN(log_id, fmt, ...)       MLOG(M_WARN, log_id, fmt, ##__VA_ARGS__)
#define MLOG_ERROR(log_id, fmt, ...)      MLOG(M_ERROR, log_id, fmt, ##__VA_ARGS__)

#define MLOG_RAW(level, log_id, fmt, ...) mlog_write(level, log_id, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE_RAW(log_id, fmt, ...)  MLOG_RAW(M_TRACE, log_id, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG_RAW(log_id, fmt, ...)  MLOG_RAW(M_DEBUG, log_id, fmt, ##__VA_ARGS__)
#define MLOG_INFO_RAW(log_id, fmt, ...)   MLOG_RAW(M_INFO, log_id, fmt, ##__VA_ARGS__)
#define MLOG_WARN_RAW(log_id, fmt, ...)   MLOG_RAW(M_WARN, log_id, fmt, ##__VA_ARGS__)
#define MLOG_ERROR_RAW(log_id, fmt, ...)  MLOG_RAW(M_ERROR, log_id, fmt, ##__VA_ARGS__)

/*!
 * singleton mode:
 * slogInit_("./log", "mlog.log", M_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 */
#define SLOG(level, fmt, ...)             MLOG(level, 0, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...)              SLOG(M_TRACE, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...)              SLOG(M_DEBUG, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)               SLOG(M_INFO, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)               SLOG(M_WARN, fmt, ##__VA_ARGS__)
#define SLOG_ERROR(fmt, ...)              SLOG(M_ERROR, fmt, ##__VA_ARGS__)

#define SLOG_RAW(level, fmt, ...)         MLOG_RAW(level, 0, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...)          SLOG_RAW(M_TRACE, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...)          SLOG_RAW(M_DEBUG, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)           SLOG_RAW(M_INFO, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)           SLOG_RAW(M_WARN, fmt, ##__VA_ARGS__)
#define SLOG_ERROR_RAW(fmt, ...)          SLOG_RAW(M_ERROR, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* _UCL_MLOG_H_ */