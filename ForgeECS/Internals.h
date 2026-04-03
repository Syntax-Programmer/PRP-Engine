#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"
#include "../Data-Types/DSArr.h"
#include "../Diagnostics/Assert.h"
#include "Defs.h"

/*
 * A general rule for any developer visiting this file:
 * In context of the world and the fecs ctx, anything that is not DT_DSArr is
 * one time define and never delete kind of definition(which is like most of
 * them).
 *
 * Only a world can be dynamically created and destroyed, hence the use of a
 * DT_DSArr.
 *
 *
 * One another note: The functions in this file will not prevent against
 * duplicate defs and it is the job of the fecs wrappers of themse functions to
 * explicitly check for the detection of duplicate.
 *
 * The functions in the file don't perform any checks to prevent any invalid
 * argument, the fecs wrapper also has to handle it. This file checks for array
 * internals invalidity only since checking them beforehand is costly, while
 * checking them as they are being processed is the better choice.
 */

/* ----  FECS ---- */

/**
 * The correct order of how things shall be created to make sure everything
 * works correctly is specified below.
 *
 * 1. Initialize the global context(g_ctx).
 * 2. Register the components.
 * 3. Register the behaviors.
 * 4. Register the queries.
 * 5. Register the systems.
 * 6. Turn on the schema lock.
 * 7. Start the world creation/deletion/whatever.
 */

/**
 * Contains only once defined definition of things as well as a manager for the
 * created worlds.
 */
typedef struct {
    /*
     * The below definitions can only be defined and not undefined later. So we
     * use struct macros to define all of them at once.
     */
    DT_Arr *comps;
    DT_Arr *behaviors;
    DT_Arr *queries;
    DT_Arr *systems;
    /*
     * This tells the fecs that the definition of the above arrays have
     * completed and now we will start making the worlds.
     * The fecs.c will enforce this lock.
     */
    DT_bool schema_lock;
    /*
     * Worlds can be dynamically added/removed or randomly referenced in code,
     * so storing it in a ds arr is worth it.
     */
    DT_DSArr *worlds;
} Context;

extern Context *g_ctx;

#define CTX_INVARIANT_EXPR                                                     \
    (g_ctx != DT_null && DT_ArrIsValid(g_ctx->comps) &&                        \
     DT_ArrIsValid(g_ctx->behaviors) && DT_ArrIsValid(g_ctx->queries) &&       \
     DT_ArrIsValid(g_ctx->systems) && DT_DSArrIsValid(g_ctx->worlds))
#define ASSERT_CTX_INVARIANT_EXPR                                              \
    DIAG_ASSERT_MSG(CTX_INVARIANT_EXPR,                                        \
                    "FECS is either not initialized or is corrupted.")

/* ----  WORLD ---- */

/**
 * A world is a runtime envirnoment of layouts, entities and things that
 * operate on said layouts and entities.
 */
typedef struct {
    DT_Arr *layouts;
    DT_Arr *system_caches;
} World;

#define WORLD_INVARIANT_EXPR(world)                                            \
    ((world) != DT_null && DT_ArrIsValid((world)->layouts) &&                  \
     DT_ArrIsValid((world)->system_caches))
#define ASSERT_WORLD_INVARIANT_EXPR(world)                                     \
    DIAG_ASSERT_MSG(WORLD_INVARIANT_EXPR(world),                               \
                    "The given world is either DT_null, or is corrupted.")

DT_DSId WorldCreate(DT_void);
DT_void WorldDelete(DT_DSId *pWorld_id);
PRP_Result WorldDeleteCb(DT_void *world);

DT_void WorldSystemExecAll(DT_DSId world_id, DT_void *user_data);
DT_void WorldSystemExecOne(DT_DSId world_id, DT_size system_cache_idx,
                           DT_void *user_data);
PRP_Result WorldSystemExecMany(DT_DSId world_id, DT_Arr *system_cache_idxs,
                               DT_void *user_data);
PRP_Result WorldUpdate(DT_DSId world_id, DT_f32 dt);
PRP_Result WorldSetSystemExecOrder(DT_DSId world_id, DT_Arr *system_exec_order);
PRP_Result WorldSync(DT_DSId world_id);
PRP_Result WorldEnableSystem(DT_DSId world_id, DT_size system_cache_idx);
PRP_Result WorldDisableSystem(DT_DSId world_id, DT_size system_cache_idx);

/* ----  COMPS ---- */

// Exceeding this limit will lead to name truncation.
#define COMP_NAME_MAX_SIZE (56)

/**
 * We only store the component metadata for memory allocation purposes, and then
 * we return the raw pointer to the user which we trust they will cast
 * correctly.
 */
typedef struct {
    DT_char name[COMP_NAME_MAX_SIZE];
    DT_size size;
} ComponentMetadata;

/*
 * NOTE: The name and size being a AOS type layout can cause locality issues but
 * we will deal with that later.
 */

