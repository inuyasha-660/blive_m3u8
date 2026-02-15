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

void printn(const char *msg);
size_t api_curl_finish(void *buffer, size_t size, size_t nmemb, void *userp);