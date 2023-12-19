#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "math_.h"


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


int swap_mem(void* a, void* b, int size)
{
    uint8_t  tmp;
    uint8_t* p1 = (uint8_t*)a;
    uint8_t* p2 = (uint8_t*)b;

    for (int i = 0; i < size; i++) {
        tmp   = p1[i];
        p1[i] = p2[i];
        p2[i] = tmp;
    }

    return 0;
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
            swap_mem(&szNum[pos], &szNum[pos - 1], sizeof(char));
            pos--;
        }
    }
    else { /* Move decimal point to the right */
        for (int i = 0; i < scaler; i++) {
            if (szNum[pos + 1] == '\0') {
                szNum[pos + 1] = '0';
                szNum[pos + 2] = '\0';
            }
            swap_mem(&szNum[pos], &szNum[pos + 1], sizeof(char));
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


int shell_sort(int* arr, int len)
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


typedef union {
    int64_t int64_;
    double  double_;
} StatVar;

struct _StatUnit_ {
    struct {
        int size;
        int type;
    } setting;

    struct {
        StatVar value;
    } input;

    struct {
        StatVar min;
        StatVar max;
        StatVar avg;
        StatVar sum;
    } output;

    struct {
        bool     is_first;
        int      count;
        StatVar* his;
        int      head;
    } priv;
};


static bool _is_sliding_window(StatUnit stat)
{
    if (stat == NULL)
        return false;

    return stat->setting.size > 0;
}


static bool _has_started_sliding(StatUnit stat)
{
    if (stat == NULL)
        return false;

    return stat->priv.count >= stat->setting.size;
}


static bool _cmp_equal(StatVar var1, StatVar var2)
{
    if (0 == memcmp(&var1, &var2, sizeof(StatVar)))
        return true;

    return false;
}


static StatVar _cmp_min(int type, StatVar var1, StatVar var2)
{
    switch (type) {
    case STAT_TYPE_DOUBLE:
        if (var1.double_ < var2.double_)
            return var1;
        break;

    case STAT_TYPE_INT:
    default:
        if (var1.int64_ < var2.int64_)
            return var1;
        break;
    }

    return var2;
}


static StatVar _cmp_max(int type, StatVar var1, StatVar var2)
{
    switch (type) {
    case STAT_TYPE_DOUBLE:
        if (var1.double_ > var2.double_)
            return var1;
        break;

    case STAT_TYPE_INT:
    default:
        if (var1.int64_ > var2.int64_)
            return var1;
        break;
    }

    return var2;
}


static StatVar _calc_minus(int type, StatVar var1, StatVar var2)
{
    StatVar ret;

    switch (type) {
    case STAT_TYPE_DOUBLE:
        ret.double_ = var1.double_ - var2.double_;
        break;

    case STAT_TYPE_INT:
    default:
        ret.int64_ = var1.int64_ - var2.int64_;
        break;
    }

    return ret;
}


static StatVar _calc_plus(int type, StatVar var1, StatVar var2)
{
    StatVar ret;

    switch (type) {
    case STAT_TYPE_DOUBLE:
        ret.double_ = var1.double_ + var2.double_;
        break;

    case STAT_TYPE_INT:
    default:
        ret.int64_ = var1.int64_ + var2.int64_;
        break;
    }

    return ret;
}


static StatVar _calc_div(int type, StatVar var1, StatVar var2)
{
    StatVar ret;

    switch (type) {
    case STAT_TYPE_DOUBLE:
        ret.double_ = var1.double_ / var2.double_;
        break;

    case STAT_TYPE_INT:
    default:
        ret.int64_ = var1.int64_ / var2.int64_;
        break;
    }

    return ret;
}


static StatVar _int2statvar(int type, int64_t value)
{
    StatVar ret;

    switch (type) {
    case STAT_TYPE_DOUBLE:
        ret.double_ = (double)value;
        break;

    case STAT_TYPE_INT:
    default:
        ret.int64_ = value;
        break;
    }

    return ret;
}


static int _update_sum_avg(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    stat->output.sum = _calc_plus(stat->setting.type, stat->output.sum, stat->input.value);

    int     flag = _is_sliding_window(stat) && _has_started_sliding(stat);
    StatVar cnt  = _int2statvar(stat->setting.type, stat->priv.count + !flag);

    if (flag)
        stat->output.sum = _calc_minus(stat->setting.type, stat->output.sum, stat->priv.his[stat->priv.head]);

    stat->output.avg = _calc_div(stat->setting.type, stat->output.sum, cnt);

    return 0;
}


static int _update_min_max(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    if (_is_sliding_window(stat) && _has_started_sliding(stat)) {
        if (_cmp_equal(stat->output.min, stat->priv.his[stat->priv.head])) {
            stat->output.min = stat->input.value;

            for (int i = 0; i < stat->priv.head; i++)
                stat->output.min = _cmp_min(stat->setting.type, stat->output.min, stat->priv.his[i]);

            for (int i = stat->priv.head + 1; i < stat->setting.size; i++)
                stat->output.min = _cmp_min(stat->setting.type, stat->output.min, stat->priv.his[i]);
        }
        else
            stat->output.min = _cmp_min(stat->setting.type, stat->output.min, stat->input.value);

        if (_cmp_equal(stat->output.max, stat->priv.his[stat->priv.head])) {
            stat->output.max = stat->input.value;

            for (int i = 0; i < stat->priv.head; i++)
                stat->output.max = _cmp_max(stat->setting.type, stat->output.max, stat->priv.his[i]);

            for (int i = stat->priv.head + 1; i < stat->setting.size; i++)
                stat->output.max = _cmp_max(stat->setting.type, stat->output.max, stat->priv.his[i]);
        }
        else
            stat->output.max = _cmp_max(stat->setting.type, stat->output.max, stat->input.value);
    }
    else {
        stat->output.min = _cmp_min(stat->setting.type, stat->output.min, stat->input.value);
        stat->output.max = _cmp_max(stat->setting.type, stat->output.max, stat->input.value);
    }

    return 0;
}


static int _update_his(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    if (_is_sliding_window(stat)) {
        stat->priv.his[stat->priv.head++] = stat->input.value;

        if (stat->priv.head >= stat->setting.size)
            stat->priv.head = 0;
    }

    return 0;
}


static int _stat_count_next(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    if (!_is_sliding_window(stat) || !_has_started_sliding(stat))
        stat->priv.count++;

    if (stat->priv.count >= INT32_MAX)
        stat_restart(stat);

    return 0;
}


int stat_restart(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    stat->output.min        = stat->input.value;
    stat->output.max        = stat->input.value;
    stat->output.sum.int64_ = 0;
    stat->priv.head         = 0;
    stat->priv.count        = 0;

    return 0;
}


static int _stat_exec(StatUnit stat)
{
    if (stat->priv.is_first) {
        stat_restart(stat);
        stat->priv.is_first = false;
    }

    _update_sum_avg(stat);
    _update_min_max(stat);
    _update_his(stat);

    _stat_count_next(stat);

    return 0;
}

int stat_push_int(StatUnit stat, int64_t item)
{
    stat->input.value.int64_ = item;

    return _stat_exec(stat);
}


int stat_push_fp(StatUnit stat, double item)
{
    stat->input.value.double_ = item;

    return _stat_exec(stat);
}


void* stat_min(StatUnit stat)
{
    return &stat->output.min;
}


void* stat_max(StatUnit stat)
{
    return &stat->output.max;
}


void* stat_avg(StatUnit stat)
{
    return &stat->output.avg;
}


void* stat_sum(StatUnit stat)
{
    return &stat->output.sum;
}


void* stat_cur(StatUnit stat)
{
    return &stat->input.value;
}


StatUnit stat_new(int type, int count)
{
    StatUnit stat = (StatUnit)malloc(sizeof(struct _StatUnit_));

    stat->setting.type  = type;
    stat->setting.size  = count;
    stat->priv.is_first = true;
    stat->priv.his      = NULL;

    if (count > 0)
        stat->priv.his = (StatVar*)malloc(stat->setting.size * sizeof(StatVar));

    return stat;
}


int stat_del(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    if (stat->priv.his)
        free(stat->priv.his);

    free(stat);

    return 0;
}
