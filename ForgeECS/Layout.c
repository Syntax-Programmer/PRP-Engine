#include "../Diagnostics/Assert.h"
#include "Internals.h"

static DT_void SortCompIdxs(DT_size *comp_idxs, DT_size len);

PRP_Result LayoutGetLastErrCode(DT_void) { return last_err_code; }

/* ----  BEHAVIOR ---- */

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

DT_size BehaviorRegisterWArray(DT_size *comp_idxs, DT_size len) {
    DIAG_ASSERT(comp_idxs != DT_null);
    DIAG_ASSERT(len > 0);

    /*
     * Since sorting is just order changing and doesn't change the metadata, it
     * can be done wihtout disturbing validity of arrays.
     */
    Behavior data = {0};
    data.set = DT_BitmapCreateUnchecked(DT_ArrLenUnchecked(g_ctx->comps));
    if (!data.set) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return PRP_INVALID_INDEX;
    }
    data.strides = malloc(sizeof(DT_size) * len);
    if (!data.strides) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    }

    SortCompIdxs(comp_idxs, len);
    DT_size comps_len = DT_ArrLenUnchecked(g_ctx->comps);
    DT_size stride = 0;
    for (DT_size i = 0; i < len; i++) {
        DT_size comp_idx = comp_idxs[i];
        DT_bool invalid =
            (comp_idx >= comps_len) || (i > 0 && comp_idx == comp_idxs[i - 1]);
        DIAG_ASSERT(!invalid); // Check arr internals externally also.`
        if (invalid) {
            SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
            goto free_internals;
        }
        DT_BitmapSetUnchecked(data.set, comp_idx);

        DT_size comp_size =
            ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))
                ->size;
        data.strides[i] = stride;
        stride += CHUNK_CAP * comp_size;
    }
    data.chunk_size = stride + sizeof(Chunk);

    DT_size idx = DT_ArrLenUnchecked(g_ctx->behaviors);
    PRP_Result code = DT_ArrPushUnchecked(g_ctx->behaviors, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        goto free_internals;
    }

    return idx;

free_internals:
    if (data.set) {
        DT_BitmapDeleteUnchecked(&data.set);
    }
    if (data.strides) {
        free(data.strides);
    }

    return PRP_INVALID_INDEX;
}

DT_size BehaviroRegisterWDTArr(DT_Arr *comp_idxs) {
    DIAG_ASSERT(comp_idxs != DT_null);

    DT_size len;
    DT_size *arr = (DT_size *)(DT_ArrRawUnchecked(comp_idxs, &len));
    if (!arr) {
        PRP_Result code = DT_ArrGetLastErrCode();
        DIAG_ASSERT(code != PRP_ERR_INV_ARG);
        SET_LAST_ERR_CODE(code);
        return PRP_INVALID_INDEX;
    }

    return BehaviorRegisterWArray(arr, len);
}

DT_void BehaviorDelete(Behavior *behavior) {
    DIAG_ASSERT(behavior != DT_null);
    DIAG_ASSERT(behavior->set != DT_null && behavior->strides != DT_null);

    DT_BitmapDeleteUnchecked(&behavior->set);
    free(behavior->strides);

#if !defined(PRP_NDEBUG)
    behavior->strides = DT_null;
    behavior->chunk_size = 0;
#endif
}

/* ----  LAYOUT ---- */

DT_size LayoutCreate(DT_DSId world_id, DT_size behavior_idx) {
    DIAG_ASSERT(behavior_idx < DT_ArrLenUnchecked(g_ctx->behaviors));
    World *world = DT_DSIdToDataChecked(g_ctx->worlds, world_id);
    if (!world) {
        PRP_Result code = DT_DSArrGetLastErrCode();
        DIAG_ASSERT(code != PRP_ERR_INV_ARG);
        if (code == PRP_ERR_INV_STATE || code == PRP_ERR_OOB) {
            SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        } else if (code != PRP_OK) {
            SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        }
        return PRP_INVALID_INDEX;
    }

    DT_size len;
    const Layout *layouts = DT_ArrRawUnchecked(world->layouts, &len);
    DIAG_ASSERT(behavior_idx < len);
    for (DT_size i = 0; i < len; i++) {
        if (behavior_idx == layouts[i].behavior_idx) {
            return i;
        }
    }

    Layout data = {0};
    data.chunk_ptrs =
        DT_ArrCreateUnchecked(sizeof(Chunk *), DT_ARR_DEFAULT_CAP);
    if (!data.chunk_ptrs) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    }
    data.free_chunks = DT_BitmapCreateUnchecked(DT_ARR_DEFAULT_CAP);
    if (!data.free_chunks) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    }
    data.behavior_idx = behavior_idx;

    PRP_Result code = DT_ArrPushUnchecked(world->layouts, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        goto free_internals;
    }

    return len;

free_internals:
    if (data.chunk_ptrs) {
        DT_ArrDeleteUnchecked(&data.chunk_ptrs);
    }
    if (data.free_chunks) {
        DT_BitmapDeleteUnchecked(&data.free_chunks);
    }

    return PRP_INVALID_INDEX;
}

DT_void LayoutDelete(Layout *layout) {
    DIAG_ASSERT(layout != DT_null);
    DIAG_ASSERT(layout->chunk_ptrs != DT_null &&
                layout->free_chunks != DT_null);

    DT_ArrDeleteUnchecked(&layout->chunk_ptrs);
    DT_BitmapDeleteUnchecked(&layout->free_chunks);

#if !defined(PRP_NDEBUG)
    layout->behavior_idx = PRP_INVALID_INDEX;
#endif
}


