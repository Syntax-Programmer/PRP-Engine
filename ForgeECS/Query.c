#include "Shared-Internals.h"

#define QUERY_INIT_ERROR_CHECK(x)                                              \
    do {                                                                       \
        if (!x) {                                                              \
            QueryDelCb(&query);                                                \
            return CORE_INVALID_ID;                                            \
        }                                                                      \
    } while (0);

CORE_Id QueryCreate(CORE_Id exclude_b_set_id, CORE_Id include_b_set_id) {
    Query query = {0};

    if (exclude_b_set_id != CORE_INVALID_ID) {
        DT_Bitmap **pExc_b_set =
            CORE_IdToData(g_state->b_set_id_mgr, exclude_b_set_id);
        if (!pExc_b_set) {
            return CORE_INVALID_ID;
        }
        query.exclude_comps = DT_BitmapClone(*pExc_b_set);
        QUERY_INIT_ERROR_CHECK(query.exclude_comps);
    }

    DT_Bitmap **pInc_b_set =
        CORE_IdToData(g_state->b_set_id_mgr, include_b_set_id);
    if (!pInc_b_set) {
        return CORE_INVALID_ID;
    }
    query.include_comps = DT_BitmapClone(*pInc_b_set);
    QUERY_INIT_ERROR_CHECK(query.include_comps);

    query.layout_matches = DT_ArrCreateDefault(sizeof(CORE_Id));
    QUERY_INIT_ERROR_CHECK(query.layout_matches);

    DT_u32 len;
    const Layout *layouts = CORE_IdMgrRaw(g_state->layout_id_mgr, &len);
    for (DT_u32 i = 0; i < len; i++) {
        DT_bool rslt1;
        DT_BitmapIsSubset(layouts[i].b_set, query.include_comps, &rslt1);
        if (!rslt1) {
            continue;
        }
        DT_bool rslt2 = (query.exclude_comps) ? DT_true : DT_false;
        if (rslt2) {
            DT_BitmapHasAny(layouts[i].b_set, query.exclude_comps, &rslt2);
        }
        if (rslt1 && !rslt2) {
            // This is also guaranteed.
            CORE_Id layout_id = CORE_DataIToId(g_state->layout_id_mgr, i);
            DT_ArrPush(query.layout_matches, &layout_id);
        }
    }

    return CORE_IdMgrAddData(g_state->query_id_mgr, &query);
}

PRP_Result QueryDelete(CORE_Id *pQuery_id) {
    DIAG_GUARD(pQuery_id != DT_null, PRP_ERR_INV_ARG);

    Query temp = {0};

    return CORE_IdMgrDeleteData(g_state->query_id_mgr, pQuery_id, &temp);
}

PRP_Result QueryDelCb(DT_void *query) {
    DIAG_GUARD(query != DT_null, PRP_ERR_INV_ARG);

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

    return PRP_OK;
}

PRP_Result QueryCascadeLayoutCreate(CORE_Id layout_id) {
    /*
     * This is an internal function managed by FECS so we are guaranteed to have
     * a never before seen layout here.
     */
    Layout *layout = CORE_IdToData(g_state->layout_id_mgr, layout_id);
    if (!layout) {
        return PRP_ERR_INV_ARG;
    }

    DT_u32 len;
    const Query *queries = CORE_IdMgrRaw(g_state->query_id_mgr, &len);
    for (DT_u32 i = 0; i < len; i++) {
        DT_bool rslt1;
        DT_BitmapIsSubset(layout->b_set, queries[i].include_comps, &rslt1);
        if (!rslt1) {
            continue;
        }
        DT_bool rslt2 = (queries[i].exclude_comps) ? DT_true : DT_false;
        if (rslt2) {
            DT_BitmapHasAny(layout->b_set, queries[i].exclude_comps, &rslt2);
        }
        if (rslt1 && !rslt2) {
            DT_ArrPush(queries[i].layout_matches, &layout_id);
        }
    }

    return PRP_OK;
}

PRP_Result QueryCascadeLayoutDelete(CORE_Id layout_id) {
    /*
     * This is an internal function managed by FECS so we are guaranteed to have
     * a never before seen layout here.
     */
    DT_bool rslt;
    CORE_IdIsValid(g_state->layout_id_mgr, layout_id, &rslt);
    if (!rslt) {
        return PRP_ERR_INV_ARG;
    }

    DT_u32 len;
    const Query *queries = CORE_IdMgrRaw(g_state->query_id_mgr, &len);
    for (DT_u32 i = 0; i < len; i++) {
        DT_size ids_len;
        const CORE_Id *layout_ids =
            DT_ArrRaw(queries[i].layout_matches, &ids_len);
        for (DT_size j = 0; j < ids_len; j++) {
            if (layout_ids[j] == layout_id) {
                // Since the layout matches are unordered we can do this.
                CORE_Id temp;
                DT_ArrSwap(queries[i].layout_matches, j, ids_len - 1, &temp);
                DT_ArrPop(queries[i].layout_matches, DT_null);
                break;
            }
        }
    }

    return PRP_OK;
}
