#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#  include <dirent.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#include "public_.h"


const char* log_file_path(const char* strAppName)
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


const char* config_file_path(const char* strAppName, const char* strFileName)
{
    static char strBuffer[256];

#ifndef _WIN32
    char szAbsConfigPath[256];
    char szAbsAppNamePath[256];
    char szTmp[256 + 32];

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


const char* history_file_path(const char* strAppName)
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
#endif /* _WIN32 */

    return strBuffer;
}