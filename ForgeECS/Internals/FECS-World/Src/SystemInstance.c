#include "ForgeECS/Internals/FECS-World/World-Internals.h"
#include "ForgeECS/Internals/FECS/FECS-Internals.h"

struct SystemData {
    FECS_SystemFunc func;

    const DT_Bitword *pComp_set_bitwords;
    DT_size *pComp_arr_strides;
    DT_u16 *pWord_prefix_popcnts;

    DT_void *pUser_data;

    DT_u8 *pChunk_mem;
};

/**
 * Acts as an intermediate chunk level dispatcher for the system function.
 * Called via DT_ArrForEach_...
 *
 * @param pVal The FECS_Chunk ** we will revieve from the arrforeach function.
 * @param pUser_data The system data needed for execution of the system func.
 *
 * @return PRP_OK on success or on an empty chunk.
 */
static PRP_Result ExecCb(DT_void *pVal, DT_void *pUser_data);

DT_void SystemInstanceCreate(FECS_SystemInstanceCreateInfo *pCreate_info,
                             FECS_SystemInstance *pSystem_instance) {
    *pSystem_instance = (FECS_SystemInstance){0};
    pSystem_instance->system_id = pCreate_info->system_id;
    pSystem_instance->layout_id_match_count =
        pCreate_info->layout_id_match_count;
    pSystem_instance->pLayout_id_matches = pCreate_info->pLayout_id_matches;

    // Invalidating to prevent access via caller again.
    pCreate_info->pLayout_id_matches = DT_null;
}

DT_void SystemInstanceDelete(FECS_SystemInstance *pSystem_instance) {
    DIAG_ASSERT(pSystem_instance != DT_null);

    free(pSystem_instance->pLayout_id_matches);

#if !defined(PRP_NDEBUG)
    pSystem_instance->system_id = PRP_INVALID_INDEX;
    pSystem_instance->layout_id_match_count = 0;
    pSystem_instance->pLayout_id_matches = DT_null;
#endif
}

static PRP_Result ExecCb(DT_void *pVal, DT_void *pUser_data) {
    FECS_SystemExecInternalData *pExec_internals = pUser_data;
    pExec_internals->pChunk_mem = (*(FECS_Chunk **)pVal)->pChunk_mem;
    FECS_SystemExecOccupancyMask occupancy_mask =
        ~(*(FECS_Chunk **)pVal)->free_slot_bitset;
    if (occupancy_mask == 0) {
        return PRP_OK;
    }

    pExec_internals->func(pExec_internals, occupancy_mask,
                          pExec_internals->pUser_data);

    return PRP_OK;
}

DT_void SystemInstanceExec(FECS_World *pWorld,
                           FECS_SystemInstanceId system_instance_id,
                           DT_void *pUser_data) {
    FECS_SystemInstance *pSystem_instance =
        &pWorld->pSystem_instances[system_instance_id];
    FECS_SystemFunc *pFunc =
        DT_ArrGetUnchecked(g_ctx->pSystem_funcs, pSystem_instance->system_id);

    FECS_LayoutId *pLayout_ids = pSystem_instance->pLayout_id_matches;
    FECS_SystemExecInternalData exec_internals = {.func = *pFunc,
                                                  .pUser_data = pUser_data};
    for (DT_size i = 0; i < pSystem_instance->layout_id_match_count; i++) {
        FECS_Layout *pLayout = &pWorld->pLayouts[pLayout_ids[i]];
        DT_size _;

        exec_internals.pComp_set_bitwords =
            DT_BitmapRawUnchecked(pLayout->pComp_set, &_, &_);
        exec_internals.pComp_arr_strides = pLayout->pComp_arr_strides;
        exec_internals.pWord_prefix_popcnts = pLayout->pWord_prefix_popcnts;

        DT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ExecCb, &exec_internals);
    }
}

DT_void *
SystemInstanceFetchComp(const FECS_SystemExecInternalData *pExec_internals,
                        FECS_CompId comp_id) {
    DT_size word_i = WORD_I(comp_id);
    DT_Bitword comp_id_word = pExec_internals->pComp_set_bitwords[word_i];

    if (!PRP_BIT_IS_SET(comp_id_word, BIT_I(comp_id))) {
        return DT_null;
    }

    // number of bits set in the word before the word the comp id lies in.
    DT_u16 prefix_popcnt = pExec_internals->pWord_prefix_popcnts[word_i];
    DT_u16 rank_in_word =
        (DT_u16)DT_BitwordPopCnt(comp_id_word & (BIT_MASK(comp_id) - 1));

    DT_size stride =
        pExec_internals->pComp_arr_strides[prefix_popcnt + rank_in_word];

    return pExec_internals->pChunk_mem + stride;
}
