#include "../Diagnostics/Assert.h"
#include "Internals.h"
#include <string.h>

/* ----  LAYOUTS ---- */

/**
 * Deletes the created chunks inside layout.chunk_ptr through DT_ArrForEach.
 *
 * @param ptr: The Chunk** that we will be given.
 * @param user_data: Not imp param, will not be used.
 *
 * @return Should return PRP_OK, return type exists to satisfy the foreach
 * requirement.
 */
static PRP_Result ChunkPtrDelCb(DT_void *ptr, DT_void *user_data);
/**
 * Creates a new chunk for the given layout.
 *
 * @param layout: The layout in which to create a new chunk.
 *
 * @return PRP_ERR_OOM if chunk allocation fails,
 * PRP_ERR_OOM/PRP_ERR_RES_EXHAUSTED if we can't push any more chunks, PRP_OOM
 * if we can't handle metadata for the chunk(free_chunks).
 */
static PRP_Result ChunkCreate(Layout *layout);
/**
 * Initializes a new layout.
 *
 * @param layout: The layout to initialize.
 * @param behavior_idx: The behavior the layout is supposed to be derieved from.
 *
 * @return The err codes propogated by internal function call, otherwise PRP_OK.
 */
static PRP_Result LayoutInitialize(Layout *layout, DT_size behavior_idx);

PRP_Result LayoutGetLastErrCode(DT_void) { return last_err_code; }

static PRP_Result ChunkCreate(Layout *layout) {
    DIAG_ASSERT(layout != DT_null);

    Behavior *behavior =
        DT_ArrGetUnchecked(g_ctx->behaviors, layout->behavior_idx);
    Chunk *chunk = malloc(behavior->chunk_size);
    if (!chunk) {
        return PRP_ERR_OOM;
    }

    DT_size push_idx = DT_ArrLenUnchecked(layout->chunk_ptrs);
    PRP_Result code = DT_ArrPushUnchecked(layout->chunk_ptrs, &chunk);
    if (code != PRP_OK) {
        free(chunk);
        return code;
    }

    /*
     * Sets all the gens to u8 max. And the free_slot's all the bits to 1.
     * Essentially initializing in a single optimized call instead of manual
     * assigning.
     *
     * This also means starting gen of any entity is 255, not zero which is
     * fine since int wrap around is permitted.
     *
     *  We can use sizeof(Chunk) inthis bcuz the chunk data is a flex array memb
     * and doesn't count in the size of struct.
     */
    memset(chunk, 0XFF, sizeof(Chunk));
    DT_size bit_cap = DT_BitmapBitCapUnchecked(layout->free_chunks);
    if (push_idx >= bit_cap) {
        DT_size new_bit_cap;
        if (DT_BITMAP_MAX_BIT_CAP / 2 < bit_cap) {
            new_bit_cap = DT_BITMAP_MAX_BIT_CAP;
        } else {
            new_bit_cap = bit_cap * 2;
        }
        code = DT_BitmapChangeSizeUnchecked(layout->free_chunks, new_bit_cap);
        if (code != PRP_OK) {
            DT_ArrPopUnchecked(layout->chunk_ptrs, DT_null);
            free(chunk);
            return code;
        }
    }
    DT_BitmapSetUnchecked(layout->free_chunks, push_idx);

    return PRP_OK;
}

static PRP_Result LayoutInitialize(Layout *layout, DT_size behavior_idx) {
    DIAG_ASSERT(layout != DT_null);
    DIAG_ASSERT(behavior_idx < DT_ArrLenUnchecked(g_ctx->behaviors));

    PRP_Result code;
    layout->behavior_idx = behavior_idx;
    layout->chunk_ptrs =
        DT_ArrCreateUnchecked(sizeof(Chunk *), DT_ARR_DEFAULT_CAP);
    if (!layout->chunk_ptrs) {
        code = DT_ArrGetLastErrCode();
        goto free_internals;
    }
    layout->free_chunks = DT_BitmapCreateUnchecked(DT_ARR_DEFAULT_CAP);
    if (!layout->free_chunks) {
        code = DT_ArrGetLastErrCode();
        goto free_internals;
    }

    code = ChunkCreate(layout);
    if (code != PRP_OK) {
        goto free_internals;
    }

    return PRP_OK;

free_internals:
    if (layout->chunk_ptrs) {
        DT_ArrForEachUnchecked(layout->chunk_ptrs, ChunkPtrDelCb, DT_null);
        DT_ArrDeleteUnchecked(&layout->chunk_ptrs);
    }
    if (layout->free_chunks) {
        DT_BitmapDeleteUnchecked(&layout->free_chunks);
    }

    return code;
}

