#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <threads.h>

const char *LogLevelStr[3] = {"Info", "Warn", "Error"};

const char *LogLevelColor[4] = {
    "\x1b[1;32m", // 绿色
    "\x1b[1;33m", // 黄色
    "\x1b[1;31m", // 红色
    "\x1b[1;35m"  // 紫色(func)
};

void _log(int log_level, const char *func, char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    mtx_t lock_log;
    mtx_init(&lock_log, mtx_plain);
    FILE *out = stdout;
    if (log_level == 2)
        out = stderr;

    mtx_lock(&lock_log);

    fprintf(out, "[%s%s\x1b[0m](%s%s\x1b[0m) ", LogLevelColor[log_level],
            LogLevelStr[log_level], LogLevelColor[3], func);
    vfprintf(out, format, ap);
    fprintf(out, "\n");
    va_end(ap);

    mtx_unlock(&lock_log);
}

void printn(const char *msg) { fprintf(stderr, "%s\n", msg); }