#ifndef FILESYSTEM_HAL_H_
#define FILESYSTEM_HAL_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef CONFIG_SYSTEM_FILE_SEPARATOR
#  define CONFIG_SYSTEM_FILE_SEPARATOR '/'
#endif

typedef FILE*                    FileHandle;
typedef struct sDirectoryHandle* DirectoryHandle;


FileHandle      FileSystem_openFile(char* pathName, bool readWrite);
int             FileSystem_readFile(FileHandle handle, uint8_t* buffer, int maxSize);
int             FileSystem_writeFile(FileHandle handle, uint8_t* buffer, int size);
int             FileSystem_fflushFile(FileHandle handle);
void            FileSystem_closeFile(FileHandle handle);
bool            FileSystem_getFileInfo(char* filename, uint32_t* fileSize, uint64_t* lastModificationTimestamp);
bool            FileSystem_deleteFile(char* filename);
bool            FileSystem_renameFile(char* oldFilename, char* newFilename);
DirectoryHandle FileSystem_openDirectory(char* directoryName);
char*           FileSystem_readDirectory(DirectoryHandle directory, bool* isDirectory);
void            FileSystem_closeDirectory(DirectoryHandle directory);


#ifdef __cplusplus
}
#endif

#endif /* FILESYSTEM_HAL_H_ */
