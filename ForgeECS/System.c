#include "../Diagnostics/Assert.h"
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

DT_size SystemRegister(FECS_System system) {
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
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(system_idx < DT_ArrLenUnchecked(g_ctx->systems));
    DIAG_ASSERT(query_idx < DT_ArrLenUnchecked(g_ctx->queries));

    SystemCache data = {.system_idx = system_idx, .query_idx = query_idx};
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
    DIAG_ASSERT(system_cache != DT_null);
    DIAG_ASSERT(system_cache->system_idx < DT_ArrLenUnchecked(g_ctx->systems));
    DIAG_ASSERT(system_cache->query_idx < DT_ArrLenUnchecked(g_ctx->queries));
    DIAG_ASSERT(system_cache->layout_matches != DT_null);

    DT_ArrDeleteUnchecked(&system_cache->layout_matches);

#if !defined(PRP_NDEBUG)
    system_cache->system_idx = PRP_INVALID_INDEX;
    system_cache->query_idx = PRP_INVALID_INDEX;
#endif
}
