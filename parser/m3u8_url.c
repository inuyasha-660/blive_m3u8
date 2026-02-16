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
    char *begin_seg_buffer = NULL;
    *init_seg = NULL;
    *begin_seg = NULL;

    char *line = strtok(m3u8_str, "\n");
    while (line != NULL) {
        if (*init_seg != NULL && *begin_seg != NULL) {
            break;
        }

        // 解析初始化片段
        if (strstr(line, "#EXT-X-MEDIA-SEQUENCE")) {
            *init_seg = strdup(line + 22);
        }
        // 解析开始片段
        if (strstr(line, "#EXT-X-MAP:URI")) {
            begin_seg_buffer = strdup(line + 15);
        }

        line = strtok(NULL, "\n");
    }

    if (*init_seg == NULL || begin_seg_buffer == NULL) {
        error("Failed to parse init_seg || begin_seg");
    }

    size_t len_begin_seg_buffer = strlen(begin_seg_buffer);
    *begin_seg = (char *)malloc((len_begin_seg_buffer) * sizeof(char));
    snprintf(*begin_seg, len_begin_seg_buffer - 1, "%s", begin_seg_buffer + 1);

    free(begin_seg_buffer);
    return err;
}