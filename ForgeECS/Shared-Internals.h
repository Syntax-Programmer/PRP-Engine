#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Core/IdMgr.h"
#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"
#include "../Utils/Logger.h"

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

/**
 * This id also is a physical index into the component array to get that
 * comp_id's metadata.
 */
typedef DT_size FECS_CompId;
#define FECS_INVALID_COMP_ID ((FECS_CompId)(-1))

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

CORE_Id LayoutCreate(CORE_Id b_set_id);
PRP_FnCode LayoutDelete(CORE_Id *pLayout_id);

PRP_FnCode LayoutDelCb(DT_void *layout);
// DT_u64 LayoutGetSlot(CORE_Id layout_id);
// PRP_FnCode LayoutFreeSlot(CORE_Id layout_id, DT_u64 entity_id);
// PRP_FnCode LayoutIsEntityIdValid(CORE_Id layout_id, DT_u64 entity_id);

/* ----  ENTITY  ---- */

// typedef struct {
//     CORE_Id layout_id;
//     DT_size chunk_i;
//     DT_u8 slot;
//     DT_u8 gen;
// } FECS_EntityId;

/* ----  QUERY  ---- */

typedef struct {
    DT_Bitmap *include_comps;
    DT_Bitmap *exclude_comps;
    DT_Arr *layout_matches;
} Query;

CORE_Id QueryCreate(CORE_Id exclude_b_set_id, CORE_Id include_b_set_id);
PRP_FnCode QueryDelete(CORE_Id *pQuery_id);

PRP_FnCode QueryDelCb(DT_void *query);
PRP_FnCode QueryCascadeLayoutCreate(CORE_Id layout_id);
PRP_FnCode QueryCascadeLayoutDelete(CORE_Id layout_id);

/* ----  SYSTEM  ---- */

/**
 * A system function is a function that is executed by the engine on a set of
 * components that match a query.
 */
typedef DT_void (*FECS_SysFn)(DT_void *comp_arr, DT_size len,
                              DT_void *user_data, DT_u32 sys_data);

typedef struct {
    DT_void *user_data;
    FECS_SysFn fn;
    CORE_Id query_id;
} System;

/**
 * Thes macros abstract the method of iteration over the entities of matching
 * the query of the system.
 */
#define FECS_SYS_DATA_LOOP(sys_data) while ((sys_data))
#define FECS_SYS_GET_I(sys_data, i)                                            \
    do {                                                                       \
        DT_Bitword mask = (sys_data) & -(sys_data);                            \
        (i) = DT_BitwordCtz(mask);                                             \
        sys_data ^= mask;                                                      \
    } while (0);

/**
 * Creates the system user specified function and the query it applies to.
 *
 * @param query_id: The id of the query the system will apply to.
 * @param func: The function that will be executed by the system.
 * @param user_data: The user data that will be passed to the function.
 *
 * @return The id of the created system.
 */
CORE_Id SystemCreate(CORE_Id query_id, FECS_SysFn func, DT_void *user_data);
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
