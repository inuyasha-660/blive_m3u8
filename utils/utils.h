#include <stddef.h>

// 日志等级
enum {
    LOG_INFO = 0,
    LOG_WARN = 1,
    LOG_ERR = 2,
};

void _log(int log_level, const char *func, char *format, ...);
#define info(...)  _log(LOG_INFO, __FUNCTION__, ##__VA_ARGS__)
#define warn(...)  _log(LOG_WARN, __FUNCTION__, ##__VA_ARGS__)
#define error(...)  _log(LOG_ERR, __FUNCTION__, ##__VA_ARGS__)

#if defined(_WIN32)
    #include <windows.h>
    #define __sleep_ms(ms) do { Sleep(ms); } while(0)
#elif defined (__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__OpenBSD__) || defined(__APPLE__)
    #include <time.h> 
    #define __sleep_ms(ms) do { \
        struct timespec duration = { \
            .tv_sec = 0, \
            .tv_nsec = ms * 1000 * 1000 \
        }; \
        struct timespec rem; \
        \
        nanosleep(&duration, &rem); \
    } while (0)
#else
    #error "Unsupported system"
#endif

void printn(const char *msg);
size_t api_curl_finish(void *buffer, size_t size, size_t nmemb, void *userp);
char *read_file(const char *filename);