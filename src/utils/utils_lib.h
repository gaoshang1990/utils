#ifndef _UTILS_LIB_H___
#define _UTILS_LIB_H___

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UCL_LITTLE_ENDIAN = 0,
    UCL_BIG_ENDIAN,
};


/* 0x10 -> 10(0x0a) */
uint8_t bcd2hex(uint8_t bcd);

/* 0x10(16) -> 0x16 */
uint8_t hex2bcd(uint8_t hex);

uint32_t hex2bcd32(uint32_t hex);

/* memcpy in an opposite direction */
void memcpy_r(uint8_t* dst, uint8_t* src, int len);

/* memory data inversion of specified length */
int mem_rev(uint8_t* buf, int len);

void mem_swap(void* a, void* b, int size);

/**
 * @brief   Convert a series of bytes into an integer
 * @param   buf:    byte array
 * @param   offset: offset of the byte array, if offset is NULL, it will be ignored,
 * otherwise it will increase by len
 * @param   len:    length of the byte array, should <= sizeof(int64_t)
 * @param   mode:   0-little endian, 1-big endian
 */
int64_t byte2int(uint8_t* buf, int* offset, int len, int mode);

double byte2fp(uint8_t* buf, int* offset, int len, int mode);

/* string to number, strlen should <= 8 */
int atox_(const char* str, int len);

/**
 * \brief   string turn to uint8 array:
 *          "680100" -> [0x68, 0x01, 0x00]
 */
int str2byte(uint8_t* buf, int bufSize, const char* str);

/**
 * \brief   uint8 array turn to string:
 *          [0x68, 0x01, 0x00] -> "680100"
 */
int byte2str(char* str, uint8_t* buf, int bufLen);

int reduce_gap(uint8_t* data, int len, uint8_t gap);
int add_gap(uint8_t* data, int len, uint8_t gap);

/**
 * @brief   Get the first number in a string
 * @retval  -1: no number found
 */
int num_in_str(const char* str);

#ifdef __cplusplus
}
#endif


#endif /* _UTILS_LIB_H___ */
