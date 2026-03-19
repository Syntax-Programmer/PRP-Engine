#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"

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

#ifdef __cplusplus
}
#endif
