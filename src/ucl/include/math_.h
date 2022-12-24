#ifndef USER_MATH_H
#define USER_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* 注意：传入两个无符号值可能会出错 */
#ifndef aabs
#    define aabs(x) ((x) > 0 ? (x) : (-(x)))
#endif

/* bit数组 */
#define BIT_MASK(b)   (1 << ((b) % 8))
#define BIT_SET(a, b) ((a)[(b) / 8] |= BIT_MASK(b))    /* 设置位数组a第b位为1 */
#define BIT_CLR(a, b) ((a)[(b) / 8] &= ~BIT_MASK(b))   /* 设置位数组a第b位为0 */
#define BIT_GET(a, b) (!!((a)[(b) / 8] & BIT_MASK(b))) /* 读取位数组a第b位值 */
#define BYTE_NUM(nb)  ((nb + 8 - 1) / 8)               /* To declare an array of nb bits */

typedef enum _E_ArrayType { 
    TYPE_CHAR, 
    TYPE_INT, 
    TYPE_FLOAT 
} ArrayType_e;

extern int    randNum_(int min, int max);
extern void** createArray2_(uint16_t rows, uint16_t cols, ArrayType_e type);
extern int    freeArray2_(void** arr, uint16_t rows);
extern int    numstrScalerDeal_(char* szNum, int scaler);
extern int    shellSort_(int* arr, int len);


#ifdef __cplusplus
}
#endif

#endif /* USER_MATH_H */