#ifndef UCL_SLOG_H
#define UCL_SLOG_H

#include <stdint.h>
#include <stdio.h>

#include "config_.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _E_SlogLevel {
    S_TRACE = 1,
    S_DEBUG = 2,
    S_INFO  = 3,
    S_WARN  = 4,
    S_ERROR = 5
} SlogLevel;


/* ע�⣺Ŀ¼Ŀǰֻ���Զ��½�һ���ļ��� */
int slogInit_(const char* log_dir, const char* file_name, SlogLevel level);
/* ע�⣺�ӿ�һ�������������󳤶�ԼΪ10KB���������Ȼᷢ���ضϡ�ͨ��MAX_LOG_LINE����޸���󳤶� */
void writeLog_(SlogLevel level, int braw, const char* func_name, int line, const char* fmt, ...);

#if (UCL_USE_SLOG == 1)
#    define SLOG_ERROR(fmt, ...) writeLog_(S_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_WARN(fmt, ...)  writeLog_(S_WARN,  0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_INFO(fmt, ...)  writeLog_(S_INFO,  0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_DEBUG(fmt, ...) writeLog_(S_DEBUG, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_TRACE(fmt, ...) writeLog_(S_TRACE, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_ERROR_RAW(fmt, ...) writeLog_(S_ERROR, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_WARN_RAW(fmt, ...)  writeLog_(S_WARN,  1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_INFO_RAW(fmt, ...)  writeLog_(S_INFO,  1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_DEBUG_RAW(fmt, ...) writeLog_(S_DEBUG, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#    define SLOG_TRACE_RAW(fmt, ...) writeLog_(S_TRACE, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#else
#    define SLOG_ERROR     printf
#    define SLOG_WARN      printf
#    define SLOG_INFO      printf
#    define SLOG_DEBUG     printf
#    define SLOG_TRACE     printf
#    define SLOG_ERROR_RAW printf
#    define SLOG_WARN_RAW  printf
#    define SLOG_INFO_RAW  printf
#    define SLOG_DEBUG_RAW printf
#    define SLOG_TRACE_RAW printf
#endif

/* demo
 * slogInit_("./log", "slog.log", S_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 * SLOG_WARN("SLOG WARN TEST");
 */


#ifdef __cplusplus
}
#endif

#endif /* UCL_SLOG_H */