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

/* ----  LAYOUT ---- */

PRP_FN_API CORE_Id PRP_FN_CALL FECS_LayoutCreate(CORE_Id b_set_id) {
    STATE_VALIDITY_CHECK(CORE_INVALID_ID);

    CORE_Id layout_id = LayoutCreate(b_set_id);
    if (layout_id != CORE_INVALID_ID) {
        QueryCascadeLayoutCreate(layout_id);
    }

    return layout_id;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_LayoutDelete(CORE_Id *pLayout_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    if (pLayout_id && *pLayout_id != CORE_INVALID_ID) {
        QueryCascadeLayoutCreate(*pLayout_id);
    }

    return LayoutDelete(pLayout_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_LayoutCreateEntity(CORE_Id layout_id, FECS_EntityId *entity_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return LayoutCreateEntity(layout_id, entity_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_LayoutDeleteEntity(FECS_EntityId *entity_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return LayoutDeleteEntity(entity_id);
}

PRP_FN_API FECS_EntityIdBatch *PRP_FN_CALL
FECS_LayoutCreateEntityBatch(CORE_Id layout_id, DT_size count) {
    STATE_VALIDITY_CHECK(DT_null);

    return LayoutCreateEntityBatch(layout_id, count);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_LayoutDeleteEntityBatch(FECS_EntityIdBatch **pEntity_batch) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return LayoutDeleteEntityBatch(pEntity_batch);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_LayoutEntityOperateComp(
    FECS_EntityId entity_id, FECS_CompId comp_id,
    PRP_FnCode (*fn)(DT_void *data, DT_void *user_data), DT_void *user_data) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return LayoutEntityOperateComp(entity_id, comp_id, fn, user_data);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_LayoutEntityBatchOperateComp(
    FECS_EntityIdBatch *entity_batch, FECS_CompId comp_id,
    PRP_FnCode (*fn)(DT_void *data, DT_void *user_data), DT_void *user_data) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return LayoutEntityBatchOperateComp(entity_batch, comp_id, fn, user_data);
}

/* ----  QUERY  ---- */

PRP_FN_API CORE_Id PRP_FN_CALL FECS_QueryCreate(CORE_Id exclude_b_set_id,
                                                CORE_Id include_b_set_id) {
    STATE_VALIDITY_CHECK(CORE_INVALID_ID);

    return QueryCreate(exclude_b_set_id, include_b_set_id);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_QueryDelete(CORE_Id *pQuery_id) {
    STATE_VALIDITY_CHECK(PRP_FN_NULL_ERROR);

    return QueryDelete(pQuery_id);
}

/* ----  SYSTEM  ---- */

PRP_FN_API CORE_Id PRP_FN_CALL FECS_SystemCreate(CORE_Id query_id,
                                                 FECS_SysFn fn,
                                                 DT_void *user_data) {
    STATE_VALIDITY_CHECK(CORE_INVALID_ID);

    return SystemCreate(query_id, fn, user_data);
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

    g_state->b_set_id_mgr =
        CORE_IdMgrCreate(sizeof(DT_Bitmap *), BehaviorSetDelCb);
    STATE_INIT_ERROR_CHECK(g_state->b_set_id_mgr);

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
    if (g_state->b_set_id_mgr) {
        CORE_IdMgrDelete(&g_state->b_set_id_mgr);
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
