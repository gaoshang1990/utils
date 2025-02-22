#include <stdio.h>

#include "utils_log.h"
#include "utils_math.h"

static int _userShift(const char* src, int scaler)
{
    char szNum[32];

    sprintf(szNum, "%s", src);
    shift_decimal_point(szNum, scaler);
    SLOG_DEBUG("%s -> %d: %s", src, scaler, szNum);

    return 0;
}

static int shiftDecimalPoint_test()
{
    SLOG_INFO("--- shiftDecimalPoint test start ---\n");

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
    SLOG_DEBUG_RAW("\n");

    _userShift("12.34", -3);
    _userShift("12.34", -2);
    _userShift("12.34", -1);
    _userShift("12.34", 0);
    _userShift("12.34", 1);
    _userShift("12.34", 2);
    _userShift("12.34", 3);
    SLOG_DEBUG_RAW("\n");

    _userShift("-12.34", -3);
    _userShift("-12.34", -2);
    _userShift("-12.34", -1);
    _userShift("-12.34", 0);
    _userShift("-12.34", 1);
    _userShift("-12.34", 2);
    _userShift("-12.34", 3);
    SLOG_DEBUG_RAW("\n");

    _userShift("+12.34", -3);
    _userShift("+12.34", -2);
    _userShift("+12.34", -1);
    _userShift("+12.34", 0);
    _userShift("+12.34", 1);
    _userShift("+12.34", 2);
    _userShift("+12.34", 3);
    SLOG_DEBUG_RAW("\n");

    _userShift("1200", -1);
    _userShift("1200", -2);

    SLOG_INFO("--- shiftDecimalPoint test done ---\n");

    return 0;
}


int randNum_test()
{
    SLOG_INFO("--- randNum test start ---\n");

    int a[10000];
    int b[10] = {0};

    for (int i = 0; i < 10000; i++) {
        a[i] = rand_num(0, 9);
        b[a[i]]++;
        if (i % 100 == 0)
            SLOG_DEBUG("random number in [0, 9]: %d, i = %d", rand_num(0, 9), i);
    }

    for (int i = 0; i < 10; i++)
        SLOG_DEBUG("%d occurs %d times", i, b[i]);

    SLOG_INFO("--- randNum test done ---\n");

    return 0;
}


int stat_test()
{
    SLOG_INFO("--- stat test start ---\n");

    SLOG_DEBUG_RAW("\n");

    StatUnit stat = stat_new(3);
    for (int i = 1; i < 10; i++) {
        stat_push(stat, i);
        SLOG_DEBUG("min = %lf, max = %lf, avg = %lf, sum = %lf",
                   stat_min(stat),
                   stat_max(stat),
                   stat_avg(stat),
                   stat_sum(stat));
    }
    stat_del(stat);
    SLOG_DEBUG_RAW("\n");

    stat = stat_new(0);
    for (int i = 1; i < 10; i++) {
        if (i == 5)
            stat_reset(stat);

        stat_push(stat, i);
        SLOG_DEBUG("min = %lf, max = %lf, avg = %lf, sum = %lf",
                   stat_min(stat),
                   stat_max(stat),
                   stat_avg(stat),
                   stat_sum(stat));
    }
    stat_del(stat);
    SLOG_DEBUG_RAW("\n");

    SLOG_INFO("--- stat test done ---\n");

    return 0;
}


int math_test()
{
    SLOG_INFO("-- math test start --\n");

    shiftDecimalPoint_test();
    randNum_test();
    stat_test();

    SLOG_INFO("-- math test done --\n");

    return 0;
}
