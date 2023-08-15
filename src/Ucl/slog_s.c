#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#  include <windows.h>
#  include <io.h>
#else
#  include <unistd.h>
#  include <pthread.h>
#  include <semaphore.h>
#endif

#include "slog_s.h"


#ifdef _WIN32
#  define mkdir_(path, arg)     _mkdir(path)
#  define access                _access
#  define snprintf              _snprintf
#  define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#  define SLOG_COLOR_ENABLE     /* enalbe print color. support on linux/unix platform */
#else
#  define mkdir_(path, arg)     mkdir(path, arg)
#  define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#  define SLOG_COLOR_ENABLE     /* enalbe print color. support on linux/unix platform */
#endif


#define SLOG_PRINT_ENABLE                      /* enalbe print output to console fd=stdout */
#define SLOG_FILE_MAX_SIZE   (5 * 1024 * 1024) /* max size of logfile */
#define SLOG_FILE_MAX_ROTATE (5)               /* rotate file max num */
#define MAX_TIME_STR         (20)
#define TIME_STR_FMT         "%04d-%02d-%02d %02d:%02d:%02d"
#define MAX_FILE_PATH_LEN                      (256)
#define MAX_LOG_LINE         (10 * 1024)


#ifdef SLOG_COLOR_ENABLE
/**
 * CSI(Control Sequence Introducer/Initiator) sign
 * more information on https://en.wikipedia.org/wiki/ANSI_escape_code
 */
#  define CSI_START "\033["
#  define CSI_END   "\033[0m"
/* output log front color */
#  define F_BLACK   "30;"
#  define F_RED     "31;"
#  define F_GREEN   "32;"
#  define F_YELLOW  "33;"
#  define F_BLUE    "34;"
#  define F_MAGENTA "35;"
#  define F_CYAN    "36;"
#  define F_WHITE   "37;"
/* output log background color */
#  define B_NULL
#  define B_BLACK          "40;"
#  define B_RED            "41;"
#  define B_GREEN          "42;"
#  define B_YELLOW         "43;"
#  define B_BLUE           "44;"
#  define B_MAGENTA        "45;"
#  define B_CYAN           "46;"
#  define B_WHITE          "47;"
/* output log fonts style */
#  define STYLE_BOLD       "1m"
#  define STYLE_UNDERLINE  "4m"
#  define STYLE_BLINK      "5m"
#  define STYLE_NORMAL     "22m"

/*---------------------------------------------------------------------------*/
/* enable log color */

/* change the some level logs to not default color if you want */
// #define SLOG_COLOR_ASSERT							 F_MAGENTA B_NULL STYLE_NORMAL
#  define SLOG_COLOR_ERROR F_RED B_NULL STYLE_NORMAL
#  define SLOG_COLOR_WARN  F_YELLOW B_NULL STYLE_NORMAL
#  define SLOG_COLOR_INFO  F_CYAN B_NULL STYLE_NORMAL
#  define SLOG_COLOR_DEBUG F_GREEN B_NULL STYLE_NORMAL
#  define SLOG_COLOR_TRACE F_BLUE B_NULL STYLE_NORMAL

#endif /* SLOG_COLOR_ENABLE */

typedef void* SlogMutex;

static const char* s_szLevel[] = {"[TRACE]", "[DEBUG]", "[INFO ]", "[WARN ]", "[ERROR]"};

typedef struct _T_LoggerCfg {
    char      fileDir[128]; /* file path */
    char      fileName[64]; /* file name */
    FILE*     fp;           /* log file pointer */
    SlogMutex mtx;          /* mutex for safety */
    SlogLevel logLevel;     /* output levle control*/
    int       inited;       /* initial flag*/
    int       maxSize;      /* file max size */
    int       maxRotateCnt; /* max rotate file count */
} Slogger;

static Slogger s_logger = {{0}, {0}, NULL, NULL, S_TRACE, false, SLOG_FILE_MAX_SIZE, SLOG_FILE_MAX_ROTATE};


static SlogMutex s_slog_init_mutex(int initialValue)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, initialValue, 1, NULL);
#else
    SlogMutex self = malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, initialValue);
#endif

    return self;
}


static void s_slog_lock(SlogMutex self)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)self, INFINITE);
#else
    sem_wait((sem_t*)self);
#endif
}


