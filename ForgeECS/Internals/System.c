#include "FECS-Internals.h"

PRP_Result SystemRegister(FECS_SystemFunc system_func,
                          FECS_SystemId *pSystem_id) {
    PRP_Result code = DT_ArrPushUnchecked(g_ctx->systems, &system_func);
    if (code != PRP_OK) {
        return code;
    }

    *pSystem_id = DT_ArrLen(g_ctx->systems) - 1;

    return PRP_OK;
}

DT_bool SystemIsRegistered(FECS_SystemFunc system_func,
                           FECS_SystemId *pFound_id) {
    DT_size len;
    const FECS_SystemFunc *systems = DT_ArrRawUnchecked(g_ctx->systems, &len);

    for (DT_size i = 0; i < len; i++) {
        if (systems[i] == system_func) {
            *pFound_id = i;
            return DT_true;
        }
    }

    return DT_false;
}
