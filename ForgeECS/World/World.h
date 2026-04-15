#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Data-Types/DSArr.h"
#include "../Defs.h"

PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldCreate(DT_DSId *pWorld_id);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDelete(DT_DSId *pWorld_id);

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemCacheCreate(DT_DSId world_id,
                                                         DT_size system_idx,
                                                         DT_size query_idx,
                                                         DT_size *pIdx);

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldSystemCacheExecAll(DT_DSId world_id, DT_void *user_data);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldSystemCacheExecOne(
    DT_DSId world_id, DT_size system_cache_idx, DT_void *user_data);

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldEnableSystemCache(DT_DSId world_id, DT_size system_cache_idx);
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldDisableSystemCache(DT_DSId world_id, DT_size system_cache_idx);

/* ----  LAYOUTS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutCreate(DT_DSId world_id,
                                                    DT_size behavior_idx,
                                                    DT_size *pIdx);

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutSpawnEntity(DT_DSId world_id,
                                                         DT_size layout_idx,
                                                         FECS_Entity *pEntity);
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSpawnEntities(DT_DSId world_id, DT_size layout_idx, DT_size count,
                         FECS_EntityBatch **pEntities);
PRP_FN_API PRP_Result FECS_LayoutIsEntityValid(DT_DSId world_id,
                                               const FECS_Entity entity,
                                               DT_bool *pRslt);
PRP_FN_API PRP_Result FECS_LayoutAreEntitiesValid(
    DT_DSId world_id, const FECS_EntityBatch *entities, DT_bool *pRslt);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutKillEntity(DT_DSId world_id,
                                                        FECS_Entity entity);
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutKillEntities(DT_DSId world_id, FECS_EntityBatch *entities);
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutGetEntityComp(DT_DSId world_id, const FECS_Entity entity,
                         DT_size comp_idx, DT_void **dest);
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSetEntityComp(DT_DSId world_id, const FECS_Entity entity,
                         DT_size comp_idx, const DT_void *data);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutForEachEntities(
    DT_DSId world_id, FECS_EntityBatch *entities, DT_size comp_idx,
    PRP_Result (*cb)(DT_void *comp_data, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
