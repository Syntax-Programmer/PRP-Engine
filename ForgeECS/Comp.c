#include "Shared-Internals.h"

FECS_CompId CompIdRegister(DT_size comp_size) {
    if (!comp_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Array can't be made with memb_size=0.");
        return FECS_INVALID_COMP_ID;
    }

    FECS_CompId id = DT_ArrLen(g_state->comp_registry.comp_sizes);
    if (DT_ArrPush(g_state->comp_registry.comp_sizes, &comp_size)) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot create anymore FECS_CompIds.");
        return FECS_INVALID_COMP_ID;
    }

    return id;
}
