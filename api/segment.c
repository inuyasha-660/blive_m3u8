#include "api.h"
#include "utils/utils.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

extern LiveInfo *live_info;
int              shouldClose = 0;

int api_download(const char *url, const char *filename)
{
    if (url == NULL) {
        error("url is NULL");
        return -1;
    }

    CURL *curl = curl_easy_init();
    if (curl) {
        int      err = 0;
        CURLcode code_download;
        FILE    *out_f;

        // 在原文件后追加直播流片段
        out_f = fopen(filename, "ab");
        if (out_f == NULL) {
            error("Failed to open %s", filename);
            err = -1;
            goto end;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_f);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        code_download = curl_easy_perform(curl);
        if (code_download != CURLE_OK) {
            error("Failed to fetch %s", url);
            err = -1;
            goto end;
        }
        long status;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
        if (status != 200) {
            error("Failed(%ld) to fetch %s", status, url);
            err = -1;
            goto end;
        }
    end:
        fclose(out_f);
        curl_easy_cleanup(curl);
        return err;
    } else {
        error("Failed to initialize curl");
        return -1;
    }

    return 0;
}

int get_should_close(void *arg)
{
    char  *input = NULL;
    size_t len_input = 0;
    printn("Type 'q' to stop recording");
    if ((getline(&input, &len_input, stdin) < 0) ||
        (input[0] == 'q' || input[0] == 'Q')) {
        shouldClose = 1;
    }

    return 0;
}

int api_segment_download(const char *base_url, const char *init_seg,
                         const char *begin_seg, const char *out_name)
{
    // 下载初始化片段
    char *url_init_seg = NULL;
    asprintf(&url_init_seg, "%s%s", base_url, init_seg);
    if (api_download(url_init_seg, out_name) < 0) {
        error("Failed to download init_seg: %s", url_init_seg);
        free(url_init_seg);
        return 0;
    }

    thrd_t th_get_should_close;
    thrd_create(&th_get_should_close, get_should_close, NULL);

    int idx_seg = atoi(begin_seg);
    while (!shouldClose) {
        char *url_seg = NULL;
        asprintf(&url_seg, "%s%d.m4s", base_url, idx_seg);
        if (api_download(url_seg, out_name) < 0) {
            goto sleep;
        }
        idx_seg += 1;

    sleep:
        __sleep_ms(TTL_MS);
    }
    info("Save to %s", out_name);

    free(url_init_seg);
    return 0;
}