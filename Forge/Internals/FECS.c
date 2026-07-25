#include "Forge/FECS.h"
#include "Forge/Internals/FECS-World/World-Internals.h"
#include "Forge/Internals/FECS/FECS-Internals.h"
#include "Forge/Internals/World-Compiler/Compiler-Internals.h"

/* ----  COMPS ---- */

PRP_API PRP_Result PRP_CALL FECS_CompRegister(PRP_Char8 *pName,
                                              PRP_Size name_len,
                                              PRP_Size comp_size,
                                              FECS_CompId *pComp_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pName != NULL);
    DIAG_ASSERT(name_len > 0);
    DIAG_ASSERT(pComp_id != NULL);

    if (!pName || !name_len || !pComp_id) {
        return PRP_ERR_INV_ARG;
    }
    *pComp_id = FECS_INVALID_ID;

    PRP_Result code = CompRegister(pName, name_len, comp_size, pComp_id);
    if (code == PRP_ERR_ALREADY_EXISTS) {
        DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                       "The Component: %.*s, already exists.",
                       (PRP_I32)name_len, pName);
    }

    return code;
}

/* ----  SYSTEMS ---- */

PRP_API PRP_Result PRP_CALL FECS_SystemRegister(PRP_Char8 *pName,
                                                PRP_Size name_len,
                                                FECS_SystemFunc system_func,
                                                PRP_Size comp_ids_needed_count,
                                                FECS_CompId *pComp_ids_needed,
                                                FECS_SystemId *pSystem_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pName != NULL);
    DIAG_ASSERT(name_len > 0);
    DIAG_ASSERT(system_func != NULL);
    DIAG_ASSERT(pSystem_id != NULL);

    if (!pName || !name_len || !system_func || !pSystem_id) {
        return PRP_ERR_INV_ARG;
    }
    *pSystem_id = FECS_INVALID_ID;

    /*
     * We don't assert for pComp_ids validity too here since that routine is
     * also handled by the SystemRegister function internally.
     * This is unlike the EntityGroup functions where we do assert in this
     * level, and that is intentional.
     */
    PRP_Result code =
        SystemRegister(pName, name_len, system_func, comp_ids_needed_count,
                       pComp_ids_needed, pSystem_id);
    if (code == PRP_ERR_ALREADY_EXISTS) {
        DIAG_LOG_ERROR(DIAG_LOG_CODE_INVALID_ARG,
                       "The System: %.*s, already exists.", (PRP_I32)name_len,
                       pName);
    }

    return code;
}

/* ----  WORLD ---- */

PRP_API PRP_Result PRP_CALL FECS_WorldLoad(const PRP_Char8 *pFile_path,
                                           FECS_WorldId *pWorld_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pFile_path != NULL);
    DIAG_ASSERT(pWorld_id != NULL);

    if (!pFile_path || !pWorld_id) {
        return PRP_ERR_INV_ARG;
    }
    *pWorld_id = FECS_INVALID_ID;

    FECS_WorldCreateInfo world_create_info;
    PRP_Result code = CompilerCompile(pFile_path, &world_create_info);
    if (code != PRP_OK) {
        return code;
    }

    FECS_World world;
    code = WorldCreate(&world_create_info, &world);
    if (code != PRP_OK) {
        // The entire create info is consumed regardless.
        return code;
    }

    code = CONT_DSArrAddUnchecked(g_ctx->pWorlds, &world, pWorld_id);
    if (code != PRP_OK) {
        WorldDeleteCb(&world);
        return code;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL FECS_WorldUnload(FECS_WorldId *pWorld_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pWorld_id != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, *pWorld_id),
                    "The given world id: %zu, is not valid.", (*pWorld_id));

    if (!pWorld_id) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrDelElemChecked(g_ctx->pWorlds, pWorld_id);
}

PRP_API PRP_Result PRP_CALL FECS_WorldFindLayoutId(FECS_WorldId world_id,
                                                   const char *pName,
                                                   PRP_Size name_len,
                                                   FECS_LayoutId *pLayout_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pName != NULL);
    DIAG_ASSERT(name_len > 0);
    DIAG_ASSERT(pLayout_id != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);

    if (!pName || !name_len || !pLayout_id) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    *pLayout_id = WorldFindLayout(pWorld, pName, name_len);
    if (*pLayout_id == FECS_INVALID_ID) {
        return PRP_ERR_NOT_FOUND;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL FECS_WorldFindSystemInstanceId(
    FECS_WorldId world_id, const char *pName, PRP_Size name_len,
    FECS_SystemInstanceId *pSystem_instance_id) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pName != NULL);
    DIAG_ASSERT(name_len > 0);
    DIAG_ASSERT(pSystem_instance_id != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);

    if (!pName || !name_len || !pSystem_instance_id) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    *pSystem_instance_id = WorldFindSystemInstance(pWorld, pName, name_len);
    if (*pSystem_instance_id == FECS_INVALID_ID) {
        return PRP_ERR_NOT_FOUND;
    }

    return PRP_OK;
}

