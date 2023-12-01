#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifndef _WIN32
#  include <unistd.h>
#endif

#include "file_.h"

#ifdef _WIN32
#  define mkdir(path, arg) _mkdir(path)
#  define access           _access
#else
#endif

#define MAX_FILE_PATH_LEN (256)


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


/* Create a multi-level directory */
int mkdir_m(const char* dir)
{
    if (dir == NULL)
        return -1;

    int len = strlen(dir) + 1;
    if (len <= 0 || len > MAX_FILE_PATH_LEN) {
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
