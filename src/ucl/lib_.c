#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* BCD转HEX: 0x10 -> 10(0x0a) */
uint8_t bcd2hex_(uint8_t bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0f));
}


/* HEX转BCD: 0x10(16) -> 0x16 */
uint8_t hex2bcd_(uint8_t hex)
{
    if (hex > 99) {
        return 0x99;
    }
    uint8_t bcd = hex % 10;
    uint8_t tmp = (hex / 10) % 10;
    tmp <<= 4;
    bcd |= tmp;

    return bcd;
}

/* 反向memcpy */
int memcpy_r_(uint8_t* dst, uint8_t* src, uint16_t len)
{
    if (dst == NULL || src == NULL) {
        return -1;
    }
    uint16_t i = 0;
    for (i = 0; i < len; i++) {
        dst[i] = src[len - i - 1];
    }
    return 0;
}

/* mode: 0-小端 1-大端 */
int32_t buf2int32_(uint8_t* pBuf, uint8_t mode)
{
    if (pBuf == NULL) {
        printf("pBuf is NULL!\n");
    }
    int32_t ret = 0;
    if (mode) {
        memcpy_r_((uint8_t*)&ret, pBuf, sizeof(int32_t));
    }
    else {
        memcpy((void*)&ret, pBuf, sizeof(int32_t));
    }

    return ret;
}


/* mode: 0-小端 1-大端 */
int16_t buf2int16_(uint8_t* pBuf, uint8_t mode)
{
    if (pBuf == NULL) {
        printf("pBuf is NULL!\n");
    }
    int16_t ret = 0;
    if (mode) {
        memcpy_r_((uint8_t*)&ret, pBuf, sizeof(int16_t));
    }
    else {
        memcpy((void*)&ret, pBuf, sizeof(int16_t));
    }

    return ret;
}


/* mode: 0-小端 1-大端 */
double buf2double_(uint8_t* pBuf, uint8_t mode)
{
    if (pBuf == NULL) {
        printf("pBuf is NULL!\n");
    }
    double ret = 0;
    if (mode) {
        memcpy_r_((uint8_t*)&ret, pBuf, sizeof(double));
    }
    else {
        memcpy((void*)&ret, pBuf, sizeof(double));
    }

    return ret;
}


/* 字符串（最长支持8位）转16进制数 */
int atox_(const char* str, uint8_t len)
{
    int hex = 0;

    if (len > 8) { /* 最长支持8位 */
        len = 8;
    }
    uint8_t i;
    for (i = 0; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            hex = hex * 16 + str[i] - '0';
        }
        else if (str[i] >= 'a' && str[i] <= 'f') {
            hex = hex * 16 + str[i] - 'a' + 10;
        }
        else if (str[i] >= 'A' && str[i] <= 'F') {
            hex = hex * 16 + str[i] - 'A' + 10;
        }
        else {
            return -1;
        }
    }

    return hex;
}


int str2hex_(uint8_t* buf, const char* str, int bufSize)
{
    if (buf == NULL || str == NULL) {
        printf("buf or str is NULL!\n");
        return -1;
    }

    uint16_t j      = 0;
    uint16_t bufLen = 0;
    char     tmp[2] = {0};

    uint16_t i;
    for (i = 0; i < strlen(str); i++) {
        if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f') ||
            (str[i] >= 'A' && str[i] <= 'F')) {
            tmp[j++] = str[i];
            if (j >= 2) {
                buf[bufLen++] = atox_(tmp, 2);
                if (bufLen >= bufSize) {
                    break;
                }
                memset(tmp, 0, 2);
                j = 0;
            }
        }
    }

    return bufLen;
}


int hex2str_(char* str, uint8_t* buf, uint16_t bufLen)
{
    if (buf == NULL || str == NULL) {
        printf("buf or str is NULL!\n");
        return -1;
    }

    uint16_t i;
    for (i = 0; i < bufLen; i++) {
        sprintf(str + i * 2, "%02x", buf[i]);
    }

    return 0;
}
