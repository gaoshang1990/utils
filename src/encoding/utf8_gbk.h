/*
 * convert.h
 *
 *  Created on: 2016-8-6
 *      Author: wilson
 */

#ifndef _UTF8_GBK_H_
#define _UTF8_GBK_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   判断是否是UTF-8
 */
bool is_utf8(const char* str);

/**
 * @brief   判断是否是GBK
 */
bool is_gbk(const char* str);

/**
 * @brief       gbk to utf8
 * @param[in]   pin_buf 输入缓冲区
 * @param[in]   in_len  输入长度
 * @param[out]  ptr     输出缓冲区
 * @return      转换后的长度, < 0 失败, 其它成功
 */
int gbk2utf8(char** ptr, const char* pin_buf, int in_len);

/**
 * @brief       utf8 to gbk
 * @param[in]   pin_buf 输入缓冲区
 * @param[in]   in_len  输入长度
 * @param[out]  ptr     输出缓冲区
 * @return      转换后的长度, < 0 失败, 其它成功
 */
int utf82gbk(char** ptr, const char* pin_buf, int in_len);


#ifdef __cplusplus
}
#endif

#endif /* _UTF8_GBK_H_ */