DT_size LayoutCreate(DT_DSId world_id, DT_size behavior_idx) {
    DIAG_ASSERT(behavior_idx < DT_ArrLenUnchecked(g_ctx->behaviors));
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);

    Layout data = {0};
    PRP_Result code = LayoutInitialize(&data, behavior_idx);
    if (code != PRP_OK) {
        SET_LAST_ERR_CODE(code);
        return PRP_INVALID_INDEX;
    }

    code = DT_ArrPushUnchecked(world->layouts, &data);
    if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        LayoutDelete(&data);
        return PRP_INVALID_INDEX;
    } else if (code != PRP_OK) {
        SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        LayoutDelete(&data);
        return PRP_INVALID_INDEX;
    }

    // The -1 to convert len to idx.
    return DT_ArrLenUnchecked(world->layouts) - 1;
}

static PRP_Result ChunkPtrDelCb(DT_void *ptr, DT_void *user_data) {
    DIAG_ASSERT(ptr != DT_null);
    (DT_void) user_data;

    Chunk **pChunk_ptr = ptr;
    free(*pChunk_ptr);

    return PRP_OK;
}

DT_void LayoutDelete(Layout *layout) {
    DIAG_ASSERT(layout != DT_null);
    DIAG_ASSERT(layout->chunk_ptrs != DT_null &&
                layout->free_chunks != DT_null);

    DT_ArrForEachUnchecked(layout->chunk_ptrs, ChunkPtrDelCb, DT_null);
    DT_ArrDeleteUnchecked(&layout->chunk_ptrs);
    DT_BitmapDeleteUnchecked(&layout->free_chunks);

#if !defined(PRP_NDEBUG)
    layout->behavior_idx = PRP_INVALID_INDEX;
#endif
}

/* ----  ENTITY ---- */

#define CHUNK(layout, chunk_idx)                                               \
    (*(Chunk **)DT_ArrGetUnchecked((layout)->chunk_ptrs, (chunk_idx)))

#define ENTITY_SLOT_MASK ((DT_size)31)
#define ENTITY_SLOT_BITS (5)
// Explicit encoding instead of just multiplying, to show intent.
#define ENTITY_IDX(chunk_idx, slot_idx)                                        \
    (((DT_size)(chunk_idx) << ENTITY_SLOT_BITS) |                              \
     ((DT_size)(slot_idx) & ENTITY_SLOT_MASK))

#define MAX_ENTITY_CAP(layout)                                                 \
    (DT_ArrLenUnchecked(layout->chunk_ptrs) * CHUNK_CAP)

static DT_size GetCompStride(DT_size behavior_idx, DT_size comp_idx);

FECS_Entity EntitySpawn(DT_DSId world_id, DT_size layout_idx) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(layout_idx < DT_ArrLenUnchecked(world->layouts));

    Layout *layout = DT_ArrGetUnchecked(world->layouts, layout_idx);
    DT_size free_chunk = DT_BitmapFFSUnchecked(layout->free_chunks);
    if (free_chunk == PRP_INVALID_INDEX) {
        PRP_Result code = ChunkCreate(layout);
        if (code != PRP_OK) {
            SET_LAST_ERR_CODE(code);
            return (FECS_Entity){.layout_idx = PRP_INVALID_INDEX,
                                 .data.entity_idx = PRP_INVALID_INDEX};
        }
        free_chunk = DT_BitmapFFSUnchecked(layout->free_chunks);
        DIAG_ASSERT(free_chunk != PRP_INVALID_INDEX);
    }

    Chunk *chunk = CHUNK(layout, free_chunk);
    // This is guaranteed to be of b/w 0-31.
    DT_u8 free_slot = (DT_u8)DT_BitwordFFS((DT_Bitword)chunk->free_slot);

    FECS_Entity entity = {.layout_idx = layout_idx,
                          .data.gen = chunk->gen[free_slot],
                          .data.entity_idx = ENTITY_IDX(free_chunk, free_slot)};
    PRP_BIT_CLR(chunk->free_slot, free_slot);
    if (!chunk->free_slot) {
        DT_BitmapClrUnchecked(layout->free_chunks, free_chunk);
    }

    return entity;
}

