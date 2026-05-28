#include "ForgeECS/Internals/FECS-Internals.h"
#include "World-Internals.h"
#include <string.h>

/* ----  LAYOUTS ---- */

/**
 * Deletes the chunks inside layout.
 * Called via DT_ArrForEach_...
 *
 * @param ppChunk Chunk** to free.
 *
 * @return PRP_OK on success.
 */
static PRP_Result ChunkPtrDelCb(DT_void *ppChunk, DT_void *_);
/**
 * Adds new chunk to layout.
 *
 * @param pLayout Layout instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ChunkCreate(Layout *pLayout);
/**
 * Initializes a new layout.
 *
 * @param pLayout     Layout instance.
 * @param behavior_id The underlying behavior of the layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result LayoutInitialize(Layout *pLayout,
                                   FECS_BehaviorId behavior_id);

static PRP_Result ChunkCreate(Layout *pLayout) {
    Behavior *pBehavior =
        DT_ArrGetUnchecked(g_ctx->behaviors, pLayout->behavior_id);
    Chunk *pChunk = malloc(pBehavior->chunk_size);
    if (!pChunk) {
        return PRP_ERR_OOM;
    }

    DT_size push_idx = DT_ArrLen(pLayout->pChunk_ptrs);
    PRP_Result code = DT_ArrPushUnchecked(pLayout->pChunk_ptrs, &pChunk);
    if (code != PRP_OK) {
        free(pChunk);
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
    memset(pChunk, 0XFF, sizeof(Chunk));
    DT_size bit_cap = DT_BitmapBitCap(pLayout->pFree_chunk_idxs);
    if (push_idx >= bit_cap) {
        DT_size new_bit_cap;
        if (DT_BITMAP_MAX_BIT_CAP / 2 < bit_cap) {
            new_bit_cap = DT_BITMAP_MAX_BIT_CAP;
        } else {
            new_bit_cap = bit_cap * 2;
        }
        code = DT_BitmapChangeSizeUnchecked(pLayout->pFree_chunk_idxs,
                                            new_bit_cap);
        if (code != PRP_OK) {
            DT_ArrPopUnchecked(pLayout->pChunk_ptrs, DT_null);
            free(pChunk);
            return code;
        }
    }
    DT_BitmapSetUnchecked(pLayout->pFree_chunk_idxs, push_idx);

    return PRP_OK;
}

static PRP_Result LayoutInitialize(Layout *pLayout,
                                   FECS_BehaviorId behavior_id) {
    pLayout->behavior_id = behavior_id;
    PRP_Result code;

    code = DT_ArrCreateUnchecked(sizeof(Chunk *), DT_ARR_DEFAULT_CAP,
                                 &pLayout->pChunk_ptrs);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_BitmapCreateUnchecked(DT_ARR_DEFAULT_CAP,
                                    &pLayout->pFree_chunk_idxs);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = ChunkCreate(pLayout);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    if (pLayout->pChunk_ptrs) {
        DT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ChunkPtrDelCb, DT_null);
        DT_ArrDeleteUnchecked(&pLayout->pChunk_ptrs);
    }
    if (pLayout->pFree_chunk_idxs) {
        DT_BitmapDeleteUnchecked(&pLayout->pFree_chunk_idxs);
    }

    return code;
}

PRP_Result LayoutCreate(World *pWorld, FECS_BehaviorId behavior_id,
                        FECS_LayoutId *pLayout_id) {
    Layout data = {0};
    PRP_Result code = LayoutInitialize(&data, behavior_id);
    if (code != PRP_OK) {
        return code;
    }

    code = DT_ArrPushUnchecked(pWorld->layouts, &data);
    if (code != PRP_OK) {
        LayoutDelete(&data, DT_null);
        return code;
    }

    *pLayout_id = DT_ArrLen(pWorld->layouts) - 1;

    return PRP_OK;
}

static PRP_Result ChunkPtrDelCb(DT_void *ppChunk, DT_void *_) {
    (DT_void) _;
    Chunk *pChunk = *(Chunk **)ppChunk;

    free(pChunk);

    return PRP_OK;
}

PRP_Result LayoutDelete(DT_void *pLayout, DT_void *_) {
    (DT_void) _;
    Layout *pLayout_instance = pLayout;

    DT_ArrForEachUnchecked(pLayout_instance->pChunk_ptrs, ChunkPtrDelCb,
                           DT_null);
    DT_ArrDeleteUnchecked(&pLayout_instance->pChunk_ptrs);
    DT_BitmapDeleteUnchecked(&pLayout_instance->pFree_chunk_idxs);

#if !defined(PRP_NDEBUG)
    pLayout_instance->behavior_id = PRP_INVALID_INDEX;
#endif

    return PRP_OK;
}

DT_bool LayoutIsAlreadyExisting(World *pWorld, FECS_BehaviorId behavior_id,
                                FECS_LayoutId *pFound_id) {
    DT_size len;
    const Layout *pLayouts = DT_ArrRawUnchecked(pWorld->layouts, &len);

    for (DT_size i = 0; i < len; i++) {
        if (pLayouts[i].behavior_id == behavior_id) {
            *pFound_id = i;
            return DT_true;
        }
    }

    return DT_false;
}

/* ----  ENTITY ---- */

