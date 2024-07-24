#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "utils_math.h"


/* return a random nuber: [min, max]  */
int rand_num(int min, int max)
{
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = true;
    }

    return min + rand() % (max + 1 - min);
}


int shift_decimal_point(char* szNum, int scaler)
{
    if (scaler == 0)
        return 0;

    uint8_t pos; /* Decimal point index */
    uint8_t len = (uint8_t)strlen(szNum);
    for (pos = 0; pos < len; pos++) {
        if (szNum[pos] == '.')
            break;
    }
    if (pos == len) {
        szNum[pos]     = '.'; /* If there is no decimal point, add one at the end */
        szNum[pos + 1] = '\0';
    }

    if (scaler < 0) { /* Move decimal point to the left */
        scaler *= -1;

        uint8_t nbLeft = pos; /* Number of digits to the left of the decimal point */
        if (szNum[0] == '-' || szNum[0] == '+') {
            nbLeft--;
        }
        uint8_t nbZero = (nbLeft > scaler) ? 0 : scaler - nbLeft + 1; /* Number of zeros to be added */
        if (nbZero > 0) {
            int bsign = szNum[0] == '-' || szNum[0] == '+';
            memmove(&szNum[nbZero + bsign], &szNum[bsign], strlen(szNum) + 1); /* If there is a sign, offset by 1 */
            memset(&szNum[bsign], '0', nbZero);
            pos += nbZero;
        }
        for (int i = 0; i < scaler; i++) {
            char tmp       = szNum[pos];
            szNum[pos]     = szNum[pos - 1];
            szNum[pos - 1] = tmp;
            pos--;
        }
    }
    else { /* Move decimal point to the right */
        for (int i = 0; i < scaler; i++) {
            if (szNum[pos + 1] == '\0') {
                szNum[pos + 1] = '0';
                szNum[pos + 2] = '\0';
            }
            char tmp       = szNum[pos];
            szNum[pos]     = szNum[pos + 1];
            szNum[pos + 1] = tmp;
            pos++;
        }
    }

    /* Handle trailing zeros */
    len = (uint8_t)strlen(szNum) - 1;
    while (szNum[len] == '0') {
        szNum[len--] = '\0';
    }
    if (szNum[len] == '.') {
        szNum[len--] = '\0';
    }

    return 0;
}


int shell_sort(int arr[], int len)
{
    for (int gap = len / 2; gap > 0; gap /= 2) {
        for (int i = 0; i < gap; ++i) {
            for (int j = i + gap; j < len; j += gap) {
                int tmp = arr[j];
                int k   = j - gap;
                while (k >= 0 && arr[k] > tmp) {
                    arr[k + gap] = arr[k];
                    k -= gap;
                }
                arr[k + gap] = tmp;
            }
        }
    }

    return 0;
}


/* --- stat unit --- */

struct _StatUnit_ {
    struct {
        int size;
    } setting;

    struct {
        double value;
    } input;

    struct {
        double min;
        double max;
        double avg;
        double sum;
    } output;

    struct {
        bool    first;
        int     count;
        int     head;
        bool    sliding; /* 已开始滑窗 */
        double* his;
    } priv; /* 私有变量 */
};

/**
 * @brief   创建统计单元
 * @param   count: 统计窗口大小, 0-一直统计, 直至上限(INT32_MAX)
 */
StatUnit stat_new(int size)
{
    StatUnit self = (StatUnit)malloc(sizeof(struct _StatUnit_));
    memset(self, 0, sizeof(struct _StatUnit_));

    self->setting.size = size;
    self->priv.first   = true;
    self->priv.head    = 0;
    self->priv.sliding = false;
    self->priv.his     = NULL;

    if (size > 0)
        self->priv.his = (double*)malloc(self->setting.size * sizeof(double));

    return self;
}


void stat_del(StatUnit self)
{
    if (self->priv.his)
        free(self->priv.his);

    free(self);
}

/**
 * @brief   判断是否为滑动窗口统计
 * @note    滑动窗口统计: 统计数据量固定, 循环覆盖
 *          非滑动窗口统计: 统计数据量不固定, 一直累加, 到上限后清零重新开始
 */
static bool _is_sliding_window(StatUnit self)
{
    return self->setting.size > 0;
}

