#include "FECS-Internals.h"

/**
 * Sorts given array using insertion sort.
 *
 * @param comp_idxs Array to sort.
 * @param len       Len of the array.
 */
static DT_void SortCompIdxs(FECS_CompId *pComp_ids, DT_size comp_count);

static DT_void SortCompIdxs(FECS_CompId *pComp_ids, DT_size comp_count) {
    for (DT_size i = 1; i < comp_count; i++) {
        FECS_CompId key = pComp_ids[i];
        DT_size j = i;
        while (j > 0 && pComp_ids[j - 1] > key) {
            pComp_ids[j] = pComp_ids[j - 1];
            j--;
        }
        pComp_ids[j] = key;
    }
}

PRP_Result BehaviorRegister(FECS_CompId *pComp_ids, DT_size comp_count,
                            FECS_BehaviorId *pBehavior_id) {
    PRP_Result code;
    Behavior data = {0};
    code = DT_BitmapCreateUnchecked(DT_ArrLen(g_ctx->comps), &data.set);
    if (code != PRP_OK) {
        goto err_path;
    }
    data.strides = malloc(sizeof(DT_size) * comp_count);
    if (!data.strides) {
        code = PRP_ERR_OOM;
        goto err_path;
    }

    /*
     * Since sorting is just order changing and doesn't change the metadata, it
     * can be done wihtout disturbing validity of arrays.
     */
    SortCompIdxs(pComp_ids, comp_count);
    DT_size comps_len = DT_ArrLen(g_ctx->comps);
    DT_size stride = 0;
    for (DT_size i = 0; i < comp_count; i++) {
        FECS_CompId comp_id = pComp_ids[i];
        if (comp_id >= comps_len) {
            code = PRP_ERR_INV_ARG;
            goto err_path;
        }
        DT_BitmapSetUnchecked(data.set, comp_id);

        DT_size comp_size =
            ((Component *)DT_ArrGetUnchecked(g_ctx->comps, comp_id))->size;
        data.strides[i] = stride;
        stride += CHUNK_CAP * comp_size;
    }
    data.chunk_size = stride + sizeof(Chunk);

    code = DT_ArrPushUnchecked(g_ctx->behaviors, &data);
    if (code != PRP_OK) {
        goto err_path;
    }

    *pBehavior_id = DT_ArrLen(g_ctx->behaviors) - 1;

    return PRP_OK;

err_path:
    if (data.set) {
        DT_BitmapDeleteUnchecked(&data.set);
    }
    if (data.strides) {
        free(data.strides);
    }

    return code;
}

DT_bool BehaviorIsRegistered(FECS_CompId *pComp_ids, DT_size comp_count,
                             FECS_BehaviorId *pFound_id) {
    DT_size behaviors_len;
    const Behavior *pBehaviors =
        DT_ArrRawUnchecked(g_ctx->behaviors, &behaviors_len);

    for (DT_size i = 0; i < behaviors_len; i++) {
        const Behavior *pBehavior = &pBehaviors[i];
        if (DT_BitmapSetCount(pBehavior->set) != comp_count) {
            continue;
        }

        DT_bool is_registered = DT_true;
        for (DT_size j = 0; j < comp_count; j++) {
            if (!DT_BitmapIsSetUnchecked(pBehavior->set, pComp_ids[j])) {
                is_registered = DT_false;
                break;
            }
        }
        if (is_registered) {
            *pFound_id = i;
            return DT_true;
        }
    }

    return DT_false;
}

PRP_Result BehaviorDelete(DT_void *pBehavior, DT_void *_) {
    (DT_void) _;
    Behavior *pBehavior_instance = pBehavior;

    DT_BitmapDeleteUnchecked(&pBehavior_instance->set);
    free(pBehavior_instance->strides);

#if !defined(PRP_NDEBUG)
    pBehavior_instance->strides = DT_null;
    pBehavior_instance->chunk_size = 0;
#endif

    return PRP_OK;
}

DT_size BehaviorGetCompStride(FECS_BehaviorId behavior_id,
                              FECS_CompId comp_id) {
    Behavior *pBehavior = DT_ArrGetUnchecked(g_ctx->behaviors, behavior_id);
    if (comp_id >= DT_BitmapBitCap(pBehavior->set) ||
        !DT_BitmapIsSetUnchecked(pBehavior->set, comp_id)) {
        return PRP_INVALID_SIZE;
    }

    DT_size idx = DT_BitmapBitRankUnchecked(pBehavior->set, comp_id);

    return pBehavior->strides[idx];
}
