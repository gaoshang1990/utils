#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

#include "file_.h"


/* 读取文件，返回字符串，由调用者释放 */
const char* getFileContent_(const char* path)
{
    uint32_t fileSize = 0;
    char*    content  = NULL;
    FILE*    fp       = fopen(path, "rb");
    if (fp == NULL) {
        printf("open file \"%s\" failed!\n", path);
        fp = fopen(path, "rb");
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


uint8_t isFileExist_(const char* szFilePath)
{
#ifdef _WIN32
    return (_access(szFilePath, 0) == 0);
#else
    return (access(szFilePath, F_OK) == 0);
#endif
}