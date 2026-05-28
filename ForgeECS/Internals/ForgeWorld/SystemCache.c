#include "ForgeECS/Internals/FECS-Internals.h"
#include "World-Internals.h"

/**
 * Internal state with everything needed to execute system and user
 * interactablity.
 */
struct SystemData {
    Behavior *pBehavior;
    DT_void *pUser_data;
    FECS_SystemFunc system_func;
    DT_void *pChunk_mem;
    DT_u32 occupied_slots;
};

/**
 * Binary search to an element in the given array.
 *
 * @param pBehavior_ids Array to search into.
 * @param len           Len of the given array.
 * @param to_find       The data to find.
 *
 * @return DT_true if found.
 * @return DT_false if not-found.
 */
static DT_bool BSearchBehavior(const FECS_BehaviorId *pBehavior_ids,
                               DT_size len, FECS_BehaviorId to_find);
/**
 * Executes a system function for the given chunk.
 * Called via DT_ArrForEach_...
 *
 * @param ppChunk      Pointer to chunk pointer to execute on
 * @param pSystem_data Engine state and user data to correctly call system.
 *
 * @return PRP_OK on success.
 */
static PRP_Result SystemExecForEachCb(DT_void *ppChunk, DT_void *pSystem_data);

static DT_bool BSearchBehavior(const FECS_BehaviorId *pBehavior_ids,
                               DT_size len, FECS_BehaviorId to_find) {
    DT_size low = 0, high = len;

    while (low < high) {
        DT_size mid = low + ((high - low) / 2);
        if (pBehavior_ids[mid] == to_find) {
            return DT_true;
        } else if (pBehavior_ids[mid] < to_find) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return DT_false;
}

PRP_Result SystemCacheCreate(World *pWorld, FECS_SystemId system_id,
                             FECS_QueryId query_id,
                             FECS_SystemCacheId *pSystem_cache_id) {
    SystemCache data = {
        .system_id = system_id, .query_id = query_id, .is_enabled = DT_true};
    data.system_func =
        *(FECS_SystemFunc *)DT_ArrGetUnchecked(g_ctx->systems, system_id);
    PRP_Result code;

    Query *pQuery = DT_ArrGetUnchecked(g_ctx->queries, query_id);
    const DT_size CACHE_INIT_SIZE = 4;
    code = DT_ArrCreateUnchecked(sizeof(FECS_LayoutId), CACHE_INIT_SIZE,
                                 &data.layout_id_matches);
    if (code != PRP_OK) {
        return code;
    }

    DT_size behavior_match_len;
    const FECS_BehaviorId *pBehavior_matches =
        DT_ArrRawUnchecked(pQuery->behavior_id_matches, &behavior_match_len);
    if (behavior_match_len) {
        DT_size layouts_len;
        const Layout *pLayouts =
            DT_ArrRawUnchecked(pWorld->layouts, &layouts_len);

        for (DT_size i = 0; i < layouts_len; i++) {
            FECS_BehaviorId behavior_id = pLayouts[i].behavior_id;
            if (!BSearchBehavior(pBehavior_matches, behavior_match_len,
                                 behavior_id)) {
                continue;
            }
            code = DT_ArrPushUnchecked(data.layout_id_matches, &i);
            if (code != PRP_OK) {
                goto err_path;
            }
        }
    }

    code = DT_ArrPushUnchecked(pWorld->system_caches, &data);
    if (code != PRP_OK) {
        goto err_path;
    }

    *pSystem_cache_id = DT_ArrLen(pWorld->system_caches) - 1;

    return PRP_OK;

err_path:
    if (data.layout_id_matches) {
        DT_ArrDeleteUnchecked(&data.layout_id_matches);
    }

    return code;
}

DT_bool SystemCacheIsAlreadyExisting(World *pWorld, FECS_SystemId system_id,
                                     FECS_QueryId query_id,
                                     FECS_SystemCacheId *pFound_Id) {
    DT_size len;
    const SystemCache *pSystem_caches =
        DT_ArrRawUnchecked(pWorld->system_caches, &len);

    for (DT_size i = 0; i < len; i++) {
        const SystemCache *pSystem_cache = &pSystem_caches[i];
        if (pSystem_cache->system_id == system_id &&
            pSystem_cache->query_id == query_id) {
            *pFound_Id = i;
            return DT_true;
        }
    }

    return DT_false;
}

PRP_Result SystemCacheDelete(DT_void *pSystem_cache, DT_void *_) {
    (DT_void) _;
    SystemCache *pSystem_cache_instance = pSystem_cache;

    DT_ArrDeleteUnchecked(&pSystem_cache_instance->layout_id_matches);

#if !defined(PRP_NDEBUG)
    pSystem_cache_instance->system_func = DT_null;
    pSystem_cache_instance->system_id = PRP_INVALID_INDEX;
    pSystem_cache_instance->query_id = PRP_INVALID_INDEX;
#endif

    return PRP_OK;
}

static PRP_Result SystemExecForEachCb(DT_void *ppChunk, DT_void *pSystem_data) {
    Chunk *pChunk = *(Chunk **)ppChunk;
    FECS_SystemData *pData = pSystem_data;
    pData->occupied_slots = ~pChunk->free_slot;
    if (pData->occupied_slots == 0) {
        return PRP_OK;
    }
    pData->pChunk_mem = pChunk->mem;

    pData->system_func(pData, pData->pUser_data);

    return PRP_OK;
}

DT_void SystemExec(World *pWorld, const SystemCache *pSystem_cache,
                   DT_void *pUser_data) {
    DT_size matches_len, layouts_len;
    const DT_size *pLayout_id_matches =
        DT_ArrRawUnchecked(pSystem_cache->layout_id_matches, &matches_len);
    const Layout *pLayouts = DT_ArrRawUnchecked(pWorld->layouts, &layouts_len);

    FECS_SystemData system_data = {.pUser_data = pUser_data,
                                   .system_func = pSystem_cache->system_func};
    for (DT_size i = 0; i < matches_len; i++) {
        FECS_LayoutId layout_id = pLayout_id_matches[i];
        const Layout *pLayout = &pLayouts[layout_id];
        Behavior *pBehavior =
            DT_ArrGetUnchecked(g_ctx->behaviors, pLayout->behavior_id);

        system_data.pBehavior = pBehavior;

        DT_ArrForEachUnchecked(pLayout->pChunk_ptrs, SystemExecForEachCb,
                               &system_data);
    }
}

// DT_void *SystemCacheGetComp(const FECS_SystemData *system_data,
//                             DT_size comp_idx) {
//     /**
//      * Pop counting for inding components is not that bad after all, since,
//      even
//      * at 1024 components behavior, there will only be around 16 pop countss
//      * done to find the index of any comp in the strides array.
//      */
//     // WoRK in progress.
//     return DT_null;
// }

/**
 * NOTE: Major memory optimization opportunity:
 * replace the query matches and layout matches arrays into bitmaps where each
 * index set is a match at that index.
 */
