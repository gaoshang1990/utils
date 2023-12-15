#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdint.h>

#define BASE64_ENCODE_OUT_SIZE(s) ((uint32_t)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((uint32_t)(((s) / 4) * 3))

#ifdef __cplusplus
extern "C" {
#endif

/*
 * out is null-terminated encode string.
 * return values is out length, exclusive terminating `\0'
 */
uint32_t base64_encode(const uint8_t* in, uint32_t inlen, char* out);

/*
 * return values is out length
 */
uint32_t base64_decode(const char* in, uint32_t inlen, uint8_t* out);

#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_ */
