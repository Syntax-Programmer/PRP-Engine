#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/Bitmap.h"
#include "ForgeECS/Defs.h"

/* ----  WORLDS ---- */

/**
 * A world is a scene that derives data from schema defs.
 */
typedef struct {
    DT_Arr *layouts;
    DT_Arr *system_caches;
} World;

#define WORLD_INVARIANT_EXPR(world)                                            \
    ((world) != DT_null && DT_ArrIsValid((world)->layouts) &&                  \
     DT_ArrIsValid((world)->system_caches))

/**
 * Deletes a given world.
 * Used inside the DT_DSArr's elem_del_cb.
 *
 * @param pWorld World to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result WorldDeleteCb(DT_void *pWorld);

/* ----  SYSTEMS ---- */

typedef struct {
    DT_bool is_enabled;
    /**
     * Caching the function pointer itself rather than the system_idx, this is
     * because the function pointers are stable so we can do it, they reduce the
     * indirection it takes.
     */
    FECS_SystemFunc system_func;
    /*
     * Storing this too along side the system to maintain the identity of the
     * system we store with the g_ctx.
     */
    FECS_SystemId system_id;
    /*
     * Same purpose as the system_idx, to link it to the g_ctx and world. Even
     * though we store the layout matches also.
     */
    FECS_QueryId query_id;
    DT_Arr *layout_id_matches;
} SystemCache;

#define SYSTEM_CACHE_INVARIANT_EXPR(system_cache)                              \
    ((system_cache) != DT_null && (system_cache)->system_func != DT_null &&    \
     (system_cache)->system_id < DT_ArrLen(g_ctx->systems) &&                  \
     (system_cache)->query_id < DT_ArrLen(g_ctx->queries) &&                   \
     (system_cache)->layout_id_matches != DT_null &&                           \
     (system_cache)->system_func ==                                            \
         (*(FECS_SystemFunc *)DT_ArrGetUnchecked(g_ctx->systems,               \
                                                 (system_cache)->system_id)))

/**
 * Creates a new system cache into the world.
 *
 * @param pWorld           World to create into.
 * @param system_id        System that will be executed.
 * @param query_id         Query on whose matches the system will run.
 * @param pSystem_cache_id The pointer to hold the index of the system cache.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result SystemCacheCreate(World *pWorld, FECS_SystemId system_id,
                             FECS_QueryId query_id,
                             FECS_SystemCacheId *pSystem_cache_id);
/**
 * Checks if given system cache already exists.
 *
 * @param pWorld    World to create into.
 * @param system_id System that will be executed.
 * @param query_id  Query on whose matches the system will run.
 * @param pFound_id The pointer to store the index of the existing system cache
 *
 * @return DT_true if already existing.
 * @return DT_false if it doesn't exist already.
 */
DT_bool SystemCacheIsAlreadyExisting(World *pWorld, FECS_SystemId system_id,
                                     FECS_QueryId query_id,
                                     FECS_SystemCacheId *pFound_id);
/**
 * Deletes the internals of given system cache.
 * Called via DT_ArrForEach_...
 *
 * @param pSystem_cache The system cache to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result SystemCacheDelete(DT_void *pSystem_cache, DT_void *_);
/**
 * Executes the given system cache.
 *
 * @param pWorld         World the system cache belongs to.
 * @param pSystem_cache System cache to execute.
 * @param pUser_data    The user given/managed data.
 */
DT_void SystemExec(World *pWorld, const SystemCache *pSystem_cache,
                   DT_void *pUser_data);

/* ----  LAYOUTS ---- */

/**
 * A layout is a container/manager for entities and their lifetimes. Entities
 * originate from a layout and end into it. No entity can be modified without an
 * established layout contract.
 */
typedef struct {
    FECS_BehaviorId behavior_id;
    /*
     * This stores pointer to each allocated chunk of memory where each chunk
     * holds 32 entities.
     */
    DT_Arr *pChunk_ptrs;
    /*
     * Each on bit tell that the chunk at that index has at least one slot
     * empty.
     */
    DT_Bitmap *pFree_chunk_idxs;
} Layout;

/**
 * NOTE: Since validating entity batches completely before the parsing of them
 * is very expensive, that check is done during the parsing and if invalidity is
 * found it is reported and we exit at that instant.
 */

