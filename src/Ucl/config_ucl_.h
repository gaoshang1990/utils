#ifndef _UCL_CONFIG_H
#define _UCL_CONFIG_H

#define UCL_VERSION "0.1"

/* 模块配置 */
#define UCL_USE_SLOG 1 /* 启用日志模块 */


#define UCL_EXPORT 1 /* 生成动态库时置1 使用时置0 */

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