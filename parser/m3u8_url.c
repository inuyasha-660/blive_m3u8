#include "api/api.h"
#include "parser.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern LiveInfo *live_info;

// 从 url.m3u8 解析出目标画质的 URL，将会破坏原字符串
char *url_get_url(char *m3u8_str)
{
    char *url_qn = NULL;
    char *target = NULL;
    asprintf(&target, "BILI-QN=%s", live_info->qn);

    size_t found_url = 0;
    char  *line = strtok(m3u8_str, "\n");
    while (line != NULL) {
        if (found_url) {
            url_qn = strdup(line);
            break;
        }
        if (strstr(line, target)) {
            found_url = 1;
        }

        line = strtok(NULL, "\n");
    }

    return url_qn;
}

// 从 index.m3u8 解析初始化片段(hxxxx.m4s)和直播流列表
// '#EXT-X-MEDIA-SEQUENCE:' [22]； '#EXT-X-MAP:URI=' [15]
int index_m3u8_parser(char *m3u8_str, char **init_seg, char **begin_seg)
{
    int   err = 0;
    char *init_seg_buffer = NULL;
    *init_seg = NULL;
    *begin_seg = NULL;

    char *line = strtok(m3u8_str, "\n");
    while (line != NULL) {
        if (*init_seg != NULL && *begin_seg != NULL) {
            break;
        }

        // 解析初始化片段
        if (strstr(line, "#EXT-X-MAP:URI=")) {
            init_seg_buffer = strdup(line + 15);
        }
        // 解析开始片段
        if (strstr(line, "#EXT-X-MEDIA-SEQUENCE:")) {
            *begin_seg = strdup(line + 22);
        }

        line = strtok(NULL, "\n");
    }

    if (*begin_seg == NULL || init_seg_buffer == NULL) {
        error("Failed to parse init_seg || begin_seg");
    }

    size_t len_init_seg_buffer = strlen(init_seg_buffer);
    *init_seg = (char *)malloc((len_init_seg_buffer) * sizeof(char));
    snprintf(*init_seg, len_init_seg_buffer - 1, "%s", init_seg_buffer + 1);

    free(init_seg_buffer);
    return err;
}

char *get_base_url(char *url_qn)
{
    char *base_url = NULL;

    // 获取 base url 长度
    char *found = strstr(url_qn, "index.m3u8");
    long  offset = found - url_qn;

    base_url = (char *)malloc((offset + 1) * sizeof(char));
    snprintf(base_url, offset + 1, "%s", url_qn);

    return base_url;
}