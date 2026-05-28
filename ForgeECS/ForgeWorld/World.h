#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ForgeECS/Defs.h"

/**
 * Creates a new empty world.
 *
 * @param pWorld_id Output pointer recieving the world id.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldCreate(FECS_WorldId *pWorld_id);
/**
 * Deletes the world and invalidates the id.
 *
 * @param pWorld_id Pointer to world id.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDelete(FECS_WorldId *pWorld_id);

/* ----  SYSTEMS ---- */

/**
 * Creates a new system cache into the world.
 *
 * @param world_id         World to create into.
 * @param system_id        System that will be executed.
 * @param query_id         Query on whose matches the system will run.
 * @param pSystem_cache_id The pointer to hold the index of the system cache.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemCacheCreate(
    FECS_WorldId world_id, FECS_SystemId system_id, FECS_QueryId query_id,
    FECS_SystemCacheId *pSystem_cache_id);

/**
 * Executes all enabled system caches in the world.
 *
 * @param world_id   World to execute the caches of.
 * @param pUser_data The user given/managed data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldSystemCacheExecAll(FECS_WorldId world_id, DT_void *pUser_data);
/**
 * Executes a singular enabled system caches in the world.
 *
 * @param world_id         World the system cache belongs to.
 * @param system_cache_id  The system cache to execute.
 * @param pUser_data       The user given/managed data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldSystemCacheExecOne(
    FECS_WorldId world_id, FECS_SystemCacheId system_cache_id,
    DT_void *pUser_data);

/**
 * Enables the given system cache idx.
 *
 * @param world_id         World the system cache belongs to.
 * @param system_cache_id  The system cache to enable.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldEnableSystemCache(
    FECS_WorldId world_id, FECS_SystemCacheId system_cache_id);
/**
 * Disables the given system cache idx.
 *
 * @param world_id         World the system cache belongs to.
 * @param system_cache_id  The system cache to disable.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDisableSystemCache(
    FECS_WorldId world_id, FECS_SystemCacheId system_cache_id);

/* ----  LAYOUTS ---- */

/**
 * Creates a new layout into the world.
 *
 * @param world_id    World to create into.
 * @param behavior_id Behavior that the layout derives from.
 * @param pLayout_id  The pointer to hold the index of the layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutCreate(FECS_WorldId world_id,
                                                    FECS_BehaviorId behavior_id,
                                                    FECS_LayoutId *pLayout_id);

/**
 * Spawns a new entity into the given layout.
 *
 * @param world_id  World, the layout belongs to.
 * @param layout_id The layout to spawn entity from.
 * @param pEntity   The pointer to where the entity will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutSpawnEntity(
    FECS_WorldId world_id, FECS_LayoutId layout_id, FECS_Entity *pEntity);
/**
 * Spawns multiple new entities at once into the given layout.
 *
 * Will return a batch with less entities created if it fails mid allocation.
 *
 * @param world_id     World, the layout belongs to.
 * @param layout_id    The layout to spawn entities from.
 * @param entity_count The number of entities to spawn.
 * @param ppBatch      The pointer to where the entities will be stored.d.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSpawnEntities(FECS_WorldId world_id, FECS_LayoutId layout_id,
                         DT_size entity_count, FECS_EntityBatch **ppBatch);

/**
 * Checks if the given entity is valid.
 *
 * @param world_id World, the entity belongs to.
 * @param entity  The entitiy to check.
 * @param pRslt   The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result FECS_LayoutIsEntityValid(FECS_WorldId world_id,
                                               const FECS_Entity entity,
                                               DT_bool *pRslt);
/**
 * Checks if the given entity batch is valid.
 *
 * @param world_id World, the entities belongs to.
 * @param pBatch   The entitiy batch to check.
 * @param pRslt    The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result FECS_LayoutAreEntitiesValid(
    FECS_WorldId world_id, const FECS_EntityBatch *pBatch, DT_bool *pRslt);

/**
 * Kills the given entity.
 *
 * @param world_id World, the entity belongs to.
 * @param entity   The entitiy to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutKillEntity(FECS_WorldId world_id,
                                                        FECS_Entity entity);
/**
 * Kills the given entity and nullifies the pointer.
 *
 * @param world_id World, the entities belongs to.
 * @param ppBatch  The pointer to the entities to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutKillEntities(FECS_WorldId world_id, FECS_EntityBatch **ppbatch);

/**
 * Fetches the pointer to the specific component of an entity.
 *
 * @param world_id   World the entity belongs to.
 * @param entity     The entity whose component to get.
 * @param comp_id    The component to fetch of the entity.
 * @param ppDest_ptr The pointer to the memory location of the component data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutGetEntityComp(FECS_WorldId world_id, const FECS_Entity entity,
                         FECS_CompId comp_id, DT_void **ppDest_ptr);
/**
 * Sets the value to the specific component of an entity.
 *
 * @param world_id World the entity belongs to.
 * @param entity   The entity whose component to set.
 * @param comp_id  The component to set of the entity.
 * @param pData    The pointer to the value to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSetEntityComp(FECS_WorldId world_id, const FECS_Entity entity,
                         FECS_CompId comp_id, const DT_void *pData);
/**
 * Iterates over all entities of a batch.
 *
 * @param world_id   World, the entities belongs to.
 * @param pBatch     The entities to operate on.
 * @param comp_id    The component to operate on.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutForEachEntities(
    FECS_WorldId world_id, FECS_EntityBatch *pBatch, FECS_CompId comp_id,
    PRP_Result (*cb)(DT_void *pComp_data, DT_void *pUser_data),
    DT_void *pUser_data);

#ifdef __cplusplus
}
#endif
