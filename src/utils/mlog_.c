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

#include "mlog_.h"


#ifdef _WIN32
#  define mkdir(path, arg)      _mkdir(path)
#  define access                _access
#  define snprintf              _snprintf
#  define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#  define MLOG_COLOR_ENABLE     (1)
#else
#  define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#  define MLOG_COLOR_ENABLE     (1) /* enalbe print color. support on linux/unix platform */
#endif

#define MLOG_PRINT_ENABLE    (1)               /* enalbe print output to console fd=stdout */
#define MLOG_FILE_MAX_SIZE   (5 * 1024 * 1024) /* max size of logfile */
#define MLOG_FILE_MAX_ROTATE (5)               /* rotate file max num */
#define MAX_TIME_STR         (20)
#define MAX_FILE_PATH_LEN    (256)
#define LINE_CONTENT_MAX_LEN (10 * 1024)
#define LINE_PREFIX_MAX_LEN  (256)


#if MLOG_COLOR_ENABLE
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

/* change the some level logs to not default color if you want */
#  define MLOG_COLOR_ERROR F_RED B_NULL STYLE_NORMAL
#  define MLOG_COLOR_WARN  F_YELLOW B_NULL STYLE_NORMAL
#  define MLOG_COLOR_INFO  F_CYAN B_NULL STYLE_NORMAL
#  define MLOG_COLOR_DEBUG F_GREEN B_NULL STYLE_NORMAL
#  define MLOG_COLOR_TRACE F_BLUE B_NULL STYLE_NORMAL

#endif /* MLOG_COLOR_ENABLE */

typedef void* MLogMutex_t;

typedef struct _LoggerCfg_ {
    int         id;
    char        dir[128];
    char        name[64];
    FILE*       fp;
    MLogMutex_t mtx;
    int         level;
    int         max_size; /* file max size */
    int         max_num;  /* max rotate file count */

    char line_prefix[LINE_PREFIX_MAX_LEN];  /* prefix of log */
    char line_content[LINE_PREFIX_MAX_LEN]; /* content of log */
} MLogger_t;


static struct {
    int         num;
    MLogger_t** loggers;
} _mlog = {0};


static MLogMutex_t _mlog_lock_init(int init_value)
{
#ifdef _WIN32
    HANDLE self = CreateSemaphore(NULL, init_value, 1, NULL);
#else
    MLogMutex_t self = malloc(sizeof(sem_t));
    sem_init((sem_t*)self, 0, init_value);
#endif

    return self;
}


static void _mlog_lock(MLogMutex_t self)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)self, INFINITE);
#else
    sem_wait((sem_t*)self);
#endif
}


static void _mlog_unlock(MLogMutex_t self)
{
#ifdef _WIN32
    ReleaseSemaphore((HANDLE)self, 1, NULL);
#else
    sem_post((sem_t*)self);
#endif
}


static char* _process_info(void)
{
    static char cur_process_info[10] = {0};

    int pid =
#ifdef _WIN32
        (int)GetCurrentProcessId();
#else
        (int)getpid();
#endif
    snprintf(cur_process_info, 10, "pid: %04d", pid);

    return cur_process_info;
}


static char* _thread_info(void)
{
    static char cur_thread_info[10] = {0};

    int tid =
#ifdef _WIN32
        (int)GetCurrentThreadId();
#else
        (int)pthread_self();
#endif
    snprintf(cur_thread_info, 10, "tid: %04d", tid);

    return cur_thread_info;
}


static void _make_curr_timestr(char* timestr, int size)
{
    struct tm now_tm;
    time_t    now_sec = time(NULL);
    LOCAL_TIME(&now_sec, &now_tm);

    snprintf(timestr,
             size,
             "%04u-%02u-%02u %02u:%02u:%02u",
             now_tm.tm_year + 1900,
             now_tm.tm_mon + 1,
             now_tm.tm_mday,
             now_tm.tm_hour,
             now_tm.tm_min,
             now_tm.tm_sec);
}


/* move xxx_1.log => xxx_n.log, and xxx.log => xxx_0.log */
static void _rotate_file(MLogger_t* logger)
{
    char file_name[256] = {0}; /* file name without suffix */
    char suffix[16]     = {0}; /* file suffix */
    sscanf(logger->name, "%[^.]%s", file_name, suffix);

    char old_path[256] = {0};
    char new_path[256] = {0};

    snprintf(old_path, sizeof(old_path) - 1, "%s/%s", logger->dir, file_name);
    size_t base_len = strlen(old_path);
    snprintf(new_path, base_len + 1, "%s", old_path);

    const uint8_t suffix_len = 32;
    for (int n = logger->max_num - 1; n >= 0; --n) {
        if (n == 0)
            snprintf(old_path + base_len, suffix_len, "%s", suffix);
        else
            snprintf(old_path + base_len, suffix_len, "_%d%s", n - 1, suffix);

        snprintf(new_path + base_len, suffix_len, "_%d%s", n, suffix);
        remove(new_path);
        rename(old_path, new_path);
    }
}


