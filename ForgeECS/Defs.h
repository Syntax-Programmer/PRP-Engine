#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"

/* ----  ENTITIES ---- */

/**
 * A singluar game object derived from internal data structures.
 *
 * Characteristics:
 * - Belongs to one layout(archetype) at a time.
 * - Is always unique at any given point in time.
 */
typedef struct {
    DT_size layout_idx;
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
    DT_size layout_idx;
    DT_Arr *chunks;
} FECS_EntityBatch;

/* ----  SYSTEM ---- */

/**
 * Passes along engine internal state to the caller system.
 */
typedef struct SystemData FECS_SystemData;

/**
 * A function that acts upon entities alive at the time of execution.
 */
typedef DT_void (*FECS_System)(const FECS_SystemData *system_data,
                               DT_void *user_data);

#ifdef __cplusplus
}
#endif
