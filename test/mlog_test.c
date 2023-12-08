
#include "mlog_.h"

int mlog_test()
{
    slogInit_("../../../log/test/test", "mlog.log", M_TRACE);

    SLOG_ERROR("SLOG ERROR TEST");
    SLOG_WARN("SLOG WARN TEST");
    SLOG_INFO("SLOG INFO TEST");
    SLOG_DEBUG("SLOG DEBUG TEST");
    SLOG_TRACE("SLOG TRACE TEST");
    SLOG_WARN_RAW("SLOG WARN RAW TEST\n");
    SLOG_ERROR_RAW("SLOG ERROR RAW TEST\n");
    SLOG_INFO_RAW("SLOG INFO RAW TEST\n");
    SLOG_DEBUG_RAW("SLOG DEBUG RAW TEST\n");
    SLOG_TRACE_RAW("SLOG TRACE RAW TEST\n");

    mlogInit_(1, "../../../log/test/test", "mlog1.log", M_TRACE);
    mlogInit_(2, "../../../log/test/test", "mlog2.log", M_TRACE);
    MLOG_ERROR(1, "MLOG ERROR TEST 111");
    MLOG_WARN(1, "MLOG WARN TEST 111");
    MLOG_INFO(1, "MLOG INFO TEST 111");
    MLOG_DEBUG(1, "MLOG DEBUG TEST 111");
    MLOG_TRACE(1, "MLOG TRACE TEST 111");
    MLOG_ERROR_RAW(1, "MLOG ERROR RAW TEST 111\n");
    MLOG_WARN_RAW(1, "MLOG WARN RAW TEST 111\n");
    MLOG_INFO_RAW(1, "MLOG INFO RAW TEST 111\n");
    MLOG_DEBUG_RAW(1, "MLOG DEBUG RAW TEST 111\n");
    MLOG_TRACE_RAW(1, "MLOG TRACE RAW TEST 111\n");

    MLOG_ERROR(2, "MLOG ERROR TEST 222");
    MLOG_WARN(2, "MLOG WARN TEST 222");
    MLOG_INFO(2, "MLOG INFO TEST 222");
    MLOG_DEBUG(2, "MLOG DEBUG TEST 222");
    MLOG_TRACE(2, "MLOG TRACE TEST 222");
    MLOG_ERROR_RAW(2, "MLOG ERROR RAW TEST 222\n");
    MLOG_WARN_RAW(2, "MLOG WARN RAW TEST 222\n");
    MLOG_INFO_RAW(2, "MLOG INFO RAW TEST 222\n");
    MLOG_DEBUG_RAW(2, "MLOG DEBUG RAW TEST 222\n");
    MLOG_TRACE_RAW(2, "MLOG TRACE RAW TEST 222\n");

    return 0;
}