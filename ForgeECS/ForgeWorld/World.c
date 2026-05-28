#include "World.h"
#include "ForgeECS/Internals/FECS-Internals.h"
#include "ForgeECS/Internals/ForgeWorld/World-Internals.h"

/* ----  WORLDS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldCreate(FECS_WorldId *pWorld_id) {
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

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDelete(FECS_WorldId *pWorld_id) {
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

PRP_Result WorldDeleteCb(DT_void *pWorld) {
    World *pWorld_instance = pWorld;

    DT_ArrForEachUnchecked(pWorld_instance->layouts, LayoutDelete, DT_null);
    DT_ArrForEachUnchecked(pWorld_instance->system_caches, SystemCacheDelete,
                           DT_null);
    DT_ArrDeleteUnchecked(&pWorld_instance->layouts);
    DT_ArrDeleteUnchecked(&pWorld_instance->system_caches);

    return PRP_OK;
}

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemCacheCreate(
    FECS_WorldId world_id, FECS_SystemId system_id, FECS_QueryId query_id,
    FECS_SystemCacheId *pSystem_cache_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(system_id < DT_ArrLen(g_ctx->systems),
                    "The given system_idx doesn't exist.");
    DIAG_ASSERT_MSG(query_id < DT_ArrLen(g_ctx->queries),
                    "The given system_idx doesn't exist.");
    DIAG_ASSERT(pSystem_cache_id != DT_null);
    if (code != PRP_OK || system_id >= DT_ArrLen(g_ctx->systems) ||
        query_id >= DT_ArrLen(g_ctx->queries) || !pSystem_cache_id) {
        return PRP_ERR_INV_ARG;
    }
    World *pWorld = pWorld_instance;

    if (SystemCacheIsAlreadyExisting(pWorld, system_id, query_id,
                                     pSystem_cache_id)) {
        return PRP_OK;
    }

    return SystemCacheCreate(pWorld, system_id, query_id, pSystem_cache_id);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldSystemCacheExecAll(FECS_WorldId world_id, DT_void *pUser_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    World *pWorld = pWorld_instance;

    DT_size len;
    const SystemCache *pSystem_caches =
        DT_ArrRawUnchecked(pWorld->system_caches, &len);

    for (DT_size i = 0; i < len; i++) {
        const SystemCache *pSystem_cache = &(pSystem_caches[i]);
        if (!pSystem_cache->is_enabled) {
            continue;
        }
        SystemExec(pWorld, &(pSystem_caches[i]), pUser_data);
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldSystemCacheExecOne(
    FECS_WorldId world_id, FECS_SystemCacheId system_cache_id,
    DT_void *pUser_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    World *pWorld = pWorld_instance;
    DIAG_ASSERT_MSG(system_cache_id < DT_ArrLen(pWorld->system_caches),
                    "The given system cache idx is invalid.");
    if (system_cache_id >= DT_ArrLen(pWorld->system_caches)) {
        return PRP_ERR_INV_ARG;
    }

    SystemCache *pSystem_cache =
        DT_ArrGetUnchecked(pWorld->system_caches, system_cache_id);
    if (!pSystem_cache->is_enabled) {
        DIAG_LOG_INFO(DIAG_LOG_CODE_NONE,
                      "The given system cache: %zu is disabled.",
                      system_cache_id);
        return PRP_OK;
    }

    SystemExec(pWorld, pSystem_cache, pUser_data);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldEnableSystemCache(
    FECS_WorldId world_id, FECS_SystemCacheId system_cache_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    World *pWorld = pWorld_instance;
    DIAG_ASSERT_MSG(system_cache_id < DT_ArrLen(pWorld->system_caches),
                    "The given system cache idx is invalid.");
    if (system_cache_id >= DT_ArrLen(pWorld->system_caches)) {
        return PRP_ERR_INV_ARG;
    }

    SystemCache *pSystem_cache =
        DT_ArrGetUnchecked(pWorld->system_caches, system_cache_id);
    pSystem_cache->is_enabled = DT_true;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDisableSystemCache(
    FECS_WorldId world_id, FECS_SystemCacheId system_cache_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    World *pWorld = pWorld_instance;
    DIAG_ASSERT_MSG(system_cache_id < DT_ArrLen(pWorld->system_caches),
                    "The given system cache idx is invalid.");
    if (system_cache_id >= DT_ArrLen(pWorld->system_caches)) {
        return PRP_ERR_INV_ARG;
    }

    SystemCache *pSystem_cache =
        DT_ArrGetUnchecked(pWorld->system_caches, system_cache_id);
    pSystem_cache->is_enabled = DT_false;

    return PRP_OK;
}

/* ----  LAYOUTS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutCreate(FECS_WorldId world_id,
                                                    FECS_BehaviorId behavior_id,
                                                    FECS_LayoutId *pLayout_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(behavior_id < DT_ArrLen(g_ctx->behaviors),
                    "The given behavior idx is invalid ");
    if (code != PRP_OK || behavior_id >= DT_ArrLen(g_ctx->behaviors) ||
        !pLayout_id) {
        return PRP_ERR_INV_ARG;
    }
    World *pWorld = pWorld_instance;

    if (LayoutIsAlreadyExisting(pWorld, behavior_id, pLayout_id)) {
        return PRP_OK;
    }

    return LayoutCreate(pWorld, behavior_id, pLayout_id);
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutSpawnEntity(
    FECS_WorldId world_id, FECS_LayoutId layout_id, FECS_Entity *pEntity) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pEntity != DT_null);
    if (code != PRP_OK || !pEntity) {
        return PRP_ERR_INV_ARG;
    }

    World *pWorld = pWorld_instance;
    DIAG_ASSERT_MSG(layout_id < DT_ArrLen(pWorld->layouts),
                    "The given layout idx is invalid.");
    if (layout_id >= DT_ArrLen(pWorld->layouts)) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutSpawnEntity(pWorld, layout_id, pEntity);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSpawnEntities(FECS_WorldId world_id, FECS_LayoutId layout_id,
                         DT_size entity_count, FECS_EntityBatch **ppBatch) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(ppBatch != DT_null);
    DIAG_ASSERT(entity_count > 0);
    if (code != PRP_OK || !ppBatch || !entity_count) {
        return PRP_ERR_INV_ARG;
    }

    World *pWorld = pWorld_instance;
    DIAG_ASSERT_MSG(layout_id < DT_ArrLen(pWorld->layouts),
                    "The given layout idx is invalid.");
    if (layout_id >= DT_ArrLen(pWorld->layouts)) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutSpawnEntities(pWorld, layout_id, entity_count, ppBatch);
}

PRP_FN_API PRP_Result FECS_LayoutIsEntityValid(FECS_WorldId world_id,
                                               const FECS_Entity entity,
                                               DT_bool *pRslt) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pRslt != DT_null);
    if (code != PRP_OK || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = LayoutIsEntityValid(pWorld_instance, entity);

    return PRP_OK;
}

PRP_FN_API PRP_Result FECS_LayoutAreEntitiesValid(
    FECS_WorldId world_id, const FECS_EntityBatch *pBatch, DT_bool *pRslt) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pRslt != DT_null);
    DIAG_ASSERT(pBatch != DT_null);
    if (code != PRP_OK || !pRslt || !pBatch) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = LayoutAreEntitiesValid(pWorld_instance, pBatch);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutKillEntity(FECS_WorldId world_id,
                                                        FECS_Entity entity) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DT_bool is_valid = LayoutIsEntityValid(pWorld_instance, entity);
    DIAG_ASSERT_MSG(is_valid, "The given entity is invalid");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    LayoutKillEntity(pWorld_instance, entity);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutKillEntities(FECS_WorldId world_id, FECS_EntityBatch **ppBatch) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(ppBatch != DT_null);
    if (code != PRP_OK || !ppBatch) {
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

    return LayoutKillEntities(pWorld_instance, ppBatch);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutGetEntityComp(FECS_WorldId world_id, const FECS_Entity entity,
                         FECS_CompId comp_id, DT_void **ppDest_ptr) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(comp_id < DT_ArrLen(g_ctx->comps),
                    "The given comp idx is invalid.");
    DIAG_ASSERT(ppDest_ptr != DT_null);
    if (code != PRP_OK || comp_id >= DT_ArrLen(g_ctx->comps) || !ppDest_ptr) {
        return PRP_ERR_INV_ARG;
    }
    DT_bool is_valid = LayoutIsEntityValid(pWorld_instance, entity);
    DIAG_ASSERT_MSG(is_valid, "The given entity is invalid");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutGetEntityComp(pWorld_instance, entity, comp_id, ppDest_ptr);
}

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSetEntityComp(FECS_WorldId world_id, const FECS_Entity entity,
                         FECS_CompId comp_id, const DT_void *pData) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT_MSG(comp_id < DT_ArrLen(g_ctx->comps),
                    "The given comp idx is invalid.");
    DIAG_ASSERT(pData != DT_null);
    if (code != PRP_OK || comp_id >= DT_ArrLen(g_ctx->comps) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    DT_bool is_valid = LayoutIsEntityValid(pWorld_instance, entity);
    DIAG_ASSERT_MSG(is_valid, "The given entity is invalid");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    return LayoutSetEntityComp(pWorld_instance, entity, comp_id, pData);
}

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutForEachEntities(
    FECS_WorldId world_id, FECS_EntityBatch *pBatch, FECS_CompId comp_id,
    PRP_Result (*cb)(DT_void *pComp_data, DT_void *pUser_data),
    DT_void *pUser_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    if (!g_ctx->schema_lock) {
        return PRP_ERR_INV_STATE;
    }
    DT_void *pWorld_instance;
    PRP_Result code =
        DT_DSIdToDataChecked(g_ctx->worlds, world_id, &pWorld_instance);
    DIAG_ASSERT_MSG(code == PRP_OK, "The given world id is invalid.");
    DIAG_ASSERT(pBatch != DT_null);
    DIAG_ASSERT_MSG(comp_id < DT_ArrLen(g_ctx->comps),
                    "The given comp idx is invalid.");
    DIAG_ASSERT(cb != DT_null);
    if (code != PRP_OK || !pBatch || comp_id >= DT_ArrLen(g_ctx->comps)) {
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

    return LayoutForEachEntities(pWorld_instance, pBatch, comp_id, cb,
                                 pUser_data);
}
