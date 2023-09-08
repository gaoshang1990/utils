#ifndef _UCL_LIB_H_
#define _UCL_LIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UCL_LITTLE_ENDIAN = 0,
    UCL_BIG_ENDIAN,
};


/* bcd to hex: 0x10 -> 10(0x0a) */
uint8_t bcd2hex_(uint8_t bcd);

/* hex to bcd: 0x10(16) -> 0x16 */
uint8_t hex2bcd_(uint8_t hex);

/* memcpy in an opposite direction */
int memcpy_r_(uint8_t* dst, uint8_t* src, int len);

/* memory data inversion of specified length */
int memrev_(uint8_t* buf, int len);

/**
 * \param   mode: 0-little endian, 1-big endian
 */
int16_t buf2int16_(uint8_t* buf, uint16_t* offset, int mode);
int32_t buf2int32_(uint8_t* buf, uint16_t* offset, int mode);
int64_t buf2int64_(uint8_t* buf, uint16_t* offset, int mode);
float   buf2float_(uint8_t* buf, uint16_t* offset, int mode);
double  buf2double_(uint8_t* buf, uint16_t* offset, int mode);

/* string to number, strlen should <= 8 */
int atox_(const char* str, int len);

/**
 * \brief   string turn to uint8 array:
 *          "680100" -> [0x68, 0x01, 0x00]
 */
int str2hex_(uint8_t* buf, int bufSize, const char* str);

/**
 * \brief   uint8 array turn to string:
 *          [0x68, 0x01, 0x00] -> "680100"
 */
int hex2str_(char* str, uint8_t* buf, int bufLen);


#ifdef __cplusplus
}
#endif


#endif /* _UCL_LIB_H_ */