#include "ForgeECS/Internals/FECS/FECS-Internals.h"

FECS_InternalCtx *g_ctx = DT_null;

/* ----  COMPS ---- */

PRP_Result CompRegister(DT_char *pName, DT_size name_len, DT_size comp_size,
                        FECS_CompId *pComp_id) {
    *pComp_id = FECS_INVALID_ID;

    if (DT_StrArrSearchUnchecked(g_ctx->pComp_names, pName, name_len,
                                 pComp_id)) {
        return PRP_ERR_ALREADY_EXISTS;
    }

    DT_size len = DT_ArrLen(g_ctx->pComp_sizes);
    if (len >= FECS_COMPONENTS_MAX_CAP) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Result code =
        DT_StrArrPushUnchecked(g_ctx->pComp_names, pName, name_len);
    if (code != PRP_OK) {
        return code;
    }
    code = DT_ArrPushUnchecked(g_ctx->pComp_sizes, &comp_size);
    if (code != PRP_OK) {
        DT_StrArrPopUnchecked(g_ctx->pComp_names, DT_null, DT_null);
        return code;
    }
    *pComp_id = len;

    return PRP_OK;
}

/* ----  SYTEMS ---- */

PRP_Result SystemRegister(DT_char *pName, DT_size name_len,
                          FECS_SystemFunc system_func,
                          FECS_SystemId *pSystem_id) {
    *pSystem_id = FECS_INVALID_ID;

    if (DT_StrArrSearchUnchecked(g_ctx->pSystem_names, pName, name_len,
                                 pSystem_id)) {

        return PRP_ERR_ALREADY_EXISTS;
    }
    DT_size len = DT_ArrLen(g_ctx->pSystem_funcs);
    PRP_Result code =
        DT_StrArrPushUnchecked(g_ctx->pSystem_names, pName, name_len);
    if (code != PRP_OK) {
        return code;
    }
    code = DT_ArrPushUnchecked(g_ctx->pSystem_funcs, &system_func);
    if (code != PRP_OK) {
        DT_StrArrPopUnchecked(g_ctx->pSystem_names, DT_null, DT_null);
        return code;
    }
    *pSystem_id = len;

    return PRP_OK;
}
