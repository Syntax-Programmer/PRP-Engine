#include "Internals.h"

PRP_Result WorldCreate(DT_DSId *pWorld_id) {
    PRP_Result code;
    World data = {0};

    code = DT_ArrCreateUnchecked(sizeof(Layout), DT_ARR_DEFAULT_CAP,
                                 &data.layouts);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_ArrCreateUnchecked(sizeof(SystemCache), DT_ARR_DEFAULT_CAP,
                                 &data.system_caches);
    if (code != PRP_OK) {
        goto err_path;
    }

    code = DT_DSArrAddUnchecked(g_ctx->worlds, &data, pWorld_id);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    if (data.layouts) {
        DT_ArrDeleteUnchecked(&data.layouts);
    }
    if (data.system_caches) {
        DT_ArrDeleteUnchecked(&data.system_caches);
    }

    return code;
}

DT_void WorldDelete(DT_DSId *pWorld_id) {
    DT_DSArrDelElemUnchecked(g_ctx->worlds, pWorld_id);
}

PRP_Result WorldDeleteCb(DT_void *world) {
    World *w = world;

    DT_ArrForEachUnchecked(w->layouts, LayoutDelete, DT_null);
    DT_ArrForEachUnchecked(w->system_caches, SystemCacheDelete, DT_null);
    DT_ArrDeleteUnchecked(&w->layouts);
    DT_ArrDeleteUnchecked(&w->system_caches);

    return PRP_OK;
}

DT_void WorldSystemExecAll(DT_DSId world_id, DT_void *user_data) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);

    DT_size len;
    const SystemCache *system_caches =
        DT_ArrRawUnchecked(world->system_caches, &len);

    for (DT_size i = 0; i < len; i++) {
        SystemExec(world, &(system_caches[i]), user_data);
    }
}

DT_void WorldSystemExecOne(DT_DSId world_id, DT_size system_cache_idx,
                           DT_void *user_data) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    SystemCache *system_cache =
        DT_ArrGetUnchecked(world->system_caches, system_cache_idx);

    SystemExec(world, system_cache, user_data);
}

PRP_Result WorldSystemExecMany(DT_DSId world_id, DT_Arr *system_cache_idxs,
                               DT_void *user_data);

PRP_Result WorldUpdate(DT_DSId world_id, DT_f32 dt);
PRP_Result WorldSetSystemExecOrder(DT_DSId world_id, DT_Arr *system_exec_order);
PRP_Result WorldSync(DT_DSId world_id);

DT_void WorldEnableSystem(DT_DSId world_id, DT_size system_cache_idx) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);

    SystemCache *cache = (SystemCache *)DT_ArrGetUnchecked(world->system_caches,
                                                           system_cache_idx);
    cache->enabled = DT_true;
}

DT_void WorldDisableSystem(DT_DSId world_id, DT_size system_cache_idx) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);

    SystemCache *cache = (SystemCache *)DT_ArrGetUnchecked(world->system_caches,
                                                           system_cache_idx);
    cache->enabled = DT_false;
}
