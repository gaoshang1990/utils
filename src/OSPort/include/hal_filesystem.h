#ifndef FILESYSTEM_HAL_H_
#define FILESYSTEM_HAL_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef FILE*                    FileHandle;
typedef struct sDirectoryHandle* DirHandle;


FileHandle file_open(const char* pathName, bool readWrite);
int        file_read(FileHandle handle, uint8_t* buffer, int maxSize);
int        file_write(FileHandle handle, uint8_t* buffer, int size);
int        file_flush(FileHandle handle);
void       file_close(FileHandle handle);
bool       file_info(const char* filename, uint32_t* fileSize, uint64_t* lastModificationTimestamp);
bool       file_delete(const char* filename);
bool       file_rename(const char* oldFilename, const char* newFilename);

DirHandle file_open_dir(const char* directoryName);
char*     file_read_dir(DirHandle directory, bool* isDirectory);
void      file_close_dir(DirHandle directory);

const char* file_content(const char* path);
bool        file_exist(const char* path);
int         file_size(const char* path);
int         file_size_fd(int fd);
int         file_size_fp(FILE* fp);
int         mkdir_m(const char* dir);


#ifdef __cplusplus
}
#endif

#endif /* FILESYSTEM_HAL_H_ */
