#ifndef _UCL_LIB_H_
#define _UCL_LIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t bcd2hex_(uint8_t bcd);
uint8_t hex2bcd_(uint8_t hex);
int     memcpy_r_(uint8_t* dst, uint8_t* src, uint16_t len);
int32_t buf2int32_(uint8_t* pBuf, uint8_t mode);
int16_t buf2int16_(uint8_t* pBuf, uint8_t mode);
double  buf2double_(uint8_t* pBuf, uint8_t mode);
int     atox_(const char* str, uint8_t len);
int     str2hex_(uint8_t* buf, const char* str, int bufSize);
int     hex2str_(char* str, uint8_t* buf, uint16_t bufLen);


#ifdef __cplusplus
}
#endif


#endif /* _UCL_LIB_H_ */