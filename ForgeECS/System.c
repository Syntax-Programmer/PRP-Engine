#include "Shared-Internals.h"

CORE_Id SystemCreate(CORE_Id query_id, FECS_SysFn fn, DT_void *user_data) {
    PRP_NULL_ARG_CHECK(fn, CORE_INVALID_ID);
    DT_bool rslt;

    if (CORE_IdIsValid(g_state->query_id_mgr, query_id, &rslt) !=
            PRP_FN_SUCCESS ||
        !rslt) {
        PRP_LOG_FN_INV_ARG_ERROR(query_id);
        return CORE_INVALID_ID;
    }

    System system = {.query_id = query_id, .fn = fn, .user_data = user_data};

    CORE_Id system_id = CORE_IdMgrAddData(g_state->system_id_mgr, &system);
    if (system_id == CORE_INVALID_ID) {
        PRP_LOG_FN_CODE(PRP_FN_FAILURE, "Cannot create id for the system.");
    }

    return system_id;
}

PRP_FnCode SystemDelete(CORE_Id *pSystem_id) {
    PRP_FnCode code = CORE_IdMgrDeleteData(g_state->system_id_mgr, pSystem_id);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Cannot delete the given system id.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FnCode SystemExec(CORE_Id system_id) {
    System *system = CORE_IdToData(g_state->system_id_mgr, system_id);
    if (!system) {
        PRP_LOG_FN_INV_ARG_ERROR(system_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    /*
     * This MUST be valid otherwise we can assume that someone maliciously
     * manipulated FECS internal data. If that is the case we can just give up.
     */
    Query *query = CORE_IdToData(g_state->query_id_mgr, system->query_id);
    DT_size layout_ids_len;
    const CORE_Id *layout_ids =
        DT_ArrRaw(query->layout_matches, &layout_ids_len);

    for (DT_size i = 0; i < layout_ids_len; i++) {
        Layout *layout = CORE_IdToData(g_state->layout_id_mgr, layout_ids[i]);
        DT_size comp_arr_c = DT_BitmapSetCount(layout->b_set);
        /*********TODO: The below can cause stack overflow please fix.
         * :TODO***********/
        DT_void *fn_arr[comp_arr_c];
        DT_size chunks_len;
        /*
         * Purposefully discarding const qualifier since it was worth the
         * function call overhead will affect perf. Since we are
         * still treating it as const, just that the compiler will not scream at
         * us.
         */
        Chunk **chunks = (Chunk **)DT_ArrRaw(layout->chunk_ptrs, &chunks_len);

        for (DT_size j = 0; j < chunks_len; j++) {
            Chunk *chunk = chunks[j];
            if (!(~chunk->free_slot)) {
                continue;
            }
            for (DT_size k = 0; k < comp_arr_c; k++) {
                fn_arr[k] = chunk->data + layout->comp_arr_strides[k];
            }
            system->fn(fn_arr, comp_arr_c, system->user_data,
                       ~chunk->free_slot);
        }
    }

    return PRP_FN_SUCCESS;
}
