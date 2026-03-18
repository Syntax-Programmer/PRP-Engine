#include "../Diagnostics/Assert.h"
#include "Internals.h"
#include <string.h>

PRP_Result CompGetLastErrCode(DT_void) { return last_err_code; }

DT_size CompRegister(const DT_char *name, DT_size size) {
    DIAG_ASSERT(name != DT_null);
    DIAG_ASSERT(size > 0);

    DT_size len;
    const ComponentMetadata *meta_raw = DT_ArrRawUnchecked(g_ctx->comps, &len);
    for (DT_size i = 0; i < len; i++) {
        if (strncmp(name, meta_raw[i].name, COMP_NAME_MAX_SIZE) == 0) {
            if (meta_raw[i].size != size) {
                SET_LAST_ERR_CODE(PRP_ERR_ALREADY_EXISTS);
                return PRP_INVALID_INDEX;
            }
            return i;
        }
    }

    ComponentMetadata data = {.size = size};
    strncpy(data.name, name, COMP_NAME_MAX_SIZE);
    data.name[COMP_NAME_MAX_SIZE - 1] = '\0';

    DT_size idx = DT_ArrLenUnchecked(g_ctx->comps);
    PRP_Result code = DT_ArrPushUnchecked(g_ctx->comps, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return PRP_INVALID_INDEX;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }

    return idx;
}
