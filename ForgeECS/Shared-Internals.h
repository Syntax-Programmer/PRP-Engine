#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"
#include "../Utils/Logger.h"
#include "Shared-Defs.h"

/**
 * This file contains the shared internals amongst every file.
 * This is done because in different file the include structure becomes very
 * messy and hard to handle. So a shared internal header with all the
 * definitions and functions is a convenient solution.
 *
 * Functions here are all internals. But some of the functions that need to be
 * public are wrapped by the FECS.c/h for public, performing extra bookkeeping
 * to ensure all the modules work perfectly together.
 */

/* ----  COMP ---- */

#define COMP_ID_VALIDITY_CHECK(comp_id, ret)                                   \
    do {                                                                       \
        if (comp_id >= DT_ArrLen(g_state->comp_registry.comp_sizes)) {         \
            PRP_LOG_FN_INV_ARG_ERROR(comp_id);                                 \
            return ret;                                                        \
        }                                                                      \
    } while (0)

/**
 * Registers the component of the given size to the FECS.
 *
 * @param comp_size: The size the comp struct is going to be.
 *
 * @return FECS_INVALID_COMP_ID if the comp_size=0 or we can't create any more
 * components, otherwise the componenet id of the registerted component
 */
FECS_CompId CompRegister(DT_size comp_size);

/* ----  BEHAVIOR SET ---- */

// Behavior set is just a bitmap of the comp ids added together.

/**
 * Creates an empty behavior set and returns an id to its.
 *
 * @return The id of the behavior set.
 */
CORE_Id BehaviorSetCreate(DT_void);
/**
 * Deletes the behavior set and invalidates the original CORE_Id * to
 * CORE_INVALID_ID to prevent use after free bugs.
 *
 * @param pB_set_id: The pointer to the id of the behavior set to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pB_set_id is DT_null or the id it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FnCode BehaviorSetDelete(CORE_Id *pB_set_id);
/**
 * Empties the behavior set the given id is linked to.
 *
 * #param b_set_id; The id to the behavior set to empty,
 *
 * @return PRP_FN_INV_ARG_ERROR id the given id is invalid in some way,
 * otherwise the PRP_FN_SUCCESS.
 */
PRP_FnCode BehaviorSetClear(CORE_Id b_set_id);
/**
 * Attaches the given component id to the behavior set the given b_set_id
 * points to.
 *
 * @param b_set_id: The id to the behavior set to attach the component to.
 * @param comp_id: The component id to attach to the behavior set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if the behavior_set cannot accommodate anymore
 * comp id, otherwise PRP_FN_SUCCESS.
 */
PRP_FnCode BehaviorSetAttachComp(CORE_Id b_set_id, FECS_CompId comp_id);
/**
 * Detaches the given component id from the behavior set the given b_set_id
 * points to.
 *
 * @param b_set_id: The id to the behavior set to detach the component from.
 * @param comp_id: The component id to detach from the behavior set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FnCode BehaviorSetDetachComp(CORE_Id b_set_id, FECS_CompId comp_id);
/**
 * Checks if the given component id is present in the behavior set the b_set_id
 * points to.
 *
 * @param b_set_id: The id to the behavior set to check for.
 * @param comp_id: The component id to check for in the behavior set.
 * @param pRslt: The pointer to the variable where the boolean result will be
 * stored.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FnCode BehaviorSetHasComp(CORE_Id b_set_id, FECS_CompId comp_id,
                              DT_bool *pRslt);
/**
 * Callback for the CORE_IdMgr the behavior sets belong to so that the IdMgr can
 * free data.
 *
 * @param data: The pointer to the data the IdMgr gives to us to free.
 *
 * @return Ideally should always return PRP_FN_SUCCESS, unless some internal
 * corruption happened,
 */
PRP_FnCode BehaviorSetDelCb(DT_void *pB_set);

/* ----  LAYOUT ---- */

#define CHUNK_CAP (32)

/*
 * Using chunk metadata per chunk allows for better locality of all the data
 * that the is needed for the entity creation.
 */
typedef struct {
    /*
     * Tracks the gen of each slot to prevent stale references. It is also
     * pretty unlikely that the number of gens reach u8_MAX, if that happens we
     * rollback to gen 0. And if you still have stale reference if gen 0, its
     * your fault at that point.
     */
    DT_u8 gens[CHUNK_CAP];
    // Can use ctz or ffs or similar to find free slot quickly.
    DT_u32 free_slot;
    // Mem chunk of size equal to CHUNK_CAP entities' component datas.
    DT_u8 data[];
} Chunk;

/**
 * Layout are containers for entities. Created with behavior sets defining the
 * behavior of the entities within the layout.
 */
