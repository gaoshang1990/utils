#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_s.h"


/* bcd to hex: 0x10 -> 10(0x0a) */
uint8_t bcd2hex_(uint8_t bcd)
{
    uint8_t lowerNibble = bcd & 0x0f;
    uint8_t upperNibble = (bcd >> 4) & 0x0f;

    if (lowerNibble > 9 || upperNibble > 9)
        return 0;

    return upperNibble * 10 + lowerNibble;
}


/* hex to bcd: 0x10(16) -> 0x16 */
uint8_t hex2bcd_(uint8_t hex)
{
    if (hex > 99)
        return 0x99;

    uint8_t lowerNibble = hex % 10;
    uint8_t upperNibble = (hex / 10) % 10;

    return (upperNibble << 4) | lowerNibble;
}


/* memcpy in an opposite direction */
int memcpy_r_(uint8_t* dst, uint8_t* src, int len)
{
    if (dst == NULL || src == NULL)
        return -1;

    for (int i = 0; i < len; i++)
        dst[i] = src[len - i - 1];

    return 0;
}


/* memory data inversion of specified length */
int memrev_(uint8_t* buf, int len)
{
    for (int i = 0; i < len / 2; i++) {
        uint8_t tmp      = buf[i];
        buf[i]           = buf[len - i - 1];
        buf[len - i - 1] = tmp;
    }

    return 0;
}


/**
 * \param   mode: 0-little endian, 1-big endian
 */
int16_t buf2int16_(uint8_t* buf, uint16_t* offset, int mode)
{
    if (buf == NULL) {
        printf("buf is NULL!\n");
        return -1;
    }

    if (offset)
        buf += *offset;

    int16_t ret = (mode == UCL_BIG_ENDIAN) ? (buf[0] << 8) | buf[1] : (buf[1] << 8) | buf[0];

    if (offset)
        *offset += sizeof(int16_t);

    return ret;
}


/**
 * \param   mode: 0-little endian, 1-big endian
 */
int32_t buf2int32_(uint8_t* buf, uint16_t* offset, int mode)
{
    if (buf == NULL) {
        printf("buf is NULL!\n");
        return -1;
    }
    if (offset)
        buf += *offset;

    int32_t ret = (mode == UCL_BIG_ENDIAN) ? (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3] :
                                             (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

    if (offset)
        *offset += sizeof(int32_t);

    return ret;
}


/**
 * \param   mode: 0-little endian, 1-big endian
 */
int64_t buf2int64_(uint8_t* buf, uint16_t* offset, int mode)
{
    if (buf == NULL) {
        printf("buf is NULL!\n");
        return -1;
    }
    if (offset)
        buf += *offset;

    int64_t ret = 0;
    if (mode == UCL_BIG_ENDIAN) {
        for (int i = 0; i < 8; i++)
            ret = (ret << 8) | buf[i];
    }
    else {
        for (int i = 7; i >= 0; i--)
            ret = (ret << 8) | buf[i];
    }

    if (offset)
        *offset += sizeof(int64_t);

    return ret;
}

float buf2float_(uint8_t* buf, uint16_t* offset, int mode)
{
    int32_t ret = buf2int32_(buf, offset, mode);
    float*  p   = (float*)&ret;

    return *p;
}


/* mode: 0-little endian, 1-big endian */
double buf2double_(uint8_t* buf, uint16_t* offset, int mode)
{
    int64_t ret = buf2int64_(buf, offset, mode);
    double* p   = (double*)&ret;

    return *p;
}

uint64_t BinToNWords(uint8_t* pSrcBuf, uint8_t byteLen)
{
    uint64_t value = 0;

    while (byteLen > 0) {
        if (byteLen <= 6) {
            value = value * 100;
            value += (uint64_t)pSrcBuf[byteLen - 1];
        }
        byteLen--;
    }

    return value;
}


int64_t buf2int_(uint8_t* buf, int* offset, int len, int mode)
{
    if (offset)
        buf += *offset;

    if (len > sizeof(int64_t))
        len = sizeof(int64_t);

    int64_t ret = 0;
    for (int i = 0; i < len; i++) {
        int pos = (mode == UCL_BIG_ENDIAN) ? i : len - 1 - i;

        ret = ret << 8;
        ret += buf[pos];
    }

    if (offset)
        *offset += len;

    return ret;
}


/* string to number, strlen should <= 8 */
int atox_(const char* str, int len)
{
    if (len > 8)
        len = 8;

    int hex = 0;

    for (uint8_t i = 0; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9')
            hex = hex * 16 + str[i] - '0';
        else if (str[i] >= 'a' && str[i] <= 'f')
            hex = hex * 16 + str[i] - 'a' + 10;
        else if (str[i] >= 'A' && str[i] <= 'F')
            hex = hex * 16 + str[i] - 'A' + 10;
        else
            return -1;
    }

    return hex;
}


/**
 * \brief   string turn to uint8 array:
 *          "680100" -> [0x68, 0x01, 0x00]
 */
int str2hex_(uint8_t* buf, int bufSize, const char* str)
{
    if (buf == NULL || str == NULL) {
        printf("buf or str is NULL!\n");
        return -1;
    }

    uint16_t j      = 0;
    uint16_t bufLen = 0;
    char     tmp[2] = {0};

    for (uint16_t i = 0; i < strlen(str); i++) {
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


/**
 * \brief   uint8 array turn to string:
 *          [0x68, 0x01, 0x00] -> "680100"
 */
int hex2str_(char* str, uint8_t* buf, int bufLen)
{
    if (buf == NULL || str == NULL || bufLen <= 0) {
        printf("buf/str is NULL, or bufLen <= 0\n");
        return -1;
    }

    for (int i = 0; i < bufLen; i++)
        sprintf(str + i * 2, "%02x", buf[i]);

    return 0;
}


int reduceGap_(uint8_t* data, int len, uint8_t gap)
{
    if (len <= 0 || data == NULL) {
        printf("len[%d] <= 0 or data is null", len);
        return -1;
    }

    for (int i = 0; i < len; i++)
        data[i] -= gap;

    return 0;
}


int addGap_(uint8_t* data, int len, uint8_t gap)
{
    if (len <= 0 || data == NULL) {
        printf("len[%d] <= 0 or data is null", len);
        return -1;
    }

    for (int i = 0; i < len; i++)
        data[i] += gap;

    return 0;
}
