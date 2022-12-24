#ifndef _USER_FILE_H
#define _USER_FILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* fileContent_(const char* path);
bool        fileExist_(const char* filePath);
int         mkdir_m_(const char* dir);
int         fileSize_(const char* filePath);
int         fileSize_fp_(FILE* fp);

#ifdef __cplusplus
}
#endif

#endif /* _USER_FILE_H */