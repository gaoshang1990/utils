#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#  include <dirent.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#include "slog_s.h"
#include "public_.h"


int printBuf_(const char* str, uint8_t* pBuf, uint16_t bufLen)
{
    if (pBuf == NULL || bufLen == 0) {
        return -1;
    }

    if (str != NULL || strlen(str) > 0) {
        SLOG_INFO("%s", str);
    }

    for (int i = 0; i < bufLen; i++) {
        SLOG_INFO_RAW("%02x ", pBuf[i]);
    }
    SLOG_INFO_RAW("\n");

    return 0;
}


static int makeStarStr(char* str, uint8_t nb)
{
    int8_t i = 0;
    for (i = 0; i < nb; i++) {
        str[i] = '*';
    }
    return 0;
}

static int makeInfoStr(char* str, uint8_t nb)
{
    uint8_t i   = 0;
    uint8_t len = (uint8_t)strlen(str);

    for (i = len; i < nb - 1; i++) {
        str[i] = ' ';
    }
    str[nb - 1] = '*';

    return 0;
}

int printAppInfo_(const char* szName, const char* szVersion)
{
    char strStars[128]   = {0};
    char strAppInfo[128] = {0};
    char strAppVer[128]  = {0};
    char strAppDate[128] = {0};

    uint8_t maxLen = 0;

    sprintf(strAppInfo, "* This is \"%s\" App", szName);
    sprintf(strAppVer,  "* Version: %s", szVersion);
    sprintf(strAppDate, "* Build time: %s, %s", __DATE__, __TIME__);

    if (strlen(strAppInfo) > maxLen) {
        maxLen = (uint8_t)strlen(strAppInfo);
    }
    if (strlen(strAppVer) > maxLen) {
        maxLen = (uint8_t)strlen(strAppVer);
    }
    if (strlen(strAppDate) > maxLen) {
        maxLen = (uint8_t)strlen(strAppDate);
    }

    makeStarStr(strStars,   maxLen + 2);
    makeInfoStr(strAppInfo, maxLen + 2);
    makeInfoStr(strAppVer,  maxLen + 2);
    makeInfoStr(strAppDate, maxLen + 2);

    SLOG_INFO_RAW("\n");
    SLOG_INFO("%s", strStars);
    SLOG_INFO("%s", strAppInfo);
    SLOG_INFO("%s", strAppVer);
    SLOG_INFO("%s", strAppDate);
    SLOG_INFO("%s", strStars);
    SLOG_INFO_RAW("\n");

    return 0;
}


/* 获取日志文件内容 */
const char* getLogFilePath_(const char* strAppName)
{
    static char strBuffer[256];

#ifndef _WIN32
    char szAbsAppNamePath[256];
    char szAbsLogFilePath[256];
    sprintf(szAbsAppNamePath, "/data/app/%s/", strAppName);
    sprintf(szAbsLogFilePath, "/data/app/%s/logFile/", strAppName);
    DIR* dp;
    if ((dp = opendir(szAbsAppNamePath)) == NULL) {
        mkdir(szAbsAppNamePath, 0777);
        system("sync");
    }
    else {
        closedir(dp);
    }
    if ((dp = opendir(szAbsLogFilePath)) == NULL) {
        mkdir(szAbsLogFilePath, 0777);
        system("sync");
    }
    else {
        closedir(dp);
    }
    strcpy(strBuffer, szAbsLogFilePath);
#else
    sprintf(strBuffer, "./logFile/");
#endif /* _WIN32 */

    return strBuffer;
}


/* 获取配置文件路径 */
const char* getConfigFilePath_(const char* strAppName, const char* strFileName)
{
    static char strBuffer[256];

#ifndef _WIN32
    char szAbsConfigPath[256];
    char szAbsAppNamePath[256];
    char szTmp[256];

    sprintf(szAbsAppNamePath, "/data/app/%s", strAppName);
    sprintf(szAbsConfigPath, "/data/app/%s/configFile/%s", strAppName, strFileName);

    DIR* dp;

    if ((dp = opendir(szAbsAppNamePath)) == NULL) {
        mkdir(szAbsAppNamePath, 0777);
    }
    else {
        closedir(dp);
    }

    sprintf(szAbsAppNamePath, "/data/app/%s/configFile", strAppName);
    if ((dp = opendir(szAbsAppNamePath)) == NULL) {
        mkdir(szAbsAppNamePath, 0777);
        sprintf(szTmp, "cp ./configFile/%s %s", strFileName, szAbsAppNamePath);
        system(szTmp);
        system("sync");
    }
    else {
        closedir(dp);
    }

    if (-1 == access(szAbsConfigPath, 0)) {
        sprintf(szTmp, "cp ./configFile/%s %s", strFileName, szAbsAppNamePath);
        system(szTmp);
        system("sync");
    }
    if (-1 == access(szAbsConfigPath, 0)) {
        strcpy(strBuffer, strFileName);
    }
    else {
        strcpy(strBuffer, szAbsConfigPath);
    }
#else
    sprintf(strBuffer, "./%s", strFileName);
#endif /* _WIN32 */

    return strBuffer;
}


/* 获取历史文件路径(放APP单路径) */
const char* getHistoryFilePath_(const char* strAppName)
{
    static char strBuffer[256];

#ifndef _WIN32
    char szAbsAppNamePath[256];

    sprintf(szAbsAppNamePath, "/data/app/%s/history/", strAppName);

    DIR* dp;

    if ((dp = opendir(szAbsAppNamePath)) == NULL) {
        mkdir(szAbsAppNamePath, 0777);
        system("sync");
    }
    else {
        closedir(dp);
    }

    strcpy(strBuffer, szAbsAppNamePath);
#else
    sprintf(strBuffer, "./history/");
#endif // WIN32

    return strBuffer;
}