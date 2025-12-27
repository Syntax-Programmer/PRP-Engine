#include "Shared-Internals.h"

CORE_Id SystemCreate(CORE_Id query_id, FECS_SysFn func, DT_void *user_data) {
    PRP_NULL_ARG_CHECK(func, CORE_INVALID_ID);
    DT_bool rslt;
    if (CORE_IdIsValid(g_state->query_id_mgr, query_id, &rslt) !=
            PRP_FN_SUCCESS ||
        !rslt) {
        PRP_LOG_FN_INV_ARG_ERROR(query_id);
        return CORE_INVALID_ID;
    }

    System system = {.query_id = query_id, .fn = func, .user_data = user_data};

    return CORE_IdMgrAddData(g_state->system_id_mgr, &system);
}

PRP_FnCode SystemDelete(CORE_Id *pSystem_id) {
    return CORE_IdMgrDeleteData(g_state->system_id_mgr, pSystem_id);
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
    CORE_Id *layout_ids = DT_ArrRaw(query->layout_matches, &layout_ids_len);

    for (DT_size i = 0; i < layout_ids_len; i++) {
        Layout *layout = CORE_IdToData(g_state->layout_id_mgr, layout_ids[i]);

        DT_size comp_arr_c = DT_BitmapSetCount(layout->b_set);
        /*Can cause stack overflow please fix.*/
        DT_void *fn_arr[comp_arr_c];

        DT_size chunks_len;
        Chunk **chunks = DT_ArrRaw(layout->chunk_ptrs, &chunks_len);

        for (DT_size j = 0; j < chunks_len; j++) {
            DT_bool rslt;
            // Can't really fail.
            DT_BitmapIsSet(layout->free_chunks, j, &rslt);
            if (!rslt) {
                continue;
            }
            Chunk *chunk = chunks[j];
            for (DT_size k = 0; k < comp_arr_c; k++) {
                fn_arr[k] = chunk->data + layout->comp_arr_strides[k];
            }
            system->fn(fn_arr, comp_arr_c, system->user_data,
                       ~chunk->free_slot);
        }
    }

    return PRP_FN_SUCCESS;
}
