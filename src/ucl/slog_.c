/****************** Copyright (c) 2019 南京大全电气研究院有限公司*****************
 * 程序名称：
 * 版本号：1.0
 * 功  能：日志管理模块，生成历史日志文件
 * 开发人：txx
 * 开发时间：2019.12.16
 * 修改者：
 * 修改时间：
 * 修改简要说明：参考开源easylogger做简化
***********************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#    include <Windows.h>
#    include <direct.h>
#    include <io.h>
#else
#    include <execinfo.h>
#    include <unistd.h>
#endif

#include "hal_thread.h"

#include "slog_.h"


#ifdef _WIN32
#    define snprintf              _snprintf
#    define _FILENO               _fileno
#    define _ACCESS               _access
#    define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#else
#    define _FILENO               fileno
#    define _ACCESS               access
#    define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#    define SLOG_COLOR_ENABLE     /* enalbe print color. support on linux/unix platform */
#endif


#define SLOG_PRINT_ENABLE                      /* enalbe print output to console fd=stdout */
#define SLOG_FILE_MAX_SIZE   (5 * 1024 * 1024) /* max size of logfile */
#define SLOG_FILE_MAX_ROTATE (5)               /* rotate file max num */
#define MAX_TIME_STR         (20)
#define TIME_STR_FMT         "%04d-%02d-%02d %02d:%02d:%02d"
#define MAX_FILE_PATH        (260)
#define MAX_LOG_LINE         (10 * 1024)

#ifndef TRUE
#    define TRUE (1)
#endif
#ifndef FALSE
#    define FALSE (0)
#endif

#ifdef SLOG_COLOR_ENABLE
/**
 * CSI(Control Sequence Introducer/Initiator) sign
 * more information on https://en.wikipedia.org/wiki/ANSI_escape_code
 */
#define CSI_START       "\033["
#define CSI_END         "\033[0m"
/* output log front color */
#define F_BLACK         "30;"
#define F_RED           "31;"
#define F_GREEN         "32;"
#define F_YELLOW        "33;"
#define F_BLUE          "34;"
#define F_MAGENTA       "35;"
#define F_CYAN          "36;"
#define F_WHITE         "37;"
/* output log background color */
#define B_NULL
#define B_BLACK         "40;"
#define B_RED           "41;"
#define B_GREEN         "42;"
#define B_YELLOW        "43;"
#define B_BLUE          "44;"
#define B_MAGENTA       "45;"
#define B_CYAN          "46;"
#define B_WHITE         "47;"
/* output log fonts style */
#define S_BOLD          "1m"
#define S_UNDERLINE     "4m"
#define S_BLINK         "5m"
#define S_NORMAL        "22m"

/*---------------------------------------------------------------------------*/
/* enable log color */

/* change the some level logs to not default color if you want */
//#define SLOG_COLOR_ASSERT							 F_MAGENTA B_NULL S_NORMAL
#define SLOG_COLOR_ERROR F_RED B_NULL S_NORMAL
#define SLOG_COLOR_WARN F_YELLOW B_NULL S_NORMAL
#define SLOG_COLOR_INFO F_CYAN B_NULL S_NORMAL
#define SLOG_COLOR_DEBUG F_GREEN B_NULL S_NORMAL
#define SLOG_COLOR_TRACE F_BLUE B_NULL S_NORMAL

#endif /* SLOG_COLOR_ENABLE */

#define SLOG_MUTEX Semaphore

typedef struct _T_LoggerCfg {
    char       filePath[128]; /* file path */
    char       fileName[64];  /* file name */
    FILE*      logFd;         /* log file pointer */
    SLOG_MUTEX mtx;           /* mutex for safety */
    SlogLevel  logLevel;      /* output levle control*/
    int        inited;        /* initial flag*/
    int        fileMaxSize;   /* file max size */
    int        maxRotateCnt;  /* max rotate file count */
} LoggerCfg_t;

