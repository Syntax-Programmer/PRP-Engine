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
static PRP_Result QueryRegisterInternal(DT_size *inc_comps, DT_size inc_len,
                                        DT_size *exc_comps, DT_size exc_len,
                                        DT_size *pIdx);

static PRP_Result QueryFindMatches(Query *query) {
    PRP_Result code;
    const DT_size QUERY_INIT_SIZE = 4;
    code = DT_ArrCreateUnchecked(sizeof(DT_size), QUERY_INIT_SIZE,
                                 &query->behavior_matches);
    if (code != PRP_OK) {
        return code;
    }

    DT_size len;
    const Behavior *behaviors = DT_ArrRawUnchecked(g_ctx->behaviors, &len);
    for (DT_size i = 0; i < len; i++) {
        const Behavior *behavior = &behaviors[i];
        DT_bool is_match = (!query->exc || !DT_BitmapHasAnyUnchecked(
                                               behavior->set, query->exc)) &&
                           DT_BitmapHasAllUnchecked(behavior->set, query->inc);
        if (is_match) {
            PRP_Result code = DT_ArrPushUnchecked(query->behavior_matches, &i);
            if (code != PRP_OK) {
                DT_ArrDeleteUnchecked(&query->behavior_matches);
                return code;
            }
        }
    }

    return PRP_OK;
}

static PRP_Result QueryRegisterInternal(DT_size *inc_comps, DT_size inc_len,
                                        DT_size *exc_comps, DT_size exc_len,
                                        DT_size *pIdx) {
    Query data = {0};
    DT_size total_comps = DT_ArrLen(g_ctx->comps);
    PRP_Result code;

    code = DT_BitmapCreateUnchecked(total_comps, &data.inc);
    if (code != PRP_OK) {
        goto err_path;
    }
    if (exc_comps && exc_len > 0) {
        code = DT_BitmapCreateUnchecked(total_comps, &data.exc);
        if (code != PRP_OK) {
            goto err_path;
        }
    }
    code = QueryFindMatches(&data);
    if (code != PRP_OK) {
        goto err_path;
    }

    for (DT_size i = 0; i < inc_len; i++) {
        if (inc_comps[i] >= total_comps) {
            code = PRP_ERR_INV_ARG;
            goto err_path;
        }
        DT_BitmapSetUnchecked(data.inc, inc_comps[i]);
    }
    if (data.exc) {
        for (DT_size i = 0; i < exc_len; i++) {
            if (exc_comps[i] >= total_comps) {
                code = PRP_ERR_INV_ARG;
                goto err_path;
            }
            if (DT_BitmapIsSetUnchecked(data.inc, exc_comps[i])) {
                code = PRP_ERR_INV_ARG;
                DIAG_LOG_ERROR(
                    DIAG_LOG_CODE_INVALID_ARG,
                    "The given inc and exc comps have overlapping comps");
                goto err_path;
            }
            DT_BitmapSetUnchecked(data.exc, exc_comps[i]);
        }
    }

    code = DT_ArrPushUnchecked(g_ctx->queries, &data);
    if (code != PRP_OK) {
        goto err_path;
    }

    *pIdx = DT_ArrLen(g_ctx->queries) - 1;

    return PRP_OK;

err_path:
    if (data.inc) {
        DT_BitmapDeleteUnchecked(&data.inc);
    }
    if (data.exc) {
        DT_BitmapDeleteUnchecked(&data.exc);
    }
    if (data.behavior_matches) {
        DT_ArrDeleteUnchecked(&data.behavior_matches);
    }

    return code;
}

PRP_Result QueryRegister(const DT_Arr *inc_comps, const DT_Arr *exc_comps,
                         DT_size *pIdx) {
    DT_size inc_len;
    DT_size *arr1 = (DT_size *)(DT_ArrRawUnchecked(inc_comps, &inc_len));
    DT_size exc_len = 0;
    DT_size *arr2 = DT_null;
    if (exc_comps) {
        arr2 = (DT_size *)(DT_ArrRawUnchecked(exc_comps, &exc_len));
    }

    return QueryRegisterInternal(arr1, inc_len, arr2, exc_len, pIdx);
}

DT_bool QueryIsRegistered(const DT_Arr *inc_comps, const DT_Arr *exc_comps,
                          DT_size *pOut) {
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
        if (DT_BitmapSetCount(query->inc) != inc_comps_len ||
            (query->exc && DT_BitmapSetCount(query->exc) != exc_comps_len)) {
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
            *pOut = i;
            return DT_true;
        }
    }

    return DT_false;
}

PRP_Result QueryCascadeUpdateBehavior(DT_void *q, DT_void *b) {
    Query *query = q;
    DT_size behavior_idx = *(DT_size *)b;
    Behavior *behavior = DT_ArrGetUnchecked(g_ctx->behaviors, behavior_idx);

    DT_bool is_match =
        (!query->exc || !DT_BitmapHasAnyUnchecked(behavior->set, query->exc)) &&
        DT_BitmapHasAllUnchecked(behavior->set, query->inc);

    if (!is_match) {
        return PRP_OK;
    }

    return DT_ArrPushUnchecked(query->behavior_matches, &behavior_idx);
}

PRP_Result QueryCascadingErrorCleanup(DT_void *q, DT_void *b) {
    Query *query = q;
    DT_size behavior_idx = *(DT_size *)b;
    DT_size matches_len = DT_ArrLen(query->behavior_matches);
    if (!matches_len) {
        return PRP_OK;
    }

    DT_size last_idx = *(DT_size *)DT_ArrGetUnchecked(query->behavior_matches,
                                                      matches_len - 1);
    if (last_idx == behavior_idx) {
        DT_ArrPopUnchecked(query->behavior_matches, DT_null);
    }

    return PRP_OK;
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
