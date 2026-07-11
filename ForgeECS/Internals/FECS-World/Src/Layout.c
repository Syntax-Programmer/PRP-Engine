#include "DataTypes/Arr.h"
#include "ForgeECS/Internals/FECS-World/World-Internals.h"
#include "ForgeECS/Internals/FECS/FECS-Internals.h"

/**
 * Adds new chunk to layout.
 *
 * @param pLayout Layout instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result CreateChunk(FECS_Layout *pLayout);
/**
 * Initializes internals of a new layout given the mem objects have been
 * inited.
 *
 * @param pLayout Layout instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result LayoutInitInternals(FECS_Layout *pLayout);
/**
 * Deletes the chunks inside layout.
 * Called via DT_ArrForEach_...
 *
 * @param ppChunk Chunk** to free.
 *
 * @return PRP_OK on success.
 */
static PRP_Result ChunkPtrDelCb(DT_void *ppChunk, DT_void *_);

static PRP_Result CreateChunk(FECS_Layout *pLayout) {
    FECS_Chunk *pChunk = malloc(pLayout->chunk_total_size);
    if (!pChunk) {
        return PRP_ERR_OOM;
    }
    DT_size push_idx = DT_ArrLen(pLayout->pChunk_ptrs);
    DT_size bit_cap = DT_BitmapBitCap(pLayout->pFree_chunk_bitset);
    if (push_idx >= bit_cap) {
        DT_size new_bit_cap;
        if (DT_BITMAP_MAX_BIT_CAP / 2 < bit_cap) {
            new_bit_cap = DT_BITMAP_MAX_BIT_CAP;
        } else {
            new_bit_cap = bit_cap * 2;
        }
        PRP_Result code = DT_BitmapChangeSizeUnchecked(
            pLayout->pFree_chunk_bitset, new_bit_cap);
        if (code != PRP_OK) {
            free(pChunk);
            return code;
        }
    }
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
     *  We can use sizeof(Chunk) in this bcuz the chunk data is a flex array
     memb
     * and doesn't count in the size of struct.
     */
    memset(pChunk, 0XFF, sizeof(FECS_Chunk));
    DT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset, push_idx);

    return PRP_OK;
}

static PRP_Result LayoutInitInternals(FECS_Layout *pLayout) {
    DT_size comps_len, create_info_cap, create_info_bit_cap;
    const DT_size *pComp_sizes =
        DT_ArrRawUnchecked(g_ctx->pComp_sizes, &comps_len);
    const DT_Bitword *pBitwords = DT_BitmapRawUnchecked(
        pLayout->pComp_set, &create_info_cap, &create_info_bit_cap);

    DT_size *pStride_dest = &pLayout->pComp_arr_strides[0];
    DT_size stride = 0;
    for (DT_size i = 0, j = 0; i < create_info_cap; i++) {
        DT_Bitword word = pBitwords[i];
        while (word) {
            DT_size comp_id = DT_BitwordFFS(word) + j;
            *pStride_dest = stride;
            pStride_dest++;
            stride += pComp_sizes[comp_id] * CHUNK_CAP;

            word &= word - 1;
        }
        j += sizeof(DT_Bitword) * 8;
    }
    pLayout->chunk_total_size = stride + sizeof(FECS_Chunk);

    return PRP_OK;
}

static PRP_Result ChunkPtrDelCb(DT_void *ppChunk, DT_void *_) {
    (DT_void) _;
    FECS_Chunk *pChunk = *(FECS_Chunk **)ppChunk;

    free(pChunk);

    return PRP_OK;
}

