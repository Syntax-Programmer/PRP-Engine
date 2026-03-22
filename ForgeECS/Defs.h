#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Bitmap.h"
#include "../Data-Types/Typedefs.h"

/* ----  ENTITIES ---- */

typedef struct {
    /*
     * We encode the slot and chunk idx into a singular entity index. This
     * limits our total max entity cap of a layout to 2 ^ 69 to 2 ^ 64. A
     * massive fall of but still a wayy outside practical limits that can reach.
     *
     * The encode is as follows:
     * The 5 LSB encode the slot and the rest 59 bits encode the chunk index
     * inside the layout.
     */
    DT_size entity_idx;
    DT_u32 gen;
} EntityData;

typedef struct {
    DT_size layout_idx;
    EntityData data;
} FECS_Entity;

typedef struct {
    DT_size layout_idx;
    DT_size count;
    EntityData data[];
} FECS_EntityBatch;

/* ----  SYSTEM ---- */

typedef DT_void (*FECS_System)(DT_void **comp_arr, DT_size len,
                               DT_void *user_data, DT_u32 system_data);

#define FECS_SYSTEM_LOOP(system_data) while ((system_data))
#define FECS_SYSTEM_GET_ENTITY_I(system_data, idx)                             \
    do {                                                                       \
        DT_Bitword mask = (system_data) & -(system_data);                      \
        (idx) = DT_BitwordCTZ(mask);                                           \
        (system_data) ^= mask;                                                   \
    } while (0);

#ifdef __cplusplus

#endif