/* ----  ENTITIES  ---- */

PRP_API PRP_Result PRP_CALL FECS_EntitySpawn(FECS_WorldId world_id,
                                             FECS_LayoutId layout_id,
                                             FECS_EntityId *pEntity) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pEntity != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!pEntity) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DIAG_ASSERT_MSG(
        layout_id < pWorld->layout_count,
        "The given layout id: %zu, is not a valid layout id in this world.",
        layout_id);
    if (layout_id >= pWorld->layout_count) {
        return PRP_ERR_INV_ARG;
    }

    return EntitySpawn(pWorld, layout_id, pEntity);
}

PRP_API PRP_Result PRP_CALL
FECS_EntityGroupSpawn(FECS_WorldId world_id, FECS_LayoutId layout_id,
                      PRP_Size entity_count, FECS_EntityGroupId **ppGroup) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(ppGroup != NULL);
    DIAG_ASSERT(entity_count > 0);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!ppGroup || !entity_count) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DIAG_ASSERT_MSG(
        layout_id < pWorld->layout_count,
        "The given layout id: %zu, is not a valid layout id in this world.",
        layout_id);
    if (layout_id >= pWorld->layout_count) {
        return PRP_ERR_INV_ARG;
    }

    return EntityGroupSpawn(pWorld, layout_id, entity_count, ppGroup);
}

PRP_API PRP_Result PRP_CALL FECS_EntityIsValid(FECS_WorldId world_id,
                                               const FECS_EntityId entity,
                                               PRP_Bool *pRslt) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pRslt != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = EntityIsValid(pWorld, entity);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL FECS_EntityGroupIsValid(
    FECS_WorldId world_id, const FECS_EntityGroupId *pGroup, PRP_Bool *pRslt) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pRslt != NULL);
    DIAG_ASSERT(pGroup != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!pRslt || !pGroup) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = EntityGroupIsValid(pWorld, pGroup);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL FECS_EntityKill(FECS_WorldId world_id,
                                            FECS_EntityId *pEntity) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pEntity != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!pEntity) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    PRP_Bool is_valid = EntityIsValid(pWorld, *pEntity);
    DIAG_ASSERT_MSG(is_valid,
                    "The given entity is not a valid entity in this world.");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    EntityKill(pWorld, pEntity);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL FECS_EntityGroupKill(FECS_WorldId world_id,
                                                 FECS_EntityGroupId **ppGroup) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(ppGroup != NULL);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!ppGroup) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DIAG_ASSERT_MSG(
        EntityGroupIsValid(pWorld, *ppGroup),
        "The given entity group is not a valid entity group in this world.");

    // This checks entity group validity internally so no need for extra checks.
    return EntityGroupKill(pWorld, ppGroup);
}

PRP_API PRP_Result PRP_CALL FECS_EntityGetComp(FECS_WorldId world_id,
                                               const FECS_EntityId entity,
                                               FECS_CompId comp_id,
                                               void **ppComp_ptr) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(ppComp_ptr != NULL);
    DIAG_ASSERT_MSG(
        comp_id < CONT_ArrLen(g_ctx->pComp_sizes),
        "The given comp_id: %zu, is not a valid component in the FECS runtime.",
        comp_id);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!ppComp_ptr || comp_id >= CONT_ArrLen(g_ctx->pComp_sizes)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    PRP_Bool is_valid = EntityIsValid(pWorld, entity);
    DIAG_ASSERT_MSG(is_valid,
                    "The given entity is not a valid entity in this world.");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    return EntityGetComp(pWorld, entity, comp_id, ppComp_ptr);
}

PRP_API PRP_Result PRP_CALL FECS_EntitySetComp(FECS_WorldId world_id,
                                               FECS_EntityId entity,
                                               FECS_CompId comp_id,
                                               const void *pComp_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pComp_data != NULL);
    DIAG_ASSERT_MSG(
        comp_id < CONT_ArrLen(g_ctx->pComp_sizes),
        "The given comp_id: %zu, is not a valid component in the FECS runtime.",
        comp_id);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!pComp_data || comp_id >= CONT_ArrLen(g_ctx->pComp_sizes)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    PRP_Bool is_valid = EntityIsValid(pWorld, entity);
    DIAG_ASSERT_MSG(is_valid,
                    "The given entity is not a valid entity in this world.");
    if (!is_valid) {
        return PRP_ERR_INV_ARG;
    }

    return EntitySetComp(pWorld, entity, comp_id, pComp_data);
}

PRP_API PRP_Result PRP_CALL FECS_EntityGroupForEach(
    FECS_WorldId world_id, FECS_EntityGroupId *pGroup, FECS_CompId comp_id,
    PRP_Result (*cb)(void *pComp_data, void *pUser_data), void *pUser_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pGroup != NULL);
    DIAG_ASSERT(cb != NULL);
    DIAG_ASSERT_MSG(
        comp_id < CONT_ArrLen(g_ctx->pComp_sizes),
        "The given comp_id: %zu, is not a valid component in the FECS runtime.",
        comp_id);
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    if (!pGroup || !cb || comp_id >= CONT_ArrLen(g_ctx->pComp_sizes)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DIAG_ASSERT_MSG(
        EntityGroupIsValid(pWorld, pGroup),
        "The given entity group is not a valid entity group in this world.");

    return EntityGroupForEach(pWorld, pGroup, comp_id, cb, pUser_data);
}

