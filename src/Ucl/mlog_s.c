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

#include "mlog_s.h"


#ifdef _WIN32
#  define mkdir_(path, arg)     _mkdir(path)
#  define access                _access
#  define snprintf              _snprintf
#  define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#  define MLOG_COLOR_ENABLE     /* enalbe print color. support on linux/unix platform */
#else
#  define mkdir_(path, arg)     mkdir(path, arg)
#  define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#  define MLOG_COLOR_ENABLE     /* enalbe print color. support on linux/unix platform */
#endif


#define MLOG_PRINT_ENABLE                      /* enalbe print output to console fd=stdout */
#define MLOG_FILE_MAX_SIZE   (5 * 1024 * 1024) /* max size of logfile */
#define MLOG_FILE_MAX_ROTATE (5)               /* rotate file max num */
#define MAX_TIME_STR         (20)
#define TIME_STR_FMT         "%04d-%02d-%02d %02d:%02d:%02d"
#define MAX_FILE_PATH_LEN    (256)
#define MAX_LOG_LINE         (10 * 1024)
#define MAX_LOG_NUM          (16)


#ifdef MLOG_COLOR_ENABLE
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
// #define MLOG_COLOR_ASSERT							 F_MAGENTA B_NULL STYLE_NORMAL
#  define MLOG_COLOR_ERROR F_RED B_NULL STYLE_NORMAL
#  define MLOG_COLOR_WARN  F_YELLOW B_NULL STYLE_NORMAL
#  define MLOG_COLOR_INFO  F_CYAN B_NULL STYLE_NORMAL
#  define MLOG_COLOR_DEBUG F_GREEN B_NULL STYLE_NORMAL
#  define MLOG_COLOR_TRACE F_BLUE B_NULL STYLE_NORMAL

#endif /* MLOG_COLOR_ENABLE */

typedef void* MLogMutex_t;

static const char* s_szLevel[] = {"[TRACE]", "[DEBUG]", "[INFO ]", "[WARN ]", "[ERROR]"};

typedef struct _LoggerCfg_ {
    char        dir[128]; /* file path */
    char        name[64]; /* file name */
    FILE*       fp;       /* log file pointer */
    MLogMutex_t mtx;      /* mutex for safety */
    MLogLevel_t level;    /* output levle control*/
    int         inited;   /* initial flag*/
    int         maxSize;  /* file max size */
    int         maxCnt;   /* max rotate file count */
} MLogger_t;


static MLogger_t* s_loggers[MAX_LOG_NUM] = {NULL};


static MLogMutex_t s_mlog_init_mutex(int initialValue)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, initialValue, 1, NULL);
#else
    MLogMutex_t self = malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, initialValue);
#endif

    return self;
}


static void s_mlog_lock(MLogMutex_t self)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)self, INFINITE);
#else
    sem_wait((sem_t*)self);
#endif
}


static void s_mlog_unlock(MLogMutex_t self)
{
#ifdef _WIN32
    ReleaseSemaphore((HANDLE)self, 1, NULL);
#else
    sem_post((sem_t*)self);
#endif
}


static char* s_mlog_get_process_info(void)
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


