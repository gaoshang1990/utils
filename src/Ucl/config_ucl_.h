#ifndef _UCL_CONFIG_H
#define _UCL_CONFIG_H

#define UCL_VERSION "0.1"

/* ģ������ */
#define UCL_USE_SLOG 1 /* ������־ģ�� */


#define UCL_EXPORT 1 /* ���ɶ�̬��ʱ��1 ʹ��ʱ��0 */

#ifndef UCL_API
#  define UCL_API
#  ifdef _WIN32
#    if UCL_EXPORT
#      define UCL_API __declspec(dllexport)
#    else
#      define UCL_API __declspec(dllimport)
#    endif
#  else
#    define UCL_API
#  endif
#endif

#endif /* _UCL_CONFIG_H */