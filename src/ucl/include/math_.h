#ifndef USER_MATH_H
#define USER_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ע�⣺���������޷���ֵ���ܻ���� */
#ifndef aabs
#    define aabs(x) ((x) > 0 ? (x) : (-(x)))
#endif

/* bit���� */
#define BIT_MASK(b)   (1 << ((b) % 8))
#define BIT_SET(a, b) ((a)[(b) / 8] |= BIT_MASK(b))    /* ����λ����a��bλΪ1 */
#define BIT_CLR(a, b) ((a)[(b) / 8] &= ~BIT_MASK(b))   /* ����λ����a��bλΪ0 */
#define BIT_GET(a, b) (!!((a)[(b) / 8] & BIT_MASK(b))) /* ��ȡλ����a��bλֵ */
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