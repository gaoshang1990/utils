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


uint8_t bcd2hex_(uint8_t bcd);
uint8_t hex2bcd_(uint8_t hex);
int     memcpy_r_(uint8_t* dst, uint8_t* src, int len);
int     memrev_(uint8_t* buff, int len);
int16_t buf2int16_(uint8_t* buf, uint16_t* offset, uint8_t mode);
int32_t buf2int32_(uint8_t* buf, uint16_t* offset, uint8_t mode);
int64_t buf2int64_(uint8_t* buf, uint16_t* offset, uint8_t mode);
float   buf2float_(uint8_t* buf, uint16_t* offset, uint8_t mode);
double  buf2double_(uint8_t* buf, uint16_t* offset, uint8_t mode);
int     atox_(const char* str, uint8_t len);
int     str2hex_(uint8_t* buf, int bufSize, const char* str);
int     hex2str_(char* str, uint8_t* buf, uint16_t bufLen);


#ifdef __cplusplus
}
#endif


#endif /* _UCL_LIB_H_ */