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

Buffer *api_get_url_m3u8();