#include "World.h"
#include "../Internals/World/World-Internals.h"

/* ----  WORLDS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldCreate(DT_DSId *pWorld_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(pWorld_id != DT_null);
    if (!pWorld_id) {
        return PRP_ERR_INV_ARG;
    }

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

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDelete(DT_DSId *pWorld_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DIAG_ASSERT(pWorld_id != DT_null);
    DT_bool is_id_valid = DT_DSIdIsValidUnchecked(g_ctx->worlds, *pWorld_id);
    DIAG_ASSERT_MSG(is_id_valid,
                    "The given world id is not a valid id to a world.");
    if (!pWorld_id || !is_id_valid) {
        return PRP_ERR_INV_ARG;
    }

    DT_DSArrDelElemUnchecked(g_ctx->worlds, pWorld_id);

    return PRP_OK;
}

PRP_Result WorldDeleteCb(DT_void *world) {
    World *w = world;

    DT_ArrForEachUnchecked(w->layouts, LayoutDelete, DT_null);
    DT_ArrForEachUnchecked(w->system_caches, SystemCacheDelete, DT_null);
    DT_ArrDeleteUnchecked(&w->layouts);
    DT_ArrDeleteUnchecked(&w->system_caches);

    return PRP_OK;
}

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemCacheCreate(DT_DSId world_id,
                                                         DT_size system_idx,
                                                         DT_size query_idx,
                                                         DT_size *pIdx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(system_idx < DT_ArrLen(g_ctx->systems),
                    "The given system_idx doesn't exist.");
    DIAG_ASSERT_MSG(query_idx < DT_ArrLen(g_ctx->queries),
                    "The given system_idx doesn't exist.");
    DIAG_ASSERT(pIdx != DT_null);
    if (code != PRP_OK || system_idx >= DT_ArrLen(g_ctx->systems) ||
        query_idx >= DT_ArrLen(g_ctx->queries) || !pIdx) {
        return PRP_ERR_INV_ARG;
    }
    World *world = w;

    if (SystemCacheIsAlreadyExisting(world, system_idx, query_idx, pIdx)) {
        return PRP_OK;
    }

    return SystemCacheCreate(world, system_idx, query_idx, pIdx);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldSystemCacheExecAll(DT_DSId world_id, DT_void *user_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    World *world = w;

    DT_size len;
    const SystemCache *system_caches =
        DT_ArrRawUnchecked(world->system_caches, &len);

    for (DT_size i = 0; i < len; i++) {
        const SystemCache *cache = &(system_caches[i]);
        if (!cache->is_enabled) {
            continue;
        }
        SystemExec(world, &(system_caches[i]), user_data);
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldSystemCacheExecOne(
    DT_DSId world_id, DT_size system_cache_idx, DT_void *user_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    World *world = w;
    DIAG_ASSERT_MSG(system_cache_idx < DT_ArrLen(world->system_caches),
                    "The given system cache idx is invalid.");
    if (system_cache_idx >= DT_ArrLen(world->system_caches)) {
        return PRP_ERR_INV_ARG;
    }

    SystemCache *system_cache =
        DT_ArrGetUnchecked(world->system_caches, system_cache_idx);
    if (!system_cache->is_enabled) {
        DIAG_LOG_INFO(DIAG_LOG_CODE_NONE,
                      "The given system cache: %zu is disabled.",
                      system_cache_idx);
        return PRP_OK;
    }

    SystemExec(world, system_cache, user_data);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldEnableSystemCache(DT_DSId world_id, DT_size system_cache_idx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    World *world = w;
    DIAG_ASSERT_MSG(system_cache_idx < DT_ArrLen(world->system_caches),
                    "The given system cache idx is invalid.");
    if (system_cache_idx >= DT_ArrLen(world->system_caches)) {
        return PRP_ERR_INV_ARG;
    }

    SystemCache *system_cache =
        DT_ArrGetUnchecked(world->system_caches, system_cache_idx);
    system_cache->is_enabled = DT_true;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldDisableSystemCache(DT_DSId world_id, DT_size system_cache_idx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    World *world = w;
    DIAG_ASSERT_MSG(system_cache_idx < DT_ArrLen(world->system_caches),
                    "The given system cache idx is invalid.");
    if (system_cache_idx >= DT_ArrLen(world->system_caches)) {
        return PRP_ERR_INV_ARG;
    }

    SystemCache *system_cache =
        DT_ArrGetUnchecked(world->system_caches, system_cache_idx);
    system_cache->is_enabled = DT_false;

    return PRP_OK;
}

/* ----  LAYOUTS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutCreate(DT_DSId world_id,
                                                    DT_size behavior_idx,
                                                    DT_size *pIdx) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(behavior_idx < DT_ArrLen(g_ctx->behaviors),
                    "The given behavior idx is invalid ");
    if (code != PRP_OK || behavior_idx >= DT_ArrLen(g_ctx->behaviors) ||
        !pIdx) {
        return PRP_ERR_INV_ARG;
    }
    World *world = w;

    if (LayoutIsAlreadyExisting(world, behavior_idx, pIdx)) {
        return PRP_OK;
    }

    return LayoutCreate(world, behavior_idx, pIdx);
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutSpawnEntity(DT_DSId world_id,
                                                         DT_size layout_idx,
                                                         FECS_Entity *pEntity) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pEntity != DT_null);
    if (code != PRP_OK || !pEntity) {
        return PRP_ERR_INV_ARG;
    }

    World *world = w;
    DIAG_ASSERT_MSG(layout_idx < DT_ArrLen(world->layouts),
                    "The given layout idx is invalid.");
    if (layout_idx >= DT_ArrLen(world->layouts)) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutSpawnEntity(world, layout_idx, pEntity);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSpawnEntities(DT_DSId world_id, DT_size layout_idx, DT_size count,
                         FECS_EntityBatch **pEntities) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pEntities != DT_null);
    DIAG_ASSERT(count > 0);
    if (code != PRP_OK || !pEntities || !count) {
        return PRP_ERR_INV_ARG;
    }

    World *world = w;
    DIAG_ASSERT_MSG(layout_idx < DT_ArrLen(world->layouts),
                    "The given layout idx is invalid.");
    if (layout_idx >= DT_ArrLen(world->layouts)) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutSpawnEntities(world, layout_idx, count, pEntities);
}

PRP_FN_API PRP_Result FECS_LayoutIsEntityValid(DT_DSId world_id,
                                               const FECS_Entity entity,
                                               DT_bool *pRslt) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pRslt != DT_null);
    if (code != PRP_OK || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = LayoutIsEntityValid(w, entity);

    return PRP_OK;
}

PRP_FN_API PRP_Result FECS_LayoutAreEntitiesValid(
    DT_DSId world_id, const FECS_EntityBatch *entities, DT_bool *pRslt) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pRslt != DT_null);
    DIAG_ASSERT(entities != DT_null);
    if (code != PRP_OK || !pRslt || !entities) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = LayoutAreEntitiesValid(w, entities);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutKillEntity(DT_DSId world_id,
                                                        FECS_Entity entity) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DT_bool is_valid = LayoutIsEntityValid(w, entity);
    DIAG_ASSERT_MSG(is_valid, "The given entity is invalid");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    LayoutKillEntity(w, entity);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutKillEntities(DT_DSId world_id, FECS_EntityBatch **pEntities) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pEntities != DT_null);
    if (code != PRP_OK || !pEntities) {
        return PRP_ERR_INV_ARG;
    }
    /*
     *    DT_bool is_valid = LayoutAreEntitiesValid(w, *pEntities);
     *    DIAG_ASSERT_MSG(is_valid, "The given entities are invalid");
     *    if (!is_valid) {
     *        return PRP_ERR_INV_ARG;
     *    }
     *
     * The above lines of validity checks are removed since the kill entities
     * now internally validates entities as it is parsing to improve
     * performance.
     **/

    return LayoutKillEntities(w, pEntities);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutGetEntityComp(DT_DSId world_id, const FECS_Entity entity,
                         DT_size comp_idx, DT_void **dest) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(comp_idx < DT_ArrLen(g_ctx->comps),
                    "The given comp idx is invalid.");
    DIAG_ASSERT(dest != DT_null);
    if (code != PRP_OK || comp_idx >= DT_ArrLen(g_ctx->comps) || !dest) {
        return PRP_ERR_INV_ARG;
    }
    DT_bool is_valid = LayoutIsEntityValid(w, entity);
    DIAG_ASSERT_MSG(is_valid, "The given entity is invalid");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutGetEntityComp(w, entity, comp_idx, dest);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSetEntityComp(DT_DSId world_id, const FECS_Entity entity,
                         DT_size comp_idx, const DT_void *data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(comp_idx < DT_ArrLen(g_ctx->comps),
                    "The given comp idx is invalid.");
    DIAG_ASSERT(data != DT_null);
    if (code != PRP_OK || comp_idx >= DT_ArrLen(g_ctx->comps) || !data) {
        return PRP_ERR_INV_ARG;
    }
    DT_bool is_valid = LayoutIsEntityValid(w, entity);
    DIAG_ASSERT_MSG(is_valid, "The given entity is invalid");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutSetEntityComp(w, entity, comp_idx, data);
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutForEachEntities(
    DT_DSId world_id, FECS_EntityBatch *entities, DT_size comp_idx,
    PRP_Result (*cb)(DT_void *comp_data, DT_void *user_data),
    DT_void *user_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *w;
    PRP_Result code = DT_DSIdToDataChecked(g_ctx->worlds, world_id, &w);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(entities != DT_null);
    DIAG_ASSERT_MSG(comp_idx < DT_ArrLen(g_ctx->comps),
                    "The given comp idx is invalid.");
    DIAG_ASSERT(cb != DT_null);
    if (code != PRP_OK || !entities || comp_idx >= DT_ArrLen(g_ctx->comps)) {
        return PRP_ERR_INV_ARG;
    }
    /*
     *    DT_bool is_valid = LayoutAreEntitiesValid(w, *pEntities);
     *    DIAG_ASSERT_MSG(is_valid, "The given entities are invalid");
     *    if (!is_valid) {
     *        return PRP_ERR_INV_ARG;
     *    }
     *
     * The above lines of validity checks are removed since the for each
     * entities now internally validates entities as it is parsing to improve
     * performance.
     **/

    return LayoutForEachEntities(w, entities, comp_idx, cb, user_data);
}
