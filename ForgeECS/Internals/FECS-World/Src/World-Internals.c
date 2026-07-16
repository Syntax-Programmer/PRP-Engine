#include "ForgeECS/Internals/FECS-World/World-Internals.h"

/**
 * Initializes the world struct to accomodate for entire create info.
 *
 * @param pCreate_info The create info to initialize world with.
 * @param pWorld       The world pointer to initialize.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result WorldInit(FECS_WorldCreateInfo *pCreate_info,
                            FECS_World *pWorld);

PRP_Result WorldDeleteCb(DT_void *pWorld) {
    FECS_World *pWorld_instance = pWorld;

    if (pWorld_instance->pLayouts) {
        for (DT_size i = 0; i < pWorld_instance->layout_count; i++) {
            LayoutDelete(&pWorld_instance->pLayouts[i]);
        }
        free(pWorld_instance->pLayouts);
    }
    if (pWorld_instance->pSystem_instances) {
        for (DT_size i = 0; i < pWorld_instance->system_instance_count; i++) {
            SystemInstanceDelete(&pWorld_instance->pSystem_instances[i]);
        }
        free(pWorld_instance->pSystem_instances);
    }
    if (pWorld_instance->pLayout_names) {
        DT_StrArrDeleteUnchecked(&pWorld_instance->pLayout_names);
    }
    if (pWorld_instance->pSystem_instance_names) {
        DT_StrArrDeleteUnchecked(&pWorld_instance->pSystem_instance_names);
    }
#if !defined(PRP_NDEBUG)
    pWorld_instance->pLayouts = DT_null;
    pWorld_instance->pSystem_instances = DT_null;
    pWorld_instance->pLayout_names = DT_null;
    pWorld_instance->pSystem_instance_names = DT_null;
    pWorld_instance->layout_count = 0;
    pWorld_instance->system_instance_count = 0;
#endif

    return PRP_OK;
}

static PRP_Result WorldInit(FECS_WorldCreateInfo *pCreate_info,
                            FECS_World *pWorld) {
    *pWorld = (FECS_World){0};
    pWorld->pLayout_names = pCreate_info->pLayout_names;
    pWorld->pSystem_instance_names = pCreate_info->pSystem_instance_names;

    if (pCreate_info->layout_count) {
        pWorld->pLayouts =
            malloc(sizeof(FECS_Layout) * pCreate_info->layout_count);
        if (!pWorld->pLayouts) {
            // To free names assigned.
            WorldDeleteCb(pWorld);
            return PRP_ERR_OOM;
        }
    } else {
        // No names needed so freed.
        DT_StrArrDeleteUnchecked(&pWorld->pLayout_names);
    }
    if (pCreate_info->system_instance_count) {
        pWorld->pSystem_instances = malloc(sizeof(FECS_SystemInstance) *
                                           pCreate_info->system_instance_count);
        if (!pWorld->pSystem_instances) {
            // Since the counts are set to zero, WorldDeleteCb will not try to
            // free unowned memory.
            WorldDeleteCb(pWorld);
            return PRP_ERR_OOM;
        }
    } else {
        // No names needed so freed.
        DT_StrArrDeleteUnchecked(&pWorld->pSystem_instance_names);
    }

    return PRP_OK;
}

PRP_Result WorldCreate(FECS_WorldCreateInfo *pCreate_info, FECS_World *pWorld) {
    DT_size layout_create_info_idx = 0;
    DT_size system_instance_create_info_idx = 0;

    PRP_Result code = WorldInit(pCreate_info, pWorld);
    if (code != PRP_OK) {
        goto free_create_info;
    }

    code = PRP_OK; // Never hurts to be explicit.
    if (pWorld->pLayouts) {
        FECS_Layout *pLayouts = pWorld->pLayouts;
        for (DT_size i = 0; i < pCreate_info->layout_count; i++) {
            code = LayoutCreate(pCreate_info->ppLayout_create_infos[i],
                                &pLayouts[i]);
            if (code != PRP_OK) {
                pWorld->layout_count = i;
                WorldDeleteCb(pWorld);

                layout_create_info_idx = i + 1;
                goto free_create_info;
            }
        }
        layout_create_info_idx = PRP_INVALID_INDEX;
        // If this point is reached all layouts are initializes.
        pWorld->layout_count = pCreate_info->layout_count;
    }
    if (pWorld->pSystem_instances) {
        FECS_SystemInstance *pSystem_instances = pWorld->pSystem_instances;
        for (DT_size i = 0; i < pCreate_info->system_instance_count; i++) {
            code = SystemInstanceCreate(
                &pCreate_info->pSystem_instance_create_infos[i],
                &pSystem_instances[i]);
            if (code != PRP_OK) {
                pWorld->system_instance_count = i;
                WorldDeleteCb(pWorld);

                system_instance_create_info_idx = i + 1;
                goto free_create_info;
            }
        }
        system_instance_create_info_idx = PRP_INVALID_INDEX;
        pWorld->system_instance_count = pCreate_info->system_instance_count;
    }
    goto free_create_info;

free_create_info:
    if (layout_create_info_idx != PRP_INVALID_INDEX) {
        for (DT_size i = layout_create_info_idx; i < pCreate_info->layout_count;
             i++) {
            DT_BitmapDeleteUnchecked(&pCreate_info->ppLayout_create_infos[i]);
        }
    }
    if (system_instance_create_info_idx != PRP_INVALID_INDEX) {
        for (DT_size i = system_instance_create_info_idx;
             i < pCreate_info->system_instance_count; i++) {
            free(pCreate_info->pSystem_instance_create_infos[i]
                     .pLayout_id_matches);
        }
    }
    // The names arrays are freed by the WorldDelCb.
    // This is always true regardless of where and when the failure occured.
    free(pCreate_info->ppLayout_create_infos);
    pCreate_info->ppLayout_create_infos = DT_null;
    free(pCreate_info->pSystem_instance_create_infos);
    pCreate_info->pSystem_instance_create_infos = DT_null;

    return code;
}

FECS_LayoutId WorldFindLayout(const FECS_World *pWorld, const DT_char *pName,
                              DT_size name_len) {
    DT_size idx;
    if (pWorld->layout_count == 0 ||
        !DT_StrArrSearchUnchecked(pWorld->pLayout_names, pName, name_len,
                                  &idx)) {
        return FECS_INVALID_ID;
    }

    return (FECS_LayoutId)idx;
}

FECS_SystemInstanceId WorldFindSystemInstance(const FECS_World *pWorld,
                                              const DT_char *pName,
                                              DT_size name_len) {
    DT_size idx;
    if (pWorld->system_instance_count == 0 ||
        !DT_StrArrSearchUnchecked(pWorld->pSystem_instance_names, pName,
                                  name_len, &idx)) {
        return FECS_INVALID_ID;
    }

    return (FECS_SystemInstanceId)idx;
}
