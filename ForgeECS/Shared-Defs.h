#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Core/IdMgr.h"
#include "../Data-Types/Typedefs.h"

/* ----  COMP ---- */

/**
 * This id also is a physical index into the component array to get that
 * comp_id's metadata.
 */
typedef DT_size FECS_CompId;
#define FECS_INVALID_COMP_ID ((FECS_CompId)(-1))

/* ----  LAYOUT ---- */

// Id of a single entity.
typedef struct {
    CORE_Id layout_id;
    DT_size chunk_i;
    DT_u8 slot;
    DT_u8 gen;
} FECS_EntityId;

// Data for the batch of entities defined to save memory.
typedef struct {
    DT_size chunk_i;
    DT_u8 slot;
    DT_u8 gen;
} FECS_EntityBatchData;

// Batch of entities created and destroyed at once.
typedef struct {
    CORE_Id layout_id;
    DT_size count;
    /*
     * Using AOS rather than SOA since we almost always use these three fields
     * together.
     */
    FECS_EntityBatchData entities[];
} FECS_EntityIdBatch;

/* ----  SYSTEM  ---- */

/**
 * A system function is a function that is executed by the engine on a set of
 * components that match a query.
 */
typedef DT_void (*FECS_SysFn)(DT_void *comp_arr, DT_size len,
                              DT_void *user_data, DT_u32 sys_data);

#ifdef __cplusplus
}
#endif
