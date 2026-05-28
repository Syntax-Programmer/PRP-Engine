#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/DSArr.h"

/* ----  INDEX TYPEDEFS ---- */

typedef DT_size FECS_CompId;
typedef DT_size FECS_BehaviorId;
typedef DT_size FECS_QueryId;
typedef DT_size FECS_SystemId;

typedef DT_size FECS_SystemCacheId;
typedef DT_size FECS_LayoutId;

typedef DT_DSId FECS_WorldId;

/* ----  ENTITIES ---- */

/**
 * A singluar game object derived from internal data structures.
 *
 * Characteristics:
 * - Belongs to one layout(archetype) at a time.
 * - Is always unique at any given point in time.
 */
typedef struct {
    FECS_LayoutId layout_id;
    DT_size entity_idx;
    DT_u32 gen;
} FECS_Entity;

/**
 * A more compact and better way to create many entities.
 *
 * Characteristics:
 * - Belongs to one layout(archetype) at a time.
 * - Can contain any number of entities at a time(decided during creation).
 */
typedef struct {
    FECS_LayoutId layout_id;
    DT_Arr *chunk_views;
} FECS_EntityBatch;

/* ----  SYSTEM ---- */

/**
 * Passes along engine internal state to the caller system.
 */
typedef struct SystemData FECS_SystemData;

/**
 * A function that acts upon entities alive at the time of execution.
 */
typedef DT_void (*FECS_SystemFunc)(const FECS_SystemData *pSystem_data,
                                   DT_void *pUser_data);

#ifdef __cplusplus
}
#endif
