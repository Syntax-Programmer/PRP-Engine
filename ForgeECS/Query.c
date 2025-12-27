#include "Shared-Internals.h"

CORE_Id QueryCreate(CORE_Id exclude_b_set_id, CORE_Id include_b_set_id);
PRP_FnCode QueryDelete(CORE_Id *pQuery_id);

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
