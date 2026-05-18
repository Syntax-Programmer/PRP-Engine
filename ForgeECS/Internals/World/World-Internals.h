#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Data-Types/Arr.h"
#include "Data-Types/Bitmap.h"
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
 * @param world World to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result WorldDeleteCb(DT_void *world);

/* ----  SYSTEMS ---- */

typedef struct {
    DT_bool is_enabled;
    /**
     * Caching the function pointer itself rather than the system_idx, this is
     * because the function pointers are stable so we can do it, they reduce the
     * indirection it takes.
     */
    FECS_System system;
    /*
     * Storing this too along side the system to maintain the identity of the
     * system we store with the g_ctx.
     */
    DT_size system_idx;
    /*
     * Same purpose as the system_idx, to link it to the g_ctx and world. Even
     * though we store the layout matches also.
     */
    DT_size query_idx;
    DT_Arr *layout_matches;
} SystemCache;

#define SYSTEM_CACHE_INVARIANT_EXPR(system_cache)                              \
    ((system_cache) != DT_null && (system_cache)->system != DT_null &&         \
     (system_cache)->system_idx < DT_ArrLen(g_ctx->systems) &&                 \
     (system_cache)->query_idx < DT_ArrLen(g_ctx->queries) &&                  \
     (system_cache)->layout_matches != DT_null &&                              \
     (system_cache)->system ==                                                 \
         (*(FECS_System *)DT_ArrGetUnchecked(g_ctx->systems,                   \
                                             (system_cache)->system_idx)))

/**
 * Creates a new system cache into the world.
 *
 * @param world      World to create into.
 * @param system_idx System that will be executed.
 * @param query_idx  Query on whose matches the system will run.
 * @param pIdx       The pointer to hold the index of the system cache.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result SystemCacheCreate(World *world, DT_size system_idx,
                             DT_size query_idx, DT_size *pIdx);
/**
 * Checks if given system cache already exists.
 *
 * @param world      World to check into.
 * @param system_idx System that will be executed.
 * @param query_idx  Query on whose matches the system will run.
 * @param pOut       The pointer to store the index of the existing system cache
 *
 * @return DT_true if already existing.
 * @return DT_false if it doesn't exist already.
 */
DT_bool SystemCacheIsAlreadyExisting(World *world, DT_size system_idx,
                                     DT_size query_idx, DT_size *pOut);
/**
 * Deletes the internals of given system cache.
 * Called via DT_ArrForEach_...
 *
 * @param system_cache The system cache to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result SystemCacheDelete(DT_void *system_cache, DT_void *_);
/**
 * Executes the given system cache.
 *
 * @param world        World the system cache belongs to.
 * @param system_cache System cache to execute.
 * @param user_data    The user given/managed data.
 */
DT_void SystemExec(World *world, const SystemCache *system_cache,
                   DT_void *user_data);

/* ----  LAYOUTS ---- */

/**
 * A layout is a container/manager for entities and their lifetimes. Entities
 * originate from a layout and end into it. No entity can be modified without an
 * established layout contract.
 */
typedef struct {
    DT_size behavior_idx;
    /*
     * This stores pointer to each allocated chunk of memory where each chunk
     * holds 32 entities.
     */
    DT_Arr *chunk_ptrs;
    /*
     * Each on bit tell that the chunk at that index has at least one slot
     * empty.
     */
    DT_Bitmap *free_chunks;
} Layout;

/**
 * NOTE: Since validating entity batches completely before the parsing of them
 * is very expensive, that check is done during the parsing and if invalidity is
 * found it is reported and we exit at that instant.
 */

/**
 * Creates a new layout into the world.
 *
 * @param world        World to create into.
 * @param behavior_idx Behavior that the layout derives from.
 * @param pIdx         The pointer to hold the index of the layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutCreate(World *world, DT_size behavior_idx, DT_size *pIdx);
/**
 * Deletes the internals of given layout.
 * Called via DT_ArrForEach_...
 *
 * @param layout The layout to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result LayoutDelete(DT_void *layout, DT_void *_);
/**
 * Checks if given system cache already exists.
 *
 * @param world        World to check into.
 * @param behavior_idx Behavior to check for.
 * @param pOut         The pointer to store the index of the existing layout.
 *
 * @return DT_true if already existing.
 * @return DT_false if it doesn't exist already.
 */
DT_bool LayoutIsAlreadyExisting(World *world, DT_size behavior_idx,
                                DT_size *pOut);

/**
 * Spawns a new entity into the given layout.
 *
 * @param world      World, the layout belongs to.
 * @param layout_idx The layout to spawn entity from.
 * @param pEntity    The pointer to where the entity will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutSpawnEntity(World *world, DT_size layout_idx,
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
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutSpawnEntities(World *world, DT_size layout_idx, DT_size count,
                               FECS_EntityBatch **pEntities);

/**
 * Checks if the given entity is valid.
 *
 * @param world  World, the entity belongs to.
 * @param entity The entitiy to check.
 *
 * @return DT_true if valid.
 * @return DT_false if invalid.
 */
DT_bool LayoutIsEntityValid(World *world, const FECS_Entity entity);
/**
 * Checks if the given entity batch is valid.
 *
 * @param world    World, the entities belongs to.
 * @param entities The entitiy batch to check.
 *
 * @return DT_true if valid.
 * @return DT_false if invalid.
 */
DT_bool LayoutAreEntitiesValid(World *world, const FECS_EntityBatch *entities);

/**
 * Kills the given entity.
 *
 * @param world  World, the entity belongs to.
 * @param entity The entitiy to kill.
 */
DT_void LayoutKillEntity(World *world, FECS_Entity entity);
/**
 * Kills the given entity and nullifies the pointer.
 *
 * @param world    World, the entities belongs to.
 * @param pEntities The pointer to the entities to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entities is invalid INTERNALLY.
 */
PRP_Result LayoutKillEntities(World *world, FECS_EntityBatch **pEntities);

/**
 * Fetches the pointer to the specific component of an entity.
 *
 * @param world    World the entity belongs to.
 * @param entity   The entity whose component to get.
 * @param comp_idx The component to fetch of the entity.
 * @param dest     The pointer to the memory location of the component data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entity doesn't have the component.
 */
PRP_Result LayoutGetEntityComp(World *world, const FECS_Entity entity,
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
 * @return PRP_ERR_INV_ARG if entity doesn't have the component.
 */
PRP_Result LayoutSetEntityComp(World *world, FECS_Entity entity,
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
 * @return PRP_ERR_INV_ARG if the entities don't have the component or the batch
 *                         is invalid.
 */
PRP_Result LayoutForEachEntities(World *world, FECS_EntityBatch *entities,
                                 DT_size comp_idx,
                                 PRP_Result (*cb)(DT_void *comp_data,
                                                  DT_void *user_data),
                                 DT_void *user_data);

#ifdef __cplusplus
}
#endif
