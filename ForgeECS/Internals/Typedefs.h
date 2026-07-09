#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/DSArr.h"
#include "DataTypes/Typedefs.h"
#include <string.h>

/* ----  VARIOUS IDS ---- */

typedef DT_size FECS_CompId;
typedef DT_size FECS_SystemId;

typedef DT_size FECS_LayoutId;
typedef DT_size FECS_SystemInstanceId;
typedef DT_DSId FECS_WorldId;

#define FECS_INVALID_ID ((DT_size)(-1))

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
typedef DT_void (*FECS_SystemFunc)(
    const FECS_SystemExecInternalData *pInternal_data, DT_void *pUser_data);

/* ----  STR CMP ---- */

/*
 * It shall be allocated with the
 * sizeof(FECS_String) + (len_of_str * sizeof(DT_char)).
 *
 * And the string shall be added to the pVal field.
 *
 * No Nul-terminator-byte is needed in this string impl.
 */
typedef struct {
    DT_size len;
    // Need not be nul-terminated, no need for it.
    DT_char pVal[];
} FECS_String;

#ifdef __cplusplus
}
#endif
