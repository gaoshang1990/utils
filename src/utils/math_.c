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


static void** _mallocArray2(uint16_t rows, uint16_t cols, uint8_t typeSize)
{
    uint16_t i = 0;

    void** arr = (void**)malloc(rows * cols * sizeof(void*));
    if (arr == NULL) {
        printf("_mallocArray2: malloc failed!\n");
        return NULL;
    }

    for (i = 0; i < rows; i++) {
        arr[i] = malloc(cols * typeSize);
        if (arr[i] == NULL) {
            printf("_mallocArray2: malloc failed!\n");
            return NULL;
        }
        memset(arr[i], 0, cols * typeSize);
    }

    return arr;
}


/* ��̬������ά����,ArrayTypr:0-char 1-int 2-float */
void** createArray2_(uint16_t rows, uint16_t cols, ArrayType_e type)
{
    void** arr = NULL;

    switch (type) {
    case TYPE_CHAR:
        arr = _mallocArray2(rows, cols, sizeof(char));
        break;
    case TYPE_INT:
        arr = _mallocArray2(rows, cols, sizeof(int));
        break;
    case TYPE_FLOAT:
        arr = _mallocArray2(rows, cols, sizeof(float));
        break;
    default:
        break;
    }

    return arr;
}


/* �ͷŶ�̬��ά���� */
int freeArray2_(void** arr, uint16_t rows)
{
    uint16_t i;

    for (i = 0; i < rows; i++)
        free(arr[i]);

    free(arr);

    return 0;
}


int swapInt8_(int8_t* a, int8_t* b)
{
    int8_t tmp = *a;
    *a         = *b;
    *b         = tmp;

    return 0;
}


/* �ַ������ֵı���ת�� */
int shiftDecimalPoint_(char* szNum, int scaler)
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
            swapInt8_((int8_t*)&szNum[pos], (int8_t*)&szNum[pos - 1]);
            pos--;
        }
    }
    else { /* Move decimal point to the right */
        for (int i = 0; i < scaler; i++) {
            if (szNum[pos + 1] == '\0') {
                szNum[pos + 1] = '0';
                szNum[pos + 2] = '\0';
            }
            swapInt8_((int8_t*)&szNum[pos], (int8_t*)&szNum[pos + 1]);
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


/* ϣ������ */
int shellSort_(int* arr, int len)
{
    for (int gap = len / 2; gap > 0; gap /= 2) { /* ������ʼ��Ϊ���鳤�ȵ�һ�룬ÿ�α����󲽳����� */
        for (int i = 0; i < gap; ++i) {          /* ����iΪÿ�η���ĵ�һ��Ԫ���±� */
            for (int j = i + gap; j < len; j += gap) {
                /* �Բ���Ϊgap��Ԫ�ؽ���ֱ�����򣬵�gapΪ1ʱ������ֱ������ */
                int tmp = arr[j];
                int k   = j - gap; /* k��ʼ��Ϊj��ǰһ��Ԫ�أ���j���gap���ȣ� */
                while (k >= 0 && arr[k] > tmp) {
                    arr[k + gap] = arr[k]; /* ����a[i]ǰ�ұ�tmp��ֵ���Ԫ������ƶ�һλ */
                    k -= gap;
                }
                arr[k + gap] = tmp;
            }
        }
    }

    return 0;
}


/* stat unit */


typedef union {
    int64_t int64_;
    double  double_;
} StatVar;

struct _StatUnit_ {
    struct {
        int count;
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

    return stat->setting.count > 0;
}


static bool _has_started_sliding(StatUnit stat)
{
    if (stat == NULL)
        return false;

    return stat->priv.count >= stat->setting.count;
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


static int _update_sum_avg(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    stat->output.sum = _calc_plus(stat->setting.type, stat->output.sum, stat->input.value);

    int flag = _is_sliding_window(stat) && _has_started_sliding(stat);

    StatVar tmp;
    switch (stat->setting.type) {
    case STAT_TYPE_DOUBLE:
        tmp.double_ = stat->priv.count + !flag;
        break;

    case STAT_TYPE_INT:
    default:
        tmp.int64_ = stat->priv.count + !flag;
        break;
    }

    if (flag) {
        stat->output.sum = _calc_minus(stat->setting.type, stat->output.sum, stat->priv.his[stat->priv.head]);
        stat->output.avg = _calc_div(stat->setting.type, stat->output.sum, tmp);
    }
    else {
        stat->output.avg = _calc_div(stat->setting.type, stat->output.sum, tmp);
    }

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

            for (int i = stat->priv.head + 1; i < stat->setting.count; i++)
                stat->output.min = _cmp_min(stat->setting.type, stat->output.min, stat->priv.his[i]);
        }
        else
            stat->output.min = _cmp_min(stat->setting.type, stat->output.min, stat->input.value);

        if (_cmp_equal(stat->output.max, stat->priv.his[stat->priv.head])) {
            stat->output.max = stat->input.value;

            for (int i = 0; i < stat->priv.head; i++)
                stat->output.max = _cmp_max(stat->setting.type, stat->output.max, stat->priv.his[i]);

            for (int i = stat->priv.head + 1; i < stat->setting.count; i++)
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

        if (stat->priv.head >= stat->setting.count)
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
        stat->priv.count /= 2;

    return 0;
}


static int _stat_unit(StatUnit stat)
{
    _update_sum_avg(stat);
    _update_min_max(stat);
    _update_his(stat);

    _stat_count_next(stat);

    return 0;
}


int stat_push_int(StatUnit stat, int64_t item)
{
    stat->input.value.int64_ = item;

    if (stat->priv.is_first) {
        stat_restart(stat);
        stat->priv.is_first = false;
    }

    return _stat_unit(stat);
}


int stat_push_double(StatUnit stat, double item)
{
    stat->input.value.double_ = item;

    if (stat->priv.is_first) {
        stat_restart(stat);
        stat->priv.is_first = false;
    }

    return _stat_unit(stat);
}


int64_t stat_min_int(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.min.int64_;
}


int64_t stat_max_int(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.max.int64_;
}


int64_t stat_avg_int(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.avg.int64_;
}


int64_t stat_sum_int(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.sum.int64_;
}


int64_t stat_cur_int(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->input.value.int64_;
}


double stat_min_double(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.min.double_;
}


double stat_max_double(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.max.double_;
}


double stat_avg_double(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.avg.double_;
}


double stat_sum_double(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->output.sum.double_;
}


double stat_cur_double(StatUnit stat)
{
    if (stat == NULL)
        return 0;

    return stat->input.value.double_;
}


StatUnit stat_init(int type, int count)
{
    StatUnit stat = (StatUnit)malloc(sizeof(struct _StatUnit_));

    stat->setting.type  = type;
    stat->setting.count = count;
    stat->priv.is_first = true;
    stat->priv.his      = NULL;

    if (count > 0)
        stat->priv.his = (StatVar*)malloc(stat->setting.count * sizeof(StatVar));

    return stat;
}


int stat_free(StatUnit stat)
{
    if (stat == NULL)
        return -1;

    if (stat->priv.his)
        free(stat->priv.his);

    free(stat);

    return 0;
}
