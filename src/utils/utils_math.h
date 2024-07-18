#ifndef _UTILS_MATH_H___
#define _UTILS_MATH_H___

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


#define ABS_DIFF(x, y)  ((x) >= (y) ? ((x) - (y)) : ((y) - (x)))
#define UTILS_MAX(x, y) ((x) >= (y) ? (x) : (y))
#define UTILS_MIN(x, y) ((x) <= (y) ? (x) : (y))


/* bit array */
#define BIT_MASK(b)     (1 << ((b) % 8))
#define BIT_SET(a, b)   ((a)[(b) / 8] |= BIT_MASK(b))  /* Set bit b of bit array a to 1 */
#define BIT_CLR(a, b)   ((a)[(b) / 8] &= ~BIT_MASK(b)) /* Set bit b of bit array a to 0 */
#define BIT_GET(a, b) \
    (!!((a)[(b) / 8] & BIT_MASK(b)))    /* Read the value of bit b in bit array a */
#define BYTE_NUM(nb) ((nb + 8 - 1) / 8) /* to declare an array of nb bits */


/* return a random nuber: [min, max] */
int rand_num(int min, int max);

int shift_decimal_point(char* szNum, int scaler);
int shell_sort(int* arr, int len);


typedef struct _StatUnit_* StatUnit;

StatUnit stat_new(int count);
int      stat_del(StatUnit stat);
int      stat_reset(StatUnit stat);
int      stat_push(StatUnit stat, double item);
double   stat_min(StatUnit stat);
double   stat_max(StatUnit stat);
double   stat_avg(StatUnit stat);
double   stat_sum(StatUnit stat);
double   stat_cur(StatUnit stat);
double   stat_variance(StatUnit stat);


#ifdef __cplusplus
}
#endif

#endif /* _UTILS_MATH_H___ */