static char* s_mlog_get_thread_info(void)
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
static void s_mlog_file_rotate(int logNo)
{
    char fileName[256] = {0}; /* file name without suffix */
    char suffix[16]    = {0}; /* file suffix */
    sscanf(s_loggers[logNo]->name, "%[^.]%s", fileName, suffix);

    char oldPath[256] = {0};
    char newPath[256] = {0};

    snprintf(oldPath, sizeof(oldPath) - 1, "%s/%s", s_loggers[logNo]->dir, fileName);
    size_t baseLen = strlen(oldPath);
    strncpy(newPath, oldPath, baseLen);

    const uint8_t suffixLen = 10;
    for (int n = s_loggers[logNo]->maxCnt - 1; n >= 0; --n) {
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
 * \brief   1. non thread-safe function
 *          2. can be repeatedly called to change the log level
 */
int mlogInit_(int logNo, const char* logDir, const char* fileName, MLogLevel_t level)
{
    if (logNo < 0 || logNo >= MAX_LOG_NUM) {
        printf("mlogInit_: logNo[%d] is invalid\n", logNo);
        return -1;
    }

    if (s_loggers[logNo] == NULL) {
        s_loggers[logNo]  = (MLogger_t*)malloc(sizeof(MLogger_t));
        *s_loggers[logNo] = (MLogger_t){{0}, {0}, NULL, NULL, M_TRACE, false, MLOG_FILE_MAX_SIZE, MLOG_FILE_MAX_ROTATE};
    }

    s_loggers[logNo]->level = level;

    if (true == s_loggers[logNo]->inited)
        return 0;


    if (logDir == NULL || fileName == NULL) {
        printf("mlogInit_: log_dir or file_name is NULL\n");
        return -1;
    }

    if (s_mkdir_m_(logDir) < 0) {
        printf("mlogInit_: mkdir[%s] failed\n", logDir);
        return -1;
    }

    char logFilePath[MAX_FILE_PATH_LEN] = {0};

    snprintf(s_loggers[logNo]->dir, sizeof(s_loggers[logNo]->dir) - 1, "%s", logDir);
    snprintf(s_loggers[logNo]->name, sizeof(s_loggers[logNo]->name) - 1, "%s", fileName);
    snprintf(logFilePath, sizeof(logFilePath) - 1, "%s/%s", logDir, s_loggers[logNo]->name);

    s_loggers[logNo]->fp = fopen(logFilePath, "a+");
    if (NULL == s_loggers[logNo]->fp) {
        printf("open log file[%s] failed\n", logFilePath);
        return -1;
    }

    s_loggers[logNo]->mtx    = s_mlog_init_mutex(1);
    s_loggers[logNo]->inited = true;

    return 0;
}


int slogInit_(const char* logDir, const char* fileName, MLogLevel_t level)
{
    return mlogInit_(0, logDir, fileName, level);
}


/*!
 * warn: The maximum length of the output content at one time is 10KB,
 * and exceeding the length will result in truncation.
 * Through MAX_LOG_LINE macro can modify maximum length
 */
void mlogWrite_(int logNo, MLogLevel_t level, bool braw, const char* szFunc, int line, const char* fmt, ...)
{
    static char logContent[MAX_LOG_LINE] = {0};
    static char logOutput[MAX_LOG_LINE]  = {0};

    if (logNo < 0 || logNo >= MAX_LOG_NUM) {
        printf("mlogWrite_: logNo[%d] is invalid\n", logNo);
        return;
    }

    if (s_loggers[logNo]->level > level) {
        return;
    }

    if (level > M_ERROR)
        level = M_TRACE;

    s_mlog_lock(s_loggers[logNo]->mtx);

    va_list args;
    va_start(args, fmt);
    vsnprintf(logContent, sizeof(logContent) - 1, fmt, args);
    va_end(args);

    if (!braw) {
        char timestr[MAX_TIME_STR] = {0};
        s_get_curr_time(sizeof(timestr), timestr);      /* time */
        char* process_info = s_mlog_get_process_info(); /* pid */
        char* thread_info  = s_mlog_get_thread_info();  /* tid */

        /* log format for different level please modify below */
        switch (level) {
        case M_WARN:
        case M_ERROR:
            snprintf(logOutput,
                     sizeof(logOutput) - 1,
                     "%s %s- %s:%d | %s\n",
                     s_szLevel[level],
                     timestr,
                     szFunc,
                     line,
                     logContent);
            break;
        case M_TRACE:
            snprintf(logOutput,
                     sizeof(logOutput) - 1,
                     "%s %s [%s %s]- %s:%d | %s\n",
                     s_szLevel[level],
                     timestr,
                     process_info,
                     thread_info,
                     szFunc,
                     line,
                     logContent);
            break;
        case M_DEBUG:
        case M_INFO:
        default:
            snprintf(logOutput, sizeof(logOutput) - 1, "%s %s | %s\n", s_szLevel[level], timestr, logContent);
            break;
        }
    }
    else {
        snprintf(logOutput, sizeof(logOutput) - 1, "%s", logContent); /* output raw data */
    }

#ifdef MLOG_PRINT_ENABLE   /* console print enable */
#  ifdef MLOG_COLOR_ENABLE /* console print with color */
    switch (level) {
    case M_TRACE:
        printf(CSI_START MLOG_COLOR_TRACE "%s" CSI_END, logOutput);
        break;
    case M_DEBUG:
        printf(CSI_START MLOG_COLOR_DEBUG "%s" CSI_END, logOutput);
        break;
    case M_INFO:
        printf(CSI_START MLOG_COLOR_INFO "%s" CSI_END, logOutput);
        break;
    case M_WARN:
        printf(CSI_START MLOG_COLOR_WARN "%s" CSI_END, logOutput);
        break;
    case M_ERROR:
        printf(CSI_START MLOG_COLOR_ERROR "%s" CSI_END, logOutput);
        break;
    default:
        printf("%s", logOutput);
    }
#  else
    printf("%s", logOutput);
#  endif /* MLOG_COLOR_ENABLE */
#endif   /* MLOG_PRINT_ENABLE */

    if (s_loggers[logNo]->inited == false) {
        s_mlog_unlock(s_loggers[logNo]->mtx);
        return;
    }

    /* log_file_rotate_check */
    int         fd = fileno(s_loggers[logNo]->fp);
    struct stat statbuf;
    fstat(fd, &statbuf);
    int fileSize = statbuf.st_size;
    if (fileSize >= s_loggers[logNo]->maxSize) {
        fclose(s_loggers[logNo]->fp);
        s_loggers[logNo]->fp = NULL;
        s_mlog_file_rotate(logNo);
    }
    /* reopen the log file */
    if (s_loggers[logNo]->fp == NULL) {
        char full_file_name[256] = {0};
        snprintf(full_file_name, sizeof(full_file_name) - 1, "%s/%s", s_loggers[logNo]->dir, s_loggers[logNo]->name);
        s_loggers[logNo]->fp = fopen(full_file_name, "a+");
    }

    if (s_loggers[logNo]->fp) {
        fwrite(logOutput, sizeof(char), strlen(logOutput), s_loggers[logNo]->fp);
        fflush(s_loggers[logNo]->fp);
    }

    s_mlog_unlock(s_loggers[logNo]->mtx);
}
