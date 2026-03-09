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
        DT_BitmapDeleteUnchecked(&data.set);
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return PRP_INVALID_INDEX;
    }

    SortCompIdxs(comp_idxs, len);
    DT_size comps_len = DT_ArrLenUnchecked(g_ctx->comps);
    DT_size stride = 0;
    for (DT_size i = 0; i < len; i++) {
        DT_size comp_idx = comp_idxs[i];
        DT_bool invalid =
            (comp_idx >= comps_len) || (i > 0 && comp_idx == comp_idxs[i - 1]);
        DIAG_ASSERT(!invalid);
        if (invalid) {
            DT_BitmapDeleteUnchecked(&data.set);
            free(data.strides);
            SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
            return PRP_INVALID_INDEX;
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
        return PRP_INVALID_INDEX;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        return PRP_INVALID_INDEX;
    }

    return idx;
}

DT_size BehaviroRegisterWDTArr(DT_Arr *comp_idxs) {
    DIAG_ASSERT(comp_idxs != DT_null);

    DT_size len;
    DT_size *arr = (DT_size *)(DT_ArrRawChecked(comp_idxs, &len));
    if (!arr) {
        PRP_Result code = DT_ArrGetLastErrCode();
        DIAG_ASSERT(code != PRP_ERR_INV_ARG);
        SET_LAST_ERR_CODE(code);
        return PRP_INVALID_INDEX;
    }

    return BehaviorRegisterWArray(arr, len);
}

/* ----  LAYOUT ---- */
