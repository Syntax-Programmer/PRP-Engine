#include "Shared-Internals.h"

CORE_Id BehaviorSetCreate(DT_void) {
    /*
     * This creates the behavior set that can supports all the currently defined
     * comp_ids. We can do that kind of allocation since the bitmaps are very
     * very lean, for eg,. 1 mil comp ids will only consume 122 kb of data.
     */
    DT_Bitmap *b_set =
        DT_BitmapCreate(DT_ArrLen(g_state->comp_registry.comp_sizes));
    if (!b_set) {
        return CORE_INVALID_ID;
    }

    return CORE_IdMgrAddData(g_state->b_set_id_mgr, &b_set);
}

PRP_Result BehaviorSetDelete(CORE_Id *pB_set_id) {
    DIAG_GUARD(pB_set_id, PRP_ERR_INV_ARG);

    DT_Bitmap *temp = DT_null;

    return CORE_IdMgrDeleteData(g_state->b_set_id_mgr, pB_set_id, &temp);
}

PRP_Result BehaviorSetClear(CORE_Id b_set_id) {
    /*
     * Since id_mgr is always right if this function is executed, and the below
     * function checks for id validity we don't do redundant checks for id
     * validity.
     */
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        return PRP_ERR_INV_ARG;
    }
    // This is just a step to save memory. If it fails, we don't care.
    DT_BitmapShrinkFit(*pB_set);

    return DT_BitmapReset(*pB_set);
}

PRP_Result BehaviorSetAttachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        return PRP_ERR_INV_STATE;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_ERR_INV_ARG);
    if (comp_id >= DT_BitmapBitCap(*pB_set)) {
        PRP_Result code = DT_BitmapChangeSize(
            *pB_set, DT_ArrLen(g_state->comp_registry.comp_sizes));
        if (code != PRP_OK) {
            return code;
        }
    }

    return DT_BitmapSet(*pB_set, comp_id);
}

PRP_Result BehaviorSetDetachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        return PRP_ERR_INV_ARG;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_ERR_INV_ARG);

    return DT_BitmapClr(*pB_set, comp_id);
}

PRP_Result BehaviorSetHasComp(CORE_Id b_set_id, FECS_CompId comp_id,
                              DT_bool *pRslt) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        return PRP_ERR_INV_ARG;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_ERR_INV_ARG);
    DIAG_GUARD(pRslt != DT_null, PRP_ERR_INV_ARG);

    return DT_BitmapIsSet(*pB_set, comp_id, pRslt);
}

PRP_Result BehaviorSetDelCb(DT_void *pB_set) {
    /*
     * Since we already store DT_Bitmap *in the array, the data we will get will
     * be DT_Bitmap **.
     */

    return DT_BitmapDelete(pB_set);
}
