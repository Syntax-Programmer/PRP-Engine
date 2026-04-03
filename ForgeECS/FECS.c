#include "FECS.h"
#include "Defs.h"
#include "Internals.h"

/* ----  COMPS ---- */

PRP_FN_API DT_size PRP_FN_CALL FECS_CompRegisterUnchecked(const DT_char *name,
                                                          DT_size size) {
    ASSERT_CTX_INVARIANT_EXPR;
    DIAG_ASSERT(name != DT_null);
    DIAG_ASSERT(size > 0);

    if (CompIsRegistered(name) != PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(PRP_ERR_ALREADY_EXISTS);
        DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                       "The given component name is already registered.");
        return PRP_INVALID_INDEX;
    }       
    DT_size idx = CompRegister(name, size);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(CompGetLastErrCode());
    }

    return idx;
}

PRP_FN_API DT_size PRP_FN_CALL FECS_CompRegisterChecked(const DT_char *name,
                                                        DT_size size) {
    if (!CTX_INVARIANT_EXPR) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }
    if (!name || !size) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_INDEX;
    }
    if (CompIsRegistered(name) != PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(PRP_ERR_ALREADY_EXISTS);
        DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                       "The given component name is already registered.");
        return PRP_INVALID_INDEX;
    }

    DT_size idx = CompRegister(name, size);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(CompGetLastErrCode());
    }

    return idx;
}

/* ----  BEHAVIOR ---- */

PRP_FN_API DT_size PRP_FN_CALL
FECS_BehaviorRegisterUnchecked(DT_Arr *comp_idxs) {
    ASSERT_CTX_INVARIANT_EXPR;
    DIAG_ASSERT(DT_ArrIsValid(comp_idxs) == DT_true);

    DT_size idx = BehaviorIsRegistered(comp_idxs);
    if (idx != PRP_INVALID_INDEX) {
        return idx;
    }
    idx = BehaviorRegister(comp_idxs);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(BehaviorGetLastErrCode());
    }

    return idx;
}

PRP_FN_API DT_size PRP_FN_CALL FECS_BehaviorRegisterChecked(DT_Arr *comp_idxs) {
    if (!CTX_INVARIANT_EXPR) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }
    if (!DT_ArrIsValid(comp_idxs)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_INDEX;
    }

    DT_size idx = BehaviorIsRegistered(comp_idxs);
    if (idx != PRP_INVALID_INDEX) {
        return idx;
    }
    idx = BehaviorRegister(comp_idxs);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(BehaviorGetLastErrCode());
    }

    return idx;
}

/* ----  QUERY ---- */

PRP_FN_API DT_size PRP_FN_CALL FECS_QueryRegisterUnchecked(DT_Arr *inc_comps,
                                                           DT_Arr *exc_comps) {
    ASSERT_CTX_INVARIANT_EXPR;
    DIAG_ASSERT(DT_ArrIsValid(inc_comps) == DT_true);
    if (exc_comps) {
        DIAG_ASSERT(DT_ArrIsValid(exc_comps) == DT_true);
    }

    DT_size idx = QueryIsRegistered(inc_comps, exc_comps);
    if (idx != PRP_INVALID_INDEX) {
        return idx;
    }
    idx = QueryRegister(inc_comps, exc_comps);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(QueryGetLastErrCode());
    }

    return idx;
}

PRP_FN_API DT_size PRP_FN_CALL FECS_QueryRegisterChecked(DT_Arr *inc_comps,
                                                         DT_Arr *exc_comps) {
    if (!CTX_INVARIANT_EXPR) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }
    if (!DT_ArrIsValid(inc_comps) || (exc_comps && !DT_ArrIsValid(exc_comps))) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_INDEX;
    }

    DT_size idx = QueryIsRegistered(inc_comps, exc_comps);
    if (idx != PRP_INVALID_INDEX) {
        return idx;
    }
    idx = QueryRegister(inc_comps, exc_comps);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(QueryGetLastErrCode());
    }

    return idx;
}

/* ----  SYSTEMS ---- */