typedef struct {
    /*
     * A deep copy of the behavior set the layout is derieved from.
     * We make a deep copy to prevent any chance of user modifying anything
     * about the layout.
     */
    DT_Bitmap *b_set;
    /*
     * Since the number of comps is decided at create time, the size of
     * comp_arr_stries will always be fixed and hence doesn't need DT_Arr which
     * is dynamic.
     */
    DT_size *comp_arr_strides;
    /*
     * Each on bits represents a chunk that has a free entity in it and is
     * available to use.
     */
    DT_Bitmap *free_chunks;
    /*
     * Each chunk entry contains CHUNK_CAP number of entity slot. Where
     * an entity slot is just the space of the each of the components attached
     * to it. Along with it each chunk contains a metadata header that tell
     * about the gen of each slot in the bitset and the currently free slots in
     * the chunk.
     *
     * We store in this array only the reference to the chunk. This is to
     * prevent hundreds of mbs being reallocated if the situations comes to it
     * during array growing.
     */
    DT_Arr *chunk_ptrs;
    /*
     * sizeof(Chunk) + (CHUNK_CAP * total_comp_size_of_one_entity), used to
     * alloc new chunks.
     */
    DT_size chunk_size;
} Layout;

/**
 * Creates an empty layout with the user specified behavior set and returns an
 * id to it.
 *
 * @param b_set_id : The id of the behavior set that determines the layout's
 * entities' behavior.
 *
 * @return The id of layout created.
 */
CORE_Id LayoutCreate(CORE_Id b_set_id, DT_bool *pIsDuplicate);
/**
 * Deletes the layout and invalidates the original CORE_Id * to
 * CORE_INVALID_ID to prevent use after free bugs.
 *
 * @param pLayout_id: The pointer to the id of the layout to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pLayout_id is DT_null or the id it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FnCode LayoutDelete(CORE_Id *pLayout_id);
/**
 * Callback for the CORE_IdMgr the layout belong to so that the IdMgr can
 * free data.
 *
 * @param layout: The pointer to the layout the IdMgr gives to us to free.
 *
 * @return Ideally should always return PRP_FN_SUCCESS, unless some internal
 * corruption happened,
 */
PRP_FnCode LayoutDelCb(DT_void *layout);

/**
 * Create a new entity in the layout.
 *
 * @param layout_id: The id of the layout from which to create the entity.
 * @param entity_id: The storage for the new entity.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_MALLOC_ERROR/PRP_FN_RES_EXHAUSTED_ERROR if the layout can't allocate
 * any more entities, otherwise PRP_FN_SUCCESS.
 */
PRP_FnCode LayoutCreateEntity(CORE_Id layout_id, FECS_EntityId *entity_id);
/**
 * Deletes the entity corresponding to the given id.
 *
 * @param entity_id: The id of the entity to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the entity id is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FnCode LayoutDeleteEntity(FECS_EntityId *entity_id);
/**
 * Creates a batch of entities in the layout.
 *
 * @param layout_id: The id of the layout from which to create the entities.
 * @param count: The number of entities to create.
 *
 * @return DT_NULL if the parameter is invalid in any way, otherwise a pointer
 * to the created entity batch.
 *
 * @note If count number of slots cannot be allocated, but some entities are
 * already allocated, the function will return a batch with only the allocated
 * entities.
 */
FECS_EntityIdBatch *LayoutCreateEntityBatch(CORE_Id layout_id, DT_size count);
/**
 * Deletes the entity batch and sets the original FECS_EntityIdBatch * to
 * DT_null to prevent use after free bugs.
 *
 * @param pEntity_batch: The pointer to the entity batch pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pEntity_batch is DT_null or the entity batch
 * it points to is invalid, otherwise it returns PRP_FN_SUCCESS.
 *
 * @note If any entity in the batch is deemed invalid, the function will just
 * skip over it. LIKE A BOSS.
 */
PRP_FnCode LayoutDeleteEntityBatch(FECS_EntityIdBatch **pEntity_batch);
/**
 * Provides the comp id's data of the entity to the fn for usage.
 *
 * @param entity_id: The entity id to operate on.
 * @param comp_id: The comp id to operate on.
 * @param fn: The function that will be called with the comp id's data.
 * @param user_data: The user data to pass to the function.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FnCode LayoutEntityOperateComp(FECS_EntityId entity_id, FECS_CompId comp_id,
                                   PRP_FnCode (*fn)(DT_void *data,
                                                    DT_void *user_data),
                                   DT_void *user_data);
/**
 * Provides the comp id's data of the each entity in the entity batch to the fn
 * for usage.
 *
 * @param entity_batch: The entity batch to operate on.
 * @param comp_id: The comp id to operate on.
 * @param fn: The function that will be called with the comp id's data.
 * @param user_data: The user data to pass to the function.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FnCode LayoutEntityBatchOperateComp(
    FECS_EntityIdBatch *entity_batch, FECS_CompId comp_id,
    PRP_FnCode (*fn)(DT_void *data, DT_void *user_data), DT_void *user_data);

/* ----  QUERY  ---- */