PRP_Result LayoutCreate(DT_Bitmap *pCreate_info, FECS_Layout *pLayout) {
    *pLayout = (FECS_Layout){0};
    pLayout->pComp_set = pCreate_info;

    PRP_Result code = DT_ArrCreateUnchecked(
        sizeof(FECS_Chunk *), DT_ARR_DEFAULT_CAP, &pLayout->pChunk_ptrs);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = DT_BitmapCreateUnchecked(DT_ARR_DEFAULT_CAP,
                                    &pLayout->pFree_chunk_bitset);
    if (code != PRP_OK) {
        goto err_path;
    }
    pLayout->pComp_arr_strides =
        malloc(sizeof(DT_size) * DT_BitmapSetCount(pCreate_info));
    if (!pLayout->pComp_arr_strides) {
        code = PRP_ERR_OOM;
        goto err_path;
    }
    code = LayoutInitInternals(pLayout);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    if (pLayout->pChunk_ptrs) {
        // If chunk were created it frees it.
        DT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ChunkPtrDelCb, DT_null);
        DT_ArrDeleteUnchecked(&pLayout->pChunk_ptrs);
    }
    if (pLayout->pFree_chunk_bitset) {
        DT_BitmapDeleteUnchecked(&pLayout->pFree_chunk_bitset);
    }
    DT_BitmapDeleteUnchecked(&pLayout->pComp_set);
    if (pLayout->pComp_arr_strides) {
        free(pLayout->pComp_arr_strides);
    }

    return code;
}

DT_void LayoutDelete(FECS_Layout *pLayout) {
    DIAG_ASSERT(pLayout != DT_null);

    DT_BitmapDeleteUnchecked(&pLayout->pComp_set);
    DT_BitmapDeleteUnchecked(&pLayout->pFree_chunk_bitset);

    DT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ChunkPtrDelCb, DT_null);
    DT_ArrDeleteUnchecked(&pLayout->pChunk_ptrs);

    free(pLayout->pComp_arr_strides);

#if !defined(PRP_NDEBUG)
    pLayout->pComp_arr_strides = DT_null;
#endif
}

/* ----  ENTITIES ---- */

#define CHUNK(pLayout, chunk_idx)                                              \
    (*(FECS_Chunk **)DT_ArrGetUnchecked((pLayout)->pChunk_ptrs, (chunk_idx)))

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
    DT_u32 gens[CHUNK_CAP];
} ChunkView;

/**
 * Checks if the chunk view of a entity group is valid.
 *
 * @param pVal       A chunk view from entity group.
 * @param pUser_data The layout the entities/chunk_views belong to.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the chunk view contains invlaid entities.
 */
static PRP_Result EntityGroupValidityCb(DT_void *pVal, DT_void *pUser_data);
/**
 * Kills entities of a chunk view of a entity group.
 *
 * @param pVal       A chunk view from entity batch.
 * @param pUser_data The layout the entities/chunk_views belong to.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if the chunk view contains invlaid entities.
 */
static PRP_Result EntityGroupKillCb(DT_void *pVal, DT_void *pUser_data);
/**
 * Iterates over entities of a chunk view of a entity group.
 *
 * @param pVal       A chunk view from entity batch.
 * @param pUser_data The iteration data containing all the context.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if the chunk view contains invlaid entities.
 */
static PRP_Result EntityGroupIterationCb(DT_void *pVal, DT_void *pUser_data);

PRP_Result EntitySpawn(FECS_World *pWorld, FECS_LayoutId layout_id,
                       FECS_EntityId *pEntity) {
    FECS_Layout *pLayout = &pWorld->pLayouts[layout_id];
    DT_size free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_bitset);
    if (free_chunk_idx == PRP_INVALID_INDEX) {
        PRP_Result code = CreateChunk(pLayout);
        if (code != PRP_OK) {
            return code;
        }
        free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_bitset);
    }
    FECS_Chunk *pChunk = CHUNK(pLayout, free_chunk_idx);
    DT_u32 free_slot_idx =
        (DT_u32)DT_BitwordFFS((DT_Bitword)pChunk->free_slot_bitset);
    pEntity->layout_id = layout_id;
    pEntity->gen = pChunk->gens[free_chunk_idx];
    pEntity->entity_idx = ENTITY_IDX(free_chunk_idx, free_slot_idx);
    PRP_BIT_CLR(pChunk->free_slot_bitset, BIT_MASK(free_chunk_idx));
    if (!pChunk->free_slot_bitset) {
        DT_BitmapClrUnchecked(pLayout->pFree_chunk_bitset, free_chunk_idx);
    }

    return PRP_OK;
}

