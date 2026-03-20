#include "../Diagnostics/Assert.h"
#include "Internals.h"

PRP_Result QueryGetLastErrCode(DT_void) { return last_err_code; }

DT_size QueryRegisterWArray(DT_size *inc_comps, DT_size inc_len,
                            DT_size *exc_comps, DT_size exc_len) {
    DIAG_ASSERT(inc_comps != DT_null);
    DIAG_ASSERT(inc_len > 0);
    DIAG_ASSERT(((exc_comps == DT_null && exc_len == 0) ||
                 (exc_comps != DT_null && exc_len > 0)));

    Query data = {0};
    DT_size total_comps = DT_ArrLenUnchecked(g_ctx->comps);
    data.inc = DT_BitmapCreateUnchecked(total_comps);
    if (!data.inc) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    }
    if (exc_comps && exc_len > 0) {
        data.exc = DT_BitmapCreateUnchecked(total_comps);
        if (!data.exc) {
            SET_LAST_ERR_CODE(PRP_ERR_OOM);
            goto free_internals;
        }
    }

    for (DT_size i = 0; i < inc_len; i++) {
        DIAG_ASSERT(inc_comps[i] < total_comps);
        DT_BitmapSetUnchecked(data.inc, inc_comps[i]);
    }
    if (data.exc) {
        for (DT_size i = 0; i < exc_len; i++) {
            DIAG_ASSERT(exc_comps[i] < total_comps);
            DT_BitmapSetUnchecked(data.exc, exc_comps[i]);
        }
    }

    PRP_Result code = DT_ArrPushUnchecked(g_ctx->queries, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        goto free_internals;
    }

    // The -1 to convert len to idx.
    return DT_ArrLenUnchecked(g_ctx->queries) - 1;

free_internals:
    if (data.inc) {
        DT_BitmapDeleteUnchecked(&data.inc);
    }
    if (data.exc) {
        DT_BitmapDeleteUnchecked(&data.exc);
    }

    return PRP_INVALID_INDEX;
}

DT_size QueryRegisterWArr(DT_Arr *inc_comps, DT_Arr *exc_comps) {
    DIAG_ASSERT(inc_comps != DT_null);

    DT_size inc_len;
    DT_size *arr1 = (DT_size *)(DT_ArrRawUnchecked(inc_comps, &inc_len));
    DIAG_ASSERT(arr1 != DT_null);
    DIAG_ASSERT(inc_len > 0);

    DT_size exc_len = 0;
    DT_size *arr2 = DT_null;
    if (exc_comps) {
        arr2 = (DT_size *)(DT_ArrRawUnchecked(exc_comps, &exc_len));
        DIAG_ASSERT(arr2 != DT_null);
        DIAG_ASSERT(exc_len > 0);
    }

    return QueryRegisterWArray(arr1, inc_len, arr2, exc_len);
}

DT_void QueryDelete(Query *query) {
    DIAG_ASSERT(query != DT_null);
    DIAG_ASSERT(query->inc != DT_null);

    DT_BitmapDeleteUnchecked(&query->inc);
    if (query->exc) {
        DT_BitmapDeleteUnchecked(&query->exc);
    }
}
