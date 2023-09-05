#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifndef _WIN32
#  include <unistd.h>
#endif

#include "file_s.h"

#ifdef _WIN32
#  define mkdir_(path, arg) _mkdir(path)
#  define access            _access
#else
#  define mkdir_(path, arg) mkdir(path, arg)
#endif

#define MAX_FILE_PATH_LEN (256)

/* 读取文件，返回字符串，由调用者释放 */
const char* file_content_(const char* path)
{
    uint32_t fileSize = 0;
    char*    content  = NULL;
    FILE*    fp       = fopen(path, "rb");
    if (fp == NULL) {
        printf("open file \"%s\" failed!\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    content  = (char*)malloc(fileSize + 1);
    if (content != NULL) {
        memset(content, 0, fileSize + 1);
        fseek(fp, 0, SEEK_SET);
        fread(content, 1, fileSize, fp);
    }
    fclose(fp);
    fp = NULL;

    return content;
}


bool file_exist_(const char* filePath)
{
#ifdef _WIN32
    return (_access(filePath, 0) == 0);
#else
    return (access(filePath, F_OK) == 0);
#endif
}


int file_size_(const char* filePath)
{
    struct stat statbuf;
    if (stat(filePath, &statbuf) < 0) {
        return -1;
    }

    return statbuf.st_size;
}


int fileSize_fd_(int fd)
{
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        return -1;
    }

    return statbuf.st_size;
}


int file_size_fp_(FILE* fp)
{
    if (fp == NULL) {
        return -1;
    }

    return fileSize_fd_(fileno(fp));
}


/* Create a multi-level directory */
int mkdir_m_(const char* dir)
{
    if (dir == NULL) {
        printf("_mkdir_m_: dir is NULL\n");
        return -1;
    }

    int len = strlen(dir) + 1;
    if (len <= 0 || len > MAX_FILE_PATH_LEN) {
        printf("_mkdir_m_: strlen(dir) = %d\n", len);
        return -1;
    }

    char dirTmp[256] = {0};
    strncpy(dirTmp, dir, sizeof(dirTmp) - 1);

    for (int i = 0; i < len; i++) {
        if (dirTmp[i] == '\\' || dirTmp[i] == '/' || dirTmp[i] == '\0') {
            dirTmp[i] = '\0';
            if (strlen(dirTmp) > 0 && access(dirTmp, 0) != 0) {
                if (mkdir_(dirTmp, 0755) != 0) {
                    printf("mkdir[%s] failed\n", dirTmp);
                    return -1;
                }
            }
            dirTmp[i] = dir[i];
        }
    }

    return 0;
}
