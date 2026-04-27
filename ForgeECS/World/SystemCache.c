#include "World-Internals.h"

/**
 * Binary search to an element in the given array.
 *
 * @param behaviors Array to search into.
 * @param len       Len of the given array.
 * @param to_find   The data to find.
 *
 * @return DT_true if found.
 * @return DT_false if not-found.
 */
static DT_bool BSearchBehavior(const DT_size *behaviors, DT_size len,
                               DT_size to_find);
/**
 * Executes a system function for the given chunk.
 * Called via DT_ArrForEach_...
 *
 * @param pChunk      Pointer to chunk to execute on
 * @param system_data Engine state and user data to correctly call system.
 *
 * @return PRP_OK on success.
 */
static PRP_Result SystemExecForEachCb(DT_void *pChunk, DT_void *system_data);

static DT_bool BSearchBehavior(const DT_size *behaviors, DT_size len,
                               DT_size to_find) {
    DT_size low = 0, high = len;

    while (low < high) {
        DT_size mid = low + ((high - low) / 2);
        if (behaviors[mid] == to_find) {
            return DT_true;
        } else if (behaviors[mid] < to_find) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return DT_false;
}

PRP_Result SystemCacheCreate(World *world, DT_size system_idx,
                             DT_size query_idx, DT_size *pIdx) {
    SystemCache data = {.system_idx = system_idx,
                        .query_idx = query_idx,
                        .is_enabled = DT_true};
    data.system =
        *(FECS_System *)DT_ArrGetUnchecked(g_ctx->systems, system_idx);
    PRP_Result code;

    Query *query = DT_ArrGetUnchecked(g_ctx->queries, query_idx);
    const DT_size CACHE_INIT_SIZE = 4;
    code = DT_ArrCreateUnchecked(sizeof(DT_size), CACHE_INIT_SIZE,
                                 &data.layout_matches);
    if (code != PRP_OK) {
        return code;
    }

    DT_size behavior_match_len;
    const DT_size *behavior_matches =
        DT_ArrRawUnchecked(query->behavior_matches, &behavior_match_len);
    if (behavior_match_len) {
        DT_size layouts_len;
        const Layout *layouts =
            DT_ArrRawUnchecked(world->layouts, &layouts_len);

        for (DT_size i = 0; i < layouts_len; i++) {
            DT_size behavior_idx = layouts[i].behavior_idx;
            if (!BSearchBehavior(behavior_matches, behavior_match_len,
                                 behavior_idx)) {
                continue;
            }
            code = DT_ArrPushUnchecked(data.layout_matches, &i);
            if (code != PRP_OK) {
                goto err_path;
            }
        }
    }

    code = DT_ArrPushUnchecked(world->system_caches, &data);
    if (code != PRP_OK) {
        goto err_path;
    }

    *pIdx = DT_ArrLen(world->system_caches) - 1;

    return PRP_OK;

err_path:
    if (data.layout_matches) {
        DT_ArrDeleteUnchecked(&data.layout_matches);
    }

    return code;
}

DT_bool SystemCacheIsAlreadyExisting(World *world, DT_size system_idx,
                                     DT_size query_idx, DT_size *pOut) {
    DT_size len;
    const SystemCache *system_caches =
        DT_ArrRawUnchecked(world->system_caches, &len);

    for (DT_size i = 0; i < len; i++) {
        const SystemCache *cache = &system_caches[i];
        if (cache->system_idx == system_idx && cache->query_idx == query_idx) {
            *pOut = i;
            return DT_true;
        }
    }

    return DT_false;
}

PRP_Result SystemCacheDelete(DT_void *system_cache, DT_void *_) {
    (DT_void) _;
    SystemCache *s = system_cache;

    DT_ArrDeleteUnchecked(&s->layout_matches);

#if !defined(PRP_NDEBUG)
    s->system = DT_null;
    s->system_idx = PRP_INVALID_INDEX;
    s->query_idx = PRP_INVALID_INDEX;
#endif

    return PRP_OK;
}

/**
 * Internal state with everything needed to execute system and user
 * interactablity.
 */
struct SystemData {
    Behavior *behavior;
    DT_void *user_data;
    FECS_System system;
    DT_void *chunk_ptr;
    DT_u32 occupied_slots;
};

static PRP_Result SystemExecForEachCb(DT_void *pChunk, DT_void *system_data) {
    Chunk *chunk = *(Chunk **)pChunk;
    FECS_SystemData *data = system_data;
    data->occupied_slots = ~chunk->free_slot;
    if (data->occupied_slots == 0) {
        return PRP_OK;
    }
    data->chunk_ptr = chunk->mem;

    data->system(data, data->user_data);

    return PRP_OK;
}

DT_void SystemExec(World *world, const SystemCache *system_cache,
                   DT_void *user_data) {
    DT_size matches_len, layouts_len;
    const DT_size *layout_matches =
        DT_ArrRawUnchecked(system_cache->layout_matches, &matches_len);
    const Layout *layouts = DT_ArrRawUnchecked(world->layouts, &layouts_len);

    FECS_SystemData system_data = {.user_data = user_data,
                                   .system = system_cache->system};
    for (DT_size i = 0; i < matches_len; i++) {
        DT_size layout_idx = layout_matches[i];
        const Layout *layout = &layouts[layout_idx];
        Behavior *behavior =
            DT_ArrGetUnchecked(g_ctx->behaviors, layout->behavior_idx);
        system_data.behavior = behavior;

        DT_ArrForEachUnchecked(layout->chunk_ptrs, SystemExecForEachCb,
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
