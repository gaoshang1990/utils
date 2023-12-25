#ifndef _USER_DEBUG_H_
#define _USER_DEBUG_H_


#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


enum { LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARN, LOG_LEVEL_OFF };


#define USER_ASSERT_ON 1
#define DEBUG_LEVEL    LOG_LEVEL_DEBUG


#define LOG_DEBUG(fmt, ...)                   \
    do {                                      \
        if (DEBUG_LEVEL <= LOG_LEVEL_DEBUG) { \
            printf("[DEBUG] | ");             \
            printf(fmt, ##__VA_ARGS__);       \
            printf("\n");                     \
        }                                     \
    } while (0)

#define LOG_INFO(fmt, ...)                   \
    do {                                     \
        if (DEBUG_LEVEL <= LOG_LEVEL_INFO) { \
            printf("[INFO ] | ");            \
            printf(fmt, ##__VA_ARGS__);      \
            printf("\n");                    \
        }                                    \
    } while (0)

#define LOG_WARN(fmt, ...)                               \
    do {                                                 \
        if (DEBUG_LEVEL <= LOG_LEVEL_WARN) {             \
            printf("[WARN ] ");                          \
            printf("%s: %d | ", __FUNCTION__, __LINE__); \
            printf(fmt, ##__VA_ARGS__);                  \
            printf("\n");                                \
        }                                                \
    } while (0)

#define LOG_DEBUG_RAW(fmt, ...)         \
    do {                                \
        if (DEBUG_LEVEL <= LEVEL_DEBUG) \
            printf(fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_INFO_RAW(fmt, ...)          \
    do {                                \
        if (DEBUG_LEVEL <= LEVEL_INFO)  \
            printf(fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_WARN_RAW(fmt, ...)          \
    do {                                \
        if (DEBUG_LEVEL <= LEVEL_WARN)  \
            printf(fmt, ##__VA_ARGS__); \
    } while (0)


#if USER_ASSERT_ON
#  define ASSERT_(expression_, fmt, ...)    \
      do {                                  \
          if (!(expression_)) {             \
              LOG_WARN(fmt, ##__VA_ARGS__); \
              while (1)                     \
                  ;                         \
          }                                 \
      } while (0)
#else
#  define ASSERT_(expression_, fmt, ...)
#endif


#ifdef __cplusplus
}
#endif

#endif /* _USER_DEBUG_H_ */