#define CHUNK(pLayout, chunk_idx)                                              \
    (*(Chunk **)DT_ArrGetUnchecked((pLayout)->pChunk_ptrs, (chunk_idx)))

#define ENTITY_SLOT_MASK ((DT_size)31)
#define ENTITY_SLOT_BITS (5)
// Explicit encoding instead of just multiplying, to show intent.
#define ENTITY_IDX(chunk_idx, slot_idx)                                        \
    (((DT_size)(chunk_idx) << ENTITY_SLOT_BITS) |                              \
     ((DT_size)(slot_idx) & ENTITY_SLOT_MASK))

#define MAX_ENTITY_CAP(pLayout) (DT_ArrLen(pLayout->pChunk_ptrs) * CHUNK_CAP)

/**
 * A chunk view is data upon a chunk's entire free slots allocated at once.
 */
typedef struct {
    DT_size chunk_idx;
    DT_u32 occupied_slots;
    DT_u32 gens[32];
} ChunkView;

/**
 * Checks if the chunk view of a entity batch is valid.
 *
 * @param pChunk_view A chunk view from entity batch.
 * @param pLayout     The layout the entities/chunk_views belong to.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the chunk view contains invlaid entities.
 */
static PRP_Result EntityBatchValidityCb(DT_void *pChunk_view, DT_void *pLayout);

PRP_Result LayoutSpawnEntity(World *pWorld, FECS_LayoutId layout_id,
                             FECS_Entity *pEntity) {
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, layout_id);
    DT_size free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_idxs);
    if (free_chunk_idx == PRP_INVALID_INDEX) {
        PRP_Result code = ChunkCreate(pLayout);
        if (code != PRP_OK) {
            return code;
        }
        free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_idxs);
    }

    Chunk *pChunk = CHUNK(pLayout, free_chunk_idx);
    DT_u32 free_slot_idx = (DT_u32)DT_BitwordFFS((DT_Bitword)pChunk->free_slot);

    pEntity->layout_id = layout_id;
    pEntity->gen = pChunk->gens[free_slot_idx];
    pEntity->entity_idx = ENTITY_IDX(free_chunk_idx, free_slot_idx);
    PRP_BIT_CLR(pChunk->free_slot, BIT_MASK(free_slot_idx));
    if (!pChunk->free_slot) {
        DT_BitmapClrUnchecked(pLayout->pFree_chunk_idxs, free_chunk_idx);
    }

    return PRP_OK;
}

PRP_Result LayoutSpawnEntities(World *pWorld, FECS_LayoutId layout_id,
                               DT_size entity_count,
                               FECS_EntityBatch **ppBatch) {
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, layout_id);

    DT_size min_cap = (entity_count + CHUNK_CAP - 1) / CHUNK_CAP;
    FECS_EntityBatch *pBatch = malloc(sizeof(FECS_EntityBatch));
    if (!pBatch) {
        return PRP_ERR_OOM;
    }
    PRP_Result code =
        DT_ArrCreateUnchecked(sizeof(ChunkView), min_cap, &pBatch->chunk_views);
    if (code != PRP_OK) {
        free(pBatch);
        return code;
    }

    pBatch->layout_id = layout_id;
    DT_size alloc_count = 0;
    while (alloc_count != entity_count) {
        DT_size free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_idxs);
        if (free_chunk_idx == PRP_INVALID_INDEX) {
            code = ChunkCreate(pLayout);
            if (code != PRP_OK) {
                goto err_path;
            }
            free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_idxs);
        }
        Chunk *pChunk = CHUNK(pLayout, free_chunk_idx);
        // This is correct since every free slot will now become occupied.
        DT_u32 occupied_slots_mask = pChunk->free_slot;

        DT_size left = entity_count - alloc_count;
        DT_u32 pop = (DT_u32)DT_BitwordPopCnt(occupied_slots_mask);
        while (pop > left) {
            DT_u32 msb = 31u - (DT_u32)DT_BitwordCLZ(occupied_slots_mask);
            occupied_slots_mask ^= (1u << msb);
            pop--;
        }

        ChunkView view = {.chunk_idx = free_chunk_idx,
                          .occupied_slots = occupied_slots_mask};
        // Easier to copy the entire thing than parse it.
        memcpy(view.gens, pChunk->gens, CHUNK_CAP * sizeof(DT_u32));

        code = DT_ArrPushUnchecked(pBatch->chunk_views, &view);
        if (code != PRP_OK) {
            goto err_path;
        }
        alloc_count += DT_BitwordPopCnt(occupied_slots_mask);
        PRP_BIT_CLR(pChunk->free_slot, occupied_slots_mask);
        if (!pChunk->free_slot) {
            DT_BitmapClrUnchecked(pLayout->pFree_chunk_idxs, free_chunk_idx);
        }
    }
    *ppBatch = pBatch;

    return PRP_OK;