FECS_EntityBatch *EntitySpawnN(DT_DSId world_id, DT_size layout_idx,
                               DT_size count) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(count > 0);
    DIAG_ASSERT(layout_idx < DT_ArrLenUnchecked(world->layouts));

    Layout *layout = DT_ArrGetUnchecked(world->layouts, layout_idx);
    FECS_EntityBatch *batch =
        malloc(sizeof(FECS_EntityBatch) + (sizeof(EntityData) * count));
    if (!batch) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    batch->layout_idx = layout_idx;
    batch->count = count;
    DT_size i = 0;
    while (i < count) {
        DT_size free_chunk = DT_BitmapFFSUnchecked(layout->free_chunks);
        if (free_chunk == PRP_INVALID_INDEX) {
            PRP_Result code = ChunkCreate(layout);
            if (code != PRP_OK) {
                if (i == 0) {
                    SET_LAST_ERR_CODE(code);
                    free(batch);
                    return DT_null;
                }
                batch->count = i;
                DIAG_LOG_WARN(DIAG_LOG_CODE_FALLBACK_USED,
                              "Cannot create a batch of %zu entities, a batch "
                              "with %zu entities is being created.",
                              count, i);
                return batch;
            }
            free_chunk = DT_BitmapFFSUnchecked(layout->free_chunks);
            DIAG_ASSERT(free_chunk != PRP_INVALID_INDEX);
        }
        Chunk *chunk = CHUNK(layout, free_chunk);
        while (chunk->free_slot && i < count) {
            // This is guaranteed to be of b/w 0-31.
            DT_u8 free_slot =
                (DT_u8)DT_BitwordFFS((DT_Bitword)chunk->free_slot);
            batch->data[i++] =
                (EntityData){.entity_idx = ENTITY_IDX(free_chunk, free_slot),
                             .gen = chunk->gen[free_slot]};
            PRP_BIT_CLR(chunk->free_slot, free_slot);
        }
        if (!chunk->free_slot) {
            DT_BitmapClrUnchecked(layout->free_chunks, free_chunk);
        }
    }

    return batch;
}

DT_bool EntityIsValid(DT_DSId world_id, const FECS_Entity entity) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);

    if (entity.layout_idx >= DT_ArrLenUnchecked(world->layouts)) {
        return DT_false;
    }
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);

    if (entity.data.entity_idx >= MAX_ENTITY_CAP(layout)) {
        return DT_false;
    }
    DT_size chunk_idx = entity.data.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.data.entity_idx & ENTITY_SLOT_MASK;

    if (chunk->gen[slot_idx] != entity.data.gen ||
        PRP_BIT_IS_SET(chunk->free_slot, slot_idx)) {
        return DT_false;
    }

    return DT_true;
}

DT_bool EntityBatchIsValid(DT_DSId world_id, const FECS_EntityBatch *entities) {
    DIAG_ASSERT(entities != DT_null);
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);

    if (entities->layout_idx >= DT_ArrLenUnchecked(world->layouts)) {
        return DT_false;
    }
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entities->layout_idx);
    DT_size max_entity_idx = MAX_ENTITY_CAP(layout);

    for (DT_size i = 0; i < entities->count; i++) {
        if (entities->data[i].entity_idx >= max_entity_idx) {
            return DT_false;
        }
        DT_size chunk_idx = entities->data[i].entity_idx >> ENTITY_SLOT_BITS;
        Chunk *chunk = CHUNK(layout, chunk_idx);
        DT_u8 slot_idx = entities->data[i].entity_idx & ENTITY_SLOT_MASK;
        // The PRP_BIT_IS_SET check prevents duplicate entities in the batch.
        if (chunk->gen[slot_idx] != entities->data[i].gen ||
            PRP_BIT_IS_SET(chunk->free_slot, slot_idx)) {
            return DT_false;
        }
    }

    return DT_true;
}

DT_void EntityKill(DT_DSId world_id, FECS_Entity entity) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(EntityIsValid(world_id, entity) == DT_true);

    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);
    DT_size chunk_idx = entity.data.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.data.entity_idx & ENTITY_SLOT_MASK;

    // Wrap around of gen is valid behavior.
    chunk->gen[slot_idx]++;
    PRP_BIT_SET(chunk->free_slot, slot_idx);
    DT_BitmapSetUnchecked(layout->free_chunks, chunk_idx);
}

DT_void EntityKillN(DT_DSId world_id, FECS_EntityBatch *entities) {
    DIAG_ASSERT(entities != DT_null);
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    DIAG_ASSERT(EntityBatchIsValid(world_id, entities) == DT_true);

    Layout *layout = DT_ArrGetUnchecked(world->layouts, entities->layout_idx);
    for (DT_size i = 0; i < entities->count; i++) {
        DT_size chunk_idx = entities->data[i].entity_idx >> ENTITY_SLOT_BITS;
        Chunk *chunk = CHUNK(layout, chunk_idx);
        DT_u8 slot_idx = entities->data[i].entity_idx & ENTITY_SLOT_MASK;

        // Wrap around of gen is valid behavior.
        chunk->gen[slot_idx]++;
        PRP_BIT_SET(chunk->free_slot, slot_idx);
        DT_BitmapSetUnchecked(layout->free_chunks, chunk_idx);
    }
}

