#include <stddef.h>

// 请求缓存
typedef struct {
    char *buffer;
    size_t length;
} Buffer;

typedef struct {
    char *out_path;
    char *cid;
    char *mid;
    char *qn;
} LiveInfo;

/*
 * [GET]
 * cid: 直播间长 ID
 * mid: 主播 ID
 */
#define API_URL_M3U8 "https://api.live.bilibili.com/xlive/play-gateway/master/url"

#define USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:148.0) Gecko/20100101 Firefox/148.0"

Buffer *api_get_response(const char *url);