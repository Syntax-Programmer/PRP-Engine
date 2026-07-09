#include "ForgeECS/Internals/FECS-World/World-Internals.h"

DT_void SystemInstanceCreate(FECS_SystemInstanceCreateInfo *pCreate_info,
                             FECS_SystemInstance *pSystem_instance) {
    *pSystem_instance = (FECS_SystemInstance){0};
    pSystem_instance->system_id = pCreate_info->system_id;
    pSystem_instance->is_enabled = DT_true;
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
    pSystem_instance->is_enabled = DT_false;
    pSystem_instance->layout_id_match_count = 0;
    pSystem_instance->pLayout_id_matches = DT_null;
#endif
}