err_path:
    if (alloc_count == 0) {
        DT_ArrDeleteUnchecked(&pBatch->chunk_views);
        free(pBatch);
        return code;
    }
    DIAG_LOG_WARN(DIAG_LOG_CODE_FALLBACK_USED,
                  "Cannot create a batch of %zu entities, a batch "
                  "with %zu entities is being created.",
                  entity_count, alloc_count);
    *ppBatch = pBatch;
    return PRP_OK;
}

DT_bool LayoutIsEntityValid(World *pWorld, const FECS_Entity entity) {
    if (entity.layout_id >= DT_ArrLen(pWorld->layouts)) {
        return DT_false;
    }
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, entity.layout_id);

    if (entity.entity_idx >= MAX_ENTITY_CAP(pLayout)) {
        return DT_false;
    }
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    if (pChunk->gens[slot_idx] != entity.gen ||
        PRP_BIT_IS_SET(pChunk->free_slot, BIT_MASK(slot_idx))) {
        return DT_false;
    }

    return DT_true;
}

static PRP_Result EntityBatchValidityCb(DT_void *pChunk_view,
                                        DT_void *pLayout) {
    ChunkView *pChunk_view_instance = pChunk_view;
    Layout *pLayout_instance = pLayout;

    if (pChunk_view_instance->chunk_idx >=
        DT_ArrLen(pLayout_instance->pChunk_ptrs)) {
        return PRP_ERR_INV_STATE;
    }
    Chunk *pChunk = CHUNK(pLayout_instance, pChunk_view_instance->chunk_idx);
    DT_u32 mask = pChunk_view_instance->occupied_slots;
    while (mask) {
        DT_u32 slot = (DT_u32)DT_BitwordCTZ(mask);
        if (pChunk_view_instance->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot, BIT_MASK(slot))) {
            return PRP_ERR_INV_STATE;
        }
        mask &= mask - 1;
    }

    return PRP_OK;
}

DT_bool LayoutAreEntitiesValid(World *pWorld, const FECS_EntityBatch *pBatch) {
    if (pBatch->layout_id >= DT_ArrLen(pWorld->layouts)) {
        return DT_false;
    }
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, pBatch->layout_id);
    PRP_Result code = DT_ArrForEachUnchecked(pBatch->chunk_views,
                                             EntityBatchValidityCb, pLayout);

    return code == PRP_OK;
}

DT_void LayoutKillEntity(World *pWorld, FECS_Entity entity) {
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, entity.layout_id);
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    // Wrap around of gen is valid behavior.
    pChunk->gens[slot_idx]++;
    PRP_BIT_SET(pChunk->free_slot, BIT_MASK(slot_idx));
    DT_BitmapSetUnchecked(pLayout->pFree_chunk_idxs, chunk_idx);
}