/* ----  SYSTEM INSTANCE ---- */

PRP_API PRP_Result PRP_CALL FECS_SystemInstanceExec(
    FECS_WorldId world_id, FECS_SystemInstanceId system_instance_id,
    void *pUser_data) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT_MSG(CONT_DSIdIsValidUnchecked(g_ctx->pWorlds, world_id),
                    "The given world id: %zu, is not valid.", world_id);
    FECS_World *pWorld;
    PRP_Result code =
        CONT_DSIdToDataChecked(g_ctx->pWorlds, world_id, (void **)&pWorld);
    if (code != PRP_OK) {
        return PRP_ERR_INV_ARG;
    }
    DIAG_ASSERT_MSG(system_instance_id < pWorld->system_instance_count,
                    "The given system instance id: %zu, is not a valid system "
                    "instance id in this world.",
                    system_instance_id);
    if (system_instance_id >= pWorld->system_instance_count) {
        return PRP_ERR_INV_ARG;
    }

    SystemInstanceExec(pWorld, system_instance_id, pUser_data);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL
FECS_SystemInstanceFetchComp(const FECS_SystemExecInternalData *pExec_internals,
                             PRP_Size idx, void **ppComp_arr) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }
    DIAG_ASSERT(pExec_internals != NULL);
    DIAG_ASSERT(ppComp_arr != NULL);
    if (!pExec_internals || !ppComp_arr) {
        return PRP_ERR_INV_ARG;
    }

    *ppComp_arr = SystemInstanceFetchComp(pExec_internals, idx);
    if (!(*ppComp_arr)) {
        return PRP_ERR_OOB;
    }

    return PRP_OK;
}

/* ----  FECS ---- */

PRP_API PRP_Result PRP_CALL FECS_Init(void) {
    if (CTX_INVARIANT_EXPR) {
        return PRP_OK;
    }

    g_ctx = calloc(1, sizeof(FECS_InternalCtx));
    if (!g_ctx) {
        return PRP_ERR_OOM;
    }

    PRP_Result code = CONT_ArrCreateUnchecked(
        sizeof(PRP_Size), CONT_ARR_DEFAULT_CAP, &g_ctx->pComp_sizes);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = CONT_ArrCreateUnchecked(sizeof(FECS_SystemInfo),
                                   CONT_ARR_DEFAULT_CAP, &g_ctx->pSystem_infos);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = CONT_DSArrCreateUnchecked(sizeof(FECS_World), WorldDeleteCb,
                                     &g_ctx->pWorlds);
    if (code != PRP_OK) {
        goto err_path;
    }
    const PRP_Size INIT_BFFR_SIZE = 256;
    const PRP_Size INIT_CAP = 16;
    code = CONT_StrArrCreateUnchecked(INIT_BFFR_SIZE, INIT_CAP,
                                      &g_ctx->pComp_names);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = CONT_StrArrCreateUnchecked(INIT_BFFR_SIZE, INIT_CAP,
                                      &g_ctx->pSystem_names);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    if (g_ctx->pComp_sizes) {
        CONT_ArrDeleteUnchecked(&g_ctx->pComp_sizes);
    }
    if (g_ctx->pSystem_infos) {
        CONT_ArrDeleteUnchecked(&g_ctx->pSystem_infos);
    }
    if (g_ctx->pWorlds) {
        CONT_DSArrDeleteUnchecked(&g_ctx->pWorlds);
    }
    if (g_ctx->pComp_names) {
        CONT_StrArrDeleteUnchecked(&g_ctx->pComp_names);
    }
    if (g_ctx->pSystem_names) {
        CONT_StrArrDeleteUnchecked(&g_ctx->pSystem_names);
    }
    free(g_ctx);
    g_ctx = NULL;

    return code;
}

PRP_API void PRP_CALL FECS_Exit(void) {
    if (!CTX_INVARIANT_EXPR) {
        DIAG_PANIC("The engine is corrupted/not-initilized correctly.");
    }

    CONT_ArrDeleteUnchecked(&g_ctx->pComp_sizes);
    CONT_ArrForEachUnchecked(g_ctx->pSystem_infos, SystemInfoDeleteCb, NULL);
    CONT_ArrDeleteUnchecked(&g_ctx->pSystem_infos);
    CONT_DSArrDeleteUnchecked(&g_ctx->pWorlds);
    CONT_StrArrDeleteUnchecked(&g_ctx->pComp_names);
    CONT_StrArrDeleteUnchecked(&g_ctx->pSystem_names);

    free(g_ctx);
    g_ctx = NULL;
}