static DT_size GetCompStride(DT_size behavior_idx, DT_size comp_idx) {
    DIAG_ASSERT(behavior_idx < DT_ArrLenUnchecked(g_ctx->behaviors));
    DIAG_ASSERT(comp_idx < DT_ArrLenUnchecked(g_ctx->comps));

    Behavior *behavior = DT_ArrGetUnchecked(g_ctx->behaviors, behavior_idx);
    DT_size bit_cap, word_cap;
    const DT_Bitword *set_raw =
        DT_BitmapRawUnchecked(behavior->set, &word_cap, &bit_cap);
    if (comp_idx >= bit_cap ||
        !DT_BitmapIsSetUnchecked(behavior->set, comp_idx)) {
        return PRP_INVALID_SIZE;
    }

    DT_size idx = 0;
    for (DT_size i = 0; i < WORD_I(comp_idx); i++) {
        idx += DT_BitwordPopCnt(set_raw[i]);
    }
    // The mask skips the comp_idx in the popcnt, so the index computed is
    // correct indxe and it is not off by one.
    idx +=
        DT_BitwordPopCnt(set_raw[WORD_I(comp_idx)] & (BIT_MASK(comp_idx) - 1));

    return behavior->strides[idx];
}

DT_void *EntityGetComp(DT_DSId world_id, const FECS_Entity entity,
                       DT_size comp_idx) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    // This entire check vanishes in debug mode.
    DIAG_ASSERT(EntityIsValid(world_id, entity) == DT_true);
    DIAG_ASSERT(comp_idx < DT_ArrLenUnchecked(g_ctx->comps));

    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);
    DT_size chunk_idx = entity.data.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.data.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))->size;
    DT_size comp_stride = GetCompStride(layout->behavior_idx, comp_idx);
    DIAG_ASSERT_MSG(comp_stride != PRP_INVALID_SIZE,
                    "The given comp idx to extract for the entity, is not part "
                    "of the entity.");

    return (DT_u8 *)chunk->mem + comp_stride + (slot_idx * comp_size);
}

DT_void EntitySetComp(DT_DSId world_id, FECS_Entity entity, DT_size comp_idx,
                      const DT_void *data) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    // This entire check vanishes in debug mode.
    DIAG_ASSERT(EntityIsValid(world_id, entity) == DT_true);
    DIAG_ASSERT(comp_idx < DT_ArrLenUnchecked(g_ctx->comps));

    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);
    DT_size chunk_idx = entity.data.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.data.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))->size;
    DT_size comp_stride = GetCompStride(layout->behavior_idx, comp_idx);
    DIAG_ASSERT_MSG(comp_stride != PRP_INVALID_SIZE,
                    "The given comp idx to extract for the entity, is not part "
                    "of the entity.");
    DT_u8 *ptr = (DT_u8 *)chunk->mem + comp_stride + (slot_idx * comp_size);

    memcpy(ptr, data, comp_size);
}

PRP_Result EntityBatchForEach(DT_DSId world_id, FECS_EntityBatch *entities,
                              DT_size comp_idx,
                              PRP_Result (*cb)(DT_void *comp_data,
                                               DT_void *user_data),
                              DT_void *user_data) {
    World *world = DT_DSIdToDataUnchecked(g_ctx->worlds, world_id);
    DIAG_ASSERT(world != DT_null);
    // This entire check vanishes in debug mode.
    DIAG_ASSERT(EntityBatchIsValid(world_id, entities) == DT_true);

    Layout *layout = DT_ArrGetUnchecked(world->layouts, entities->layout_idx);
    DT_size comp_size =
        ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))->size;
    DT_size comp_stride = GetCompStride(layout->behavior_idx, comp_idx);
    DIAG_ASSERT_MSG(comp_stride != PRP_INVALID_SIZE,
                    "The given comp idx to extract for the entity, is not part "
                    "of the entities.");
    for (DT_size i = 0; i < entities->count; i++) {
        DT_size chunk_idx = entities->data[i].entity_idx >> ENTITY_SLOT_BITS;
        Chunk *chunk = CHUNK(layout, chunk_idx);
        DT_u8 slot_idx = entities->data[i].entity_idx & ENTITY_SLOT_MASK;
        DT_u8 *ptr = (DT_u8 *)chunk->mem + comp_stride + (slot_idx * comp_size);
        PRP_Result code = cb(ptr, user_data);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}
