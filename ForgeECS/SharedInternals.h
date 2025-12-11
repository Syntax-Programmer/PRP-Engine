#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Core/IdMgr.h"
#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"
#include "../Utils/Defs.h"

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

/*
 * This id also is a physical index into the component array to get that
 * comp_id's metadata.
 */
typedef DT_size FECS_CompId;
#define FECS_INVALID_COMP_ID ((FECS_CompId)(-1))

#define COMP_ID_VALIDITY_CHECK(comp_id, ret)

FECS_CompId CompIdRegister(DT_size comp_size);

/* ----  BEHAVIOR SET ---- */

typedef DT_Bitmap *FECS_BehaviorSet;

CORE_Id BehaviorSetCreate(DT_void);
PRP_FnCode BehavoirSetDelete(FECS_BehaviorSet **pB_set);
PRP_FnCode BehaviorSetClear(FECS_BehaviorSet *b_set);
PRP_FnCode BehaviorSetAttachComp(FECS_BehaviorSet *b_set, FECS_CompId comp_id);
PRP_FnCode BehaviorSetDetachComp(FECS_BehaviorSet *b_set, FECS_CompId comp_id);
PRP_FnCode BehaviorSetHasComp(FECS_BehaviorSet *b_set, FECS_CompId comp_id,
                              DT_bool *pRslt);

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
     * Since the number of comps is decided at create time, the size of
     * comp_arr_stries will always be fixed and hence doesn't need DT_Arr which
     * is dynamic.
     */
    DT_size *comp_arr_strides;
    /*
     * This is equivalent to the number of components of the layout. We have
     * this data as the set count of the comp_set, but still stored here for
     * convenient local access.
     */
    DT_size comp_count;
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

/* ----  ENTITY  ---- */

typedef struct {
    CORE_Id layout_id;
    /*
     * Bit 0-31: chunk_i
     * Bit 32-39: slot
     * Bit 40-47: gen
     */
    DT_u64 internal_id;
} FECS_EntityId;

/* ----  QUERY  ---- */

typedef struct {
    DT_Bitmap *include_comps;
    DT_Bitmap *exclude_comps;
    DT_Arr *layout_matches;
} Query;

/* ----  SYSTEM  ---- */

typedef DT_void (*FECS_SysFn)(DT_void *comp_arr, DT_size len,
                              DT_void *user_data, DT_u32 sys_data);

typedef struct {
    DT_void *user_data;
    FECS_SysFn fn;
    CORE_Id query_id;
} System;

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
    CORE_IdMgr *behavior_sets;
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
} State;

#ifdef __cplusplus
}
#endif
