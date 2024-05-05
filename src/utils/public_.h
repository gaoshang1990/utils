#ifndef _UCL_PUBLIC_H_
#define _UCL_PUBLIC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define UCL_VERSION "0.1"

#ifndef UCL_API
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


#ifndef NULL
#  define NULL ((void*)0)
#endif
#ifndef TRUE
#  define TRUE (1)
#endif
#ifndef FALSE
#  define FALSE (0)
#endif

/* OS */
#ifdef _WIN32
#  define _ACCESS _access
#  define _FILENO _fileno
#else
#  define _ACCESS access
#  define _FILENO fileno
#endif


#define ffree_(p)           \
    do {                    \
        if (p) {            \
            free((void*)p); \
            p = NULL;       \
        }                   \
    } while (0)


const char* log_file_path(const char* strAppName);
const char* config_file_path(const char* strAppName, const char* strFileName);
const char* history_file_path(const char* strAppName);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_PUBLIC_H_ */