/* Create a multi-level directory */
static int _mkdir_m_(const char* dir)
{
    if (dir == NULL) {
        printf("_mkdir_m_: dir is NULL\n");
        return -1;
    }

    int len = strlen(dir) + 1;
    if (len <= 0 || len > MAX_FILE_PATH_LEN) {
        printf("_mkdir_m_: strlen(dir) = %d\n", len);
        return -1;
    }

    char dirTmp[256] = {0};
    strncpy(dirTmp, dir, sizeof(dirTmp) - 1);

    for (int i = 0; i < len; i++) {
        if (dirTmp[i] == '\\' || dirTmp[i] == '/' || dirTmp[i] == '\0') {
            dirTmp[i] = '\0';
            if (strlen(dirTmp) > 0 && access(dirTmp, 0) != 0) {
                if (mkdir(dirTmp, 0755) != 0) {
                    printf("mkdir[%s] failed\n", dirTmp);
                    return -1;
                }
            }
            dirTmp[i] = dir[i];
        }
    }

    return 0;
}


// static int _windows_version(void)
// {
//     static int version = -1;

//     if (version == -1) {
//         OSVERSIONINFO info;
//         info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//         if (GetVersionEx(&info))
//             version = info.dwMajorVersion << 16 | info.dwMinorVersion;
//     }

//     return version;
// }


static MLogger_t* _mlog_new(int log_id)
{
    MLogger_t* logger = (MLogger_t*)malloc(sizeof(MLogger_t));

    *logger     = (MLogger_t){log_id, {0}, {0}, NULL, NULL, M_TRACE, MLOG_FILE_MAX_SIZE, MLOG_FILE_MAX_ROTATE};
    logger->mtx = _mlog_lock_init(1);

    return logger;
}


static void _mlog_grow(void)
{
    MLogger_t** loggers = (MLogger_t**)malloc(sizeof(MLogger_t*) * (_mlog.num + 1));

    memcpy(loggers, _mlog.loggers, sizeof(MLogger_t*) * _mlog.num);
    free(_mlog.loggers);

    _mlog.loggers = loggers;
}


static MLogger_t* _get_logger(int log_id)
{
    for (int i = 0; i < _mlog.num; i++) {
        if (_mlog.loggers[i]->id == log_id)
            return _mlog.loggers[i];
    }

    _mlog_grow();
    _mlog.loggers[_mlog.num] = _mlog_new(log_id);

    return _mlog.loggers[_mlog.num++];
}


/**
 * \brief   1. non thread-safe function
 *          2. can be repeatedly called to change the log level
 */
int mlog_init(int level, int log_no, const char* file_dir, const char* file_name)
{
    MLogger_t* logger = _get_logger(log_no);

    logger->level = level;

    // int version = _windows_version();

    if (file_dir == NULL || file_name == NULL) {
        printf("mlog_init: log_dir or file_name is NULL\n");
        return -1;
    }

    if (_mkdir_m_(file_dir) < 0) {
        printf("mlog_init: mkdir[%s] failed\n", file_dir);
        return -1;
    }

    snprintf(logger->dir, sizeof(logger->dir) - 1, "%s", file_dir);
    snprintf(logger->name, sizeof(logger->name) - 1, "%s", file_name);

    return 0;
}


int mlog_set_level(int level, int log_no)
{
    MLogger_t* logger = _get_logger(log_no);

    logger->level = level;

    return 0;
}


static const char* _level_str(int level)
{
    switch (level) {
    default:
    case M_TRACE:
        return "[TRACE]";
    case M_DEBUG:
        return "[DEBUG]";
    case M_INFO:
        return "[INFO ]";
    case M_WARN:
        return "[WARN ]";
    case M_ERROR:
        return "[ERROR]";
    }
}


static int _make_line_prefix(char* prefix, int level, const char* func, int line)
{
    char timestr[MAX_TIME_STR] = {0};
    _make_curr_timestr(timestr, sizeof(timestr)); /* time */

    const char* szlevel = _level_str(level);

    /* log format for different level please modify below */
    switch (level) {
    case M_WARN:
    case M_ERROR:
        snprintf(prefix, LINE_PREFIX_MAX_LEN - 1, "%s %s- %s: %d | ", szlevel, timestr, func, line);
        break;
    case M_TRACE:
        snprintf(prefix,
                 LINE_PREFIX_MAX_LEN - 1,
                 "%s %s [%s %s] - %s: %d | ",
                 szlevel,
                 timestr,
                 _process_info(),
                 _thread_info(),
                 func,
                 line);
        break;
    case M_DEBUG:
    case M_INFO:
    default:
        snprintf(prefix, LINE_PREFIX_MAX_LEN - 1, "%s %s | ", szlevel, timestr);
        break;
    }

    return 0;
}


