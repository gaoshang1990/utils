#ifndef _UCL_FILE_H_
#define _UCL_FILE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   caller should free content after use
 */
const char* fileContent_(const char* path);

int  mkdir_m_(const char* dir);
bool fileExist_(const char* filePath);
int  fileSize_(const char* filePath);
int  fileSize_fd_(int fd);
int  fileSize_fp_(FILE* fp);

#ifdef __cplusplus
}
#endif

#endif /* _UCL_FILE_H_ */