typedef struct {
    DT_Bitmap *include_comps;
    DT_Bitmap *exclude_comps;
    DT_Arr *layout_matches;
} Query;

/**
 * Creates a query with the user specified behavior set and returns an id to
 * it.
 *
 * @param exclude_b_set_id : The set of components to avoid in layout filtering,
 * if any match is found the layout is excluded.
 * @param include_b_set_id : The set of components to include in layout
 * filtering, if all the components are found in a layout, the layout is
 * included.
 *
 * @return The id of query created.
 */
CORE_Id QueryCreate(CORE_Id exclude_b_set_id, CORE_Id include_b_set_id);
/**
 * Deletes the layout and invalidates the original CORE_Id * to
 * CORE_INVALID_ID to prevent use after free bugs.
 *
 * @param pQuery_id: The pointer to the id of the query to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pQuery_id is DT_null or the id it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FnCode QueryDelete(CORE_Id *pQuery_id);

/**
 * Callback for the CORE_IdMgr the layout belong to so that the IdMgr can
 * free data.
 *
 * @param query: The pointer to the query the IdMgr gives to us to free.
 *
 * @return Ideally should always return PRP_FN_SUCCESS, unless some internal
 * corruption happened,
 */
PRP_FnCode QueryDelCb(DT_void *query);
/**
 * Updates every query to also account for the newly created layout.
 *
 * @param layout_id: The id of the layout to cascade.
 *
 * @return PRP_FN_INV_ARG_ERROR if layout_id is invalid, otherwise it returns
 * PRP_FN_SUCCESS.
 */
PRP_FnCode QueryCascadeLayoutCreate(CORE_Id layout_id);
/**
 * Updates every query to also account for the deleted layout.
 *
 * @param layout_id: The id of the layout to cascade.
 *
 * @return PRP_FN_INV_ARG_ERROR if layout_id is invalid, otherwise it returns
 * PRP_FN_SUCCESS.
 */
PRP_FnCode QueryCascadeLayoutDelete(CORE_Id layout_id);

/* ----  SYSTEM  ---- */

typedef struct {
    DT_void *user_data;
    FECS_SysFn fn;
    CORE_Id query_id;
} System;

/**
 * Creates the system user specified function and the query it applies to.
 *
 * @param query_id: The id of the query the system will apply to.
 * @param fn: The function that will be executed by the system.
 * @param user_data: The user data that will be passed to the function.
 *
 * @return The id of the created system.
 */
CORE_Id SystemCreate(CORE_Id query_id, FECS_SysFn fn, DT_void *user_data);
/**
 * Deletes the system and invalidates the original CORE_Id * to CORE_INVALID_ID
 * to prevent use after free bugs.
 *
 * @param pSystem_id: The pointer to the id of the system to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pSystem_id is DT_null or the id it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FnCode SystemDelete(CORE_Id *pSystem_id);
/**
 * Executes the system with the given id.
 *
 * @param system_id: The id of the system to execute.
 *
 * @return PRP_FN_INV_ARG_ERROR if system_id is invalid, otherwise it returns
 * PRP_FN_SUCCESS.
 */
PRP_FnCode SystemExec(CORE_Id system_id);

/* ----  STATE  ---- */

typedef struct {
    /*
     * We don't use id_mgr for comp_ids as they will prevent us to use bitmaps
     * to represent comp_sets. A comp_set is a set of components that define a
     * behaviour.
     * Using a bitmap will just be very mem lean and allow us to very quickly
     * compare comp sets.
     *
     * But this also hinders our ability to delete a comp_id after it is
     * registered. But that is a valid tradeoff since it is very very uncommon
     * to unregister a comp.
     */
    struct {
        DT_Arr *comp_sizes;
    } comp_registry;
    /*
     * These represent a set of components added together to define a behavior.
     * These are entirely user owned and when we use them to create a layout a
     * deep copy is stored in the layout for safety.
     */
    CORE_IdMgr *b_set_id_mgr;
    /*
     * Layouts are unique templates that a entity can be created from. These
     * templates are used by the caller to create entities with specific
     * behavior the layout defines.
     */
    CORE_IdMgr *layout_id_mgr;
    /*
     * The queries are filters that find specific layout ids that match some
     * conditions. Using these filter can help us execute specific functions on
     * a set of entities.
     */
    CORE_IdMgr *query_id_mgr;
    /*
     * Systems are the functions that will executed on the layouts that its
     * queries filter. They most likely execute per frame.
     */
    CORE_IdMgr *system_id_mgr;
} ECSState;

extern ECSState *g_state;

#ifdef __cplusplus
}
#endif
