
#include "utils_log.h"

int mlog_test()
{
    slog_info("- mlog test start -\n");

    slog_init(M_TRACE, "./test/log", "mlog.log");

    slog_error("slog ERROR TEST");
    slog_warn("slog WARN TEST");
    slog_info("slog INFO TEST");
    slog_debug("slog DEBUG TEST");
    slog_trace("slog TRACE TEST");
    slog_warn_raw("slog WARN RAW TEST\n");
    slog_error_raw("slog ERROR RAW TEST\n");
    slog_info_raw("slog INFO RAW TEST\n");
    slog_debug_raw("slog DEBUG RAW TEST\n");
    slog_trace_raw("slog TRACE RAW TEST\n");

    slog_set_level(M_INFO);
    slog_error("slog ERROR TEST");
    slog_debug("slog DEBUG TEST");
    slog_trace("slog TRACE TEST");

    mlog_init(1, M_TRACE, "./test/log", "mlog1.log");
    mlog_init(2, M_TRACE, "./test/log", "mlog2.log");
    mlog_error(1, "mlog ERROR TEST 111");
    mlog_warn(1, "mlog WARN TEST 111");
    mlog_info(1, "mlog INFO TEST 111");
    mlog_debug(1, "mlog DEBUG TEST 111");
    mlog_trace(1, "mlog TRACE TEST 111");
    mlog_error_raw(1, "mlog ERROR RAW TEST 111\n");
    mlog_warn_raw(1, "mlog WARN RAW TEST 111\n");
    mlog_info_raw(1, "mlog INFO RAW TEST 111\n");
    mlog_debug_raw(1, "mlog DEBUG RAW TEST 111\n");
    mlog_trace_raw(1, "mlog TRACE RAW TEST 111\n");

    mlog_error(2, "mlog ERROR TEST 222");
    mlog_warn(2, "mlog WARN TEST 222");
    mlog_info(2, "mlog INFO TEST 222");
    mlog_debug(2, "mlog DEBUG TEST 222");
    mlog_trace(2, "mlog TRACE TEST 222");
    mlog_error_raw(2, "mlog ERROR RAW TEST 222\n");
    mlog_warn_raw(2, "mlog WARN RAW TEST 222\n");
    mlog_info_raw(2, "mlog INFO RAW TEST 222\n");
    mlog_debug_raw(2, "mlog DEBUG RAW TEST 222\n");
    mlog_trace_raw(2, "mlog TRACE RAW TEST 222\n");

    slog_info("- mlog test done -\n");

    return 0;
}
