
#include "utils_log.h"
#include "utils_lib.h"
#include "utils_time.h"


int byte2int_test()
{
    slog_info("--- byte2int test start ---\n");

    uint8_t buf[] = {0x12, 0x34, 0x56, 0x78};
    slog_debug("input:");
    print_buf(M_DEBUG, buf, sizeof(buf));

    int n = (int)byte2int(buf, NULL, 4, UCL_BIG_ENDIAN);
    slog_debug("big endien:    n = 0x%08x", n);

    n = (int)byte2int(buf, NULL, 4, UCL_LITTLE_ENDIAN);
    slog_debug("little endien: n = 0x%08x", n);

    slog_info("--- byte2int test done ---\n");

    return 0;
}


int memcpy_r_test()
{
    slog_info("--- memcpy_r test start ---\n");

    uint8_t src[256];
    uint8_t dst[256];
    for (int i = 0; i < sizeof(src); i++)
        src[i] = i;

    uint64_t start = time_ms();
    for (int i = 0; i < 100000; i++)
        memcpy_r(dst, src, sizeof(src));

    slog_debug("memcpy_r: %lld ms", time_ms() - start);


    slog_info("--- memcpy_r test done ---\n");

    return 0;
}


int mem_swap_test()
{
    slog_info("--- mem_swap test start ---\n");

    uint8_t a[256];
    uint8_t b[256];
    for (int i = 0; i < sizeof(a); i++) {
        a[i] = i;
        b[i] = 0xff - i;
    }

    uint64_t start = time_ms();
    for (int i = 0; i < 10000; i++)
        mem_swap(a, b, sizeof(a));
    slog_debug("mem_swap: %lld ms", time_ms() - start);

    return 0;
}


int mem_rev_test()
{
    slog_info("--- mem_rev test start ---\n");

    uint8_t buf[256];
    for (int i = 0; i < sizeof(buf); i++)
        buf[i] = i;

    uint64_t start = time_ms();
    for (int i = 0; i < 100000; i++)
        mem_rev(buf, sizeof(buf));
    slog_debug("mem_rev: %lld ms", time_ms() - start);

    slog_info("--- mem_rev test done ---\n");

    return 0;
}


int lib_test()
{
    slog_info("-- lib test start --\n");

    byte2int_test();
    memcpy_r_test();
    mem_swap_test();
    mem_rev_test();

    slog_info("-- lib test done --\n");

    return 0;
}
