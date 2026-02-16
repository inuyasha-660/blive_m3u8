#include "api.h"
#include "utils/utils.h"
#include <curl/curl.h>
#include <stdlib.h>

extern LiveInfo *live_info;

Buffer *api_get_response(const char *url)
{
    Buffer *buffer = NULL;
    CURL   *curl_response;

    if (url == NULL) {
        error("url is NULL");
        return NULL;
    }

    curl_response = curl_easy_init();
    if (curl_response) {
        CURLcode code_response;
        buffer = (Buffer *)malloc(sizeof(Buffer));
        buffer->buffer = NULL;
        buffer->length = 0;

        curl_easy_setopt(curl_response, CURLOPT_URL, url);
        curl_easy_setopt(curl_response, CURLOPT_WRITEFUNCTION, api_curl_finish);
        curl_easy_setopt(curl_response, CURLOPT_WRITEDATA, buffer);
        curl_easy_setopt(curl_response, CURLOPT_USERAGENT, USER_AGENT);

        code_response = curl_easy_perform(curl_response);
        if (code_response != CURLE_OK) {
            error("Failed to get response of %s", url);
        }
        // 校验 http 状态码
        long status_response;
        curl_easy_getinfo(curl_response, CURLINFO_RESPONSE_CODE,
                          &status_response);
        if (status_response != 200) {
            error("Failed to get response: %ld", status_response);
            curl_easy_cleanup(curl_response);
            free(buffer->buffer);
            free(buffer);
            return NULL;
        }

        curl_easy_cleanup(curl_response);
    } else {
        error("Failed to initalize curl_response");
        return NULL;
    }

    return buffer;
}