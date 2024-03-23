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
 * @brief   �ж��Ƿ���UTF-8
 */
bool is_utf8(const char* str);

/**
 * @brief   �ж��Ƿ���GBK
 */
bool is_gbk(const char* str);

/**
 * @brief       gbk to utf8
 * @param[in]   pin_buf ���뻺����
 * @param[in]   in_len  ���볤��
 * @param[out]  ptr     ���������
 * @return      ת����ĳ���, < 0 ʧ��, �����ɹ�
 */
int gbk2utf8(char** ptr, const char* pin_buf, int in_len);

/**
 * @brief       utf8 to gbk
 * @param[in]   pin_buf ���뻺����
 * @param[in]   in_len  ���볤��
 * @param[out]  ptr     ���������
 * @return      ת����ĳ���, < 0 ʧ��, �����ɹ�
 */
int utf82gbk(char** ptr, const char* pin_buf, int in_len);


#ifdef __cplusplus
}
#endif

#endif /* _UTF8_GBK_H_ */
