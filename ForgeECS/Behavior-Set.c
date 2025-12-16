#include "Shared-Internals.h"

CORE_Id BehaviorSetCreate(DT_void) {
    /*
     * This creates the behavior set that can supports all the currently defined
     * comp_ids. We can do that kind of allocation since the bitmaps are very
     * very lean, for eg,. 1 mil comp ids will only consume 122 kb of data.
     */
    DT_Bitmap *b_set =
        DT_BitmapCreate(DT_ArrLen(g_state->comp_registry.comp_sizes));

    CORE_Id id = CORE_IdMgrAddData(g_state->behavior_set_id_mgr, &b_set);
    if (id == CORE_INVALID_ID) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot create anymore behavior sets.");
    }

    return id;
}

PRP_FnCode BehaviorSetDelete(CORE_Id *pB_set_id) {
    PRP_NULL_ARG_CHECK(pB_set_id, PRP_FN_INV_ARG_ERROR);

    // We don't do id validation since the below function will do it anyways.
    return CORE_IdMgrDeleteData(g_state->behavior_set_id_mgr, pB_set_id);
}

PRP_FnCode BehaviorSetClear(CORE_Id b_set_id) {
    /*
     * Since id_mgr is always right if this function is executed, and the below
     * function checks for id validity we don't do redundant checks for id
     * validity.
     */
    DT_Bitmap *b_set = CORE_IdToData(g_state->behavior_set_id_mgr, b_set_id);
    if (!b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    // This is just a step to save memory. If it fails, we don't care.
    DT_BitmapShrinkFit(b_set);

    return DT_BitmapReset(b_set);
}

PRP_FnCode BehaviorSetAttachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    DT_Bitmap *b_set = CORE_IdToData(g_state->behavior_set_id_mgr, b_set_id);
    if (!b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);

    return DT_BitmapSet(b_set, comp_id);
}

PRP_FnCode BehaviorSetDetachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    DT_Bitmap *b_set = CORE_IdToData(g_state->behavior_set_id_mgr, b_set_id);
    if (!b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);

    return DT_BitmapClr(b_set, comp_id);
}

PRP_FnCode BehaviorSetHasComp(CORE_Id b_set_id, FECS_CompId comp_id,
                              DT_bool *pRslt) {
    DT_Bitmap *b_set = CORE_IdToData(g_state->behavior_set_id_mgr, b_set_id);
    if (!b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    return DT_BitmapIsSet(b_set, comp_id, pRslt);
}

PRP_FnCode BehaviorSetDelCb(DT_void *pB_set) {
    /*
     * Since we already store DT_Bitmap *in the array, the data we will get will
     * be DT_Bitmap **.
     */
    return DT_BitmapDelete(pB_set);
}