/**
 * @brief   判断是否开始滑动
 * @note    priv.count 自增的情况：
 *              1. 非滑动窗口统计
 *              2. 滑动窗口统计, 但还未填满窗口
 */
static bool _has_started_sliding(StatUnit self)
{
    return self->priv.sliding;
}


static bool _float_equal(double var1, double var2)
{
    static const double _EPSILON = 1e-9;

    return fabs(var1 - var2) < _EPSILON;
}


static int _update_sum_avg(StatUnit self)
{
    self->output.sum += self->input.value;

    if (_is_sliding_window(self) && _has_started_sliding(self))
        self->output.sum = self->output.sum - self->priv.his[self->priv.head];

    self->output.avg = self->output.sum / self->priv.count;

    return 0;
}


static int _update_min_max(StatUnit self)
{
    if (_is_sliding_window(self) && _has_started_sliding(self)) {
        if (_float_equal(self->output.min, self->priv.his[self->priv.head])) {
            self->output.min = self->input.value;

            for (int i = 0; i < self->priv.head; i++)
                self->output.min = UTILS_MIN(self->output.min, self->priv.his[i]);

            for (int i = self->priv.head + 1; i < self->setting.size; i++)
                self->output.min = UTILS_MIN(self->output.min, self->priv.his[i]);
        }
        else
            self->output.min = UTILS_MIN(self->output.min, self->input.value);

        if (_float_equal(self->output.max, self->priv.his[self->priv.head])) {
            self->output.max = self->input.value;

            for (int i = 0; i < self->priv.head; i++)
                self->output.max = UTILS_MAX(self->output.max, self->priv.his[i]);

            for (int i = self->priv.head + 1; i < self->setting.size; i++)
                self->output.max = UTILS_MAX(self->output.max, self->priv.his[i]);
        }
        else
            self->output.max = UTILS_MAX(self->output.max, self->input.value);
    }
    else {
        self->output.min = UTILS_MIN(self->output.min, self->input.value);
        self->output.max = UTILS_MAX(self->output.max, self->input.value);
    }

    return 0;
}


static int _update_his(StatUnit self)
{
    if (_is_sliding_window(self)) {
        self->priv.his[self->priv.head++] = self->input.value;

        if (self->priv.head >= self->setting.size)
            self->priv.head = 0;
    }

    return 0;
}


static int _stat_count_next(StatUnit self)
{
    if ((self->priv.sliding == false) && (self->priv.count >= self->setting.size))
        self->priv.sliding = true;

    if (!_is_sliding_window(self) || !_has_started_sliding(self))
        self->priv.count++;

    if (self->priv.count >= INT32_MAX)
        stat_reset(self);

    return 0;
}


int stat_reset(StatUnit self)
{
    self->output.min = self->input.value;
    self->output.max = self->input.value;
    self->output.avg = self->input.value;
    self->output.sum = 0;
    self->priv.count = 1;

    return 0;
}


static int _stat_exec(StatUnit self)
{
    if (self->priv.first) {
        self->priv.first = false;
        self->output.min = self->input.value;
        self->output.max = self->input.value;
    }

    _stat_count_next(self);

    _update_sum_avg(self);
    _update_min_max(self);

    _update_his(self); /* 计算完统计量再更新历史数据 */

    return 0;
}


int stat_push(StatUnit self, double item)
{
    self->input.value = item;

    return _stat_exec(self);
}


double stat_min(StatUnit self)
{
    return self->output.min;
}


double stat_max(StatUnit self)
{
    return self->output.max;
}


double stat_avg(StatUnit self)
{
    return self->output.avg;
}


double stat_sum(StatUnit self)
{
    return self->output.sum;
}


double stat_cur(StatUnit self)
{
    return self->input.value;
}

/**
 * @brief   计算方差
 */
double stat_variance(StatUnit self)
{
    if (self->setting.size == 0) {
        printf("not support non sliding window\n");
        return 0;
    }

    double variance = 0;
    double avg      = stat_avg(self);

    for (int i = 0; i < self->priv.count; i++) {
        variance += (self->priv.his[i] - avg) * (self->priv.his[i] - avg);
    }

    return variance / self->priv.count;
}

/* --- stat unit end --- */
