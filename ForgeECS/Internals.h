#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"
#include "../Data-Types/DSArr.h"

/*
 * A general rule for any developer visiting this file:
 * In context of the world and the fecs ctx, anything that is not DT_DSArr is
 * one time define and never delete kind of definition(which is like most of
 * them).
 *
 * Only a world can be dynamically created and destroyed, hence the use of a
 * DT_DSArr.
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
DT_size CompRegister(DT_char *name, DT_size size);

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
    DT_u8 gen[CHUNK_CAP];
    DT_u32 free_slot;
    DT_u8 mem[];
} Chunk;

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

typedef struct {
    DT_u8 gen;
} EntityData;

typedef struct {
    DT_size layout_idx;
    /*
     * We encode the slot and chunk idx into a singular entity index. This
     * limits our total max entity cap of a layout to 2 ^ 69 to 2 ^ 64. A
     * massive fall of but still a wayy outside practical limits that can reach.
     *
     * The encode is as follows:
     * The 5 MSB encode the slot and the rest 59 bits encode the chunk index
     * inside the layout.
     */
    DT_size entity_idx;
} Entity;

/* ----  QUERY ---- */

typedef struct {
    DT_Bitmap *inc;
    DT_Bitmap *exc;
} Query;

/* ----  SYSTEMS ---- */

typedef struct {

} System;

typedef struct {
    DT_size system_idx;
    DT_size query_idx;
    DT_Arr *layout_matches;
} SystemCache;

/* ----  WORLD ---- */

/**
 * A world is a runtime envirnoment of layouts, entities and things that operate
 * on said layouts and entities.
 */
typedef struct {
    DT_Arr *layouts;
    /*
     * Entity ids point into this array,
     */
    DT_Arr *entities;
    DT_Arr *system_cache;
} World;

/* ----  FECS ---- */

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
     * Worlds can be dynamically added/removed or randomly referenced in code,
     * so storing it in a ds arr is worth it.
     */
    DT_DSArr *worlds;
    /*
     * This tells the fecs that the definition of the above arrays have
     * completed and now we will start making the worlds.
     * The fecs.c will enforce this lock.
     */
    DT_bool schema_lock;
} Context;

extern Context *g_ctx;

#ifdef __cplusplus
}
#endif
