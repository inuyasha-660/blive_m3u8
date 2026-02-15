#include "api/api.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

extern LiveInfo *live_info;

// 从 url.m3u8 解析出目标画质的 URL，将会破坏原字符串
char *url_m3u8_get_url(char *m3u8_str)
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