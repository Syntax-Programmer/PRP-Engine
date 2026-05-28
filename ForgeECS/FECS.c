#include "FECS.h"
#include "ForgeECS/Defs.h"
#include "Internals/FECS-Internals.h"
#include "Internals/ForgeWorld/World-Internals.h"

/* ----  COMPS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_CompRegister(const DT_char *name,
                                                    DT_size size,
                                                    FECS_CompId *pComp_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(name != DT_null);
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(pComp_id != DT_null);
    if (!name || !size || !pComp_id) {
        return PRP_ERR_INV_ARG;
    }

    if (CompIsRegistered(name, pComp_id)) {
        DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                       "The given component name is already registered.");
        return PRP_ERR_ALREADY_EXISTS;
    }

    return CompRegister(name, size, pComp_id);
}

/* ----  BEHAVIOR ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_BehaviorRegister(
    FECS_CompId *pComp_ids, DT_size comp_count, FECS_BehaviorId *pBehavior_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(pComp_ids != DT_null);
    DIAG_ASSERT(comp_count > 0);
    DIAG_ASSERT(pBehavior_id != DT_null);
    if (!pComp_ids || !comp_count || !pBehavior_id) {
        return PRP_ERR_INV_ARG;
    }

    if (BehaviorIsRegistered(pComp_ids, comp_count, pBehavior_id)) {
        return PRP_OK;
    }
    PRP_Result code = BehaviorRegister(pComp_ids, comp_count, pBehavior_id);
    if (code != PRP_OK) {
        return code;
    }
    code = DT_ArrForEachUnchecked(g_ctx->queries, QueryCascadeUpdateBehavior,
                                  pBehavior_id);
    if (code != PRP_OK) {
        DT_ArrForEachUnchecked(g_ctx->queries, QueryCascadingErrorCleanup,
                               pBehavior_id);
        Behavior *behavior = DT_null;
        DT_ArrPopUnchecked(g_ctx->behaviors, behavior);
        BehaviorDelete(behavior, DT_null);
        return code;
    }

    return PRP_OK;
}

/* ----  QUERY ---- */

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_QueryRegister(const FECS_CompId *pInc_comp_ids, DT_size inc_comps_count,
                   const FECS_CompId *pExc_comp_ids, DT_size exc_comps_count,
                   FECS_QueryId *pQuery_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(pInc_comp_ids != DT_null);
    DIAG_ASSERT(inc_comps_count > 0);
    if (pExc_comp_ids) {
        DIAG_ASSERT(exc_comps_count > 0);
    }
    DIAG_ASSERT(pQuery_id != DT_null);
    if (!pInc_comp_ids || !inc_comps_count ||
        (pExc_comp_ids && !exc_comps_count) || !pQuery_id) {
        return PRP_ERR_INV_ARG;
    }

    if (QueryIsRegistered(pInc_comp_ids, inc_comps_count, pExc_comp_ids,
                          exc_comps_count, pQuery_id)) {
        return PRP_OK;
    }

    return QueryRegister(pInc_comp_ids, inc_comps_count, pExc_comp_ids,
                         exc_comps_count, pQuery_id);
}

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_SystemRegister(FECS_SystemFunc system_func, FECS_SystemId *pSystem_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(system_func != DT_null);
    DIAG_ASSERT(pSystem_id != DT_null);
    if (!system_func || !pSystem_id) {
        return PRP_ERR_INV_ARG;
    }

    if (SystemIsRegistered(system_func, pSystem_id)) {
        return PRP_OK;
    }

    return SystemRegister(system_func, pSystem_id);
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

    code = DT_ArrCreateUnchecked(sizeof(Component), DT_ARR_DEFAULT_CAP,
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
    code = DT_ArrCreateUnchecked(sizeof(FECS_SystemFunc), DT_ARR_DEFAULT_CAP,
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