PRP_Result CompGetLastErrCode(DT_void);
DT_size CompRegister(const DT_char *name, DT_size size);
DT_size CompIsRegistered(const DT_char *name);

/* ----  BEHAVIOR ---- */

/**
 * Runtime metadata of what components does the layout store internally. Along
 * with other metadata that aid in chunk creation and general access of
 * components.
 *
 * A world can hold a single layout for a given behavior def, and each world can
 * hold a layout of the same behavior.
 */
typedef struct {
    DT_Bitmap *set;
    // We know the number of comps beforehand.
    DT_size *strides;
    DT_size chunk_size;
} Behavior;

PRP_Result BehaviorGetLastErrCode(DT_void);
DT_size BehaviorRegister(DT_Arr *comp_idxs);
DT_size BehaviorIsRegistered(DT_Arr *comp_idxs);
PRP_Result BehaviorDelete(DT_void *behavior, DT_void *_);

/* ----  QUERY ---- */

typedef struct {
    DT_Bitmap *inc;
    // This will be null if we don't wan't to ex
    DT_Bitmap *exc;
    // This will be null if there is no match for the query.
    DT_Arr *behavior_matches;
} Query;

PRP_Result QueryGetLastErrCode(DT_void);
DT_size QueryRegister(DT_Arr *inc_comps, DT_Arr *exc_comps);
DT_size QueryIsRegistered(DT_Arr *inc_comps, DT_Arr *exc_comps);
PRP_Result QueryDelete(DT_void *query, DT_void *_);

/* ----  SYSTEMS ---- */

typedef struct {
    // If not enabled the system will not execute.
    DT_bool enabled;
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
     (system_cache)->system_idx < DT_ArrLenUnchecked(g_ctx->systems) &&        \
     (system_cache)->query_idx < DT_ArrLenUnchecked(g_ctx->queries) &&         \
     (system_cache)->layout_matches != DT_null &&                              \
     (system_cache)->system ==                                                 \
         (*(FECS_System *)DT_ArrGetUnchecked(g_ctx->systems,                   \
                                             (system_cache)->system_idx)))
#define ASSERT_SYSTEM_CACHE_INVARIANT_EXPR(system_cache)                       \
    DIAG_ASSERT_MSG(                                                           \
        SYSTEM_CACHE_INVARIANT_EXPR(system_cache),                             \
        "The given system cache is either DT_null, or is corrupted.")

PRP_Result SystemGetLastErrCode(DT_void);
DT_size SystemRegister(FECS_System system);
DT_size SystemIsRegistered(FECS_System system);
DT_size SystemCacheCreate(DT_DSId world_id, DT_size system_idx,
                          DT_size query_idx);
PRP_Result SystemCacheDelete(DT_void *system_cache, DT_void *_);
DT_void SystemExec(World *world, const SystemCache *system_cache,
                   DT_void *user_data);

/* ----  LAYOUTS ---- */

#define CHUNK_CAP (32)

/**
 * A storage unit of entities that holds 32 entities.
 */
typedef struct {
    DT_u32 gen[CHUNK_CAP];
    DT_u32 free_slot;
    DT_u8 mem[];
} Chunk;

/*
 * Cannot use DIAG_STATIC_ASSERT since including the Assert.h library will mess
 * with the set last error code handling. I think that's the case, I may be
 * wrong, too tired to double check rn.
 */
_Static_assert(CHUNK_CAP == sizeof(DT_u32) * 8,
               "free_slot bit width must match CHUNK_CAP");

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

PRP_Result LayoutGetLastErrCode(DT_void);
DT_size LayoutCreate(DT_DSId world_id, DT_size behavior_idx);
PRP_Result LayoutDelete(DT_void *layout, DT_void *_);
DT_size LayoutIsAlreadyExisting(DT_DSId world_id, DT_size behavior_idx);

FECS_Entity LayoutEntitySpawn(DT_DSId world_id, DT_size layout_idx);
FECS_EntityBatch *LayoutEntitySpawnN(DT_DSId world_id, DT_size layout_idx,
                                     DT_size count);

DT_bool LayoutEntityIsValid(DT_DSId world_id, const FECS_Entity entity);
DT_bool LayoutEntityBatchIsValid(DT_DSId world_id,
                                 const FECS_EntityBatch *entities);

DT_void LayoutEntityKill(DT_DSId world_id, FECS_Entity entity);
DT_void LayoutEntityKillN(DT_DSId world_id, FECS_EntityBatch *entities);

DT_void *LayoutEntityGetComp(DT_DSId world_id, const FECS_Entity entity,
                             DT_size comp_idx);
DT_void LayoutEntitySetComp(DT_DSId world_id, FECS_Entity entity,
                            DT_size comp_idx, const DT_void *data);
PRP_Result LayoutEntityBatchForEach(
    DT_DSId world_id, FECS_EntityBatch *entities, DT_size comp_idx,
    PRP_Result (*cb)(DT_void *comp_data, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
