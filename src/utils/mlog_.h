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


/**
 * @brief   日志初始化
 * @return  0: success, -1: failed
 * @note    1.本函数非线程安全
 *          2.可以重复调用来更改日志级别
 */
int mlog_init(int log_id, int log_level, const char* file_dir, const char* file_name);

int  mlog_set_level(int log_id, int log_level);
void mlog_set_print_color(int log_id, bool enable);
void mlog_set_print_console(int log_id, bool enable);

void mlog_write(int         log_id,
                int         log_level,
                bool        is_raw,
                const char* szfunc,
                int         line,
                const char* fmt,
                ...);

int print_buf(int log_level, uint8_t* buf, uint16_t buf_len);
int print_app_info(const char* name,
                   const char* version,
                   const char* date,
                   const char* time);

#define SLOG_INIT(level, dir, file)    mlog_init(0, level, dir, file)
#define SLOG_SET_LEVEL(level)          mlog_set_level(0, level)
#define SLOG_SET_PRINT_COLOR(enable)   mlog_set_print_color(0, enable)
#define SLOG_SET_PRINT_CONSOLE(enable) mlog_set_print_console(0, enable)
#define PRINT_APP_INFO(name, version)  print_app_info(name, version, __DATE__, __TIME__)


/*!
 * Multiton mode:
 * mlogInit_(0, "./log", "mlog1.log", M_TRACE);
 * mlogInit_(1, "./log", "mlog2.log", M_DEBUG);
 * MLOG_ERROR(0, "MLOG ERROR TEST");
 * MLOG_WARN(1, "MLOG WARN TEST");
 */
#define MLOG(log_id, level, fmt, ...) \
    mlog_write(log_id, level, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE(log_id, fmt, ...) MLOG(M_TRACE, log_id, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG(log_id, fmt, ...) MLOG(M_DEBUG, log_id, fmt, ##__VA_ARGS__)
#define MLOG_INFO(log_id, fmt, ...)  MLOG(M_INFO, log_id, fmt, ##__VA_ARGS__)
#define MLOG_WARN(log_id, fmt, ...)  MLOG(M_WARN, log_id, fmt, ##__VA_ARGS__)
#define MLOG_ERROR(log_id, fmt, ...) MLOG(M_ERROR, log_id, fmt, ##__VA_ARGS__)

#define MLOG_RAW(level, log_id, fmt, ...) \
    mlog_write(log_id, level, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE_RAW(log_id, fmt, ...) MLOG_RAW(M_TRACE, log_id, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG_RAW(log_id, fmt, ...) MLOG_RAW(M_DEBUG, log_id, fmt, ##__VA_ARGS__)
#define MLOG_INFO_RAW(log_id, fmt, ...)  MLOG_RAW(M_INFO, log_id, fmt, ##__VA_ARGS__)
#define MLOG_WARN_RAW(log_id, fmt, ...)  MLOG_RAW(M_WARN, log_id, fmt, ##__VA_ARGS__)
#define MLOG_ERROR_RAW(log_id, fmt, ...) MLOG_RAW(M_ERROR, log_id, fmt, ##__VA_ARGS__)

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
