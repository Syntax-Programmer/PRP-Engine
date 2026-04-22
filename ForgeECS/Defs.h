#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "../Data-Types/Bitmap.h"

/* ----  ENTITIES ---- */

typedef struct {
    DT_size layout_idx;
    DT_size entity_idx;
    DT_u32 gen;
} FECS_Entity;

typedef struct {
    DT_size chunk_idx;
    DT_u32 mask;
    DT_u32 gens[32];
} ChunkView;

typedef struct {
    DT_size layout_idx;
    DT_Arr *chunks;
} FECS_EntityBatch;

/* ----  SYSTEM ---- */

typedef struct SystemData FECS_SystemData;

typedef DT_void (*FECS_System)(const FECS_SystemData *system_data,
                               DT_void *user_data);

#define FECS_SYSTEM_LOOP(free_chunk_slots) while ((free_chunk_slots))
#define FECS_SYSTEM_GET_ENTITY_I(system_data, idx)                             \
    do {                                                                       \
        DT_Bitword mask = (system_data) & -(system_data);                      \
        (idx) = DT_BitwordCTZ(mask);                                           \
        (system_data) ^= mask;                                                 \
    } while (0);

#ifdef __cplusplus
}
#endif
