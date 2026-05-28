#include "FECS-Internals.h"
#include <string.h>

PRP_Result CompRegister(const DT_char *name, DT_size size,
                        FECS_CompId *pComp_id) {
    Component data = {.size = size};
    snprintf(data.name, COMP_NAME_MAX_SIZE, "%s", name);

    PRP_Result code = DT_ArrPushUnchecked(g_ctx->comps, &data);
    if (code != PRP_OK) {
        return code;
    }

    *pComp_id = DT_ArrLen(g_ctx->comps) - 1;

    return PRP_OK;
}

DT_bool CompIsRegistered(const DT_char *name, FECS_CompId *pFound_id) {
    DT_size len;
    const Component *comps = DT_ArrRawUnchecked(g_ctx->comps, &len);

    for (DT_size i = 0; i < len; i++) {
        if (strncmp(comps[i].name, name, COMP_NAME_MAX_SIZE) == 0) {
            *pFound_id = i;
            return DT_true;
        }
    }

    return DT_false;
}
