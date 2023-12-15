#ifndef _UCL_MATH_H_
#define _UCL_MATH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define ABS_DIFF(x, y) ((x) >= (y) ? ((x) - (y)) : ((y) - (x)))


/* bit array */
#define BIT_MASK(b)    (1 << ((b) % 8))
#define BIT_SET(a, b)  ((a)[(b) / 8] |= BIT_MASK(b))    /* Set bit b of bit array a to 1 */
#define BIT_CLR(a, b)  ((a)[(b) / 8] &= ~BIT_MASK(b))   /* Set bit b of bit array a to 0 */
#define BIT_GET(a, b)  (!!((a)[(b) / 8] & BIT_MASK(b))) /* Read the value of bit b in bit array a */
#define BYTE_NUM(nb)   ((nb + 8 - 1) / 8)               /* to declare an array of nb bits */


/* return a random nuber: [min, max] */
int rand_num(int min, int max);

int swap_bytes(void* a, void* b, int size);
int shift_decimal_point(char* szNum, int scaler);
int shell_sort(int* arr, int len);


enum E_STAT_TYPE {
    STAT_TYPE_INT,
    STAT_TYPE_DOUBLE,
};

typedef struct _StatUnit_* StatUnit;

StatUnit stat_init(int type, int count);
int      stat_free(StatUnit stat);
int      stat_restart(StatUnit stat);
int      stat_push_int(StatUnit stat, int64_t item);
int      stat_push_fp(StatUnit stat, double item);
int64_t  stat_min_int(StatUnit stat);
int64_t  stat_max_int(StatUnit stat);
int64_t  stat_avg_int(StatUnit stat);
int64_t  stat_sum_int(StatUnit stat);
int64_t  stat_cur_int(StatUnit stat);
double   stat_min_fp(StatUnit stat);
double   stat_max_fp(StatUnit stat);
double   stat_avg_fp(StatUnit stat);
double   stat_sum_fp(StatUnit stat);
double   stat_cur_fp(StatUnit stat);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_MATH_H_ */