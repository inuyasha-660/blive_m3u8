#include "utils/utils.h"
#include "api/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    } else {
        return 0;
    }
}

size_t api_curl_finish(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t  len_buffer = size * nmemb;
    Buffer *buffer_s = (Buffer *)userp;
    buffer_s->buffer = (char *)realloc(
        buffer_s->buffer, (buffer_s->length + len_buffer + 1) * sizeof(char));

    memcpy(buffer_s->buffer + buffer_s->length, buffer, len_buffer);
    buffer_s->length += len_buffer;
    buffer_s->buffer[buffer_s->length] = '\0';

    return len_buffer;
}

char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        error("Failed to open %s", filename);
        return NULL;
    }

    // 获取文件大小
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc((size + 1) * sizeof(char));

    // 批量读取整个文件
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != (size_t)size) {
        error("fread failed on %s", filename);
        free(buffer);
        fclose(file);
        return NULL;

    } else {
        buffer[size] = '\0';
    }

    fclose(file);
    return buffer;
}