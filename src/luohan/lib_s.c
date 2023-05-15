#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* BCD转HEX: 0x10 -> 10(0x0a) */
uint8_t bcd2hex_(uint8_t bcd)
{
    if (((bcd & 0x0f) > 9) || (((bcd >> 4) & 0x0f) > 9)) {
        return 0;
    }
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
int memcpy_r_(uint8_t* dst, uint8_t* src, int len)
{
    if (dst == NULL || src == NULL) {
        return -1;
    }
    int i = 0;
    for (i = 0; i < len; i++) {
        dst[i] = src[len - i - 1];
    }
    return 0;
}


/* 指定长度的内存数据反转 */
int memrev_(uint8_t* buff, int len)
{
    for (int i = 0; i < len / 2; i++) {
        uint8_t tmp       = buff[i];
        buff[i]           = buff[len - i - 1];
        buff[len - i - 1] = tmp;
    }

    return 0;
}


/* mode: 0-小端 1-大端 */
int16_t buf2int16_(uint8_t* buf, uint16_t* offset, uint8_t mode)
{
    if (buf == NULL) {
        printf("buf is NULL!\n");
        return -1;
    }
    if (offset) {
        buf += *offset;
    }
    int16_t ret = mode ? (buf[0] << 8) | buf[1] : (buf[1] << 8) | buf[0];

    if (offset) {
        *offset += sizeof(int16_t);
    }
    return ret;
}


/* mode: 0-小端 1-大端 */
int32_t buf2int32_(uint8_t* buf, uint16_t* offset, uint8_t mode)
{
    if (buf == NULL) {
        printf("buf is NULL!\n");
        return -1;
    }
    if (offset) {
        buf += *offset;
    }
    int32_t ret = mode ? (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3] :
                         (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

    if (offset) {
        offset += 4;
    }
    return ret;
}


/* mode: 0-小端 1-大端 */
int64_t buf2int64_(uint8_t* buf, uint16_t* offset, uint8_t mode)
{
    if (buf == NULL) {
        printf("buf is NULL!\n");
        return -1;
    }
    if (offset) {
        buf += *offset;
    }
    int64_t ret =
        mode ? ((int64_t)buf[0] << 56) | ((int64_t)buf[1] << 48) | ((int64_t)buf[2] << 40) | ((int64_t)buf[3] << 32) |
                   ((int64_t)buf[4] << 24) | ((int64_t)buf[5] << 16) | ((int64_t)buf[6] << 8) | (int64_t)buf[7] :
               ((int64_t)buf[7] << 56) | ((int64_t)buf[6] << 48) | ((int64_t)buf[5] << 40) | ((int64_t)buf[4] << 32) |
                   ((int64_t)buf[3] << 24) | ((int64_t)buf[2] << 16) | ((int64_t)buf[1] << 8) | (int64_t)buf[0];

    if (offset) {
        *offset += sizeof(int64_t);
    }
    return ret;
}


float buf2float_(uint8_t* buf, uint16_t* offset, uint8_t mode)
{
    int32_t ret = buf2int32_(buf, offset, mode);
    float*  p   = (float*)&ret;

    return *p;
}


/* mode: 0-小端 1-大端 */
double buf2double_(uint8_t* buf, uint16_t* offset, uint8_t mode)
{
    int64_t ret = buf2int64_(buf, offset, mode);
    double* p   = (double*)&ret;

    return *p;
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


int str2hex_(uint8_t* buf, int bufSize, const char* str)
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
        if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F')) {
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
