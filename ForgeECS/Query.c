#include "../Diagnostics/Assert.h"
#include "Internals.h"

/**
 * Finds all the behaviors that match the given query.
 *
 * @param query: The query to find the matches for.
 *
 * @return PRP_ERR_OOM if we can't create the array or can't push into it,
 * PRP_ERR_INTERNAL if some other error occur, otherwise PRP_OK.
 */
static PRP_Result QueryFindMatches(Query *query);
/**
 * Initializes and registers a query internally.
 *
 * @param inc_comps: The array of comps that the query wants to include.
 * @param inc_len: The len of the inc_comps array.
 * @param exc_comps: The array of comps that the query wants to exclude.
 * @param exc_len: The len of the exc_comps array.
 *
 * @return The query index of the fully initialized and registered query.
 */
static DT_size QueryRegisterInternal(DT_size *inc_comps, DT_size inc_len,
                                     DT_size *exc_comps, DT_size exc_len);

PRP_Result QueryGetLastErrCode(DT_void) { return last_err_code; }

static PRP_Result QueryFindMatches(Query *query) {
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
    } else {
        DT_ArrShrinkFitUnchecked(query->behavior_matches);
    }

    return PRP_OK;
}

static DT_size QueryRegisterInternal(DT_size *inc_comps, DT_size inc_len,
                                     DT_size *exc_comps, DT_size exc_len) {
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
        if (inc_comps[i] >= total_comps) {
            SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
            DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                           "The given inc comps array contains invalid comps.");
            goto free_internals;
        }
        DT_BitmapSetUnchecked(data.inc, inc_comps[i]);
    }
    if (data.exc) {
        for (DT_size i = 0; i < exc_len; i++) {
            if (exc_comps[i] >= total_comps) {
                SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
                DIAG_LOG_ERROR(
                    DIAG_LOG_CODE_INVALID_ARG,
                    "The given exc comps array contains invalid comps.");
                goto free_internals;
            }
            if (DT_BitmapIsSetUnchecked(data.inc, exc_comps[i])) {
                SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
                DIAG_LOG_ERROR(
                    DIAG_LOG_CODE_INVALID_ARG,
                    "The given inc and exc comps have overlapping comps");
                goto free_internals;
            }
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

DT_size QueryRegister(DT_Arr *inc_comps, DT_Arr *exc_comps) {
    DT_size inc_len;
    DT_size *arr1 = (DT_size *)(DT_ArrRawUnchecked(inc_comps, &inc_len));
    DT_size exc_len = 0;
    DT_size *arr2 = DT_null;
    if (exc_comps) {
        arr2 = (DT_size *)(DT_ArrRawUnchecked(exc_comps, &exc_len));
    }

    return QueryRegisterInternal(arr1, inc_len, arr2, exc_len);
}

DT_size QueryIsRegistered(DT_Arr *inc_comps, DT_Arr *exc_comps) {
    DT_size inc_comps_len = 0, exc_comps_len = 0;
    const DT_size *inc_raw = DT_ArrRawUnchecked(inc_comps, &inc_comps_len);
    const DT_size *exc_raw = DT_null;
    if (exc_comps) {
        exc_raw = DT_ArrRawUnchecked(exc_comps, &exc_comps_len);
    }

    DT_size queries_len;
    const Query *queries = DT_ArrRawUnchecked(g_ctx->queries, &queries_len);

    for (DT_size i = 0; i < queries_len; i++) {
        const Query *query = &queries[i];
        if ((exc_comps_len && !query->exc) || (!exc_comps_len && query->exc)) {
            continue;
        }
        if (DT_BitmapSetCountUnchecked(query->inc) != inc_comps_len ||
            (query->exc &&
             DT_BitmapSetCountUnchecked(query->exc) != exc_comps_len)) {
            continue;
        }

        DT_bool is_registered = DT_true;
        for (DT_size j = 0; j < inc_comps_len; j++) {
            if (!DT_BitmapIsSetUnchecked(query->inc, inc_raw[j])) {
                is_registered = DT_false;
                break;
            }
        }
        for (DT_size j = 0; is_registered && j < exc_comps_len; j++) {
            if (!DT_BitmapIsSetUnchecked(query->exc, exc_raw[j])) {
                is_registered = DT_false;
                break;
            }
        }
        if (is_registered) {
            return i;   
        }
    }

    return PRP_INVALID_INDEX;
}

PRP_Result QueryDelete(DT_void *query, DT_void *_) {
    (DT_void) _;
    Query *q = query;

    DT_BitmapDeleteUnchecked(&q->inc);
    if (q->exc) {
        DT_BitmapDeleteUnchecked(&q->exc);
    }
    if (q->behavior_matches) {
        DT_ArrDeleteUnchecked(&q->behavior_matches);
    }

    return PRP_OK;
}
