#include "Shared-Internals.h"

FECS_CompId CompRegister(DT_size comp_size) {
    if (!comp_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Array can't be made with memb_size=0.");
        return FECS_INVALID_COMP_ID;
    }

    FECS_CompId id = DT_ArrLen(g_state->comp_registry.comp_sizes);
    if (id == FECS_INVALID_COMP_ID) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Is your computer having unlimited memory?????. We ran "
                        "out of comps to create due to 64 bit int limit.");
        return FECS_INVALID_COMP_ID;
    }
    if (DT_ArrPush(g_state->comp_registry.comp_sizes, &comp_size)) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot create anymore FECS_CompIds.");
        return FECS_INVALID_COMP_ID;
    }

    return id;
}
