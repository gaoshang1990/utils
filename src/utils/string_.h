#ifndef _UCL_STRING_H_
#define _UCL_STRING_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int str2lower(char* str, int len);
int str2upper(char* str, int len);

#ifdef __cplusplus
}
#endif

#endif /* _UCL_STRING_H_ */