#include "api/api.h"
#include "parser/parser.h"
#include "utils/utils.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

LiveInfo *live_info;

int global_init()
{
    live_info = (LiveInfo *)malloc(sizeof(LiveInfo));
    live_info->cid = NULL;
    live_info->mid = NULL;
    live_info->out_path = NULL;
    live_info->qn = NULL;

    CURLcode code_init = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (code_init != CURLE_OK) {
        error("Failed to globally initialize curl");
        printn(curl_easy_strerror(code_init));
        return -1;
    }

    return 0;
}

void global_cleanup()
{
    free(live_info->cid);
    free(live_info->mid);
    free(live_info->out_path);
    free(live_info->qn);
    free(live_info);
}

/*
 * 参数顺序
 * 1: cid(直播间长ID) 2: mid(主播ID) 3: qn(数字表示) 4: out_path
 *
 * 注意: 无输入顺序，合法性检查；输出目录必须存在，且参数无 '/'
 */
int main(int argc, char *argv[])
{
    int err = 0;

    // 参数数量校验
    if (argc != 5) {
        error("Need four arguments but passed %d\n", argc);
        printn("1: cid 2: mid 3: qn 4: out_path\n");
        return 1;
    }

    // 全局初始化
    if (global_init() < 0) {
        error("Aborted and return 1");
        global_init();
        return 1;
    }
    live_info->cid = strdup(argv[1]);
    live_info->mid = strdup(argv[2]);
    live_info->qn = strdup(argv[3]);
    live_info->out_path = strdup(argv[4]);

    // 获取 url.m3u8
    Buffer *buffer_url_m3u8 = api_get_url_m3u8();
    if (buffer_url_m3u8 == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 获取目标 URL
    char *url_qn = url_m3u8_get_url(buffer_url_m3u8->buffer);
    if (url_qn == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

end:
    free(buffer_url_m3u8->buffer);
    free(buffer_url_m3u8);
    global_cleanup();
    return err;
}