PRP_Result EntityGroupSpawn(FECS_World *pWorld, FECS_LayoutId layout_id,
                            DT_size entity_count,
                            FECS_EntityGroupId **ppGroup) {
    FECS_Layout *pLayout = &pWorld->pLayouts[layout_id];

    DT_size min_cap = (entity_count + CHUNK_CAP - 1) / CHUNK_CAP;
    FECS_EntityGroupId *pGroup = malloc(sizeof(FECS_EntityGroupId));
    if (!pGroup) {
        return PRP_ERR_OOM;
    }
    PRP_Result code = DT_ArrCreateUnchecked(sizeof(ChunkView), min_cap,
                                            &pGroup->pChunk_views);
    if (code != PRP_OK) {
        free(pGroup);
        return code;
    }

    pGroup->layout_id = layout_id;
    DT_size alloc_count = 0;
    while (alloc_count != entity_count) {
        DT_size free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_bitset);
        if (free_chunk_idx == PRP_INVALID_INDEX) {
            code = CreateChunk(pLayout);
            if (code != PRP_OK) {
                goto err_path;
            }
            free_chunk_idx = DT_BitmapFFS(pLayout->pFree_chunk_bitset);
        }
        FECS_Chunk *pChunk = CHUNK(pLayout, free_chunk_idx);

        // This is correct since every free slot will now become occupied.
        DT_u32 occupied_slots_mask = pChunk->free_slot_bitset;
        DT_size left = entity_count - alloc_count;
        DT_u32 pop = (DT_u32)DT_BitwordPopCnt(occupied_slots_mask);
        for (; pop > left; pop--) {
            occupied_slots_mask &= occupied_slots_mask - 1;
        }

        ChunkView view = {.chunk_idx = free_chunk_idx,
                          .occupied_slots = occupied_slots_mask};
        // Easier to copy the entire thing than parse it.
        memcpy(view.gens, pChunk->gens, CHUNK_CAP * sizeof(DT_u32));

        code = DT_ArrPushUnchecked(pGroup->pChunk_views, &view);
        if (code != PRP_OK) {
            goto err_path;
        }
        alloc_count += DT_BitwordPopCnt(occupied_slots_mask);
        PRP_BIT_CLR(pChunk->free_slot_bitset, occupied_slots_mask);
        if (!pChunk->free_slot_bitset) {
            DT_BitmapClrUnchecked(pLayout->pFree_chunk_bitset, free_chunk_idx);
        }
    }
    *ppGroup = pGroup;

    return PRP_OK;

err_path:
    if (alloc_count == 0) {
        DT_ArrDeleteUnchecked(&pGroup->pChunk_views);
        free(pGroup);
        return code;
    }
    DIAG_LOG_WARN(DIAG_LOG_CODE_FALLBACK_USED,
                  "Cannot create a batch of %zu entities, a batch "
                  "with %zu entities is being created.",
                  entity_count, alloc_count);
    *ppGroup = pGroup;
    return PRP_OK;
}

DT_bool EntityIsValid(FECS_World *pWorld, const FECS_EntityId entity) {
    if (entity.layout_id >= pWorld->layout_count) {
        return DT_false;
    }
    FECS_Layout *pLayout = &pWorld->pLayouts[entity.layout_id];

    if (entity.entity_idx >= MAX_ENTITY_CAP(pLayout)) {
        return DT_false;
    }
    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    if (pChunk->gens[slot_idx] != entity.gen ||
        PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot_idx))) {
        return DT_false;
    }

    return DT_true;
}

