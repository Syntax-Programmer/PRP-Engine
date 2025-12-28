#include "Shared-Internals.h"

#define QUERY_INIT_ERROR_CHECK(x)                                              \
    do {                                                                       \
        if (!x) {                                                              \
            QueryDelCb(&query);                                                \
            PRP_LOG_FN_MALLOC_ERROR(x);                                        \
            return CORE_INVALID_ID;                                            \
        }                                                                      \
    } while (0);

CORE_Id QueryCreate(CORE_Id exclude_b_set_id, CORE_Id include_b_set_id) {
    DT_Bitmap **pExc_b_set =
        CORE_IdToData(g_state->b_set_id_mgr, exclude_b_set_id);
    if (!pExc_b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(pExc_b_set);
        return CORE_INVALID_ID;
    }
    DT_Bitmap **pInc_b_set =
        CORE_IdToData(g_state->b_set_id_mgr, include_b_set_id);
    if (!pInc_b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(pInc_b_set);
        return CORE_INVALID_ID;
    }

    Query query = {0};

    query.exclude_comps = DT_BitmapClone(*pExc_b_set);
    QUERY_INIT_ERROR_CHECK(query.exclude_comps);
    query.include_comps = DT_BitmapClone(*pInc_b_set);
    QUERY_INIT_ERROR_CHECK(query.include_comps);
    query.layout_matches = DT_ArrCreateDefault(sizeof(CORE_Id));
    QUERY_INIT_ERROR_CHECK(query.layout_matches);

    DT_u32 len;
    Layout *layouts = CORE_IdMgrRaw(g_state->layout_id_mgr, &len);
    for (DT_u32 i = 0; i < len; i++) {
        DT_bool rslt;
        // if () {
        //     DT_ArrPush(query.layout_matches, &layouts[i].id);
        // }
    }

    return CORE_IdMgrAddData(g_state->query_id_mgr, &query);
}

PRP_FnCode QueryDelete(CORE_Id *pQuery_id) {
    return CORE_IdMgrDeleteData(g_state->query_id_mgr, pQuery_id);
}

PRP_FnCode QueryDelCb(DT_void *query) {
    PRP_NULL_ARG_CHECK(query, PRP_FN_INV_ARG_ERROR);

    Query *q = query;
    if (q->include_comps) {
        DT_BitmapDelete(&q->include_comps);
    }
    if (q->exclude_comps) {
        DT_BitmapDelete(&q->exclude_comps);
    }
    if (q->layout_matches) {
        DT_ArrDelete(&q->layout_matches);
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode QueryCascadeLayoutCreate(CORE_Id layout_id);
PRP_FnCode QueryCascadeLayoutDelete(CORE_Id layout_id);
