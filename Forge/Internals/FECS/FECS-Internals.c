#include "Forge/Internals/FECS/FECS-Internals.h"
#include "Diagnostics/Assert.h"

FECS_InternalCtx *g_ctx = NULL;

/* ----  COMPS ---- */

PRP_Result CompRegister(PRP_Char8 *pName, PRP_Size name_len, PRP_Size comp_size,
                        FECS_CompId *pComp_id) {
    *pComp_id = FECS_INVALID_ID;

    if (CONT_StrArrSearchUnchecked(g_ctx->pComp_names, pName, name_len,
                                   pComp_id)) {
        return PRP_ERR_ALREADY_EXISTS;
    }

    PRP_Size len = CONT_ArrLen(g_ctx->pComp_sizes);
    if (len >= FECS_COMPONENTS_MAX_CAP) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Result code =
        CONT_StrArrPushUnchecked(g_ctx->pComp_names, pName, name_len);
    if (code != PRP_OK) {
        return code;
    }
    code = CONT_ArrPushUnchecked(g_ctx->pComp_sizes, &comp_size);
    if (code != PRP_OK) {
        CONT_StrArrPopUnchecked(g_ctx->pComp_names, NULL, NULL);
        return code;
    }
    *pComp_id = len;

    return PRP_OK;
}

/* ----  SYTEMS ---- */

PRP_Result SystemRegister(PRP_Char8 *pName, PRP_Size name_len,
                          FECS_SystemFunc system_func,
                          PRP_Size comp_ids_needed_count,
                          FECS_CompId *pComp_ids_needed,
                          FECS_SystemId *pSystem_id) {
    *pSystem_id = FECS_INVALID_ID;

    if (CONT_StrArrSearchUnchecked(g_ctx->pSystem_names, pName, name_len,
                                   pSystem_id)) {

        return PRP_ERR_ALREADY_EXISTS;
    }

    FECS_SystemInfo info = {.systmem_func = system_func,
                            .comp_ids_needed_count = comp_ids_needed_count};
    info.pComp_ids_needed = malloc(sizeof(FECS_CompId) * comp_ids_needed_count);
    if (!info.pComp_ids_needed) {
        return PRP_ERR_OOM;
    }
    PRP_Size comps_len = CONT_ArrLen(g_ctx->pComp_sizes);
    for (PRP_Size i = 0; i < comp_ids_needed_count; i++) {
        FECS_CompId comp_id = pComp_ids_needed[i];
        DIAG_ASSERT_MSG(comp_id < comps_len,
                        "The given system registration contains invalid comp "
                        "id: %zu at pComp_ids_needed[%zu]",
                        comp_id, i);
        if (comp_id >= comps_len) {
            return PRP_ERR_INV_ARG;
        }
        info.pComp_ids_needed[i] = comp_id;
    }

    PRP_Size len = CONT_ArrLen(g_ctx->pSystem_infos);
    PRP_Result code =
        CONT_StrArrPushUnchecked(g_ctx->pSystem_names, pName, name_len);
    if (code != PRP_OK) {
        return code;
    }
    code = CONT_ArrPushUnchecked(g_ctx->pSystem_infos, &info);
    if (code != PRP_OK) {
        CONT_StrArrPopUnchecked(g_ctx->pSystem_names, NULL, NULL);
        return code;
    }
    *pSystem_id = len;

    return PRP_OK;
}

PRP_Result SystemInfoDeleteCb(void *pVal, void *_) {
    (void)_;
    FECS_SystemInfo *pSystem_info = pVal;

    free(pSystem_info->pComp_ids_needed);
#ifdef PRP_DEBUG_MODE
    pSystem_info->pComp_ids_needed = NULL;
#endif

    return PRP_OK;
}