static PRP_Result EntityGroupValidityCb(DT_void *pVal, DT_void *pUser_data) {
    ChunkView *pChunk_view = pVal;
    FECS_Layout *pLayout = pUser_data;

    if (pChunk_view->chunk_idx >= DT_ArrLen(pLayout->pChunk_ptrs)) {
        return PRP_ERR_INV_STATE;
    }
    FECS_Chunk *pChunk = CHUNK(pLayout, pChunk_view->chunk_idx);
    DT_u32 mask = pChunk_view->occupied_slots;
    while (mask) {
        DT_u32 slot = (DT_u32)DT_BitwordCTZ(mask);
        if (pChunk_view->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot))) {
            return PRP_ERR_INV_STATE;
        }
        mask &= mask - 1;
    }

    return PRP_OK;
}

DT_bool EntityGroupIsValid(FECS_World *pWorld,
                           const FECS_EntityGroupId *pGroup) {
    if (pGroup->layout_id >= pWorld->layout_count) {
        return DT_false;
    }
    FECS_Layout *pLayout = &pWorld->pLayouts[pGroup->layout_id];
    PRP_Result code = DT_ArrForEachUnchecked(pGroup->pChunk_views,
                                             EntityGroupValidityCb, pLayout);

    return code == PRP_OK;
}

DT_void EntityKill(FECS_World *pWorld, FECS_EntityId *pEntity) {
    FECS_Layout *pLayout = &pWorld->pLayouts[pEntity->layout_id];
    DT_size chunk_idx = pEntity->entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = pEntity->entity_idx & ENTITY_SLOT_MASK;

    pChunk->gens[slot_idx]++;
    PRP_BIT_SET(pChunk->free_slot_bitset, BIT_MASK(slot_idx));
    DT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset, chunk_idx);

    pEntity->layout_id = PRP_INVALID_INDEX;
    pEntity->entity_idx = PRP_INVALID_INDEX;
}

static PRP_Result EntityGroupKillCb(DT_void *pVal, DT_void *pUser_data) {
    ChunkView *pChunk_view = pVal;
    FECS_Layout *pLayout = pUser_data;

    if (pChunk_view->chunk_idx >= DT_ArrLen(pLayout->pChunk_ptrs)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_Chunk *pChunk = CHUNK(pLayout, pChunk_view->chunk_idx);
    DT_u32 mask = pChunk_view->occupied_slots;
    while (mask) {
        DT_u32 slot = (DT_u32)DT_BitwordCTZ(mask);
        if (pChunk_view->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot))) {
            if (mask != pChunk_view->occupied_slots) {
                // We deleted not all entities but now chunk has free spot.
                DT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset,
                                      pChunk_view->chunk_idx);
            }
            return PRP_ERR_INV_ARG;
        }
        mask &= mask - 1;
        PRP_BIT_SET(pChunk->free_slot_bitset, BIT_MASK(slot));
        pChunk->gens[slot]++;
    }
    DT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset, pChunk_view->chunk_idx);

    return PRP_OK;
}

PRP_Result EntityGroupKill(FECS_World *pWorld, FECS_EntityGroupId **ppGroup) {
    FECS_EntityGroupId *pGroup = *ppGroup;
    FECS_Layout *pLayout = &pWorld->pLayouts[pGroup->layout_id];
    PRP_Result code = DT_ArrForEachUnchecked(pGroup->pChunk_views,
                                             EntityGroupKillCb, pLayout);
    if (code != PRP_OK) {
        return code;
    }
    DT_ArrDeleteUnchecked(&pGroup->pChunk_views);
    free(pGroup);
    *ppGroup = DT_null;

    return PRP_OK;
}

PRP_Result EntityGetComp(FECS_World *pWorld, const FECS_EntityId entity,
                         FECS_CompId comp_id, DT_void **ppComp_ptr) {
    FECS_Layout *pLayout = &pWorld->pLayouts[entity.layout_id];
    if (!DT_BitmapIsSetUnchecked(pLayout->pComp_set, comp_id)) {
        return PRP_ERR_INV_ARG;
    }

    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        (*(DT_size *)DT_ArrGetUnchecked(g_ctx->pComp_sizes, comp_id));
    DT_size stride_idx = DT_BitmapBitRankUnchecked(pLayout->pComp_set, comp_id);
    DT_size comp_stride = pLayout->pComp_arr_strides[stride_idx];

    *ppComp_ptr =
        (DT_u8 *)pChunk->pChunk_mem + comp_stride + (slot_idx * comp_size);

    return PRP_OK;
}