static LoggerCfg_t g_loggerCfg =
    {{0}, {0}, NULL, NULL, S_INFO, FALSE, SLOG_FILE_MAX_SIZE, SLOG_FILE_MAX_ROTATE};


static SLOG_MUTEX s_slog_init_mutex(void)
{
    return Semaphore_create(1);
}

static void s_slog_lock(SLOG_MUTEX mtx)
{
    Semaphore_wait(mtx);
}


static void s_slog_unlock(SLOG_MUTEX mtx)
{
    Semaphore_post(mtx);
}


static char* s_slog_get_process_info(void)
{
    static char cur_process_info[10] = {0};
    snprintf(cur_process_info, 10, "pid: %04d", Thread_getPID());

    return cur_process_info;
}


static char* s_slog_get_thread_info(void)
{
    static char cur_thread_info[10] = {0};
    snprintf(cur_thread_info, 10, "tid: %04d", Thread_getTID());

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


static const char* s_get_level_str(SlogLevel level)
{
    switch (level)
    {
    case S_TRACE:
        return "[TRACE]";
    case S_DEBUG:
        return "[DEBUG]";
    case S_INFO:
        return "[INFO ]";
    case S_WARN:
        return "[WARN ]";
    case S_ERROR:
        return "[ERROR]";
    default:
        return "[     ]";
    }
}


static int s_slog_mkdir(const char* log_dir) /* TODO..创建多级目录 */
{
#ifdef _WIN32
    if (_mkdir(log_dir) != 0) {
        return FALSE;
    }
#else
    if (mkdir(log_dir, 0744) != 0) {
        return FALSE;
    }
#endif
    return TRUE;
}


/* mv xxx_1.log => xxx_n.log, and xxx.log => xxx_0.log */
static void s_slog_file_rotate(void)
{
    char fileName[256] = {0}; /* file name without suffix */
    char suffix[16]    = {0}; /* file suffix */
    sscanf(g_loggerCfg.fileName, "%[^.]%s", fileName, suffix);

    char oldPath[256]  = {0};
    char newPath[256]  = {0};

    snprintf(oldPath, sizeof(oldPath) - 1, "%s/%s", g_loggerCfg.filePath, fileName);
    size_t baseLen = strlen(oldPath);
    strncpy(newPath, oldPath, baseLen);

    int           n;
    const uint8_t suffixLen = 10;
    for (n = g_loggerCfg.maxRotateCnt - 1; n >= 0; --n) {
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


/* external function */

/**
 * \param   logdir      log file path
 * \param   filename    log file name
 * \param   level       log output level
 */
int slogInit_(const char* log_dir, const char* file_name, SlogLevel level)
{
    char log_filepath[MAX_FILE_PATH] = {0};

    if (log_dir == NULL || file_name == NULL) {
        return FALSE;
    }
    g_loggerCfg.logLevel = level;
    if (TRUE == g_loggerCfg.inited) {
        return TRUE;
    }
    if (_ACCESS(log_dir, 0) != 0) {
        if (s_slog_mkdir(log_dir) != TRUE) {
            return FALSE;
        }
    }
    g_loggerCfg.mtx = s_slog_init_mutex();

    snprintf(g_loggerCfg.filePath, sizeof(g_loggerCfg.filePath) - 1, "%s", log_dir);
    snprintf(g_loggerCfg.fileName, sizeof(g_loggerCfg.fileName) - 1, "%s", file_name);
    snprintf(log_filepath, sizeof(log_filepath) - 1, "%s/%s", log_dir, g_loggerCfg.fileName);
    g_loggerCfg.logFd = fopen(log_filepath, "a+");
    if (NULL == g_loggerCfg.logFd) {
        printf("open log file failed!\n");
        return FALSE;
    }
    g_loggerCfg.inited = TRUE;

    return TRUE;
}


void writeLog_(SlogLevel level, int braw, const char *func_name, int line, const char *fmt, ...)
{
    va_list     args;
    const char* level_str                 = NULL;
    char*       process_info              = NULL;
    char*       thread_info               = NULL;
    char        timestr[MAX_TIME_STR]     = {0};
    static char log_content[MAX_LOG_LINE] = {0};
    static char log_line[MAX_LOG_LINE]    = {0};

    if (g_loggerCfg.logLevel > level || g_loggerCfg.inited != TRUE) {
        return;
    }

    s_slog_lock(g_loggerCfg.mtx);

    va_start(args, fmt);
    vsnprintf(log_content, sizeof(log_content) - 1, fmt, args);
    va_end(args);

#ifdef SLOG_PRINT_ENABLE     /* console print enable */
#    ifdef SLOG_COLOR_ENABLE /* console print with color */
    switch (level) {
    case S_TRACE:
        printf(CSI_START SLOG_COLOR_TRACE "%s" CSI_END, log_content);
        break;
    case S_DEBUG:
        printf(CSI_START SLOG_COLOR_DEBUG "%s" CSI_END, log_content);
        break;
    case S_INFO:
        printf(CSI_START SLOG_COLOR_INFO "%s" CSI_END, log_content);
        break;
    case S_WARN:
        printf(CSI_START SLOG_COLOR_WARN "%s" CSI_END, log_content);
        break;
    case S_ERROR:
        printf(CSI_START SLOG_COLOR_ERROR "%s" CSI_END, log_content);
        break;
    default:
        printf("%s", log_content);
    }
#    else
    printf("%s", log_content);
#    endif /* SLOG_COLOR_ENABLE */
#endif     /* SLOG_PRINT_ENABLE */

    if (!braw) {
        s_get_curr_time(sizeof(timestr), timestr); /* time */
        level_str    = s_get_level_str(level);     /* log level */
        process_info = s_slog_get_process_info();  /* pid */
        thread_info  = s_slog_get_thread_info();   /* tid */

        /* log format for different level please modify below */
        switch (level) {
            case S_TRACE:
            case S_DEBUG:
            case S_INFO:
                snprintf(log_line,
                         sizeof(log_line) - 1,
                         "%s %s | %s\n",
                         level_str,
                         timestr,
                         log_content);
                break;
            case S_WARN:
            case S_ERROR:
                snprintf(log_line,
                         sizeof(log_line) - 1,
                         "%s %s- %s:%d | %s\n",
                         level_str,
                         timestr,
                         func_name,
                         line,
                         log_content);
                break;
            default:
                snprintf(log_line,
                         sizeof(log_line) - 1,
                         "%s %s [%s %s]- %s:%d | %s\n",
                         level_str,
                         timestr,
                         process_info,
                         thread_info,
                         func_name,
                         line,
                         log_content);
        }
    }
    else {
        snprintf(log_line, sizeof(log_line) - 1, "%s", log_content);  /* output raw data */
    }

    /* log_file_rotate_check */
    struct stat statbuf;
    statbuf.st_size = 0;
    if (g_loggerCfg.logFd) {
        fstat(_FILENO(g_loggerCfg.logFd), &statbuf);
    }
    if (statbuf.st_size >= g_loggerCfg.fileMaxSize) {
        fclose(g_loggerCfg.logFd);
        g_loggerCfg.logFd = NULL;
        s_slog_file_rotate();
    }
    /* reopen the log file */
    if (g_loggerCfg.logFd == NULL) {
        char full_file_name[256] = {0};
        snprintf(full_file_name,
                 sizeof(full_file_name) - 1,
                 "%s/%s",
                 g_loggerCfg.filePath,
                 g_loggerCfg.fileName);
        g_loggerCfg.logFd = fopen(full_file_name, "a+");
    }

    if (g_loggerCfg.logFd) {
        fwrite(log_line, sizeof(char), strlen(log_line), g_loggerCfg.logFd);
        fflush(g_loggerCfg.logFd);
    }
    
    s_slog_unlock(g_loggerCfg.mtx);
}
