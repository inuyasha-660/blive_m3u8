#include "api/api.h"
#include "parser.h"
#include "utils/utils.h"
#include <string.h>
#include <yyjson.h>

extern LiveInfo *live_info;

int user_config_parser(const char *json_str)
{
    int err = 0;

    yyjson_doc *doc = yyjson_read(json_str, strlen(json_str), 0);
    if (doc == NULL) {
        error("Failed to parse doc");
        err = -1;
        goto end;
    }
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (root == NULL) {
        error("Failed to parse doc->root");
        err = -1;
        goto end;
    }

    yyjson_val *cid = yyjson_obj_get(root, "cid");
    yyjson_val *mid = yyjson_obj_get(root, "mid");
    yyjson_val *qn = yyjson_obj_get(root, "qn");
    yyjson_val *out_path = yyjson_obj_get(root, "out_path");
    if (!yyjson_is_str(cid) || !yyjson_is_str(mid) || !yyjson_is_str(qn) ||
        !yyjson_is_str(out_path)) {
        error("Failed to parse doc->root->(*)");
        err = 1;
        goto end;
    }

    live_info->cid = strdup(yyjson_get_str(cid));
    live_info->mid = strdup(yyjson_get_str(mid));
    live_info->qn = strdup(yyjson_get_str(qn));
    live_info->out_path = strdup(yyjson_get_str(out_path));

end:
    yyjson_doc_free(doc);
    return err;
}