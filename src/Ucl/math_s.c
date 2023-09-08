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


/* ���� [min,max) ����� */
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


/* �ַ������ֵı���ת�� */
int shiftDecimalPoint_(char* szNum, int scaler)
{
    if (scaler == 0) { /* ���账�� */
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
    szNum[pos] = '.'; /* ���û��С���� ��������һ�� */

    if (scaler < 0) { /* С�������� */
        scaler *= -1;
        i = pos;
        if (szNum[0] == '-' || szNum[0] == '+') { /* �����ŵ�����ǰ��Ӧ�ಹ��һ��0 */
            i--;
        }
        for (; scaler + 1 > i; i++, pos++) { /* ����ǰ��0 */
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
    else { /* С�������� */
        for (i = 0; i < scaler; i++) {
            if (szNum[pos + 1] == '\0') {
                szNum[pos + 1] = '0';
                szNum[pos + 2] = '\0';
            }
            swapInt8_((int8_t*)&szNum[pos], (int8_t*)&szNum[pos + 1]);
            pos++;
        }
    }

    /* ����ĩβ������� */
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


/* ϣ������ */
int shellSort_(int* arr, int len)
{
    int i, j, k, tmp, gap; /* gapΪ���� */

    for (gap = len / 2; gap > 0; gap /= 2) { /* ������ʼ��Ϊ���鳤�ȵ�һ�룬ÿ�α����󲽳����� */
        for (i = 0; i < gap; ++i) {          /* ����iΪÿ�η���ĵ�һ��Ԫ���±� */
            for (j = i + gap; j < len; j += gap) {
                /* �Բ���Ϊgap��Ԫ�ؽ���ֱ�����򣬵�gapΪ1ʱ������ֱ������ */
                tmp = arr[j];
                k   = j - gap; /* k��ʼ��Ϊj��ǰһ��Ԫ�أ���j���gap���ȣ� */
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
