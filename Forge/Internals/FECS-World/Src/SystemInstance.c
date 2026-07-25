#include "Forge/Internals/FECS-World/World-Internals.h"
#include "Forge/Internals/FECS/FECS-Internals.h"

struct FECS_SystemExecInternalData {
    FECS_SystemFunc func;

    PRP_Size stides_len;
    PRP_Size *pComp_arr_strides;

    void *pUser_data;

    PRP_U8 *pChunk_mem;
};

/**
 * Acts as an intermediate chunk level dispatcher for the system function.
 * Called via CONT_ArrForEach_...
 *
 * @param pVal The FECS_Chunk ** we will revieve from the arrforeach function.
 * @param pUser_data The system data needed for execution of the system func.
 *
 * @return PRP_OK on success or on an empty chunk.
 */
static PRP_Result ExecCb(void *pVal, void *pUser_data);

PRP_Result SystemInstanceCreate(FECS_SystemInstanceCreateInfo *pCreate_info,
                                FECS_SystemInstance *pSystem_instance) {
    *pSystem_instance = (FECS_SystemInstance){0};

    pSystem_instance->pStride_dispatches =
        malloc(sizeof(PRP_Size) * pCreate_info->stride_dispatch_count);
    if (!pSystem_instance->pStride_dispatches) {
        // Cleans after itself. So the generic contract of WorldCreate is ok.
        pCreate_info->layout_id_match_count = 0;
        free(pCreate_info->pLayout_id_matches);
        return PRP_ERR_OOM;
    }
    pSystem_instance->system_id = pCreate_info->system_id;
    pSystem_instance->layout_id_match_count =
        pCreate_info->layout_id_match_count;
    pSystem_instance->pLayout_id_matches = pCreate_info->pLayout_id_matches;

    // Invalidating to prevent access via caller again.
    pCreate_info->pLayout_id_matches = NULL;

    return PRP_OK;
}

void SystemInstanceDelete(FECS_SystemInstance *pSystem_instance) {
    DIAG_ASSERT(pSystem_instance != NULL);

    free(pSystem_instance->pStride_dispatches);
    free(pSystem_instance->pLayout_id_matches);

#ifdef PRP_DEBUG_MODE
    pSystem_instance->system_id = PRP_INVALID_INDEX;
    pSystem_instance->layout_id_match_count = 0;
    pSystem_instance->pLayout_id_matches = NULL;
    pSystem_instance->pStride_dispatches = NULL;
#endif
}

static PRP_Result ExecCb(void *pVal, void *pUser_data) {
    FECS_SystemExecInternalData *pExec_internals = pUser_data;
    pExec_internals->pChunk_mem = (*(FECS_Chunk **)pVal)->pChunk_mem;
    FECS_SystemExecOccupancyMask occupancy_mask =
        (FECS_SystemExecOccupancyMask)(~(*(FECS_Chunk **)pVal)
                                            ->free_slot_bitset);
    if (occupancy_mask == 0) {
        return PRP_OK;
    }

    pExec_internals->func(pExec_internals, occupancy_mask,
                          pExec_internals->pUser_data);

    return PRP_OK;
}

void SystemInstanceExec(FECS_World *pWorld,
                        FECS_SystemInstanceId system_instance_id,
                        void *pUser_data) {
    FECS_SystemInstance *pSystem_instance =
        &pWorld->pSystem_instances[system_instance_id];
    FECS_SystemInfo *pSystem_info =
        CONT_ArrGetUnchecked(g_ctx->pSystem_infos, pSystem_instance->system_id);

    FECS_LayoutId *pLayout_ids = pSystem_instance->pLayout_id_matches;
    FECS_SystemExecInternalData exec_internals = {
        .func = pSystem_info->systmem_func,
        .pUser_data = pUser_data,
        .stides_len = pSystem_info->comp_ids_needed_count,
        .pComp_arr_strides = pSystem_instance->pStride_dispatches};

    for (PRP_Size i = 0; i < pSystem_instance->layout_id_match_count; i++) {
        FECS_Layout *pLayout = &pWorld->pLayouts[pLayout_ids[i]];
        PRP_Size _;
        const CONT_Bitword *pBitwords =
            CONT_BitmapRawUnchecked(pLayout->pComp_set, &_, &_);

        // Precomputing strides for the component that the system needs.
        for (PRP_Size j = 0; j < pSystem_info->comp_ids_needed_count; j++) {
            PRP_Size comp_id = pSystem_info->pComp_ids_needed[j];
            PRP_Size word_i = WORD_I(comp_id);
            PRP_Size prefix_popcnt = pLayout->pWord_prefix_popcnts[word_i];
            PRP_U16 rank_in_word = (PRP_U16)CONT_BitwordPopCnt(
                pBitwords[word_i] & (BIT_MASK(comp_id) - 1));

            exec_internals.pComp_arr_strides[j] =
                pLayout->pComp_arr_strides[prefix_popcnt + rank_in_word];
        }

        CONT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ExecCb, &exec_internals);
    }
}

void *
SystemInstanceFetchComp(const FECS_SystemExecInternalData *pExec_internals,
                        PRP_Size idx) {
    if (idx >= pExec_internals->stides_len) {
        return NULL;
    }

    return pExec_internals->pChunk_mem +
           pExec_internals->pComp_arr_strides[idx];
}