static void s_slog_unlock(SlogMutex self)
{
#ifdef _WIN32
    ReleaseSemaphore((HANDLE)self, 1, NULL);
#else
    sem_post((sem_t*)self);
#endif
}


static char* s_slog_get_process_info(void)
{
    static char cur_process_info[10] = {0};

    int pid = 0;
#ifdef _WIN32
    pid = (int)GetCurrentProcessId();
#else
    pid = (int)getpid();
#endif
    snprintf(cur_process_info, 10, "pid: %04d", pid);

    return cur_process_info;
}


static char* s_slog_get_thread_info(void)
{
    static char cur_thread_info[10] = {0};

    int tid = 0;
#ifdef _WIN32
    tid = (uint32_t)GetCurrentThreadId();
#else
    tid = (uint32_t)pthread_self();
#endif
    snprintf(cur_thread_info, 10, "tid: %04d", tid);

    return cur_thread_info;
}


static void s_get_curr_time(int timestr_size, char timestr[])
{
    struct tm nowTm;
    time_t    nowSec = time(NULL);
    LOCAL_TIME(&nowSec, &nowTm);

    snprintf(timestr,
             timestr_size,
             TIME_STR_FMT,
             nowTm.tm_year + 1900,
             nowTm.tm_mon + 1,
             nowTm.tm_mday,
             nowTm.tm_hour,
             nowTm.tm_min,
             nowTm.tm_sec);
}


/* move xxx_1.log => xxx_n.log, and xxx.log => xxx_0.log */
static void s_slog_file_rotate(void)
{
    char fileName[256] = {0}; /* file name without suffix */
    char suffix[16]    = {0}; /* file suffix */
    sscanf(s_logger.fileName, "%[^.]%s", fileName, suffix);

    char oldPath[256] = {0};
    char newPath[256] = {0};

    snprintf(oldPath, sizeof(oldPath) - 1, "%s/%s", s_logger.fileDir, fileName);
    size_t baseLen = strlen(oldPath);
    strncpy(newPath, oldPath, baseLen);

    const uint8_t suffixLen = 10;
    for (int n = s_logger.maxRotateCnt - 1; n >= 0; --n) {
        if (n == 0) {
            snprintf(oldPath + baseLen, suffixLen, "%s", suffix);
        }
        else {
            snprintf(oldPath + baseLen, suffixLen, "_%d%s", n - 1, suffix);
        }
        snprintf(newPath + baseLen, suffixLen, "_%d%s", n, suffix);
        remove(newPath);
        rename(oldPath, newPath);
    }
}


/* Create a multi-level directory */
static int s_mkdir_m_(const char* dir)
{
    if (dir == NULL) {
        printf("s_mkdir_m_: dir is NULL\n");
        return -1;
    }

    int len = strlen(dir) + 1;
    if (len <= 0 || len > MAX_FILE_PATH_LEN) {
        printf("s_mkdir_m_: strlen(dir) = %d\n", len);
        return -1;
    }

    char dirTmp[256] = {0};
    strncpy(dirTmp, dir, sizeof(dirTmp) - 1);

    for (int i = 0; i < len; i++) {
        if (dirTmp[i] == '\\' || dirTmp[i] == '/' || dirTmp[i] == '\0') {
            dirTmp[i] = '\0';
            if (strlen(dirTmp) > 0 && access(dirTmp, 0) != 0) {
                if (mkdir_(dirTmp, 0755) != 0) {
                    printf("mkdir[%s] failed\n", dirTmp);
                    return -1;
                }
            }
            dirTmp[i] = dir[i];
        }
    }

    return 0;
}


/* external function */

/**
 * \param   logdir      log file path
 * \param   filename    log file name
 * \param   level       log output level
 */
int slogInit__(const char* log_dir, const char* file_name, SlogLevel level)
{
    s_logger.logLevel = level;

    if (true == s_logger.inited) {
        return 0;
    }

    s_logger.mtx = s_slog_init_mutex(1);

    if (log_dir == NULL || file_name == NULL) {
        return -1;
    }
    if (s_mkdir_m_(log_dir) < 0) {
        printf("mkdir[%s] failed\n", log_dir);
        return -1;
    }

    char log_filepath[MAX_FILE_PATH_LEN] = {0};

    snprintf(s_logger.fileDir, sizeof(s_logger.fileDir) - 1, "%s", log_dir);
    snprintf(s_logger.fileName, sizeof(s_logger.fileName) - 1, "%s", file_name);
    snprintf(log_filepath, sizeof(log_filepath) - 1, "%s/%s", log_dir, s_logger.fileName);
    s_logger.fp = fopen(log_filepath, "a+");
    if (NULL == s_logger.fp) {
        printf("open log file[%s] failed\n", log_filepath);
        return -1;
    }
    s_logger.inited = true;

    return 0;
}


