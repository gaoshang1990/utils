#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "hal_filesystem.h"


struct sDirectoryHandle {
    DIR* handle;
};


FileHandle file_open(const char* fileName, bool readWrite)
{
    FileHandle newHandle = NULL;

    if (readWrite)
        newHandle = (FileHandle)fopen(fileName, "w");
    else
        newHandle = (FileHandle)fopen(fileName, "r");

    return newHandle;
}


int file_read(FileHandle handle, uint8_t* buffer, int maxSize)
{
    return fread(buffer, maxSize, 1, (FILE*)handle);
}


int file_write(FileHandle handle, uint8_t* buffer, int size)
{
    return fwrite(buffer, size, 1, (FILE*)handle);
}


int file_flush(FileHandle handle)
{
    return fflush(handle);
}


void file_close(FileHandle handle)
{
    fclose((FILE*)handle);
}


bool file_delete(const char* filename)
{
    if (remove(filename) == 0)
        return true;
    else
        return false;
}


bool file_rename(const char* oldFilename, const char* newFilename)
{
    if (rename(oldFilename, newFilename) == 0)
        return true;
    else
        return false;
}


bool file_info(const char* filename, uint32_t* fileSize, uint64_t* lastModificationTimestamp)
{
    struct stat fileStats;

    if (stat(filename, &fileStats) == -1)
        return false;

    if (lastModificationTimestamp != NULL)
        *lastModificationTimestamp = (uint64_t)(fileStats.st_mtime) * 1000LL;
    // does not work on older systems --> *lastModificationTimestamp = (uint64_t) (fileStats.st_ctim.tv_sec) * 1000LL;

    if (fileSize != NULL)
        *fileSize = fileStats.st_size;

    return true;
}


DirHandle file_open_dir(const char* directoryName)
{
    DIR* dirHandle = opendir(directoryName);

    DirHandle handle = NULL;

    if (dirHandle != NULL) {
        handle         = (struct sDirectoryHandle*)malloc(sizeof(struct sDirectoryHandle));
        handle->handle = dirHandle;
    }

    return handle;
}


char* file_read_dir(DirHandle directory, bool* isDirectory)
{
    struct dirent* dir = readdir(directory->handle);

    if (dir != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            return file_read_dir(directory, isDirectory);
        else {
            if (isDirectory != NULL) {
                if (dir->d_type == DT_DIR)
                    *isDirectory = true;
                else
                    *isDirectory = false;
            }

            return dir->d_name;
        }
    }

    return NULL;
}


void file_close_dir(DirHandle directory)
{
    closedir(directory->handle);
    free(directory);
}


/**
 * \brief   caller should free content after use
 */
const char* file_content(const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (fp == NULL)
        return NULL;

    fseek(fp, 0, SEEK_END);
    long  file_size = ftell(fp);
    char* content   = (char*)malloc(file_size + 1);
    if (content != NULL) {
        memset(content, 0, file_size + 1);
        fseek(fp, 0, SEEK_SET);
        fread(content, 1, file_size, fp);
    }
    fclose(fp);

    return content;
}


bool file_exist(const char* path)
{
    return (access(path, 0) == 0);
}


int file_size(const char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) < 0)
        return -1;

    return statbuf.st_size;
}


int file_size_fd(int fd)
{
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0)
        return -1;

    return statbuf.st_size;
}


int file_size_fp(FILE* fp)
{
    if (fp == NULL)
        return -1;

    return file_size_fd(fileno(fp));
}


/* create a multi-level directory */
int mkdir_m(const char* dir)
{
    if (dir == NULL)
        return -1;

    int len = strlen(dir) + 1;
    if (len <= 0) {
        printf("mkdir_m: strlen(dir) = %d\n", len);
        return -1;
    }

    char dir_tmp[256] = {0};
    strncpy(dir_tmp, dir, sizeof(dir_tmp) - 1);

    for (int i = 0; i < len; i++) {
        if (dir_tmp[i] == '\\' || dir_tmp[i] == '/' || dir_tmp[i] == '\0') {
            dir_tmp[i] = '\0';
            if (strlen(dir_tmp) > 0 && access(dir_tmp, 0) != 0) {
                if (mkdir(dir_tmp, 0755) != 0) {
                    printf("mkdir[%s] failed\n", dir_tmp);
                    return -1;
                }
            }
            dir_tmp[i] = dir[i];
        }
    }

    return 0;
}
