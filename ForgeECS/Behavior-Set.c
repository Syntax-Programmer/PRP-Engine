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
        PRP_LOG_FN_MALLOC_ERROR(b_set);
        return CORE_INVALID_ID;
    }

    CORE_Id b_set_id = CORE_IdMgrAddData(g_state->b_set_id_mgr, &b_set);
    if (b_set_id == CORE_INVALID_ID) {
        PRP_LOG_FN_CODE(PRP_FN_FAILURE,
                        "Cannot create id for the behavior set.");
    }

    return b_set_id;
}

PRP_FnCode BehaviorSetDelete(CORE_Id *pB_set_id) {
    // We don't do id validation since the below function will do it anyways.
    PRP_FnCode code = CORE_IdMgrDeleteData(g_state->b_set_id_mgr, pB_set_id);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Cannot delete the given behavior set id.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode BehaviorSetClear(CORE_Id b_set_id) {
    /*
     * Since id_mgr is always right if this function is executed, and the below
     * function checks for id validity we don't do redundant checks for id
     * validity.
     */
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    // This is just a step to save memory. If it fails, we don't care.
    DT_BitmapShrinkFit(*pB_set);

    PRP_FnCode code = DT_BitmapReset(*pB_set);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Could not clear the given behavior set.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode BehaviorSetAttachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);

    PRP_FnCode code = DT_BitmapSet(*pB_set, comp_id);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(
            code,
            "Could not attach the given component to the given behavior set.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode BehaviorSetDetachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);

    PRP_FnCode code = DT_BitmapClr(*pB_set, comp_id);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Could not detach the given component from the "
                              "given behavior set.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode BehaviorSetHasComp(CORE_Id b_set_id, FECS_CompId comp_id,
                              DT_bool *pRslt) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    PRP_FnCode code = DT_BitmapIsSet(*pB_set, comp_id, pRslt);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Could not find if the given comp is present or "
                              "not in the given behavior set.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode BehaviorSetDelCb(DT_void *pB_set) {
    /*
     * Since we already store DT_Bitmap *in the array, the data we will get will
     * be DT_Bitmap **.
     */
    PRP_FnCode code = DT_BitmapDelete(pB_set);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Invalid behavior set stored in the id manager. "
                              "This condition should not be possible.");
        return code;
    }

    return PRP_FN_SUCCESS;
}
