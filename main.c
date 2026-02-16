#include "api/api.h"
#include "parser/parser.h"
#include "utils/utils.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <threads.h>

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
    int     err = 0;
    char   *out_name = NULL;
    char   *url_m3u8 = NULL;
    Buffer *buffer_url_m3u8 = NULL;
    Buffer *buffer_index_m3u8 = NULL;
    char   *url_qn = NULL;
    char   *base_url = NULL;
    char   *init_seg = NULL, *begin_seg = NULL;

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
    asprintf(&url_m3u8, "%s?cid=%s&mid=%s", API_URL_M3U8, live_info->cid,
             live_info->mid);
    buffer_url_m3u8 = api_get_response(url_m3u8);
    if (buffer_url_m3u8 == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 获取目标 URL
    info("Fetch url of target qn");
    url_qn = url_get_url(buffer_url_m3u8->buffer);
    if (url_qn == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 获取 index.m3u8
    info("Fetch index.m3u8");
    buffer_index_m3u8 = api_get_response(url_qn);
    if (buffer_index_m3u8 == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 获取基本 URL
    info("Parse base url");
    base_url = get_base_url(url_qn);
    if (base_url == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 解析初始化片段和开始片段
    info("Parse init_seg and begin_seg");
    index_m3u8_parser(buffer_index_m3u8->buffer, &init_seg, &begin_seg);
    if (init_seg == NULL || begin_seg == NULL) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

    // 开始录制
    info("Start recording");
    time_t     time_now = time(NULL);
    struct tm *tm_s = gmtime(&time_now);

    asprintf(&out_name, "%s/%d%02d%d-%s.mp4", live_info->out_path,
             tm_s->tm_year + 1900, tm_s->tm_mon + 1, tm_s->tm_mday,
             live_info->mid);

    if (api_segment_download(base_url, init_seg, begin_seg, out_name) < 0) {
        error("Aborted and return 1");
        err = 1;
        goto end;
    }

end:
    if (url_m3u8 != NULL) {
        free(url_m3u8);
    }
    if (base_url != NULL) {
        free(base_url);
    }
    if (init_seg != NULL) {
        free(init_seg);
    }
    if (begin_seg != NULL) {
        free(begin_seg);
    }
    if (out_name != NULL) {
        free(out_name);
    }
    if (buffer_index_m3u8->buffer != NULL) {
        free(buffer_index_m3u8->buffer);
    }
    if (buffer_index_m3u8 != NULL) {
        free(buffer_index_m3u8);
    }
    if (buffer_url_m3u8->buffer != NULL) {
        free(buffer_url_m3u8->buffer);
    }
    if (buffer_url_m3u8 != NULL) {
        free(buffer_url_m3u8);
    }
    global_cleanup();
    return err;
}