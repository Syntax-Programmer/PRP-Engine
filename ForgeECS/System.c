#include "Defs.h"
#include "FECS-Internals.h"

PRP_Result SystemRegister(FECS_System system, DT_size *pIdx) {
    PRP_Result code = DT_ArrPushUnchecked(g_ctx->systems, &system);
    if (code != PRP_OK) {
        return code;
    }

    *pIdx = DT_ArrLen(g_ctx->systems) - 1;

    return PRP_OK;
}

DT_bool SystemIsRegistered(FECS_System system, DT_size *pIdx) {
    DT_size len;
    const FECS_System *systems = DT_ArrRawUnchecked(g_ctx->systems, &len);

    for (DT_size i = 0; i < len; i++) {
        if (systems[i] == system) {
            *pIdx = i;
            return DT_true;
        }
    }

    return DT_false;
}
