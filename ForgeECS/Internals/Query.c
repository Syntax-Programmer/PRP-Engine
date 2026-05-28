#include "FECS-Internals.h"

/**
 * Finds behavior matches for the query.
 *
 * @param pQuery Query instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result QueryFindMatches(Query *pQuery);

static PRP_Result QueryFindMatches(Query *pQuery) {
    PRP_Result code;
    const DT_size QUERY_INIT_SIZE = 4;
    code = DT_ArrCreateUnchecked(sizeof(FECS_BehaviorId), QUERY_INIT_SIZE,
                                 &pQuery->behavior_id_matches);
    if (code != PRP_OK) {
        return code;
    }

    DT_size len;
    const Behavior *behaviors = DT_ArrRawUnchecked(g_ctx->behaviors, &len);
    for (DT_size i = 0; i < len; i++) {
        const Behavior *behavior = &behaviors[i];
        DT_bool is_match = (!pQuery->exc || !DT_BitmapHasAnyUnchecked(
                                                behavior->set, pQuery->exc)) &&
                           DT_BitmapHasAllUnchecked(behavior->set, pQuery->inc);
        if (is_match) {
            code = DT_ArrPushUnchecked(pQuery->behavior_id_matches, &i);
            if (code != PRP_OK) {
                DT_ArrDeleteUnchecked(&pQuery->behavior_id_matches);
                return code;
            }
        }
    }

    return PRP_OK;
}

PRP_Result QueryRegister(const FECS_CompId *pInc_comp_ids,
                         DT_size inc_comps_count,
                         const FECS_CompId *pExc_comp_ids,
                         DT_size exc_comps_count, FECS_QueryId *pQuery_id) {
    Query data = {0};
    DT_size total_comps = DT_ArrLen(g_ctx->comps);
    PRP_Result code;

    code = DT_BitmapCreateUnchecked(total_comps, &data.inc);
    if (code != PRP_OK) {
        goto err_path;
    }
    if (pExc_comp_ids && exc_comps_count > 0) {
        code = DT_BitmapCreateUnchecked(total_comps, &data.exc);
        if (code != PRP_OK) {
            goto err_path;
        }
    }
    code = QueryFindMatches(&data);
    if (code != PRP_OK) {
        goto err_path;
    }

    for (DT_size i = 0; i < inc_comps_count; i++) {
        if (pInc_comp_ids[i] >= total_comps) {
            code = PRP_ERR_INV_ARG;
            goto err_path;
        }
        DT_BitmapSetUnchecked(data.inc, pInc_comp_ids[i]);
    }
    if (data.exc) {
        for (DT_size i = 0; i < exc_comps_count; i++) {
            if (pExc_comp_ids[i] >= total_comps) {
                code = PRP_ERR_INV_ARG;
                goto err_path;
            }
            if (DT_BitmapIsSetUnchecked(data.inc, pExc_comp_ids[i])) {
                code = PRP_ERR_INV_ARG;
                DIAG_LOG_ERROR(
                    DIAG_LOG_CODE_INVALID_ARG,
                    "The given inc and exc comps have overlapping comps");
                goto err_path;
            }
            DT_BitmapSetUnchecked(data.exc, pExc_comp_ids[i]);
        }
    }

    code = DT_ArrPushUnchecked(g_ctx->queries, &data);
    if (code != PRP_OK) {
        goto err_path;
    }

    *pQuery_id = DT_ArrLen(g_ctx->queries) - 1;

    return PRP_OK;

err_path:
    if (data.inc) {
        DT_BitmapDeleteUnchecked(&data.inc);
    }
    if (data.exc) {
        DT_BitmapDeleteUnchecked(&data.exc);
    }
    if (data.behavior_id_matches) {
        DT_ArrDeleteUnchecked(&data.behavior_id_matches);
    }

    return code;
}

DT_bool QueryIsRegistered(const FECS_CompId *pInc_comp_ids,
                          DT_size inc_comps_count,
                          const FECS_CompId *pExc_comp_ids,
                          DT_size exc_comps_count, FECS_QueryId *pFound_id) {
    DT_size queries_len;
    const Query *pQueries = DT_ArrRawUnchecked(g_ctx->queries, &queries_len);

    for (DT_size i = 0; i < queries_len; i++) {
        const Query *pQuery = &pQueries[i];
        if ((exc_comps_count && !pQuery->exc) ||
            (!exc_comps_count && pQuery->exc)) {
            continue;
        }
        if (DT_BitmapSetCount(pQuery->inc) != inc_comps_count ||
            (pQuery->exc &&
             DT_BitmapSetCount(pQuery->exc) != exc_comps_count)) {
            continue;
        }

        DT_bool is_registered = DT_true;
        for (DT_size j = 0; j < inc_comps_count; j++) {
            if (!DT_BitmapIsSetUnchecked(pQuery->inc, pInc_comp_ids[j])) {
                is_registered = DT_false;
                break;
            }
        }
        for (DT_size j = 0; is_registered && j < exc_comps_count; j++) {
            if (!DT_BitmapIsSetUnchecked(pQuery->exc, pExc_comp_ids[j])) {
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

PRP_Result QueryCascadeUpdateBehavior(DT_void *pQuery, DT_void *pBehavior) {
    Query *pQuery_instance = pQuery;
    FECS_BehaviorId behavior_id = *(FECS_BehaviorId *)pBehavior;
    Behavior *behavior = DT_ArrGetUnchecked(g_ctx->behaviors, behavior_id);

    DT_bool is_match =
        (!pQuery_instance->exc ||
         !DT_BitmapHasAnyUnchecked(behavior->set, pQuery_instance->exc)) &&
        DT_BitmapHasAllUnchecked(behavior->set, pQuery_instance->inc);

    if (!is_match) {
        return PRP_OK;
    }

    return DT_ArrPushUnchecked(pQuery_instance->behavior_id_matches,
                               &behavior_id);
}

PRP_Result QueryCascadingErrorCleanup(DT_void *pQuery, DT_void *pBehavior) {
    Query *pQuery_instance = pQuery;
    FECS_BehaviorId behavior_id = *(FECS_BehaviorId *)pBehavior;
    DT_size matches_len = DT_ArrLen(pQuery_instance->behavior_id_matches);
    if (!matches_len) {
        return PRP_OK;
    }

    FECS_BehaviorId last_id = *(DT_size *)DT_ArrGetUnchecked(
        pQuery_instance->behavior_id_matches, matches_len - 1);
    if (last_id == behavior_id) {
        DT_ArrPopUnchecked(pQuery_instance->behavior_id_matches, DT_null);
    }

    return PRP_OK;
}

PRP_Result QueryDelete(DT_void *pQuery, DT_void *_) {
    (DT_void) _;
    Query *pQuery_instance = pQuery;

    DT_BitmapDeleteUnchecked(&pQuery_instance->inc);
    if (pQuery_instance->exc) {
        DT_BitmapDeleteUnchecked(&pQuery_instance->exc);
    }
    if (pQuery_instance->behavior_id_matches) {
        DT_ArrDeleteUnchecked(&pQuery_instance->behavior_id_matches);
    }

    return PRP_OK;
}
