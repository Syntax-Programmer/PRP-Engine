#include "World-Internals.h"
#include <string.h>

/* ----  LAYOUTS ---- */

/**
 * Deletes the chunks inside layout.
 * Called via DT_ArrForEach_...
 *
 * @param ptr Chunk** to free.
 *
 * @return PRP_OK on success.
 */
static PRP_Result ChunkPtrDelCb(DT_void *ptr, DT_void *_);
/**
 * Adds new chunk to layout.
 *
 * @param layout Layout instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ChunkCreate(Layout *layout);
/**
 * Initializes a new layout.
 *
 * @param layout       Layout instance.
 * @param behavior_idx The underlying behavior of the layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result LayoutInitialize(Layout *layout, DT_size behavior_idx);

static PRP_Result ChunkCreate(Layout *layout) {
    Behavior *behavior =
        DT_ArrGetUnchecked(g_ctx->behaviors, layout->behavior_idx);
    Chunk *chunk = malloc(behavior->chunk_size);
    if (!chunk) {
        return PRP_ERR_OOM;
    }

    DT_size push_idx = DT_ArrLen(layout->chunk_ptrs);
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
    DT_size bit_cap = DT_BitmapBitCap(layout->free_chunks);
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
    layout->behavior_idx = behavior_idx;
    PRP_Result code;

    code = DT_ArrCreateUnchecked(sizeof(Chunk *), DT_ARR_DEFAULT_CAP,
                                 &layout->chunk_ptrs);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_BitmapCreateUnchecked(DT_ARR_DEFAULT_CAP, &layout->free_chunks);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = ChunkCreate(layout);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    if (layout->chunk_ptrs) {
        DT_ArrForEachUnchecked(layout->chunk_ptrs, ChunkPtrDelCb, DT_null);
        DT_ArrDeleteUnchecked(&layout->chunk_ptrs);
    }
    if (layout->free_chunks) {
        DT_BitmapDeleteUnchecked(&layout->free_chunks);
    }

    return code;
}

PRP_Result LayoutCreate(World *world, DT_size behavior_idx, DT_size *pIdx) {
    Layout data = {0};
    PRP_Result code = LayoutInitialize(&data, behavior_idx);
    if (code != PRP_OK) {
        return code;
    }

    code = DT_ArrPushUnchecked(world->layouts, &data);
    if (code != PRP_OK) {
        LayoutDelete(&data, DT_null);
        return code;
    }

    *pIdx = DT_ArrLen(world->layouts) - 1;

    return PRP_OK;
}

static PRP_Result ChunkPtrDelCb(DT_void *ptr, DT_void *_) {
    (DT_void) _;

    Chunk **pChunk_ptr = ptr;
    free(*pChunk_ptr);

    return PRP_OK;
}

PRP_Result LayoutDelete(DT_void *layout, DT_void *_) {
    (DT_void) _;
    Layout *l = layout;

    DT_ArrForEachUnchecked(l->chunk_ptrs, ChunkPtrDelCb, DT_null);
    DT_ArrDeleteUnchecked(&l->chunk_ptrs);
    DT_BitmapDeleteUnchecked(&l->free_chunks);

#if !defined(PRP_NDEBUG)
    l->behavior_idx = PRP_INVALID_INDEX;
#endif

    return PRP_OK;
}

DT_bool LayoutIsAlreadyExisting(World *world, DT_size behavior_idx,
                                DT_size *pOut) {
    DT_size len;
    const Layout *layouts = DT_ArrRawUnchecked(world->layouts, &len);

    for (DT_size i = 0; i < len; i++) {
        if (layouts[i].behavior_idx == behavior_idx) {
            *pOut = i;
            return DT_true;
        }
    }

    return DT_false;
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

#define MAX_ENTITY_CAP(layout) (DT_ArrLen(layout->chunk_ptrs) * CHUNK_CAP)

/**
 * A chunk view is data upon a chunk's entire free slots allocated at once.
 */
typedef struct {
    DT_size chunk_idx;
    DT_u32 occupied_slots;
    DT_u32 gens[32];
} ChunkView;

/**
 * Fetches the stride of component in the given behavior.
 *
 * @param beahvior_idx The behavior to fetch stride from.
 * @param comp_idx     The comp to fetch stride of.
 *
 * @return PRP_INVALID_SIZE if comp doesn't exist in behavior.
 * @return The stride of the comp in the behavior otherwise.
 */
static DT_size GetCompStride(DT_size behavior_idx, DT_size comp_idx);
/**
 * Checks if the chunk view of a entity batch is valid.
 *
 * @param chunk_view A chunk view from entity batch.
 * @param layout     The layout the entities/chunk_views belong to.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the chunk view contains invlaid entities.
 */
static PRP_Result EntityBatchValidityCb(DT_void *chunk_view, DT_void *layout);

