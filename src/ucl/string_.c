// #include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "string_.h"


/* 字符串全部转为小写字母 */
int str2lower_(char* pStr)
{
    int i = 0;
    while (pStr[i]) {
        pStr[i] = tolower(pStr[i]);
        i++;
    }
    return 0;
}

/* 字符串全部转为大写字母 */
int str2upper_(char* pStr)
{
    int i = 0;
    while (pStr[i]) {
        pStr[i] = toupper(pStr[i]);
        i++;
    }
    return 0;
}