/* warn: The maximum length of the output content at one time is 10KB,
 * and exceeding the length will result in truncation.
 * Through MAX_LOG_LINE macro can modify maximum length
 */
void slogWrite_(SlogLevel level, bool braw, const char* szFunc, int line, const char* fmt, ...)
{
    static char log_content[MAX_LOG_LINE] = {0};
    static char log_line[MAX_LOG_LINE]    = {0};

    if (s_logger.logLevel > level) {
        return;
    }

    if (level > S_ERROR)
        level = S_TRACE;

    s_slog_lock(s_logger.mtx);

    va_list args;
    va_start(args, fmt);
    vsnprintf(log_content, sizeof(log_content) - 1, fmt, args);
    va_end(args);

    if (!braw) {
        char timestr[MAX_TIME_STR] = {0};
        s_get_curr_time(sizeof(timestr), timestr);      /* time */
        char* process_info = s_slog_get_process_info(); /* pid */
        char* thread_info  = s_slog_get_thread_info();  /* tid */

        /* log format for different level please modify below */
        switch (level) {
        case S_DEBUG:
        case S_INFO:
            snprintf(log_line, sizeof(log_line) - 1, "%s %s | %s\n", s_szLevel[level], timestr, log_content);
            break;
        case S_WARN:
        case S_ERROR:
            snprintf(log_line,
                     sizeof(log_line) - 1,
                     "%s %s- %s:%d | %s\n",
                     s_szLevel[level],
                     timestr,
                     szFunc,
                     line,
                     log_content);
            break;
        case S_TRACE:
            snprintf(log_line,
                     sizeof(log_line) - 1,
                     "%s %s [%s %s]- %s:%d | %s\n",
                     s_szLevel[level],
                     timestr,
                     process_info,
                     thread_info,
                     szFunc,
                     line,
                     log_content);
            break;
        }
    }
    else {
        snprintf(log_line, sizeof(log_line) - 1, "%s", log_content); /* output raw data */
    }

#ifdef SLOG_PRINT_ENABLE   /* console print enable */
#  ifdef SLOG_COLOR_ENABLE /* console print with color */
    switch (level) {
    case S_TRACE:
        printf(CSI_START SLOG_COLOR_TRACE "%s" CSI_END, log_line);
        break;
    case S_DEBUG:
        printf(CSI_START SLOG_COLOR_DEBUG "%s" CSI_END, log_line);
        break;
    case S_INFO:
        printf(CSI_START SLOG_COLOR_INFO "%s" CSI_END, log_line);
        break;
    case S_WARN:
        printf(CSI_START SLOG_COLOR_WARN "%s" CSI_END, log_line);
        break;
    case S_ERROR:
        printf(CSI_START SLOG_COLOR_ERROR "%s" CSI_END, log_line);
        break;
    default:
        printf("%s", log_line);
    }
#  else
    printf("%s", log_line);
#  endif /* SLOG_COLOR_ENABLE */
#endif   /* SLOG_PRINT_ENABLE */

    if (s_logger.inited == false) {
        s_slog_unlock(s_logger.mtx);
        return;
    }

    /* log_file_rotate_check */
    int         fd = fileno(s_logger.fp);
    struct stat statbuf;
    fstat(fd, &statbuf);
    int fileSize = statbuf.st_size;
    if (fileSize >= s_logger.maxSize) {
        fclose(s_logger.fp);
        s_logger.fp = NULL;
        s_slog_file_rotate();
    }
    /* reopen the log file */
    if (s_logger.fp == NULL) {
        char full_file_name[256] = {0};
        snprintf(full_file_name, sizeof(full_file_name) - 1, "%s/%s", s_logger.fileDir, s_logger.fileName);
        s_logger.fp = fopen(full_file_name, "a+");
    }

    if (s_logger.fp) {
        fwrite(log_line, sizeof(char), strlen(log_line), s_logger.fp);
        fflush(s_logger.fp);
    }

    s_slog_unlock(s_logger.mtx);
}