PRP_Result LayoutSpawnEntity(World *world, DT_size layout_idx,
                             FECS_Entity *pEntity) {
    Layout *layout = DT_ArrGetUnchecked(world->layouts, layout_idx);
    DT_size free_chunk = DT_BitmapFFS(layout->free_chunks);
    if (free_chunk == PRP_INVALID_INDEX) {
        PRP_Result code = ChunkCreate(layout);
        if (code != PRP_OK) {
            return code;
        }
        free_chunk = DT_BitmapFFS(layout->free_chunks);
    }

    Chunk *chunk = CHUNK(layout, free_chunk);
    DT_u32 free_slot = (DT_u32)DT_BitwordFFS((DT_Bitword)chunk->free_slot);

    pEntity->layout_idx = layout_idx;
    pEntity->gen = chunk->gens[free_slot];
    pEntity->entity_idx = ENTITY_IDX(free_chunk, free_slot);
    PRP_BIT_CLR(chunk->free_slot, BIT_MASK(free_slot));
    if (!chunk->free_slot) {
        DT_BitmapClrUnchecked(layout->free_chunks, free_chunk);
    }

    return PRP_OK;
}

PRP_Result LayoutSpawnEntities(World *world, DT_size layout_idx, DT_size count,
                               FECS_EntityBatch **pEntities) {
    Layout *layout = DT_ArrGetUnchecked(world->layouts, layout_idx);

    DT_size min_cap = (count + CHUNK_CAP - 1) / CHUNK_CAP;
    FECS_EntityBatch *batch = malloc(sizeof(FECS_EntityBatch));
    if (!batch) {
        return PRP_ERR_OOM;
    }
    PRP_Result code =
        DT_ArrCreateUnchecked(sizeof(ChunkView), min_cap, &batch->chunks);
    if (code != PRP_OK) {
        free(batch);
        return code;
    }

    batch->layout_idx = layout_idx;
    DT_size alloc_count = 0;
    while (alloc_count != count) {
        DT_size free_chunk = DT_BitmapFFS(layout->free_chunks);
        if (free_chunk == PRP_INVALID_INDEX) {
            code = ChunkCreate(layout);
            if (code != PRP_OK) {
                goto err_path;
            }
            free_chunk = DT_BitmapFFS(layout->free_chunks);
        }
        Chunk *chunk = CHUNK(layout, free_chunk);
        DT_u32 mask = chunk->free_slot;

        DT_size left = count - alloc_count;
        DT_u32 pop = DT_BitwordPopCnt(mask);
        while (pop > left) {
            DT_u32 msb = 31u - DT_BitwordCLZ(mask);
            mask ^= (1u << msb);
            pop--;
        }

        ChunkView view = {.chunk_idx = free_chunk, .occupied_slots = mask};
        // Easier to copy the entire thing than parse it.
        memcpy(view.gens, chunk->gens, CHUNK_CAP * sizeof(DT_u32));

        code = DT_ArrPushUnchecked(batch->chunks, &view);
        if (code != PRP_OK) {
            goto err_path;
        }
        alloc_count += DT_BitwordPopCnt(mask);
        PRP_BIT_CLR(chunk->free_slot, mask);
        if (!chunk->free_slot) {
            DT_BitmapClrUnchecked(layout->free_chunks, free_chunk);
        }
    }
    *pEntities = batch;

    return PRP_OK;

err_path:
    if (alloc_count == 0) {
        DT_ArrDeleteUnchecked(&batch->chunks);
        free(batch);
        return code;
    }
    DIAG_LOG_WARN(DIAG_LOG_CODE_FALLBACK_USED,
                  "Cannot create a batch of %zu entities, a batch "
                  "with %zu entities is being created.",
                  count, alloc_count);
    *pEntities = batch;
    return PRP_OK;
}

DT_bool LayoutIsEntityValid(World *world, const FECS_Entity entity) {
    if (entity.layout_idx >= DT_ArrLen(world->layouts)) {
        return DT_false;
    }
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);

    if (entity.entity_idx >= MAX_ENTITY_CAP(layout)) {
        return DT_false;
    }
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    if (chunk->gens[slot_idx] != entity.gen ||
        PRP_BIT_IS_SET(chunk->free_slot, BIT_MASK(slot_idx))) {
        return DT_false;
    }

    return DT_true;
}

static PRP_Result EntityBatchValidityCb(DT_void *chunk_view, DT_void *layout) {
    ChunkView *view = chunk_view;
    Layout *l = layout;

    if (view->chunk_idx >= DT_ArrLen(l->chunk_ptrs)) {
        return PRP_ERR_INV_STATE;
    }
    Chunk *chunk = CHUNK(l, view->chunk_idx);
    DT_u32 mask = view->occupied_slots;
    while (mask) {
        DT_u32 slot = DT_BitwordCTZ(mask);
        if (view->gens[slot] != chunk->gens[slot] ||
            PRP_BIT_IS_SET(chunk->free_slot, BIT_MASK(slot))) {
            return PRP_ERR_INV_STATE;
        }
        mask &= mask - 1;
    }

    return PRP_OK;
}

