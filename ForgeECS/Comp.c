#include "../Diagnostics/Assert.h"
#include "Internals.h"
#include <string.h>

PRP_Result CompGetLastErrCode(DT_void) { return last_err_code; }

DT_size CompRegister(const DT_char *name, DT_size size) {
    DIAG_ASSERT(name != DT_null);
    DIAG_ASSERT(size > 0);

    ComponentMetadata data = {.size = size};
    strncpy(data.name, name, COMP_NAME_MAX_SIZE);
    data.name[COMP_NAME_MAX_SIZE - 1] = '\0';

    PRP_Result code = DT_ArrPushUnchecked(g_ctx->comps, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return PRP_INVALID_INDEX;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }

    // The -1 to convert len to idx.
    return DT_ArrLenUnchecked(g_ctx->comps) - 1;
}
