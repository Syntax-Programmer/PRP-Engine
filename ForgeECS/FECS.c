#include "FECS.h"
#include "DataTypes/Arr.h"
#include "DataTypes/Typedefs.h"
#include "Diagnostics/Assert.h"
#include "Internals/FECS-Internals.h"
#include "Internals/ForgeWorld/World-Internals.h"

/* ----  COMPS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_CompRegister(const DT_char *name,
                                                    DT_size size,
                                                    DT_size *pIdx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(name != DT_null);
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(pIdx != DT_null);
    if (!name || !size || !pIdx) {
        return PRP_ERR_INV_ARG;
    }

    if (CompIsRegistered(name, pIdx)) {
        DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                       "The given component name is already registered.");
        return PRP_ERR_ALREADY_EXISTS;
    }

    return CompRegister(name, size, pIdx);
}

/* ----  BEHAVIOR ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_BehaviorRegister(DT_size *comp_idxs,
                                                        DT_size comp_count,
                                                        DT_size *pIdx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(comp_idxs != DT_null);
    DIAG_ASSERT(comp_count > 0);
    DIAG_ASSERT(pIdx != DT_null);
    if (!comp_idxs || !comp_count || !pIdx) {
        return PRP_ERR_INV_ARG;
    }

    if (BehaviorIsRegistered(comp_idxs, comp_count, pIdx)) {
        return PRP_OK;
    }
    PRP_Result code = BehaviorRegister(comp_idxs, comp_count, pIdx);
    if (code != PRP_OK) {
        return code;
    }
    code = DT_ArrForEachUnchecked(g_ctx->queries, QueryCascadeUpdateBehavior,
                                  pIdx);
    if (code != PRP_OK) {
        DT_ArrForEachUnchecked(g_ctx->queries, QueryCascadingErrorCleanup,
                               pIdx);
        Behavior *behavior = DT_null;
        DT_ArrPopUnchecked(g_ctx->behaviors, behavior);
        BehaviorDelete(behavior, DT_null);
        return code;
    }

    return PRP_OK;
}

/* ----  QUERY ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_QueryRegister(const DT_size *inc_comps,
                                                     DT_size inc_comps_count,
                                                     const DT_size *exc_comps,
                                                     DT_size exc_comps_count,
                                                     DT_size *pIdx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(inc_comps != DT_null);
    DIAG_ASSERT(inc_comps_count > 0);
    if (exc_comps) {
        DIAG_ASSERT(exc_comps_count > 0);
    }
    DIAG_ASSERT(pIdx != DT_null);
    if (!inc_comps || !inc_comps_count || (exc_comps && !exc_comps_count) ||
        !pIdx) {
        return PRP_ERR_INV_ARG;
    }

    if (QueryIsRegistered(inc_comps, inc_comps_count, exc_comps,
                          exc_comps_count, pIdx)) {
        return PRP_OK;
    }

    return QueryRegister(inc_comps, inc_comps_count, exc_comps, exc_comps_count,
                         pIdx);
}

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemRegister(FECS_System system,
                                                      DT_size *pIdx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(system != DT_null);
    DIAG_ASSERT(pIdx != DT_null);
    if (!system || !pIdx) {
        return PRP_ERR_INV_ARG;
    }

    if (SystemIsRegistered(system, pIdx)) {
        return PRP_OK;
    }

    return SystemRegister(system, pIdx);
}

/* ----  FECS ---- */

Context *g_ctx = DT_null;

PRP_FN_API DT_void PRP_FN_CALL FECS_LockSchemaDefs(DT_void) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }

    g_ctx->schema_lock = DT_true;
    DT_ArrShrinkFitUnchecked(g_ctx->comps);
    DT_ArrShrinkFitUnchecked(g_ctx->behaviors);
    DT_ArrShrinkFitUnchecked(g_ctx->queries);
    DT_ArrShrinkFitUnchecked(g_ctx->systems);
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void) {
    if (CTX_INVARIANT_EXPR) {
        return PRP_OK;
    }

    g_ctx = calloc(1, sizeof(Context));
    if (!g_ctx) {
        return PRP_ERR_OOM;
    }
    g_ctx->schema_lock = DT_false;
    PRP_Result code;

    code = DT_ArrCreateUnchecked(sizeof(ComponentMetadata), DT_ARR_DEFAULT_CAP,
                                 &g_ctx->comps);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_ArrCreateUnchecked(sizeof(Behavior), DT_ARR_DEFAULT_CAP,
                                 &g_ctx->behaviors);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_ArrCreateUnchecked(sizeof(Query), DT_ARR_DEFAULT_CAP,
                                 &g_ctx->queries);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_ArrCreateUnchecked(sizeof(FECS_System), DT_ARR_DEFAULT_CAP,
                                 &g_ctx->systems);
    if (code != PRP_OK) {
        goto err_path;
    }
    code =
        DT_DSArrCreateUnchecked(sizeof(World), WorldDeleteCb, &g_ctx->worlds);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
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

PRP_FN_API DT_void PRP_FN_CALL FECS_Exit(DT_void) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
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
}
