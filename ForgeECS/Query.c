#include "../Diagnostics/Assert.h"
#include "Internals.h"

static PRP_Result QueryFindMatches(Query *query);

PRP_Result QueryGetLastErrCode(DT_void) { return last_err_code; }

static PRP_Result QueryFindMatches(Query *query) {
    DIAG_ASSERT(query != DT_null);
    DIAG_ASSERT(query->inc != DT_null);

    if (query->behavior_matches) {
        DT_ArrResetUnchecked(query->behavior_matches);
    } else {
        query->behavior_matches =
            DT_ArrCreateUnchecked(sizeof(DT_size), DT_ARR_DEFAULT_CAP);
        if (!query->behavior_matches) {
            return PRP_ERR_OOM;
        }
    }

    DT_size len;
    const Behavior *behaviors = DT_ArrRawUnchecked(g_ctx->behaviors, &len);
    DIAG_ASSERT(behaviors != DT_null);
    for (DT_size i = 0; i < len; i++) {
        const Behavior *behavior = &behaviors[i];
        DT_bool is_match = (!query->exc || !DT_BitmapHasAnyUnchecked(
                                               behavior->set, query->exc)) &&
                           DT_BitmapHasAllUnchecked(behavior->set, query->inc);
        if (is_match) {
            PRP_Result code = DT_ArrPushUnchecked(query->behavior_matches, &i);
            if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
                DT_ArrDeleteUnchecked(&query->behavior_matches);
                return PRP_ERR_OOM;
            } else if (code != PRP_OK) {
                DT_ArrDeleteUnchecked(&query->behavior_matches);
                return PRP_ERR_INTERNAL;
            }
        }
    }
    if (DT_ArrLenUnchecked(query->behavior_matches) == 0) {
        /*
         * If no match, no point in wasting memory. This is a valid documented
         * condition.
         */
        DT_ArrDeleteUnchecked(&query->behavior_matches);
    }

    return PRP_OK;
}

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
    PRP_Result code = QueryFindMatches(&data);
    if (code != PRP_OK) {
        SET_LAST_ERR_CODE(code);
        goto free_internals;
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

    code = DT_ArrPushUnchecked(g_ctx->queries, &data);
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
    if (data.behavior_matches) {
        DT_ArrDeleteUnchecked(&data.behavior_matches);
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
    if (query->behavior_matches) {
        DT_ArrDeleteUnchecked(&query->behavior_matches);
    }
}
