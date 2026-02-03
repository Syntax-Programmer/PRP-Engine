#include "Shared-Internals.h"

FECS_CompId CompRegister(DT_size comp_size) {
    DIAG_GUARD(comp_size > 0, FECS_INVALID_COMP_ID);

    FECS_CompId id = DT_ArrLen(g_state->comp_registry.comp_sizes);
    if (DT_ArrPush(g_state->comp_registry.comp_sizes, &comp_size)) {
        return FECS_INVALID_COMP_ID;
    }

    return id;
}
