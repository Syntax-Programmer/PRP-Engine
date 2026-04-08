#include "Internals.h"
#include <string.h>

PRP_Result CompRegister(const DT_char *name, DT_size size, DT_size *pIdx) {
    ComponentMetadata data = {.size = size};
    snprintf(data.name, COMP_NAME_MAX_SIZE, "%s", name);

    PRP_Result code = DT_ArrPushUnchecked(g_ctx->comps, &data);
    if (code != PRP_OK) {
        return code;
    }

    *pIdx = DT_ArrLen(g_ctx->behaviors) - 1;

    return PRP_OK;
}

DT_bool CompIsRegistered(const DT_char *name, DT_size *pOut) {
    DT_size len;
    const ComponentMetadata *meta = DT_ArrRawUnchecked(g_ctx->comps, &len);

    for (DT_size i = 0; i < len; i++) {
        if (strncmp(meta[i].name, name, COMP_NAME_MAX_SIZE) == 0) {
            *pOut = i;
            return DT_true;
        }
    }

    return DT_false;
}
