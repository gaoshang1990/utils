#include "slog_.h"
#include "file_.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif


int main(int argc, char *argv[])
{

    // slogInit_("../../../log/test/test", "slog.log", S_TRACE);

    SLOG_ERROR("SLOG ERROR TEST");
    SLOG_WARN("SLOG WARN TEST");
    SLOG_INFO("SLOG INFO TEST");
    SLOG_DEBUG("SLOG DEBUG TEST");
    SLOG_TRACE("SLOG TRACE TEST");

    SLOG_ERROR_RAW("SLOG ERROR RAW TEST\n");
    SLOG_WARN_RAW("SLOG WARN RAW TEST\n");
    SLOG_INFO_RAW("SLOG INFO RAW TEST\n");
    SLOG_DEBUG_RAW("SLOG DEBUG RAW TEST\n");
    SLOG_TRACE_RAW("SLOG TRACE RAW TEST\n");

FILE* fp = fopen("../../../test.txt", "rb");

    int fileSize = fileSize_fp_(fp);
    printf("file size: %d\n", fileSize);

    return 0;
}