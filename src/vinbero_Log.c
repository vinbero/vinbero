#include <err.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "vinbero_Log.h"

static const char* vinbero_Log_levelString(int level) {
    static const char* levelStrings[] = {
        "\x1B[37mTRACE\x1B[0m",
        "\x1B[36mDEBUG\x1B[0m",
        "\x1B[32mINFO\x1B[0m",
        "\x1B[33mWARN\x1B[0m",
        "\x1B[35mERROR\x1B[0m",
        "\x1B[31mFATAL\x1B[0m",
    };
    if(0 <= level && level < sizeof(levelStrings))
        return levelStrings[level];
    return "UNKNOWN";
}

int vinbero_Log_raw(int level, const char* source, int line, const char* format, ...) {
    time_t t = time(NULL);
    struct tm now;
    localtime_r(&t, &now);
    fprintf(stderr, "\x1B[1;30m[%02d/%02d/%d/%02d:%02d:%02d]\x1B[0m ", now.tm_mday, now.tm_mon + 1, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec);
    fprintf(stderr, "%s %s: %d: ", vinbero_Log_levelString(level), source, line);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    return 0;
}