static void _print_in_console(bool en_print, bool en_color, int level, const char* prefix, const char* content)
{
    if (!en_print)
        return;

    if (en_color) {
        switch (level) {
        default:
        case M_TRACE:
            printf(CSI_START MLOG_COLOR_TRACE "%s%s" CSI_END, prefix, content);
            break;
        case M_DEBUG:
            printf(CSI_START MLOG_COLOR_DEBUG "%s%s" CSI_END, prefix, content);
            break;
        case M_INFO:
            printf(CSI_START MLOG_COLOR_INFO "%s%s" CSI_END, prefix, content);
            break;
        case M_WARN:
            printf(CSI_START MLOG_COLOR_WARN "%s%s" CSI_END, prefix, content);
            break;
        case M_ERROR:
            printf(CSI_START MLOG_COLOR_ERROR "%s%s" CSI_END, prefix, content);
            break;
        }
    }
    else {
        printf("%s%s", prefix, content);
    }
}


static void _mlog_file_rotate(MLogger_t* logger)
{
    if (logger->fp == NULL)
        return;

    int         fd = fileno(logger->fp);
    struct stat statbuf;
    fstat(fd, &statbuf);

    int fileSize = statbuf.st_size;
    if (fileSize >= logger->max_size) {
        fclose(logger->fp);
        logger->fp = NULL;
        _rotate_file(logger);
    }
}


static void _mlog_file_open(MLogger_t* logger)
{
    if (logger->fp == NULL) {
        char file_path[256] = {0};
        snprintf(file_path, sizeof(file_path) - 1, "%s/%s", logger->dir, logger->name);
        logger->fp = fopen(file_path, "a+");
    }
}


static void _mlog_file_write(MLogger_t* logger, const char* prefix, const char* content)
{
    _mlog_file_open(logger);

    if (logger->fp) {
        fwrite(prefix, sizeof(char), strlen(prefix), logger->fp);
        fwrite(content, sizeof(char), strlen(content), logger->fp);
        fflush(logger->fp);
    }
}


/*!
 * warn: The maximum length of the output content at one time is 10KB,
 * and exceeding the length will result in truncation.
 * Through MAX_LOG_LINE macro can modify maximum length
 */
void mlog_write(int level, int log_id, bool is_raw, const char* func, int line, const char* fmt, ...)
{
    MLogger_t* logger = _get_logger(log_id);

    if (logger->level > level)
        return;

    if (level > M_ERROR)
        level = M_ERROR;

    _mlog_lock(logger->mtx);

    char* line_content = logger->line_content;
    char* line_prefix  = logger->line_prefix;
    line_content[0]    = '\0';
    line_prefix[0]     = '\0';

    va_list args;
    va_start(args, fmt);
    vsnprintf(line_content, LINE_CONTENT_MAX_LEN - 1, fmt, args);
    va_end(args);

    if (!is_raw) {
        _make_line_prefix(line_prefix, level, func, line);
        snprintf(line_content + strlen(line_content), LINE_CONTENT_MAX_LEN - strlen(line_content) - 1, "\n");
    }

    _print_in_console(MLOG_PRINT_ENABLE, MLOG_COLOR_ENABLE, level, line_prefix, line_content);

    _mlog_file_rotate(logger);
    _mlog_file_write(logger, line_prefix, line_content);

    _mlog_unlock(logger->mtx);
}


int print_buf(int log_level, uint8_t* buf, uint16_t buf_len)
{
    if (buf == NULL && buf_len == 0)
        return -1;

    for (int i = 0; i < buf_len; i++)
        SLOG_RAW(log_level, "%02x ", buf[i]);
    SLOG_RAW(log_level, "\n");

    return 0;
}


static int _make_star_str(char* str, uint8_t nb)
{
    for (uint8_t i = 0; i < nb; i++)
        str[i] = '*';

    return 0;
}


static int _make_info_str(char* str, uint8_t nb)
{
    for (uint8_t i = (uint8_t)strlen(str); i < nb - 1; i++)
        str[i] = ' ';

    str[nb - 1] = '*';

    return 0;
}


int print_app_info(const char* name, const char* version, const char* date, const char* time)
{
    char stars[128]    = {0};
    char app_info[128] = {0};
    char app_ver[128]  = {0};
    char app_date[128] = {0};

    snprintf(app_info, sizeof(app_info), "* This is \"%s\" App", name);
    snprintf(app_ver, sizeof(app_ver), "* Version: %s", version);
    snprintf(app_date, sizeof(app_date), "* Build time: %s, %s", date, time);

    int max_len = strlen(app_info);
    if (strlen(app_ver) > max_len)
        max_len = strlen(app_ver);
    if (strlen(app_date) > max_len)
        max_len = strlen(app_date);

    _make_star_str(stars, max_len + 2);
    _make_info_str(app_info, max_len + 2);
    _make_info_str(app_ver, max_len + 2);
    _make_info_str(app_date, max_len + 2);

    SLOG_INFO_RAW("\n");
    SLOG_INFO("%s", stars);
    SLOG_INFO("%s", app_info);
    SLOG_INFO("%s", app_ver);
    SLOG_INFO("%s", app_date);
    SLOG_INFO("%s", stars);
    SLOG_INFO_RAW("\n");

    return 0;
}
