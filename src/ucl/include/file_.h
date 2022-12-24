#ifndef _USER_FILE_H
#define _USER_FILE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char* getFileContent_(const char* path);
extern uint8_t     isFileExist_(const char* szFilePath);

#ifdef __cplusplus
}
#endif

#endif /* _USER_FILE_H */