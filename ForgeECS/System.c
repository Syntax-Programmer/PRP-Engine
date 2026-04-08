#include "Defs.h"
#include "Internals.h"

/**
 * Performs a binary search on the given array to find the given element.
 *
 * @param behaviors: The array to search into.
 * @param len: The len of the given array.
 * @param to_find: The data to find.
 *
 * @return DT_false if the element is not found, otherwise DT_true.
 */
static DT_bool BSearchBehavior(const DT_size *behaviors, DT_size len,
                               DT_size to_find);
/**
 * A foreach callback for the system exec function that is run on the chunk ptr
 * array of a layout.
 *
 * @param pVal: The Chunk ** we will get.
 * @param user_data: The data we supply to the callback to correctly call system
 * function on the chunk.
 *
 * @return PRP_OK.
 */
static PRP_Result SystemExecForEachCb(DT_void *pVal, DT_void *user_data);

PRP_Result SystemRegister(FECS_System system, DT_size *pIdx) {
    PRP_Result code = DT_ArrPushUnchecked(g_ctx->systems, &system);
    if (code != PRP_OK) {
        return code;
    }

    *pIdx = DT_ArrLen(g_ctx->systems) - 1;

    return PRP_OK;
}

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

DT_bool SystemIsRegistered(FECS_System system, DT_size *pIdx) {
    DT_size len;
    const FECS_System *systems = DT_ArrRawUnchecked(g_ctx->systems, &len);

    for (DT_size i = 0; i < len; i++) {
        if (systems[i] == system) {
            *pIdx = i;
            return DT_true;
        }
    }

    return DT_false;
}

PRP_Result SystemCacheCreate(World *world, DT_size system_idx,
                             DT_size query_idx, DT_size *pIdx) {
    SystemCache data = {
        .system_idx = system_idx, .query_idx = query_idx, .enabled = DT_true};
    data.system =
        *(FECS_System *)DT_ArrGetUnchecked(g_ctx->systems, system_idx);
    PRP_Result code;

    Query *query = DT_ArrGetUnchecked(g_ctx->queries, query_idx);
    if (DT_ArrLen(query->behavior_matches) == 0) {
        data.layout_matches = DT_null;
    } else {
        code = DT_ArrCreateUnchecked(sizeof(DT_size), DT_ARR_DEFAULT_CAP,
                                     &data.layout_matches);
        if (code != PRP_OK) {
            goto err_path;
        }

        DT_size behavior_match_len, layouts_len;
        const DT_size *behavior_matches =
            DT_ArrRawUnchecked(query->behavior_matches, &behavior_match_len);
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
                                     DT_size query_idx, DT_size *pIdx) {
    DT_size len;
    const SystemCache *system_caches =
        DT_ArrRawUnchecked(world->system_caches, &len);

    for (DT_size i = 0; i < len; i++) {
        const SystemCache *cache = &system_caches[i];
        if (cache->system_idx == system_idx && cache->query_idx == query_idx) {
            *pIdx = i;
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

struct SystemData {
    DT_size *strides;
    DT_void *user_data;
    FECS_System system;
    DT_size comp_count;
    DT_void *chunk_ptr;
    DT_u32 occupied_slots;
};

static PRP_Result SystemExecForEachCb(DT_void *pVal, DT_void *user_data) {
    Chunk *chunk = *(Chunk **)pVal;
    FECS_SystemData *data = user_data;
    data->chunk_ptr = chunk->mem;
    data->occupied_slots = ~chunk->free_slot;

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
        DIAG_ASSERT(layout_idx < layouts_len);
        const Layout *layout = &layouts[layout_idx];
        Behavior *behavior =
            DT_ArrGetUnchecked(g_ctx->behaviors, layout->behavior_idx);
        system_data.strides = behavior->strides;
        system_data.comp_count = DT_BitmapSetCount(behavior->set);

        DT_ArrForEachUnchecked(layout->chunk_ptrs, SystemExecForEachCb,
                               &system_data);
    }
}
