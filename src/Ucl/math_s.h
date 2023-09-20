#ifndef _UCL_MATH_H_
#define _UCL_MATH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* 注意：传入两个无符号值可能会出错 */
#ifndef aabs
#  define aabs(x) ((x) > 0 ? (x) : (-(x)))
#endif

#define ABS_DIFF(x, y) ((x) >= (y) ? ((x) - (y)) : ((y) - (x)))


/* bit数组 */
#define BIT_MASK(b)    (1 << ((b) % 8))
#define BIT_SET(a, b)  ((a)[(b) / 8] |= BIT_MASK(b))    /* 设置位数组a第b位为1 */
#define BIT_CLR(a, b)  ((a)[(b) / 8] &= ~BIT_MASK(b))   /* 设置位数组a第b位为0 */
#define BIT_GET(a, b)  (!!((a)[(b) / 8] & BIT_MASK(b))) /* 读取位数组a第b位值 */
#define BYTE_NUM(nb)   ((nb + 8 - 1) / 8)               /* To declare an array of nb bits */

typedef enum _E_ArrayType { TYPE_CHAR, TYPE_INT, TYPE_FLOAT } ArrayType_e;

/**
 * \brief   return a random nuber: [min, max]
 */
int randNum_(int min, int max);

void** createArray2_(uint16_t rows, uint16_t cols, ArrayType_e type);
int    freeArray2_(void** arr, uint16_t rows);
int    swapInt8_(int8_t* a, int8_t* b);
int    shiftDecimalPoint_(char* szNum, int scaler);
int    shellSort_(int* arr, int len);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_MATH_H_ */