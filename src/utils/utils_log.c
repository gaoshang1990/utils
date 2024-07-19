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

#include "utils_log.h"


#ifdef _WIN32
#  define mkdir(path, arg)      _mkdir(path)
#  define access                _access
#  define snprintf              _snprintf
#  define LOCAL_TIME(pSec, pTm) localtime_s(pTm, pSec)
#else
#  define LOCAL_TIME(pSec, pTm) localtime_r(pSec, pTm)
#endif

#define MLOG_FILE_MAX_SIZE   (5 * 1024 * 1024) /* max size of logfile */
#define MLOG_FILE_MAX_ROTATE (5)               /* rotate file max num */
#define MAX_TIME_STR         (20)
#define MAX_FILE_PATH_LEN    (256)
#define LINE_CONTENT_MAX_LEN (10 * 1024)
#define LINE_PREFIX_MAX_LEN  (256)


/**
 * CSI(Control Sequence Introducer/Initiator) sign
 * more information on https://en.wikipedia.org/wiki/ANSI_escape_code
 */
#define CSI_START            "\033["
#define CSI_END              "\033[0m"
/* output log front color */
#define F_BLACK              "30;"
#define F_RED                "31;"
#define F_GREEN              "32;"
#define F_YELLOW             "33;"
#define F_BLUE               "34;"
#define F_MAGENTA            "35;"
#define F_CYAN               "36;"
#define F_WHITE              "37;"
/* output log background color */
#define B_NULL
#define B_BLACK          "40;"
#define B_RED            "41;"
#define B_GREEN          "42;"
#define B_YELLOW         "43;"
#define B_BLUE           "44;"
#define B_MAGENTA        "45;"
#define B_CYAN           "46;"
#define B_WHITE          "47;"
/* output log fonts style */
#define STYLE_BOLD       "1m"
#define STYLE_UNDERLINE  "4m"
#define STYLE_BLINK      "5m"
#define STYLE_NORMAL     "22m"

/* change the some level logs to not default color if you want */
#define MLOG_COLOR_ERROR F_RED B_NULL STYLE_NORMAL
#define MLOG_COLOR_WARN  F_YELLOW B_NULL STYLE_NORMAL
#define MLOG_COLOR_INFO  F_CYAN B_NULL STYLE_NORMAL
#define MLOG_COLOR_DEBUG F_GREEN B_NULL STYLE_NORMAL
#define MLOG_COLOR_TRACE F_BLUE B_NULL STYLE_NORMAL


#define VSNSPRINTF_WRAPPER(buf, buf_size, fmt)  \
    do {                                        \
        va_list args___;                        \
        va_start(args___, fmt);                 \
        vsnprintf(buf, buf_size, fmt, args___); \
        va_end(args___);                        \
    } while (0)


typedef void* MLogMutex_t;

typedef struct _LoggerCfg_ {
    int         id;
    int         level;
    char        dir[128];
    char        name[64];
    FILE*       fp;
    MLogMutex_t mtx;
    int         max_size; /* file max size */
    int         max_num;  /* max rotate file count */

    bool en_print; /* enable print output to console fd=stdout */
    bool en_color; /* enalbe print color. support on linux/unix platform */

    char line_prefix[LINE_PREFIX_MAX_LEN];   /* prefix of log */
    char line_content[LINE_CONTENT_MAX_LEN]; /* content of log */
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
    memset(logger, 0, sizeof(MLogger_t));

    logger->id       = log_id;
    logger->level    = M_INFO;
    logger->mtx      = _mlog_lock_init(1);
    logger->max_size = MLOG_FILE_MAX_SIZE;
    logger->max_num  = MLOG_FILE_MAX_ROTATE;
    logger->en_print = true;
    logger->en_color = true;

    return logger;
}

/**
 * @brief   自动扩充logger实例, 考虑到日志实例数量不会太多, 每次只增加一个
 */
