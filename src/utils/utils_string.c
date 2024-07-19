#include <ctype.h>
#include <stdio.h>

#include "utils_string.h"


int str2lower(char* str, size_t len)
{
    if (str == NULL || len < 0)
        return -1;

    for (size_t i = 0; str[i] && i < len; i++)
        str[i] = tolower(str[i]);

    return 0;
}


int str2upper(char* str, size_t len)
{
    if (str == NULL || len < 0)
        return -1;

    for (size_t i = 0; str[i] && i < len; i++)
        str[i] = toupper(str[i]);

    return 0;
}
