#include <stdio.h>

#include "utils_log.h"
#include "utils_math.h"

static int _userShift(const char* src, int scaler)
{
    char szNum[32];

    sprintf(szNum, "%s", src);
    shift_decimal_point(szNum, scaler);
    slog_debug("%s -> %d: %s", src, scaler, szNum);

    return 0;
}

static int shiftDecimalPoint_test()
{
    slog_info("--- shiftDecimalPoint test start ---\n");

    char   szNum[32];
    double d;

    _userShift("0", 3);
    _userShift("0", -3);
    _userShift("1000", 3);
    _userShift("1000", -3);
    _userShift("12", -2);
    _userShift("12", -1);
    _userShift("12", 0);
    _userShift("12", 1);
    slog_debug_raw("\n");

    _userShift("12.34", -3);
    _userShift("12.34", -2);
    _userShift("12.34", -1);
    _userShift("12.34", 0);
    _userShift("12.34", 1);
    _userShift("12.34", 2);
    _userShift("12.34", 3);
    slog_debug_raw("\n");

    _userShift("-12.34", -3);
    _userShift("-12.34", -2);
    _userShift("-12.34", -1);
    _userShift("-12.34", 0);
    _userShift("-12.34", 1);
    _userShift("-12.34", 2);
    _userShift("-12.34", 3);
    slog_debug_raw("\n");

    _userShift("+12.34", -3);
    _userShift("+12.34", -2);
    _userShift("+12.34", -1);
    _userShift("+12.34", 0);
    _userShift("+12.34", 1);
    _userShift("+12.34", 2);
    _userShift("+12.34", 3);
    slog_debug_raw("\n");

    _userShift("1200", -1);
    _userShift("1200", -2);

    slog_info("--- shiftDecimalPoint test done ---\n");

    return 0;
}


int randNum_test()
{
    slog_info("--- randNum test start ---\n");

    int a[10000];
    int b[10] = {0};

    for (int i = 0; i < 10000; i++) {
        a[i] = rand_num(0, 9);
        b[a[i]]++;
        if (i % 100 == 0)
            slog_debug("random number in [0, 9]: %d, i = %d", rand_num(0, 9), i);
    }

    for (int i = 0; i < 10; i++)
        slog_debug("%d occurs %d times", i, b[i]);

    slog_info("--- randNum test done ---\n");

    return 0;
}


int stat_test()
{
    slog_info("--- stat test start ---\n");

    slog_debug_raw("\n");

    StatUnit stat = stat_new(3);
    for (int i = 1; i < 10; i++) {
        stat_push(stat, i);
        slog_debug("min = %lf, max = %lf, avg = %lf, sum = %lf",
                   stat_min(stat),
                   stat_max(stat),
                   stat_avg(stat),
                   stat_sum(stat));
    }
    stat_del(stat);
    slog_debug_raw("\n");

    stat = stat_new(0);
    for (int i = 1; i < 10; i++) {
        if (i == 5)
            stat_reset(stat);

        stat_push(stat, i);
        slog_debug("min = %lf, max = %lf, avg = %lf, sum = %lf",
                   stat_min(stat),
                   stat_max(stat),
                   stat_avg(stat),
                   stat_sum(stat));
    }
    stat_del(stat);
    slog_debug_raw("\n");

    slog_info("--- stat test done ---\n");

    return 0;
}


int math_test()
{
    slog_info("-- math test start --\n");

    shiftDecimalPoint_test();
    randNum_test();
    stat_test();

    slog_info("-- math test done --\n");

    return 0;
}
