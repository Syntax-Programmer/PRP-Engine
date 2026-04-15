#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../FECS-Internals.h"

/* ----  WORLDS ---- */

typedef struct {
    DT_Arr *layouts;
    DT_Arr *system_caches;
} World;

#define WORLD_INVARIANT_EXPR(world)                                            \
    ((world) != DT_null && DT_ArrIsValid((world)->layouts) &&                  \
     DT_ArrIsValid((world)->system_caches))

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

PRP_Result SystemCacheCreate(World *world, DT_size system_idx,
                             DT_size query_idx, DT_size *pIdx);
DT_bool SystemCacheIsAlreadyExisting(World *world, DT_size system_idx,
                                     DT_size query_idx, DT_size *pIdx);
PRP_Result SystemCacheDelete(DT_void *system_cache, DT_void *_);
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

PRP_Result LayoutCreate(World *world, DT_size behavior_idx, DT_size *pIdx);
PRP_Result LayoutDelete(DT_void *layout, DT_void *_);
DT_bool LayoutIsAlreadyExisting(World *world, DT_size behavior_idx,
                                DT_size *pIdx);

PRP_Result LayoutSpawnEntity(World *world, DT_size layout_idx,
                             FECS_Entity *pEntity);
PRP_Result LayoutSpawnEntities(World *world, DT_size layout_idx, DT_size count,
                               FECS_EntityBatch **pEntities);

DT_bool LayoutIsEntityValid(World *world, const FECS_Entity entity);
DT_bool LayoutAreEntitiesValid(World *world, const FECS_EntityBatch *entities);

DT_void LayoutKillEntity(World *world, FECS_Entity entity);
DT_void LayoutKillEntities(World *world, FECS_EntityBatch *entities);

PRP_Result LayoutGetEntityComp(World *world, const FECS_Entity entity,
                               DT_size comp_idx, DT_void **dest);
PRP_Result LayoutSetEntityComp(World *world, FECS_Entity entity,
                               DT_size comp_idx, const DT_void *data);
PRP_Result LayoutForEachEntities(World *world, FECS_EntityBatch *entities,
                                 DT_size comp_idx,
                                 PRP_Result (*cb)(DT_void *comp_data,
                                                  DT_void *user_data),
                                 DT_void *user_data);

#ifdef __cplusplus
}
#endif
