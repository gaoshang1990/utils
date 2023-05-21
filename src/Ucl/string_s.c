// #include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "string_s.h"


/* 字符串全部转为小写字母 */
int str2lower_(char* str, int len)
{
	if (str == NULL || len < 0) {
		return -1;
	}
	
    for (int i = 0; str[i] && i < len; i++) {
        str[i] = tolower(str[i]);
    }
    return 0;
}

/* 字符串全部转为大写字母 */
int str2upper_(char* str, int len)
{
	if (str == NULL || len < 0) {
		return -1;
	}
	
    for (int i = 0; str[i] && i < len; i++) {
        str[i] = toupper(str[i]);
    }
    return 0;
}

