#include "FECS-Internals.h"
#include <stdlib.h>

/**
 * Sorts the given comp idx array using insertion sort.
 *
 * @param comp_idxs: The array to sort.
 * @param len: The len of the array.
 */
static DT_void SortCompIdxs(DT_size *comp_idxs, DT_size len);

static DT_void SortCompIdxs(DT_size *comp_idxs, DT_size len) {
    for (DT_size i = 1; i < len; i++) {
        DT_size key = comp_idxs[i];
        DT_size j = i;
        while (j > 0 && comp_idxs[j - 1] > key) {
            comp_idxs[j] = comp_idxs[j - 1];
            j--;
        }
        comp_idxs[j] = key;
    }
}

PRP_Result BehaviorRegister(DT_Arr *comp_idxs, DT_size *pIdx) {
    DT_size len;
    DT_size *arr = (DT_size *)(DT_ArrRawUnchecked(comp_idxs, &len));
    PRP_Result code;
    Behavior data = {0};
    code = DT_BitmapCreateUnchecked(DT_ArrLen(g_ctx->comps), &data.set);
    if (code != PRP_OK) {
        goto err_path;
    }
    data.strides = malloc(sizeof(DT_size) * len);
    if (!data.strides) {
        code = PRP_ERR_OOM;
        goto err_path;
    }

    /*
     * Since sorting is just order changing and doesn't change the metadata, it
     * can be done wihtout disturbing validity of arrays.
     */
    SortCompIdxs(arr, len);
    DT_size comps_len = DT_ArrLen(g_ctx->comps);
    DT_size stride = 0;
    for (DT_size i = 0; i < len; i++) {
        DT_size comp_idx = arr[i];
        if (comp_idx >= comps_len) {
            code = PRP_ERR_INV_ARG;
            goto err_path;
        }
        DT_BitmapSetUnchecked(data.set, comp_idx);

        DT_size comp_size =
            ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))
                ->size;
        data.strides[i] = stride;
        stride += CHUNK_CAP * comp_size;
    }
    data.chunk_size = stride + sizeof(Chunk);

    code = DT_ArrPushUnchecked(g_ctx->behaviors, &data);
    if (code != PRP_OK) {
        goto err_path;
    }

    *pIdx = DT_ArrLen(g_ctx->behaviors) - 1;

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

DT_bool BehaviorIsRegistered(DT_Arr *comp_idxs, DT_size *pOut) {
    DT_size comps_len;
    const DT_size *comps = DT_ArrRawUnchecked(comp_idxs, &comps_len);
    DT_size behaviors_len;
    const Behavior *behaviors =
        DT_ArrRawUnchecked(g_ctx->behaviors, &behaviors_len);

    for (DT_size i = 0; i < behaviors_len; i++) {
        const Behavior *behavior = &behaviors[i];
        if (DT_BitmapSetCount(behavior->set) != comps_len) {
            continue;
        }

        DT_bool is_registered = DT_true;
        for (DT_size j = 0; j < comps_len; j++) {
            if (!DT_BitmapIsSetUnchecked(behavior->set, comps[j])) {
                is_registered = DT_false;
                break;
            }
        }
        if (is_registered) {
            *pOut = i;
            return DT_true;
        }
    }

    return DT_false;
}

PRP_Result BehaviorDelete(DT_void *behavior, DT_void *_) {
    (DT_void) _;
    Behavior *b = behavior;

    DT_BitmapDeleteUnchecked(&b->set);
    free(b->strides);

#if !defined(PRP_NDEBUG)
    b->strides = DT_null;
    b->chunk_size = 0;
#endif

    return PRP_OK;
}
