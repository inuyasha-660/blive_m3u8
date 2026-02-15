#include "api.h"
#include "utils/utils.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdio.h>
#include <stdlib.h>

extern LiveInfo *live_info;

/*
 * [GET]
 * cid: 直播间长 ID
 * mid: 主播 ID
 */
static const char *API_URL_M3U8 =
    "https://api.live.bilibili.com/xlive/play-gateway/master/url";

/*
 * 获取 url.m3u8
 *
 * url.m3u8: 包含特定画质的 index.m3u8 获取链接
 */
Buffer *api_get_url_m3u8()
{
    Buffer *buffer_url_m3u8 = NULL;
    CURL   *curl_url_m3u8;

    if (live_info == NULL) {
        error("live_info is NULL");
        return NULL;
    }

    curl_url_m3u8 = curl_easy_init();
    if (curl_url_m3u8) {
        buffer_url_m3u8 = (Buffer *)malloc(sizeof(Buffer));
        buffer_url_m3u8->buffer = NULL;
        buffer_url_m3u8->length = 0;
        char    *url_url_m3u8 = NULL;
        CURLcode url_m3u8_code;

        asprintf(&url_url_m3u8, "%s?cid=%s&mid=%s", API_URL_M3U8,
                 live_info->cid, live_info->mid);

        curl_easy_setopt(curl_url_m3u8, CURLOPT_URL, url_url_m3u8);
        curl_easy_setopt(curl_url_m3u8, CURLOPT_WRITEFUNCTION, api_curl_finish);
        curl_easy_setopt(curl_url_m3u8, CURLOPT_WRITEDATA, buffer_url_m3u8);

        url_m3u8_code = curl_easy_perform(curl_url_m3u8);
        if (url_m3u8_code != CURLE_OK) {
            error("Failed to get url.m3u8");
        }

        if (url_url_m3u8 != NULL) {
            free(url_url_m3u8);
        }
        curl_easy_cleanup(curl_url_m3u8);
    } else {
        error("Failed to initalize curl_url_m3u8");
        return NULL;
    }

    return buffer_url_m3u8;
}