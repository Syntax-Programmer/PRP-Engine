#include "../Diagnostics/Assert.h"
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

PRP_Result SystemGetLastErrCode(DT_void) { return last_err_code; }

DT_size SystemRegister(FECS_System system) {
    ASSERT_CTX_INVARIANT_EXPR;
    DIAG_ASSERT(system != DT_null);

    PRP_Result code = DT_ArrPushUnchecked(g_ctx->systems, &system);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return PRP_INVALID_INDEX;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }

    // The -1 to convert len to idx.
    return DT_ArrLenUnchecked(g_ctx->systems) - 1;
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

DT_size SystemCacheCreate(DT_DSId world_id, DT_size system_idx,
                          DT_size query_idx) {
    ASSERT_CTX_INVARIANT_EXPR;
    DIAG_ASSERT(DT_DSIdIsValidUnchecked(g_ctx->worlds, world_id));
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    ASSERT_WORLD_INVARIANT_EXPR(world);
    DIAG_ASSERT(system_idx < DT_ArrLenUnchecked(g_ctx->systems));
    DIAG_ASSERT(query_idx < DT_ArrLenUnchecked(g_ctx->queries));

    SystemCache data = {.system_idx = system_idx, .query_idx = query_idx};
    data.system =
        *(FECS_System *)DT_ArrGetUnchecked(g_ctx->systems, system_idx);
    Query *query = DT_ArrGetUnchecked(g_ctx->queries, query_idx);

    if (!query->behavior_matches) {
        data.layout_matches = DT_null;
    } else {
        data.layout_matches =
            DT_ArrCreateUnchecked(sizeof(DT_size), DT_ARR_DEFAULT_CAP);
        if (!data.layout_matches) {
            SET_LAST_ERR_CODE(PRP_ERR_OOM);
            goto free_internals;
        }

        DT_size behavior_match_len, layouts_len;
        const DT_size *behavior_matches =
            DT_ArrRawUnchecked(query->behavior_matches, &behavior_match_len);
        const Layout *layouts =
            DT_ArrRawUnchecked(world->layouts, &layouts_len);
        DIAG_ASSERT(layouts != DT_null);

        for (DT_size i = 0; i < layouts_len; i++) {
            DT_size behavior_idx = layouts[i].behavior_idx;
            if (!BSearchBehavior(behavior_matches, behavior_match_len,
                                 behavior_idx)) {
                continue;
            }
            PRP_Result code = DT_ArrPushUnchecked(data.layout_matches, &i);
            if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
                SET_LAST_ERR_CODE(PRP_ERR_OOM);
                goto free_internals;
            } else if (code != PRP_OK) {
                SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
                goto free_internals;
            }
        }
    }
    // Every system is enabled by default.
    data.enabled = DT_true;

    PRP_Result code = DT_ArrPushUnchecked(world->system_caches, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        goto free_internals;
    }

    // The -1 to convert len to idx.
    return DT_ArrLenUnchecked(world->system_caches) - 1;

free_internals:
    if (data.layout_matches) {
        DT_ArrDeleteUnchecked(&data.layout_matches);
    }

    return PRP_INVALID_INDEX;
}

DT_void SystemCacheDelete(SystemCache *system_cache) {
    ASSERT_SYSTEM_CACHE_INVARIANT_EXPR(system_cache);

    DT_ArrDeleteUnchecked(&system_cache->layout_matches);

#if !defined(PRP_NDEBUG)
    system_cache->system = DT_null;
    system_cache->system_idx = PRP_INVALID_INDEX;
    system_cache->query_idx = PRP_INVALID_INDEX;
#endif
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

DT_void SystemExec(World *world, DT_size system_cache_idx, DT_void *user_data) {
    ASSERT_CTX_INVARIANT_EXPR;
    ASSERT_WORLD_INVARIANT_EXPR(world);
    DIAG_ASSERT(system_cache_idx < DT_ArrLenUnchecked(world->system_caches));
    SystemCache *system_cache =
        DT_ArrGetUnchecked(world->system_caches, system_cache_idx);
    ASSERT_SYSTEM_CACHE_INVARIANT_EXPR(system_cache);

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
        DIAG_ASSERT(behavior != DT_null);
        system_data.strides = behavior->strides;
        system_data.comp_count = DT_BitmapSetCountUnchecked(behavior->set);

        DT_ArrForEachUnchecked(layout->chunk_ptrs, SystemExecForEachCb,
                               &system_data);
    }
}
