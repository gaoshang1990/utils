#ifndef UCL_SLOG_H
#define UCL_SLOG_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum _E_SlogLevel { S_TRACE = 0, S_DEBUG, S_INFO, S_WARN, S_ERROR } SlogLevel;


int slogInit__(const char* log_dir, const char* file_name, SlogLevel level);
/* 注意：接口一次输出的内容最大长度约为10KB，超出长度会发生截断。通过MAX_LOG_LINE宏可修改最大长度 */
void slogWrite_(SlogLevel level, bool braw, const char* func_name, int line, const char* fmt, ...);


#define SLOG_ERROR(fmt, ...)     slogWrite_(S_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)      slogWrite_(S_WARN, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)      slogWrite_(S_INFO, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...)     slogWrite_(S_DEBUG, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...)     slogWrite_(S_TRACE, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_ERROR_RAW(fmt, ...) slogWrite_(S_ERROR, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)  slogWrite_(S_WARN, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)  slogWrite_(S_INFO, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...) slogWrite_(S_DEBUG, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...) slogWrite_(S_TRACE, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


/* demo
 * slogInit_("./log", "slog.log", S_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 * SLOG_WARN("SLOG WARN TEST");
 */


#ifdef __cplusplus
}
#endif

#endif /* UCL_SLOG_H */