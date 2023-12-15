#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "string_.h"


int str2lower(char* str, int len)
{
    if (str == NULL || len < 0)
        return -1;

    for (int i = 0; str[i] && i < len; i++)
        str[i] = tolower(str[i]);

    return 0;
}


int str2upper(char* str, int len)
{
    if (str == NULL || len < 0)
        return -1;

    for (int i = 0; str[i] && i < len; i++)
        str[i] = toupper(str[i]);

    return 0;
}
