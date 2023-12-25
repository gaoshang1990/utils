
#include "mlog_.h"
#include "lib_.h"


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


int lib_test()
{
    SLOG_INFO("-- lib test start --\n");

    byte2int_test();

    SLOG_INFO("-- lib test done --\n");

    return 0;
}