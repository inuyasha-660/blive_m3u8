#include "api/api.h"
#include "parser/parser.h"
#include "utils/utils.h"
#include <curl/curl.h>
#include <stdlib.h>

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

int main(int argc, char *argv[])
{
    int err = 0;

    // 参数数量校验
    if (argc != 2) {
        error("Need one arguments but passed %d\n", argc);
        printn("blive_m3u8 <config>\n");
        return 1;
    }

    // 全局初始化
    info("Initialize...");
    if (global_init() < 0) {
        error("Aborted and return 1");
        global_init();
        return 1;
    }

    // 读取用户配置
    info("Read user config");
    char *json_str = read_file(argv[1]);
    if (user_config_parser(json_str) < 0) {
        error("Aborted and return 1");
        return 1;
    }
    free(json_str);

    // 获取 url.m3u8
    info("Fetch url.m3u8");
    char *url_m3u8 = NULL;
    asprintf(&url_m3u8, "%s?cid=%s&mid=%s", API_URL_M3U8, live_info->cid,
             live_info->mid);
    Buffer *buffer_url_m3u8 = api_get_response(url_m3u8);
    if (buffer_url_m3u8 == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 获取目标 URL
    info("Fetch url of target qn");
    char *url_qn = url_get_url(buffer_url_m3u8->buffer);
    if (url_qn == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 获取 index.m3u8
    info("Fetch index.m3u8");
    Buffer *buffer_index_m3u8 = api_get_response(url_qn);
    if (buffer_index_m3u8 == NULL) {
        error("Aborted and return 1");
        free(url_qn);
        err = 1;
        goto end;
    }

    // 解析初始化片段和开始片段
    info("Parse init_seg and begin_seg");
    char *init_seg = NULL, *begin_seg = NULL;
    index_m3u8_parser(buffer_index_m3u8->buffer, &init_seg, &begin_seg);
    if (init_seg == NULL || begin_seg == NULL) {
        error("Aborted and return 1");
        free(buffer_index_m3u8->buffer);
        free(buffer_index_m3u8);
        err = 1;
        goto end;
    }

    puts(init_seg);
    puts(begin_seg);

end:
    if (url_m3u8 != NULL) {
        free(url_m3u8);
    }
    free(buffer_url_m3u8->buffer);
    free(buffer_url_m3u8);
    global_cleanup();
    return err;
}