PRP_Result EntitySetComp(FECS_World *pWorld, FECS_EntityId entity,
                         FECS_CompId comp_id, const DT_void *pComp_data) {
    FECS_Layout *pLayout = &pWorld->pLayouts[entity.layout_id];
    if (!DT_BitmapIsSetUnchecked(pLayout->pComp_set, comp_id)) {
        return PRP_ERR_INV_ARG;
    }

    DT_size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    DT_u8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    DT_size comp_size =
        (*(DT_size *)DT_ArrGetUnchecked(g_ctx->pComp_sizes, comp_id));
    DT_size stride_idx = DT_BitmapBitRankUnchecked(pLayout->pComp_set, comp_id);
    DT_size comp_stride = pLayout->pComp_arr_strides[stride_idx];

    DT_u8 *ptr =
        (DT_u8 *)pChunk->pChunk_mem + comp_stride + (slot_idx * comp_size);
    memcpy(ptr, pComp_data, comp_size);

    return PRP_OK;
}

typedef struct {
    FECS_Layout *pLayout;
    DT_size comp_size;
    DT_size comp_stride;
    PRP_Result (*cb)(DT_void *pComp_data, DT_void *pUser_data);
    DT_void *pUser_data;
} IterationData;

static PRP_Result EntityGroupIterationCb(DT_void *pVal, DT_void *pUser_data) {
    ChunkView *pChunk_view = pVal;
    IterationData *pI_data = pUser_data;

    if (pChunk_view->chunk_idx >= DT_ArrLen(pI_data->pLayout->pChunk_ptrs)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_Chunk *pChunk = CHUNK(pI_data->pLayout, pChunk_view->chunk_idx);
    DT_u32 mask = pChunk_view->occupied_slots;
    while (mask) {
        DT_u32 slot = (DT_u32)DT_BitwordCTZ(mask);
        if (pChunk_view->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot))) {
            if (mask != pChunk_view->occupied_slots) {
                // We deleted not all entities but now chunk has free spot.
                DT_BitmapSetUnchecked(pI_data->pLayout->pFree_chunk_bitset,
                                      pChunk_view->chunk_idx);
            }
            return PRP_ERR_INV_ARG;
        }
        mask &= mask - 1;
        DT_u8 *ptr = (DT_u8 *)pChunk->pChunk_mem + pI_data->comp_stride +
                     (slot * pI_data->comp_size);
        PRP_Result code = pI_data->cb(ptr, pI_data->pUser_data);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}

PRP_Result EntityGroupForEach(FECS_World *pWorld, FECS_EntityGroupId *pGroup,
                              FECS_CompId comp_id,
                              PRP_Result (*cb)(DT_void *pComp_data,
                                               DT_void *pUser_data),
                              DT_void *pUser_data) {
    IterationData i_data = {.cb = cb, .pUser_data = pUser_data};
    i_data.pLayout = &pWorld->pLayouts[pGroup->layout_id];
    if (!DT_BitmapIsSetUnchecked(i_data.pLayout->pComp_set, comp_id)) {
        return PRP_ERR_INV_ARG;
    }

    i_data.comp_size =
        (*(DT_size *)DT_ArrGetUnchecked(g_ctx->pComp_sizes, comp_id));
    DT_size stride_idx =
        DT_BitmapBitRankUnchecked(i_data.pLayout->pComp_set, comp_id);
    i_data.comp_stride = i_data.pLayout->pComp_arr_strides[stride_idx];

    return DT_ArrForEachUnchecked(pGroup->pChunk_views, EntityGroupIterationCb,
                                  &i_data);
}
