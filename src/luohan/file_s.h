#ifndef _UCL_FILE_H
#define _UCL_FILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* file_content_(const char* path);
bool        file_exist_(const char* filePath);
int         mkdir_m_(const char* dir);
int         file_size_(const char* filePath);
int         file_size_fp_(FILE* fp);

#ifdef __cplusplus
}
#endif

#endif /* _UCL_FILE_H */