static void _mlog_grow(void)
{
    MLogger_t** loggers = (MLogger_t**)malloc(sizeof(MLogger_t*) * (_mlog.num + 1));

    memcpy(loggers, _mlog.loggers, sizeof(MLogger_t*) * _mlog.num);
    free(_mlog.loggers);

    _mlog.loggers = loggers;
}

/**
 * @brief   根据log_id获取logger, 如果不存在则创建一个新的
 */
static MLogger_t* _get_logger(int log_id)
{
    static MLogMutex_t _mlog_mtx = NULL;
    if (_mlog_mtx == NULL)
        _mlog_mtx = _mlog_lock_init(1);

    for (int i = 0; i < _mlog.num; i++) {
        if (_mlog.loggers[i]->id == log_id)
            return _mlog.loggers[i];
    }

    MLogger_t* logger = NULL;

    _mlog_lock(_mlog_mtx);
    {
        /* not found, create a new logger */
        _mlog_grow();
        logger                     = _mlog_new(log_id);
        _mlog.loggers[_mlog.num++] = logger;
    }
    _mlog_unlock(_mlog_mtx);

    return logger;
}


/**
 * @brief   日志初始化
 * @return  0: success, -1: failed
 * @note    可以重复调用来更改日志级别
 */
int mlog_init(int log_id, int log_level, const char* file_dir, const char* file_name)
{
    MLogger_t* logger = _get_logger(log_id);

    logger->level = log_level;

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


int mlog_set_level(int log_id, int log_level)
{
    MLogger_t* logger = _get_logger(log_id);

    logger->level = log_level;

    return 0;
}


void mlog_set_print_console(int log_id, bool enable)
{
    MLogger_t* logger = _get_logger(log_id);
    logger->en_print  = enable;
}


void mlog_set_print_color(int log_id, bool enable)
{
    MLogger_t* logger = _get_logger(log_id);
    logger->en_color  = enable;
}


static const char* _level_str(int log_level)
{
    switch (log_level) {
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


static int _make_line_prefix(char* prefix, int log_level, const char* func, int line)
{
    char timestr[MAX_TIME_STR] = {0};
    _make_curr_timestr(timestr, sizeof(timestr)); /* time */

    const char* szlevel = _level_str(log_level);

    /* log format for different level please modify below */
    switch (log_level) {
    case M_WARN:
    case M_ERROR:
        snprintf(prefix,
                 LINE_PREFIX_MAX_LEN - 1,
                 "%s %s- %s: %d | ",
                 szlevel,
                 timestr,
                 func,
                 line);
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


static void _print_in_console(MLogger_t* logger, int log_level)
{
    if (logger->en_print == false)
        return;

    if (logger->en_color) {
        switch (log_level) {
        default:
        case M_TRACE:
            printf(CSI_START MLOG_COLOR_TRACE "%s%s" CSI_END,
                   logger->line_prefix,
                   logger->line_content);
            break;
        case M_DEBUG:
            printf(CSI_START MLOG_COLOR_DEBUG "%s%s" CSI_END,
                   logger->line_prefix,
                   logger->line_content);
            break;
        case M_INFO:
            printf(CSI_START MLOG_COLOR_INFO "%s%s" CSI_END,
                   logger->line_prefix,
                   logger->line_content);
            break;
        case M_WARN:
            printf(CSI_START MLOG_COLOR_WARN "%s%s" CSI_END,
                   logger->line_prefix,
                   logger->line_content);
            break;
        case M_ERROR:
            printf(CSI_START MLOG_COLOR_ERROR "%s%s" CSI_END,
                   logger->line_prefix,
                   logger->line_content);
            break;
        }
    }
    else {
        printf("%s%s", logger->line_prefix, logger->line_content);
    }

    return;
}


static void _mlog_file_rotate(MLogger_t* logger)
{
    if (logger->fp == NULL)
        return;

    int         fd = fileno(logger->fp);
    struct stat statbuf;
    fstat(fd, &statbuf);

    int file_size = statbuf.st_size;
    if (file_size >= logger->max_size) {
        fclose(logger->fp);
        logger->fp = NULL;
        _rotate_file(logger);
    }

    return;
}


static void _mlog_file_open(MLogger_t* logger)
{
    if (logger->fp == NULL) {
        char file_path[256] = {0};
        snprintf(file_path, sizeof(file_path) - 1, "%s/%s", logger->dir, logger->name);
        logger->fp = fopen(file_path, "a+");
    }

    return;
}


static void _mlog_file_write(MLogger_t* logger)
{
    _mlog_file_open(logger);

    const char* prefix  = logger->line_prefix;
    const char* content = logger->line_content;

    if (logger->fp) {
        fwrite(prefix, sizeof(char), strlen(prefix), logger->fp);
        fwrite(content, sizeof(char), strlen(content), logger->fp);
        fflush(logger->fp);
    }

    return;
}


/*!
 * warn: The maximum length of the output content at one time is 10KB,
 * and exceeding the length will result in truncation.
 * Through MAX_LOG_LINE macro can modify maximum length
 */
void mlog_write(int         log_id,
                int         log_level,
                bool        is_raw,
                const char* szFunc,
                int         line,
                const char* fmt,
                ...)
{
    MLogger_t* logger = _get_logger(log_id);
    if (logger->level > log_level)
        return;

    if (log_level > M_ERROR)
        log_level = M_ERROR;

    _mlog_lock(logger->mtx);
    {
        char* line_content = logger->line_content;
        char* line_prefix  = logger->line_prefix;
        line_content[0]    = '\0';
        line_prefix[0]     = '\0';

        VSNSPRINTF_WRAPPER(line_content, LINE_CONTENT_MAX_LEN - 1, fmt);

        if (is_raw == false) {
            _make_line_prefix(line_prefix, log_level, szFunc, line);
            snprintf(line_content + strlen(line_content),
                     LINE_CONTENT_MAX_LEN - strlen(line_content) - 1,
                     "\n");
        }

        _print_in_console(logger, log_level);
        _mlog_file_rotate(logger);
        _mlog_file_write(logger);
    }
    _mlog_unlock(logger->mtx);

    return;
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

/**
 * @brief   make star string with '*'
 */
static void _make_star_str(char* str, size_t nb)
{
    for (size_t i = 0; i < nb; i++)
        str[i] = '*';

    return;
}

/**
 * @brief   make info string with ' *' at the end
 */
static void _make_info_str(char* str, size_t nb)
{
    for (size_t i = strlen(str); i < nb - 1; i++)
        str[i] = ' ';

    str[nb - 1] = '*';

    return;
}


int print_app_info(const char* name,
                   const char* version,
                   const char* date,
                   const char* time)
{
    char stars[128]    = {0};
    char app_info[128] = {0};
    char app_ver[128]  = {0};
    char app_date[128] = {0};

    snprintf(app_info, sizeof(app_info), "* This is \"%s\" App", name);
    snprintf(app_ver, sizeof(app_ver), "* Version: %s", version);
    snprintf(app_date, sizeof(app_date), "* Build time: %s, %s", date, time);

    /* get the max length of the three strings */
    int max_len = strlen(app_info);
    if (strlen(app_ver) > max_len)
        max_len = strlen(app_ver);
    if (strlen(app_date) > max_len)
        max_len = strlen(app_date);

    max_len += 2; /* add 2 for ' ' and '*' */

    _make_star_str(stars, max_len);
    _make_info_str(app_info, max_len);
    _make_info_str(app_ver, max_len);
    _make_info_str(app_date, max_len);

    SLOG_INFO_RAW("\n");
    SLOG_INFO("%s", stars);
    SLOG_INFO("%s", app_info);
    SLOG_INFO("%s", app_ver);
    SLOG_INFO("%s", app_date);
    SLOG_INFO("%s", stars);
    SLOG_INFO_RAW("\n");

    return 0;
}