/**
 * Creates a new layout into the world.
 *
 * @param pWorld      World to create into.
 * @param behavior_id Behavior that the layout derives from.
 * @param pLayout_id  The pointer to hold the index of the layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutCreate(World *pWorld, FECS_BehaviorId behavior_id,
                        FECS_LayoutId *pLayout_id);
/**
 * Deletes the internals of given layout.
 * Called via DT_ArrForEach_...
 *
 * @param pLayout The layout to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result LayoutDelete(DT_void *pLayout, DT_void *_);
/**
 * Checks if given system cache already exists.
 *
 * @param pWorld      World to check into.
 * @param behavior_id Behavior to check for.
 * @param pFound_id   The pointer to store the index of the existing layout.
 *
 * @return DT_true if already existing.
 * @return DT_false if it doesn't exist already.
 */
DT_bool LayoutIsAlreadyExisting(World *pWorld, FECS_BehaviorId behavior_id,
                                FECS_LayoutId *pFound_id);

/**
 * Spawns a new entity into the given layout.
 *
 * @param pWorld    World, the layout belongs to.
 * @param layout_id The layout to spawn entity from.
 * @param pEntity   The pointer to where the entity will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutSpawnEntity(World *pWorld, FECS_LayoutId layout_id,
                             FECS_Entity *pEntity);
/**
 * Spawns multiple new entities at once into the given layout.
 *
 * Will return a batch with less entities created if it fails mid allocation.
 *
 * @param pWorld       World, the layout belongs to.
 * @param layout_id    The layout to spawn entities from.
 * @param entity_count The number of entities to spawn.
 * @param ppBatch      The pointer to where the entities will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutSpawnEntities(World *pWorld, FECS_LayoutId layout_id,
                               DT_size entity_count,
                               FECS_EntityBatch **ppBatch);

/**
 * Checks if the given entity is valid.
 *
 * @param pworld  World, the entity belongs to.
 * @param entity  The entitiy to check.
 *
 * @return DT_true if valid.
 * @return DT_false if invalid.
 */
DT_bool LayoutIsEntityValid(World *pWorld, const FECS_Entity entity);
/**
 * Checks if the given entity batch is valid.
 *
 * @param pWorld World, the entities belongs to.
 * @param pBatch The entitiy batch to check.
 *
 * @return DT_true if valid.
 * @return DT_false if invalid.
 */
DT_bool LayoutAreEntitiesValid(World *pWorld, const FECS_EntityBatch *pBatch);

/**
 * Kills the given entity.
 *
 * @param pWorld  World, the entity belongs to.
 * @param entity The entitiy to kill.
 */
DT_void LayoutKillEntity(World *pWorld, FECS_Entity entity);
/**
 * Kills the given entity and nullifies the pointer.
 *
 * @param pWorld  World, the entities belongs to.
 * @param ppBatch The pointer to the entities to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entities is invalid INTERNALLY.
 */
PRP_Result LayoutKillEntities(World *pWorld, FECS_EntityBatch **ppBatch);

/**
 * Fetches the pointer to the specific component of an entity.
 *
 * @param pWorld     World the entity belongs to.
 * @param entity     The entity whose component to get.
 * @param comp_id    The component to fetch of the entity.
 * @param ppDest_ptr The pointer to the memory location of the component data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entity doesn't have the component.
 */
PRP_Result LayoutGetEntityComp(World *pWorld, const FECS_Entity entity,
                               FECS_CompId comp_id, DT_void **ppDest_ptr);
/**
 * Sets the value to the specific component of an entity.
 *
 * @param pWorld  World the entity belongs to.
 * @param entity  The entity whose component to set.
 * @param comp_id The component to set of the entity.
 * @param pData   The pointer to the value to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entity doesn't have the component.
 */
PRP_Result LayoutSetEntityComp(World *pWorld, FECS_Entity entity,
                               FECS_CompId comp_id, const DT_void *pData);
/**
 * Iterates over all entities of a batch.
 *
 * @param pWorld     World, the entities belongs to.
 * @param pBatch     The entities to operate on.
 * @param comp_id    The component to operate on.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if the entities don't have the component or the batch
 *                         is invalid.
 */
PRP_Result LayoutForEachEntities(World *pWorld, FECS_EntityBatch *pBatch,
                                 FECS_CompId comp_id,
                                 PRP_Result (*cb)(DT_void *pComp_data,
                                                  DT_void *pUser_data),
                                 DT_void *pUser_data);

#ifdef __cplusplus
}
#endif
