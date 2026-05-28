#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/Bitmap.h"
#include "DataTypes/DSArr.h"
#include "Diagnostics/Assert.h"
#include "ForgeECS/Defs.h"

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

/* ----  COMPS ---- */

// Exceeding this limit will lead to name truncation.
#define COMP_NAME_MAX_SIZE (56)

/**
 * We only store the component metadata for memory allocation purposes, and then
 * we return the raw pointer to the user which we trust they will cast
 * correctly.
 */
typedef struct {
    DT_size size;
    DT_char name[COMP_NAME_MAX_SIZE];
} ComponentMetadata;

/*
 * NOTE: The name and size being a AOS type layout can cause locality issues but
 * we will deal with that later.
 */

/**
 * Registers a new component into the FECS.
 *
 * @param name The name of the component.
 * @param size Size (in bytes) of the component.
 * @param pIdx The pointer to hold the index of the comp.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result CompRegister(const DT_char *name, DT_size size, DT_size *pIdx);
/**
 * Checks if given comp details are already taken.
 *
 * @param name The name of the component.
 * @param size Size (in bytes) of the component.
 *
 * @return DT_true if already registered.
 * @return DT_false if it doesn't exist already.
 */
DT_bool CompIsRegistered(const DT_char *name, DT_size *pOut);

/* ----  BEHAVIOR ---- */

#define CHUNK_CAP (32)

/**
 * A storage unit of entities that holds 32 entities.
 */
typedef struct {
    DT_u32 gens[CHUNK_CAP];
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

/**
 * Registers a new behavior into the FECS.
 *
 * @param comp_idxs  Array of comp behavior includes. **Array will be sorted.**
 * @param comp_count Len of the comp_idxs array.
 * @param pIdx       The pointer to hold the index of the behavior.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if comps in the given arrays are invalid.
 */
PRP_Result BehaviorRegister(DT_size *comp_idxs, DT_size comp_count,
                            DT_size *pIdx);
/**
 * Checks if given behavior already exists.
 *
 * @param comp_idxs  Array of comp behavior includes. **Array will be sorted.**
 * @param comp_count Len of the comp_idxs array.
 * @param pOut       The pointer to store the index of the existing behavior.
 *
 * @return DT_true if already registered.
 * @return DT_false if it doesn't exist already.
 */
DT_bool BehaviorIsRegistered(DT_size *comp_idxs, DT_size comp_count,
                             DT_size *pOut);
/**
 * Deletes the internals of given behavior.
 * Called via DT_ArrForEach_...
 *
 * @param behavior The behavior to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result BehaviorDelete(DT_void *behavior, DT_void *_);
/**
 * Fetches the stride of component in the given behavior.
 *
 * @param beahvior_idx The behavior to fetch stride from.
 * @param comp_idx     The comp to fetch stride of.
 *
 * @return PRP_INVALID_SIZE if comp doesn't exist in behavior.
 * @return The stride of the comp in the behavior otherwise.
 */
DT_size BehaviorGetCompStride(DT_size behavior_idx, DT_size comp_idx);

/* ----  QUERY ---- */

typedef struct {
    DT_Bitmap *inc;
    // This will be null if we don't wan't to exc
    DT_Bitmap *exc;
    DT_Arr *behavior_matches;
} Query;

/**
 * Registers a new query into the FECS.
 * Comps in inc_comps and exc_comps can't overlap.
 *
 * @param inc_comps       Array of comps query includes.
 * @param inc_comps_count Len of the inc_comps array.
 * @param exc_comps       Array of comps query excludes.
 * @param exc_comps_count Len of the exc_comps array.
 * @param pIdx            The pointer to hold the index of the query.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if comps in the given arrays are invalid.
 */
PRP_Result QueryRegister(const DT_size *inc_comps, DT_size inc_comps_count,
                         const DT_size *exc_comps, DT_size exc_comps_count,
                         DT_size *pIdx);
/**
 * Checks if given query already exists.
 *
 * @param inc_comps       Array of comps query includes.
 * @param inc_comps_count Len of the inc_comps array.
 * @param exc_comps       Array of comps query excludes.
 * @param exc_comps_count Len of the exc_comps array.
 * @param pOut            The pointer to store the index of the existing query.
 *
 * @return DT_true if already registered.
 * @return DT_false if it doesn't exist already.
 */
DT_bool QueryIsRegistered(const DT_size *inc_comps, DT_size inc_comps_count,
                          const DT_size *exc_comps, DT_size exc_comps_count,
                          DT_size *pOut);
/**
 * Cascades a newly created behavior into all the queries.
 * Called via DT_ArrForEach_...
 *
 * @param q The query instance.
 * @param b The behavior index to cascasde.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result QueryCascadeUpdateBehavior(DT_void *q, DT_void *b);
/**
 * Cleans up the queries if the cascading fails.
 * Called via DT_ArrForEach_...
 *
 * @param q The query instance.
 * @param b The behavior for which cleanup happens.
 *
 * @return PRP_OK on success.
 */
PRP_Result QueryCascadingErrorCleanup(DT_void *q, DT_void *b);
/**
 * Deletes the internals of given query.
 * Called via DT_ArrForEach_...
 *
 * @param query The query to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result QueryDelete(DT_void *query, DT_void *_);

/* ----  SYSTEMS ---- */

/**
 * Registers a new system into the FECS.
 *
 * @param system The system function to register.
 * @param pIdx   The pointer to hold the index of the system.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result SystemRegister(FECS_System system, DT_size *pIdx);
/**
 * Checks if given system func is already registered.
 *
 * @param system The system function to check for.
 * @param pOut   The pointer to store the index of the existing system.
 *
 * @return DT_true if already registered.
 * @return DT_false if it doesn't exist already.
 */
DT_bool SystemIsRegistered(FECS_System system, DT_size *pIdx);

#ifdef __cplusplus
}
#endif
