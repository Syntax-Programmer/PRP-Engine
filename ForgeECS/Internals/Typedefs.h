#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/Bitmap.h"
#include "DataTypes/DSArr.h"
#include <string.h>

/* ----  VARIOUS IDS ---- */

typedef DT_size FECS_CompId;
typedef DT_size FECS_SystemId;

typedef DT_size FECS_LayoutId;
typedef DT_size FECS_SystemInstanceId;
typedef DT_DSId FECS_WorldId;

#define FECS_INVALID_ID ((DT_size)(-1))
/*
 * Maximum number of components that can be registered with FECS.
 *
 * FECS_Layout stores per-word prefix population counts of its component bitmap
 * in a DT_u16 array (FECS_Layout::pWord_prefix_popcnts). Since the largest
 * possible prefix count equals the maximum number of registered components,
 * this value must not exceed DT_U16_MAX.
 *
 * Keeping the component cap equal to DT_U16_MAX guarantees that every prefix
 * population count fits within DT_u16 without overflow.
 *
 * This is not a type/physical limit of the FECS, but more of a semantic limit
 * so that the layouts can work perfectly.
 */
#define FECS_COMPONENTS_MAX_CAP (DT_U16_MAX)

/* ----  ENTITIES ---- */

typedef struct {
    FECS_LayoutId layout_id;
    DT_size entity_idx;
    DT_u32 gen;
} FECS_EntityId;

typedef struct {
    FECS_LayoutId layout_id;
    DT_Arr *pChunk_views;
} FECS_EntityGroupId;

/* ----  SYSTEMS ---- */

typedef struct SystemData FECS_SystemExecInternalData;
typedef DT_u64 FECS_SystemExecOccupancyMask;
typedef DT_void (*FECS_SystemFunc)(
    const FECS_SystemExecInternalData *pExec_internals,
    FECS_SystemExecOccupancyMask occupancy_mask, DT_void *pUser_data);
/**
 * The user will use the idx provided to index into their component arrays
 * provided by the fetch function.
 */
#define FECS_SYSTEM_EXEC_FOREACH_OCCUPIED(occupancy_mask, idx)                 \
    while ((occupancy_mask) != 0 &&                                            \
           (((idx) = DT_BitwordFFS(occupancy_mask)), 1) &&                     \
           (((occupancy_mask) &= (occupancy_mask) - 1), 1))

#ifdef __cplusplus
}
#endif
