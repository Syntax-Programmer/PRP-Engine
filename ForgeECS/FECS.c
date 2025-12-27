#include "FECS.h"
#include "Shared-Internals.h"

/* ----  INTERNAL  ---- */

ECSState *g_state = DT_null;

#define STATE_VALIDITY_CHECK(ret)                                              \
    do {                                                                       \
        if (!g_state) {                                                        \
            PRP_LOG_FN_NULL_ERROR(g_state);                                    \
            return ret;                                                        \
        }                                                                      \
    } while (0)

/* ----  COMP ---- */

PRP_FN_API FECS_CompId PRP_FN_CALL FECS_CompRegister(DT_size comp_size) {
    STATE_VALIDITY_CHECK(FECS_INVALID_COMP_ID);

    return CompRegister(comp_size);
}

/* ----  BEHAVIOR SET ---- */

PRP_FN_API CORE_Id PRP_FN_CALL FECS_BehaviorSetCreate(DT_void) {
    STATE_VALIDITY_CHECK(CORE_INVALID_ID);

    return BehaviorSetCreate();
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_BehaviorSetDelete(CORE_Id *pB_set_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return BehaviorSetDelete(pB_set_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_BehaviorSetClear(CORE_Id b_set_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return BehaviorSetClear(b_set_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_BehaviorSetAttachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return BehaviorSetAttachComp(b_set_id, comp_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_BehaviorSetDetachComp(CORE_Id b_set_id, FECS_CompId comp_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return BehaviorSetDetachComp(b_set_id, comp_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_BehaviorSetHasComp(CORE_Id b_set_id,
                                                          FECS_CompId comp_id,
                                                          DT_bool *pRslt) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return BehaviorSetHasComp(b_set_id, comp_id, pRslt);
}

/* ----  SYSTEM  ---- */

PRP_FN_API CORE_Id PRP_FN_CALL FECS_SystemCreate(CORE_Id query_id,
                                                 FECS_SysFn func,
                                                 DT_void *user_data) {
    STATE_VALIDITY_CHECK(CORE_INVALID_ID);

    return SystemCreate(query_id, func, user_data);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_SystemDelete(CORE_Id *pSystem_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return SystemDelete(pSystem_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_SystemExec(CORE_Id system_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return SystemExec(system_id);
}

/* ----  STATE  ---- */

#define STATE_INIT_ERROR_CHECK(x)                                              \
    do {                                                                       \
        if (!x) {                                                              \
            FECS_Exit();                                                       \
            PRP_LOG_FN_MALLOC_ERROR(x);                                        \
            return PRP_FN_MALLOC_ERROR;                                        \
        }                                                                      \
    } while (0);

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_Init(DT_void) {
    g_state = calloc(1, sizeof(ECSState));
    if (!g_state) {
        PRP_LOG_FN_MALLOC_ERROR(g_state);
        return PRP_FN_MALLOC_ERROR;
    }

    g_state->comp_registry.comp_sizes = DT_ArrCreateDefault(sizeof(DT_size));
    STATE_INIT_ERROR_CHECK(g_state->comp_registry.comp_sizes);

    g_state->behavior_set_id_mgr =
        CORE_IdMgrCreate(sizeof(DT_Bitmap *), BehaviorSetDelCb);
    STATE_INIT_ERROR_CHECK(g_state->behavior_set_id_mgr);

    g_state->layout_id_mgr = CORE_IdMgrCreate(sizeof(Layout), LayoutDelCb);
    STATE_INIT_ERROR_CHECK(g_state->layout_id_mgr);

    g_state->query_id_mgr = CORE_IdMgrCreate(sizeof(Query), QueryDelCb);
    STATE_INIT_ERROR_CHECK(g_state->query_id_mgr);

    g_state->system_id_mgr = CORE_IdMgrCreate(sizeof(System), DT_null);
    STATE_INIT_ERROR_CHECK(g_state->system_id_mgr);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_Exit(DT_void) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    if (g_state->comp_registry.comp_sizes) {
        DT_ArrDelete(&g_state->comp_registry.comp_sizes);
    }
    if (g_state->behavior_set_id_mgr) {
        CORE_IdMgrDelete(&g_state->behavior_set_id_mgr);
    }
    if (g_state->layout_id_mgr) {
        CORE_IdMgrDelete(&g_state->layout_id_mgr);
    }
    // Deleting system before queries to prevent dangling internal ptrs.
    if (g_state->system_id_mgr) {
        CORE_IdMgrDelete(&g_state->system_id_mgr);
    }
    if (g_state->query_id_mgr) {
        CORE_IdMgrDelete(&g_state->query_id_mgr);
    }

    free(g_state);
    g_state = DT_null;

    return PRP_FN_SUCCESS;
}
