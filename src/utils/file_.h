#ifndef _UCL_FILE_H_
#define _UCL_FILE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* caller should free content after use */
const char* file_content(const char* path);

int  mkdir_m(const char* dir);
bool file_exist(const char* filePath);
int  file_size(const char* filePath);
int  file_size_fd(int fd);
int  file_size_fp(FILE* fp);

#ifdef __cplusplus
}
#endif

#endif /* _UCL_FILE_H_ */