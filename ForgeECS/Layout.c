#include "Shared-Internals.h"

CORE_Id LayoutCreate(CORE_Id b_set_id);

PRP_FnCode LayoutDelete(CORE_Id *pLayout_id) {
    PRP_NULL_ARG_CHECK(pLayout_id, PRP_FN_INV_ARG_ERROR);

    // We don't do id validation since the below function will do it anyways.
    return CORE_IdMgrDeleteData(g_state->layout_id_mgr, pLayout_id);
}

PRP_FnCode LayoutDelCb(DT_void *layout) {
    PRP_NULL_ARG_CHECK(layout, PRP_FN_INV_ARG_ERROR);

    Layout *l = layout;
    if (l->comp_arr_strides) {
        free(l->comp_arr_strides);
        l->comp_arr_strides = DT_null;
    }
    if (l->behavior_set) {
        DT_BitmapDelete(&l->behavior_set);
    }
    if (l->free_chunks) {
        DT_BitmapDelete(&l->free_chunks);
    }
    if (l->chunk_ptrs) {
        DT_ArrDelete(&l->chunk_ptrs);
    }
    l->chunk_size = 0;

    return PRP_FN_SUCCESS;
}

DT_u64 LayoutGetSlot(CORE_Id layout_id);
PRP_FnCode LayoutFreeSlot(CORE_Id layout_id, DT_u64 entity_id);
PRP_FnCode LayoutIsEntityIdValid(CORE_Id layout_id, DT_u64 entity_id);
