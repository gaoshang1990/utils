#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "math_s.h"

#ifndef ffree_
#  define ffree_(p)           \
      do {                    \
          if (p) {            \
              free((void*)p); \
              p = NULL;       \
          }                   \
      } while (0)
#endif


/* 返回 [min,max) 随机数 */
int randNum_(int min, int max)
{
    srand((unsigned)time(NULL) + rand());
    return min + rand() % (max - min);
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


/* 动态创建二维数组,ArrayTypr:0-char 1-int 2-float */
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


/* 释放动态二维数组 */
int freeArray2_(void** arr, uint16_t rows)
{
    uint16_t i;

    for (i = 0; i < rows; i++) {
        ffree_(arr[i]);
    }
    ffree_(arr);

    return 0;
}


int swapInt8_(int8_t* a, int8_t* b)
{
    int8_t tmp = *a;
    *a         = *b;
    *b         = tmp;

    return 0;
}


/* 字符串数字的倍率转换 */
int shiftDecimalPoint_(char* szNum, int scaler)
{
    if (scaler == 0) { /* 无需处理 */
        return 0;
    }
    uint8_t i;
    uint8_t len = (uint8_t)strlen(szNum);
    char    szTmp[32];

    uint8_t pos;
    for (pos = 0; pos < len; pos++) {
        if (szNum[pos] == '.') {
            break;
        }
    }
    szNum[pos] = '.'; /* 如果没有小数点 则在最后加一个 */

    if (scaler < 0) { /* 小数点左移 */
        scaler *= -1;
        i = pos;
        if (szNum[0] == '-' || szNum[0] == '+') { /* 带符号的数字前面应多补偿一个0 */
            i--;
        }
        for (; scaler + 1 > i; i++, pos++) { /* 数字前补0 */
            strcpy(szTmp, szNum);
            if (szNum[0] == '-') {
                sprintf(szNum, "-0%s", szTmp + 1);
            }
            else if (szNum[0] == '+') {
                sprintf(szNum, "+0%s", szTmp + 1);
            }
            else {
                sprintf(szNum, "0%s", szTmp);
            }
        }
        for (i = 0; i < scaler; i++) {
            swapInt8_((int8_t*)&szNum[pos], (int8_t*)&szNum[pos - 1]);
            pos--;
        }
    }
    else { /* 小数点右移 */
        for (i = 0; i < scaler; i++) {
            if (szNum[pos + 1] == '\0') {
                szNum[pos + 1] = '0';
                szNum[pos + 2] = '\0';
            }
            swapInt8_((int8_t*)&szNum[pos], (int8_t*)&szNum[pos + 1]);
            pos++;
        }
    }

    /* 处理末尾多余的零 */
    len = (uint8_t)strlen(szNum) - 1;
    while (szNum[len] == '0') {
        szNum[len--] = '\0';
    }
    if (szNum[len] == '.') {
        szNum[len--] = '\0';
    }
    if (len == 0) {
        szNum[0] = '0';
    }

    return 0;
}


/* 希尔排序 */
int shellSort_(int* arr, int len)
{
    int i, j, k, tmp, gap; /* gap为步长 */

    for (gap = len / 2; gap > 0; gap /= 2) { /* 步长初始化为数组长度的一半，每次遍历后步长减半 */
        for (i = 0; i < gap; ++i) {          /* 变量i为每次分组的第一个元素下标 */
            for (j = i + gap; j < len; j += gap) {
                /* 对步长为gap的元素进行直插排序，当gap为1时，就是直插排序 */
                tmp = arr[j];
                k   = j - gap; /* k初始化为j的前一个元素（与j相差gap长度） */
                while (k >= 0 && arr[k] > tmp) {
                    arr[k + gap] = arr[k]; /* 将在a[i]前且比tmp的值大的元素向后移动一位 */
                    k -= gap;
                }
                arr[k + gap] = tmp;
            }
        }
    }

    return 0;
}