DT_bool LayoutAreEntitiesValid(World *world, const FECS_EntityBatch *entities) {
    if (entities->layout_idx >= DT_ArrLen(world->layouts)) {
        return DT_false;
    }
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entities->layout_idx);
    PRP_Result code =
        DT_ArrForEachUnchecked(entities->chunks, EntityBatchValidityCb, layout);

    return code == PRP_OK;
}

DT_void LayoutKillEntity(World *world, FECS_Entity entity) {
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    // Wrap around of gen is valid behavior.
    chunk->gens[slot_idx]++;
    PRP_BIT_SET(chunk->free_slot, BIT_MASK(slot_idx));
    DT_BitmapSetUnchecked(layout->free_chunks, chunk_idx);
}

PRP_Result LayoutKillEntities(World *world, FECS_EntityBatch **pEntities) {
    FECS_EntityBatch *entities = *pEntities;
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entities->layout_idx);

    DT_size len;
    const ChunkView *views = DT_ArrRawUnchecked(entities->chunks, &len);
    for (DT_size i = 0; i < len; i++) {
        const ChunkView *view = &views[i];
        if (view->chunk_idx >= DT_ArrLen(layout->chunk_ptrs)) {
            return PRP_ERR_INV_ARG;
        }
        Chunk *chunk = CHUNK(layout, view->chunk_idx);
        DT_u32 mask = view->occupied_slots;
        while (mask) {
            DT_u32 slot = DT_BitwordCTZ(mask);
            if (view->gens[slot] != chunk->gens[slot] ||
                PRP_BIT_IS_SET(chunk->free_slot, BIT_MASK(slot))) {
                if (mask != view->occupied_slots) {
                    // We deleted some entities so chunk is free.
                    DT_BitmapSetUnchecked(layout->free_chunks, view->chunk_idx);
                }
                return PRP_ERR_INV_ARG;
            }
            mask &= mask - 1;
            PRP_BIT_SET(chunk->free_slot, BIT_MASK(slot));
            chunk->gens[slot]++;
        }
        DT_BitmapSetUnchecked(layout->free_chunks, view->chunk_idx);
    }
    DT_ArrDeleteUnchecked(&entities->chunks);
    free(entities);
    *pEntities = DT_null;

    return PRP_OK;
}

static DT_size GetCompStride(DT_size behavior_idx, DT_size comp_idx) {
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

PRP_Result LayoutGetEntityComp(World *world, const FECS_Entity entity,
                               DT_size comp_idx, DT_void **dest) {
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))->size;
    DT_size comp_stride = GetCompStride(layout->behavior_idx, comp_idx);
    if (comp_stride == PRP_INVALID_SIZE) {
        return PRP_ERR_INV_ARG;
    }

    *dest = (DT_u8 *)chunk->mem + comp_stride + (slot_idx * comp_size);

    return PRP_OK;
}

PRP_Result LayoutSetEntityComp(World *world, FECS_Entity entity,
                               DT_size comp_idx, const DT_void *data) {
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entity.layout_idx);
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *chunk = CHUNK(layout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))->size;
    DT_size comp_stride = GetCompStride(layout->behavior_idx, comp_idx);
    if (comp_stride == PRP_INVALID_SIZE) {
        return PRP_ERR_INV_ARG;
    }
    DT_u8 *ptr = (DT_u8 *)chunk->mem + comp_stride + (slot_idx * comp_size);
    memcpy(ptr, data, comp_size);

    return PRP_OK;
}

PRP_Result LayoutForEachEntities(World *world, FECS_EntityBatch *entities,
                                 DT_size comp_idx,
                                 PRP_Result (*cb)(DT_void *comp_data,
                                                  DT_void *user_data),
                                 DT_void *user_data) {
    Layout *layout = DT_ArrGetUnchecked(world->layouts, entities->layout_idx);
    DT_size comp_size =
        ((ComponentMetadata *)DT_ArrGetUnchecked(g_ctx->comps, comp_idx))->size;
    DT_size comp_stride = GetCompStride(layout->behavior_idx, comp_idx);
    if (comp_stride == PRP_INVALID_SIZE) {
        return PRP_ERR_INV_ARG;
    }
    DT_size len;
    const ChunkView *views = DT_ArrRawUnchecked(entities->chunks, &len);
    for (DT_size i = 0; i < len; i++) {
        const ChunkView *view = &views[i];
        if (view->chunk_idx >= DT_ArrLen(layout->chunk_ptrs)) {
            return PRP_ERR_INV_ARG;
        }
        Chunk *chunk = CHUNK(layout, view->chunk_idx);
        DT_u32 mask = view->occupied_slots;
        while (mask) {
            DT_u32 slot = DT_BitwordCTZ(mask);
            if (view->gens[slot] != chunk->gens[slot] ||
                PRP_BIT_IS_SET(chunk->free_slot, BIT_MASK(slot))) {
                return PRP_ERR_INV_ARG;
            }
            mask &= mask - 1;
            DT_u8 *ptr = (DT_u8 *)chunk->mem + comp_stride + (slot * comp_size);
            PRP_Result code = cb(ptr, user_data);
            if (code != PRP_OK) {
                return code;
            }
        }
    }

    return PRP_OK;
}
