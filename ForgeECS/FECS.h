#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Internals/Typedefs.h"

/* ----  COMPS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_CompRegister(DT_char *pName,
                                                    DT_size name_len,
                                                    DT_size comp_size,
                                                    FECS_CompId *pComp_id);

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_SystemRegister(DT_char *pName, DT_size name_len,
                    FECS_SystemFunc system_func, FECS_SystemId *pSystem_id);

/* ----  WORLD ---- */

PRP_Result FECS_WorldLoad(const DT_char *pFile_path, FECS_WorldId *pWorld_id);
PRP_Result FECS_WorldUnload(FECS_WorldId *pWorld_id);

PRP_Result FECS_WorldFindLayoutId(FECS_WorldId world_id, const char *pName,
                                  DT_size name_len, FECS_LayoutId *pLayout_id);
PRP_Result
FECS_WorldFindSystemInstanceId(FECS_WorldId world_id, const char *pName,
                               DT_size name_len,
                               FECS_SystemInstanceId *pSystem_instance_id);

/* ----  ENTITIES  ---- */

PRP_Result FECS_EntitySpawn(FECS_WorldId world_id, FECS_LayoutId layout_id,
                            FECS_EntityId *pEntity);
PRP_Result FECS_EntityGroupSpawn(FECS_WorldId world_id, FECS_LayoutId layout_id,
                                 DT_size entity_count,
                                 FECS_EntityGroupId **ppGroup);

PRP_Result FECS_EntityIsValid(FECS_WorldId world_id, const FECS_EntityId entity,
                              DT_bool *pRslt);
PRP_Result FECS_EntityGroupIsValid(FECS_WorldId world_id,
                                   const FECS_EntityGroupId *pGroup,
                                   DT_bool *pRslt);

PRP_Result FECS_EntityKill(FECS_WorldId world_id, FECS_EntityId *pEntity);
PRP_Result FECS_EntityGroupKill(FECS_WorldId world_id,
                                FECS_EntityGroupId **ppGroup);

PRP_Result FECS_EntityGetComp(FECS_WorldId world_id, const FECS_EntityId entity,
                              FECS_CompId comp_id, DT_void **ppComp_ptr);
PRP_Result FECS_EntitySetComp(FECS_WorldId world_id, FECS_EntityId entity,
                              FECS_CompId comp_id, const DT_void *pComp_data);

PRP_Result FECS_EntityGroupForEach(
    FECS_WorldId world_id, FECS_EntityGroupId *pGroup, FECS_CompId comp_id,
    PRP_Result (*cb)(DT_void *pComp_data, DT_void *pUser_data),
    DT_void *pUser_data);

/* ----  SYSTEM INSTANCE ---- */

/* ----  FECS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void);
PRP_FN_API DT_void PRP_FN_CALL FECS_Exit(DT_void);

#ifdef __cplusplus
}
#endif
