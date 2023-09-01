#ifndef _UCL_PUBLIC_H_
#define _UCL_PUBLIC_H_

#include <stdint.h>

#include "mlog_s.h"


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


#ifndef sswap_
#  define sswap_(x, y)      \
      do {                  \
          if ((x) != (y)) { \
              (x) ^= (y);   \
              (y) ^= (x);   \
              (x) ^= (y);   \
          }                 \
      } while (0)
#endif

#define ffree_(p)           \
    do {                    \
        if (p) {            \
            free((void*)p); \
            p = NULL;       \
        }                   \
    } while (0)


int                printBuf_(MLogLevel_t logLevel, const char* str, uint8_t* pBuf, uint16_t bufLen);
extern int         printAppInfo_(const char* szName, const char* szVersion);
extern const char* getLogFilePath_(const char* strAppName);
extern const char* getConfigFilePath_(const char* strAppName, const char* strFileName);
extern const char* getHistoryFilePath_(const char* strAppName);


#ifdef __cplusplus
}
#endif

#endif /* _UCL_PUBLIC_H_ */