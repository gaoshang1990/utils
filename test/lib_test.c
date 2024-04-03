
#include "mlog_.h"
#include "lib_.h"
#include "time_.h"


int byte2int_test()
{
    SLOG_INFO("--- byte2int test start ---\n");

    uint8_t buf[] = {0x12, 0x34, 0x56, 0x78};
    SLOG_DEBUG("input:");
    print_buf(M_DEBUG, buf, sizeof(buf));

    int n = (int)byte2int(buf, NULL, 4, UCL_BIG_ENDIAN);
    SLOG_DEBUG("big endien:    n = 0x%08x", n);

    n = (int)byte2int(buf, NULL, 4, UCL_LITTLE_ENDIAN);
    SLOG_DEBUG("little endien: n = 0x%08x", n);

    SLOG_INFO("--- byte2int test done ---\n");

    return 0;
}


int memcpy_r_test()
{
    SLOG_INFO("--- memcpy_r test start ---\n");

    uint8_t src[256];
    uint8_t dst[256];
    for (int i = 0; i < sizeof(src); i++)
        src[i] = i;

    uint64_t start = time_ms();
    for (int i = 0; i < 100000; i++)
        memcpy_r(dst, src, sizeof(src));

    SLOG_DEBUG("memcpy_r: %lld ms", time_ms() - start);


    SLOG_INFO("--- memcpy_r test done ---\n");

    return 0;
}


int mem_swap_test()
{
    SLOG_INFO("--- mem_swap test start ---\n");

    uint8_t a[256];
    uint8_t b[256];
    for (int i = 0; i < sizeof(a); i++) {
        a[i] = i;
        b[i] = 0xff - i;
    }

    uint64_t start = time_ms();
    for (int i = 0; i < 10000; i++)
        mem_swap(a, b, sizeof(a));
    SLOG_DEBUG("mem_swap: %lld ms", time_ms() - start);

    return 0;
}


int mem_rev_test()
{
    SLOG_INFO("--- mem_rev test start ---\n");

    uint8_t buf[256];
    for (int i = 0; i < sizeof(buf); i++)
        buf[i] = i;

    uint64_t start = time_ms();
    for (int i = 0; i < 100000; i++)
        mem_rev(buf, sizeof(buf));
    SLOG_DEBUG("mem_rev: %lld ms", time_ms() - start);

    SLOG_INFO("--- mem_rev test done ---\n");

    return 0;
}


int lib_test()
{
    SLOG_INFO("-- lib test start --\n");

    byte2int_test();
    memcpy_r_test();
    mem_swap_test();
    mem_rev_test();

    SLOG_INFO("-- lib test done --\n");

    return 0;
}
