#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"
#include "../Data-Types/DSArr.h"
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
 */

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
DT_size BehaviorRegisterWArray(DT_size *comp_idxs, DT_size len);
DT_size BehaviorRegisterWDTArr(DT_Arr *comp_idxs);
DT_void BehaviorDelete(Behavior *behavior);

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
DT_void LayoutDelete(Layout *layout);

/* ----  ENTITIES ---- */

FECS_Entity EntitySpawn(DT_DSId world_id, DT_size layout_idx);
FECS_EntityBatch *EntitySpawnN(DT_DSId world_id, DT_size layout_idx,
                               DT_size count);

DT_bool EntityIsValid(DT_DSId world_id, const FECS_Entity entity);
DT_bool EntityBatchIsValid(DT_DSId world_id, const FECS_EntityBatch *entities);

DT_void EntityKill(DT_DSId world_id, FECS_Entity entity);
DT_void EntityKillN(DT_DSId world_id, FECS_EntityBatch *entities);

DT_void *EntityGetComp(DT_DSId world_id, const FECS_Entity entity,
                       DT_size comp_idx);
DT_void EntitySetComp(DT_DSId world_id, FECS_Entity entity, DT_size comp_idx,
                      const DT_void *data);
PRP_Result EntityBatchForEach(DT_DSId world_id, FECS_EntityBatch *entities,
                              DT_size comp_idx,
                              PRP_Result (*cb)(DT_void *comp_data,
                                               DT_void *user_data),
                              DT_void *user_data);

/* ----  QUERY ---- */

typedef struct {
    DT_Bitmap *inc;
    // This will be null if we don't wan't to ex
    DT_Bitmap *exc;
    // This will be null if there is no match for the query.
    DT_Arr *behavior_matches;
} Query;

PRP_Result QueryGetLastErrCode(DT_void);
DT_size QueryRegisterWArray(DT_size *inc_comps, DT_size inc_len,
                            DT_size *exc_comps, DT_size exc_len);
DT_size QueryRegisterWArr(DT_Arr *inc_comps, DT_Arr *exc_comps);
DT_void QueryDelete(Query *query);

/* ----  SYSTEMS ---- */

typedef struct {
    DT_size system_idx;
    DT_size query_idx;
    DT_Arr *layout_matches;
} SystemCache;

DT_size SystemRegister(FECS_System system);
DT_size SystemCacheCreate(DT_DSId world_id, DT_size system_idx,
                          DT_size query_idx);
DT_void SystemCacheDelete(SystemCache *system_cache);

/* ----  WORLD ---- */

/**
 * A world is a runtime envirnoment of layouts, entities and things that
 * operate on said layouts and entities.
 */
typedef struct {
    DT_Arr *layouts;
    DT_Arr *system_caches;
} World;

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

#ifdef __cplusplus
}
#endif