PRP_Result LayoutKillEntities(World *pWorld, FECS_EntityBatch **ppBatch) {
    FECS_EntityBatch *pBatch = *ppBatch;
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, pBatch->layout_id);

    DT_size len;
    const ChunkView *pChunk_views =
        DT_ArrRawUnchecked(pBatch->chunk_views, &len);
    for (DT_size i = 0; i < len; i++) {
        const ChunkView *pChunk_view = &pChunk_views[i];
        if (pChunk_view->chunk_idx >= DT_ArrLen(pLayout->pChunk_ptrs)) {
            return PRP_ERR_INV_ARG;
        }
        Chunk *pChunk = CHUNK(pLayout, pChunk_view->chunk_idx);
        DT_u32 occupied_slots_mask = pChunk_view->occupied_slots;
        while (occupied_slots_mask) {
            DT_u32 slot_idx = (DT_u32)DT_BitwordCTZ(occupied_slots_mask);
            if (pChunk_view->gens[slot_idx] != pChunk->gens[slot_idx] ||
                PRP_BIT_IS_SET(pChunk->free_slot, BIT_MASK(slot_idx))) {
                if (occupied_slots_mask != pChunk_view->occupied_slots) {
                    // We deleted some entities so chunk is free.
                    DT_BitmapSetUnchecked(pLayout->pFree_chunk_idxs,
                                          pChunk_view->chunk_idx);
                }
                return PRP_ERR_INV_ARG;
            }
            occupied_slots_mask &= occupied_slots_mask - 1;
            PRP_BIT_SET(pChunk->free_slot, BIT_MASK(slot_idx));
            pChunk->gens[slot_idx]++;
        }
        DT_BitmapSetUnchecked(pLayout->pFree_chunk_idxs,
                              pChunk_view->chunk_idx);
    }
    DT_ArrDeleteUnchecked(&pBatch->chunk_views);
    free(pBatch);
    *ppBatch = DT_null;

    return PRP_OK;
}

PRP_Result LayoutGetEntityComp(World *pWorld, const FECS_Entity entity,
                               FECS_CompId comp_id, DT_void **ppDest_ptr) {
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, entity.layout_id);
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        ((Component *)DT_ArrGetUnchecked(g_ctx->comps, comp_id))->size;
    DT_size comp_stride = BehaviorGetCompStride(pLayout->behavior_id, comp_id);
    if (comp_stride == PRP_INVALID_SIZE) {
        return PRP_ERR_INV_ARG;
    }

    *ppDest_ptr = (DT_u8 *)pChunk->mem + comp_stride + (slot_idx * comp_size);

    return PRP_OK;
}

PRP_Result LayoutSetEntityComp(World *pWorld, FECS_Entity entity,
                               FECS_CompId comp_id, const DT_void *pData) {
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, entity.layout_id);
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        ((Component *)DT_ArrGetUnchecked(g_ctx->comps, comp_id))->size;
    DT_size comp_stride = BehaviorGetCompStride(pLayout->behavior_id, comp_id);
    if (comp_stride == PRP_INVALID_SIZE) {
        return PRP_ERR_INV_ARG;
    }
    DT_u8 *ptr = (DT_u8 *)pChunk->mem + comp_stride + (slot_idx * comp_size);
    memcpy(ptr, pData, comp_size);

    return PRP_OK;
}

PRP_Result LayoutForEachEntities(World *pWorld, FECS_EntityBatch *pBatch,
                                 FECS_CompId comp_id,
                                 PRP_Result (*cb)(DT_void *pComp_data,
                                                  DT_void *pUser_data),
                                 DT_void *pUser_data) {
    Layout *pLayout = DT_ArrGetUnchecked(pWorld->layouts, pBatch->layout_id);
    DT_size comp_size =
        ((Component *)DT_ArrGetUnchecked(g_ctx->comps, comp_id))->size;
    DT_size comp_stride = BehaviorGetCompStride(pLayout->behavior_id, comp_id);
    if (comp_stride == PRP_INVALID_SIZE) {
        return PRP_ERR_INV_ARG;
    }
    DT_size len;
    const ChunkView *pChunk_views =
        DT_ArrRawUnchecked(pBatch->chunk_views, &len);
    for (DT_size i = 0; i < len; i++) {
        const ChunkView *pChunk_view = &pChunk_views[i];
        if (pChunk_view->chunk_idx >= DT_ArrLen(pLayout->pChunk_ptrs)) {
            return PRP_ERR_INV_ARG;
        }
        Chunk *pChunk = CHUNK(pLayout, pChunk_view->chunk_idx);
        DT_u32 occupied_slots_mask = pChunk_view->occupied_slots;
        while (occupied_slots_mask) {
            DT_u32 slot_idx = (DT_u32)DT_BitwordCTZ(occupied_slots_mask);
            if (pChunk_view->gens[slot_idx] != pChunk->gens[slot_idx] ||
                PRP_BIT_IS_SET(pChunk->free_slot, BIT_MASK(slot_idx))) {
                return PRP_ERR_INV_ARG;
            }
            occupied_slots_mask &= occupied_slots_mask - 1;
            DT_u8 *ptr =
                (DT_u8 *)pChunk->mem + comp_stride + (slot_idx * comp_size);
            PRP_Result code = cb(ptr, pUser_data);
            if (code != PRP_OK) {
                return code;
            }
        }
    }

    return PRP_OK;
}
