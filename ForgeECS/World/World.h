#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Data-Types/DSArr.h"
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
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldCreate(DT_DSId *pWorld_id);
/**
 * Deletes the world and invalidates the id.
 *
 * @param pWorld_id Pointer to world id.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldDelete(DT_DSId *pWorld_id);

/* ----  SYSTEMS ---- */

/**
 * Creates a new system cache into the world.
 *
 * @param world_id   World to create into.
 * @param system_idx System that will be executed.
 * @param query_idx  Query on whose matches the system will run.
 * @param pIdx       The pointer to hold the index of the system cache.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemCacheCreate(DT_DSId world_id,
                                                         DT_size system_idx,
                                                         DT_size query_idx,
                                                         DT_size *pIdx);

/**
 * Executes all enabled system caches in the world.
 *
 * @param world_id  World to execute the caches of.
 * @param user_data The user given/managed data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldSystemCacheExecAll(DT_DSId world_id, DT_void *user_data);
/**
 * Executes a singular enabled system caches in the world.
 *
 * @param world_id         World the system cache belongs to.
 * @param system_cache_idx The system cache to execute.
 * @param user_data        The user given/managed data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_WorldSystemCacheExecOne(
    DT_DSId world_id, DT_size system_cache_idx, DT_void *user_data);

/**
 * Enables the given system cache idx.
 *
 * @param world_id         World the system cache belongs to.
 * @param system_cache_idx The system cache to enable.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldEnableSystemCache(DT_DSId world_id, DT_size system_cache_idx);
/**
 * Disables the given system cache idx.
 *
 * @param world_id         World the system cache belongs to.
 * @param system_cache_idx The system cache to disable.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_WorldDisableSystemCache(DT_DSId world_id, DT_size system_cache_idx);

/* ----  LAYOUTS ---- */

/**
 * Creates a new layout into the world.
 *
 * @param world        World to create into.
 * @param behavior_idx Behavior that the layout derives from.
 * @param pIdx         The pointer to hold the index of the layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutCreate(DT_DSId world_id,
                                                    DT_size behavior_idx,
                                                    DT_size *pIdx);

/**
 * Spawns a new entity into the given layout.
 *
 * @param world      World, the layout belongs to.
 * @param layout_idx The layout to spawn entity from.
 * @param pEntity    The pointer to where the entity will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutSpawnEntity(DT_DSId world_id,
                                                         DT_size layout_idx,
                                                         FECS_Entity *pEntity);
/**
 * Spawns multiple new entities at once into the given layout.
 *
 * Will return a batch with less entities created if it fails mid allocation.
 *
 * @param world      World, the layout belongs to.
 * @param layout_idx The layout to spawn entities from.
 * @param count      The number of entities to spawn.
 * @param pEntities  The pointer to where the entities will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSpawnEntities(DT_DSId world_id, DT_size layout_idx, DT_size count,
                         FECS_EntityBatch **pEntities);

/**
 * Checks if the given entity is valid.
 *
 * @param world  World, the entity belongs to.
 * @param entity The entitiy to check.
 * @param pRslt  The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result FECS_LayoutIsEntityValid(DT_DSId world_id,
                                               const FECS_Entity entity,
                                               DT_bool *pRslt);
/**
 * Checks if the given entity batch is valid.
 *
 * @param world    World, the entities belongs to.
 * @param entities The entitiy batch to check.
 * @param pRslt    The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result FECS_LayoutAreEntitiesValid(
    DT_DSId world_id, const FECS_EntityBatch *entities, DT_bool *pRslt);

/**
 * Kills the given entity.
 *
 * @param world  World, the entity belongs to.
 * @param entity The entitiy to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if argument is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutKillEntity(DT_DSId world_id,
                                                        FECS_Entity entity);
/**
 * Kills the given entity and nullifies the pointer.
 *
 * @param world    World, the entities belongs to.
 * @param pEntities The pointer to the entities to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutKillEntities(DT_DSId world_id, FECS_EntityBatch **pEntities);

/**
 * Fetches the pointer to the specific component of an entity.
 *
 * @param world    World the entity belongs to.
 * @param entity   The entity whose component to get.
 * @param comp_idx The component to fetch of the entity.
 * @param dest     The pointer to the memory location of the component data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutGetEntityComp(DT_DSId world_id, const FECS_Entity entity,
                         DT_size comp_idx, DT_void **dest);
/**
 * Sets the value to the specific component of an entity.
 *
 * @param world    World the entity belongs to.
 * @param entity   The entity whose component to set.
 * @param comp_idx The component to set of the entity.
 * @param data     The pointer to the value to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_LayoutSetEntityComp(DT_DSId world_id, const FECS_Entity entity,
                         DT_size comp_idx, const DT_void *data);
/**
 * Iterates over all entities of a batch.
 *
 * @param world     World, the entities belongs to.
 * @param entities  The entities to operate on.
 * @param comp_idx  The component to operate on.
 * @param cb        Callback invoked per element.
 * @param user_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_STATE if the schema lock is NOT initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_LayoutForEachEntities(
    DT_DSId world_id, FECS_EntityBatch *entities, DT_size comp_idx,
    PRP_Result (*cb)(DT_void *comp_data, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