PRP_FN_API DT_size PRP_FN_CALL
FECS_SystemRegisterUnchecked(FECS_System system) {
    ASSERT_CTX_INVARIANT_EXPR;
    DIAG_ASSERT(system != DT_null);

    DT_size idx = SystemIsRegistered(system);
    if (idx != PRP_INVALID_INDEX) {
        return idx;
    }
    idx = SystemRegister(system);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(SystemGetLastErrCode());
    }

    return idx;
}

PRP_FN_API DT_size PRP_FN_CALL FECS_SystemRegisterChecked(FECS_System system) {
    if (!CTX_INVARIANT_EXPR) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }
    if (!system) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_INDEX;
    }

    DT_size idx = SystemIsRegistered(system);
    if (idx != PRP_INVALID_INDEX) {
        return idx;
    }
    idx = SystemRegister(system);
    if (idx == PRP_INVALID_INDEX) {
        SET_LAST_ERR_CODE(SystemGetLastErrCode());
    }

    return idx;
}

/* ----  LAYOUTS ---- */

/* ----  FECS ---- */

Context *g_ctx = DT_null;

PRP_FN_API PRP_Result PRP_FN_CALL FECS_GetLastErrCode(DT_void) {
    return last_err_code;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void) {
    if (g_ctx) {
        return PRP_OK;
    }

    g_ctx = calloc(1, sizeof(Context));
    if (!g_ctx) {
        return PRP_ERR_OOM;
    }
    g_ctx->schema_lock = DT_true;
    PRP_Result code;
    g_ctx->comps =
        DT_ArrCreateUnchecked(sizeof(ComponentMetadata), DT_ARR_DEFAULT_CAP);
    if (!g_ctx->comps) {
        code = DT_ArrGetLastErrCode();
        goto free_internals;
    }
    g_ctx->behaviors =
        DT_ArrCreateUnchecked(sizeof(Behavior), DT_ARR_DEFAULT_CAP);
    if (!g_ctx->behaviors) {
        code = DT_ArrGetLastErrCode();
        goto free_internals;
    }
    g_ctx->queries = DT_ArrCreateUnchecked(sizeof(Query), DT_ARR_DEFAULT_CAP);
    if (!g_ctx->queries) {
        code = DT_ArrGetLastErrCode();
        goto free_internals;
    }
    g_ctx->systems =
        DT_ArrCreateUnchecked(sizeof(FECS_System), DT_ARR_DEFAULT_CAP);
    if (!g_ctx->systems) {
        code = DT_ArrGetLastErrCode();
        goto free_internals;
    }
    g_ctx->worlds = DT_DSArrCreateUnchecked(sizeof(World), WorldDeleteCb);
    if (!g_ctx->worlds) {
        code = DT_DSArrGetLastErrCode();
        goto free_internals;
    }

    return PRP_OK;

free_internals:
    if (g_ctx->comps) {
        DT_ArrDeleteUnchecked(&g_ctx->comps);
    }
    if (g_ctx->behaviors) {
        DT_ArrDeleteUnchecked(&g_ctx->behaviors);
    }
    if (g_ctx->queries) {
        DT_ArrDeleteUnchecked(&g_ctx->queries);
    }
    if (g_ctx->systems) {
        DT_ArrDeleteUnchecked(&g_ctx->systems);
    }
    if (g_ctx->worlds) {
        DT_DSArrDeleteUnchecked(&g_ctx->worlds);
    }
    g_ctx = DT_null;

    return code;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_Exit(DT_void) {
    if (!g_ctx) {
        return PRP_ERR_INV_STATE;
    }

    DT_ArrDeleteUnchecked(&g_ctx->comps);
    DT_ArrForEachUnchecked(g_ctx->behaviors, BehaviorDelete, DT_null);
    DT_ArrDeleteUnchecked(&g_ctx->behaviors);
    DT_ArrForEachUnchecked(g_ctx->queries, QueryDelete, DT_null);
    DT_ArrDeleteUnchecked(&g_ctx->queries);
    DT_ArrDeleteUnchecked(&g_ctx->systems);
    DT_DSArrDeleteUnchecked(&g_ctx->worlds);

    free(g_ctx);
    g_ctx = DT_null;

    return PRP_OK;
}
