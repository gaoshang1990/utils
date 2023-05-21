#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
// #    include <io.h>
#else
#  include <unistd.h>
#endif

#include "file_s.h"


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


/* 创建多级目录 */
int mkdir_m_(const char* dir)
{
    int len = strlen(dir) + 1;
    if (dir == NULL || len == 0 || len > 256) {
        return -1;
    }

    char dirTmp[256] = {0};
    strncpy(dirTmp, dir, sizeof(dirTmp) - 1);

    for (int i = 0; i < len; i++) {
        if (dirTmp[i] == '\\' || dirTmp[i] == '/' || dirTmp[i] == '\0') {
            dirTmp[i] = '\0';
            if (file_exist_(dirTmp) == false) {
#ifdef _WIN32
                if (_mkdir(dirTmp) != 0) {
                    return -1;
                }
#else
                if (mkdir(dirTmp, 0755) != 0) { /* FIXME:这里有问题 */
                    printf("mkdir %s failed!\n", dirTmp);
                    return -1;
                }
#endif
            }
            dirTmp[i] = dir[i];
        }
    }

    return 0;
}