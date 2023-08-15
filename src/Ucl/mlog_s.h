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
void mlogWrite_(int logNo, MLogLevel_t level, bool braw, const char* szFunc, int line, const char* fmt, ...);

/*!
 * Multi instance mode:
 * mlogInit_(0, "./log", "mlog1.log", M_TRACE);
 * mlogInit_(1, "./log", "mlog2.log", M_DEBUG);
 * MLOG_ERROR(0, "MLOG ERROR TEST");
 * MLOG_WARN(1, "MLOG WARN TEST");
 */
#define MLOG_ERROR(logNo, fmt, ...)     mlogWrite_(logNo, M_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_WARN(logNo, fmt, ...)      mlogWrite_(logNo, M_WARN, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_INFO(logNo, fmt, ...)      mlogWrite_(logNo, M_INFO, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG(logNo, fmt, ...)     mlogWrite_(logNo, M_DEBUG, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE(logNo, fmt, ...)     mlogWrite_(logNo, M_TRACE, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_ERROR_RAW(logNo, fmt, ...) mlogWrite_(logNo, M_ERROR, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_WARN_RAW(logNo, fmt, ...)  mlogWrite_(logNo, M_WARN, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_INFO_RAW(logNo, fmt, ...)  mlogWrite_(logNo, M_INFO, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_DEBUG_RAW(logNo, fmt, ...) mlogWrite_(logNo, M_DEBUG, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MLOG_TRACE_RAW(logNo, fmt, ...) mlogWrite_(logNo, M_TRACE, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

/*!
 * single instance mode:
 * slogInit_("./log", "mlog.log", M_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 */
#define SLOG_ERROR(fmt, ...)            mlogWrite_(0, M_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)             mlogWrite_(0, M_WARN, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)             mlogWrite_(0, M_INFO, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...)            mlogWrite_(0, M_DEBUG, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...)            mlogWrite_(0, M_TRACE, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_ERROR_RAW(fmt, ...)        mlogWrite_(0, M_ERROR, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)         mlogWrite_(0, M_WARN, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)         mlogWrite_(0, M_INFO, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...)        mlogWrite_(0, M_DEBUG, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...)        mlogWrite_(0, M_TRACE, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* _UCL_MLOG_H_ */