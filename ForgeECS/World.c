#include "../Diagnostics/Assert.h"
#include "Internals.h"

DT_DSId WorldCreate(DT_void) {
    ASSERT_CTX_INVARIANT_EXPR;

    World data = {0};
    data.layouts = DT_ArrCreateUnchecked(sizeof(Layout), DT_ARR_DEFAULT_CAP);
    data.system_caches =
        DT_ArrCreateUnchecked(sizeof(SystemCache), DT_ARR_DEFAULT_CAP);
    if (!data.layouts || !data.system_caches) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    }

    DT_DSId world = DT_DSArrAddUnchecked(g_ctx->worlds, &data);
    if (world == DT_DS_INVALID_ID) {
        PRP_Result code = DT_DSArrGetLastErrCode();
        if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
            SET_LAST_ERR_CODE(PRP_ERR_OOM);
            goto free_internals;
        } else if (code != PRP_OK) {
            SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
            goto free_internals;
        }
    }

    return world;

free_internals:
    if (data.layouts) {
        DT_ArrDeleteUnchecked(&data.layouts);
    }
    if (data.system_caches) {
        DT_ArrDeleteUnchecked(&data.system_caches);
    }

    return DT_DS_INVALID_ID;
}

DT_void WorldDelete(DT_DSId *pWorld_id) {
    ASSERT_CTX_INVARIANT_EXPR;

    DT_DSArrDelElemUnchecked(g_ctx->worlds, pWorld_id);
}



PRP_Result WorldSystemExecAll(DT_DSId world_id);
PRP_Result WorldSystemExecOne(DT_DSId world_id, DT_size system_cache_idx);
PRP_Result WorldSystemExecMany(DT_DSId world_id, DT_Arr *system_cache_idxs);
PRP_Result WorldUpdate(DT_DSId world_id, DT_f32 dt);
PRP_Result WorldSetSystemExecOrder(DT_DSId world_id, DT_Arr *system_exec_order);
PRP_Result WorldSync(DT_DSId world_id);

PRP_Result WorldEnableSystem(DT_DSId world_id, DT_size system_cache_idx) {
    ASSERT_CTX_INVARIANT_EXPR;
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(system_cache_idx < DT_ArrLenUnchecked(world->system_caches));

    SystemCache *cache = (SystemCache *)DT_ArrGetUnchecked(world->system_caches,
                                                           system_cache_idx);
    cache->enabled = DT_true;

    return PRP_OK;
}

PRP_Result WorldDisableSystem(DT_DSId world_id, DT_size system_cache_idx) {
    ASSERT_CTX_INVARIANT_EXPR;
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(system_cache_idx < DT_ArrLenUnchecked(world->system_caches));

    SystemCache *cache = (SystemCache *)DT_ArrGetUnchecked(world->system_caches,
                                                           system_cache_idx);
    cache->enabled = DT_false;

    return PRP_OK;
}
