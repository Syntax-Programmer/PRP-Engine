#include "Forge/Internals/FECS-World/World-Internals.h"
#include "Forge/Internals/FECS/FECS-Internals.h"

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
 * Called via CONT_ArrForEach_...
 *
 * @param ppChunk Chunk** to free.
 *
 * @return PRP_OK on success.
 */
static PRP_Result ChunkPtrDelCb(void *ppChunk, void *_);

static PRP_Result CreateChunk(FECS_Layout *pLayout) {
    FECS_Chunk *pChunk = malloc(pLayout->chunk_total_size);
    if (!pChunk) {
        return PRP_ERR_OOM;
    }
    PRP_Size push_idx = CONT_ArrLen(pLayout->pChunk_ptrs);
    PRP_Size bit_cap = CONT_BitmapBitCap(pLayout->pFree_chunk_bitset);
    if (push_idx >= bit_cap) {
        PRP_Size new_bit_cap;
        if (CONT_BITMAP_MAX_BIT_CAP / 2 < bit_cap) {
            new_bit_cap = CONT_BITMAP_MAX_BIT_CAP;
        } else {
            new_bit_cap = bit_cap * 2;
        }
        PRP_Result code = CONT_BitmapChangeSizeUnchecked(
            pLayout->pFree_chunk_bitset, new_bit_cap);
        if (code != PRP_OK) {
            free(pChunk);
            return code;
        }
    }
    PRP_Result code = CONT_ArrPushUnchecked(pLayout->pChunk_ptrs, &pChunk);
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
    CONT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset, push_idx);

    return PRP_OK;
}

static PRP_Result LayoutInitInternals(FECS_Layout *pLayout) {
    PRP_Size comps_len, comp_set_cap, comp_set_bit_cap;
    const PRP_Size *pComp_sizes =
        CONT_ArrRawUnchecked(g_ctx->pComp_sizes, &comps_len);
    const CONT_Bitword *pBitwords = CONT_BitmapRawUnchecked(
        pLayout->pComp_set, &comp_set_cap, &comp_set_bit_cap);

    pLayout->pWord_prefix_popcnts[0] = 0;
    PRP_Size *pStride_dest = &pLayout->pComp_arr_strides[0];
    PRP_Size stride = 0;
    for (PRP_Size i = 0, j = 0; i < comp_set_cap; i++) {
        CONT_Bitword word = pBitwords[i];
        if (i < comp_set_cap - 1) {
            pLayout->pWord_prefix_popcnts[i + 1] =
                pLayout->pWord_prefix_popcnts[i] +
                (PRP_U16)CONT_BitwordPopCnt(word);
        }
        while (word) {
            PRP_Size comp_id = CONT_BitwordFFS(word) + j;
            *pStride_dest = stride;
            pStride_dest++;
            stride += pComp_sizes[comp_id] * CHUNK_CAP;

            word &= word - 1;
        }
        j += sizeof(CONT_Bitword) * 8;
    }
    pLayout->chunk_total_size = stride + sizeof(FECS_Chunk);

    return PRP_OK;
}

static PRP_Result ChunkPtrDelCb(void *ppChunk, void *_) {
    (void)_;
    FECS_Chunk *pChunk = *(FECS_Chunk **)ppChunk;

    free(pChunk);

    return PRP_OK;
}

PRP_Result LayoutCreate(CONT_Bitmap *pCreate_info, FECS_Layout *pLayout) {
    *pLayout = (FECS_Layout){0};
    pLayout->pComp_set = pCreate_info;

    PRP_Result code = CONT_ArrCreateUnchecked(
        sizeof(FECS_Chunk *), CONT_ARR_DEFAULT_CAP, &pLayout->pChunk_ptrs);
    if (code != PRP_OK) {
        goto err_path;
    }
    code = CONT_BitmapCreateUnchecked(CONT_ARR_DEFAULT_CAP,
                                      &pLayout->pFree_chunk_bitset);
    if (code != PRP_OK) {
        goto err_path;
    }
    pLayout->pComp_arr_strides =
        malloc(sizeof(PRP_Size) * CONT_BitmapSetCount(pCreate_info));
    if (!pLayout->pComp_arr_strides) {
        code = PRP_ERR_OOM;
        goto err_path;
    }
    pLayout->pWord_prefix_popcnts =
        malloc(sizeof(PRP_U16) * (WORD_I(CONT_BitmapBitCap(pCreate_info)) + 1));
    if (!pLayout->pWord_prefix_popcnts) {
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
        CONT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ChunkPtrDelCb, NULL);
        CONT_ArrDeleteUnchecked(&pLayout->pChunk_ptrs);
    }
    if (pLayout->pFree_chunk_bitset) {
        CONT_BitmapDeleteUnchecked(&pLayout->pFree_chunk_bitset);
    }
    CONT_BitmapDeleteUnchecked(&pLayout->pComp_set);
    if (pLayout->pComp_arr_strides) {
        free(pLayout->pComp_arr_strides);
    }
    if (pLayout->pWord_prefix_popcnts) {
        free(pLayout->pWord_prefix_popcnts);
    }

    return code;
}

void LayoutDelete(FECS_Layout *pLayout) {
    DIAG_ASSERT(pLayout != NULL);

    CONT_BitmapDeleteUnchecked(&pLayout->pComp_set);
    CONT_BitmapDeleteUnchecked(&pLayout->pFree_chunk_bitset);

    CONT_ArrForEachUnchecked(pLayout->pChunk_ptrs, ChunkPtrDelCb, NULL);
    CONT_ArrDeleteUnchecked(&pLayout->pChunk_ptrs);

    free(pLayout->pComp_arr_strides);
    free(pLayout->pWord_prefix_popcnts);

#ifdef PRP_DEBUG_MODE
    pLayout->pComp_arr_strides = NULL;
    pLayout->pWord_prefix_popcnts = NULL;
#endif
}

/* ----  ENTITIES ---- */

#define CHUNK(pLayout, chunk_idx)                                              \
    (*(FECS_Chunk **)CONT_ArrGetUnchecked((pLayout)->pChunk_ptrs, (chunk_idx)))

#define ENTITY_SLOT_MASK ((PRP_Size)63)
#define ENTITY_SLOT_BITS (6)
// Explicit encoding instead of just multiplying, to show intent.
#define ENTITY_IDX(chunk_idx, slot_idx)                                        \
    (((PRP_Size)(chunk_idx) << ENTITY_SLOT_BITS) |                             \
     ((PRP_Size)(slot_idx) & ENTITY_SLOT_MASK))

#define MAX_ENTITY_CAP(pLayout) (CONT_ArrLen(pLayout->pChunk_ptrs) * CHUNK_CAP)

/**
 * A chunk view is data upon a chunk's entire free slots allocated at once.
 */
typedef struct {
    PRP_Size chunk_idx;
    FECS_ChunkFreeSlotType occupied_slots;
    PRP_U32 gens[CHUNK_CAP];
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
static PRP_Result EntityGroupValidityCb(void *pVal, void *pUser_data);
/**
 * Kills entities of a chunk view of a entity group.
 *
 * @param pVal       A chunk view from entity batch.
 * @param pUser_data The layout the entities/chunk_views belong to.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if the chunk view contains invlaid entities.
 */
static PRP_Result EntityGroupKillCb(void *pVal, void *pUser_data);
/**
 * Iterates over entities of a chunk view of a entity group.
 *
 * @param pVal       A chunk view from entity batch.
 * @param pUser_data The iteration data containing all the context.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if the chunk view contains invlaid entities.
 */
static PRP_Result EntityGroupIterationCb(void *pVal, void *pUser_data);

PRP_Result EntitySpawn(FECS_World *pWorld, FECS_LayoutId layout_id,
                       FECS_EntityId *pEntity) {
    FECS_Layout *pLayout = &pWorld->pLayouts[layout_id];
    PRP_Size free_chunk_idx = CONT_BitmapFFS(pLayout->pFree_chunk_bitset);
    if (free_chunk_idx == PRP_INVALID_INDEX) {
        PRP_Result code = CreateChunk(pLayout);
        if (code != PRP_OK) {
            return code;
        }
        free_chunk_idx = CONT_BitmapFFS(pLayout->pFree_chunk_bitset);
    }
    FECS_Chunk *pChunk = CHUNK(pLayout, free_chunk_idx);
    FECS_ChunkFreeSlotType free_slot_idx =
        (FECS_ChunkFreeSlotType)CONT_BitwordFFS(
            (CONT_Bitword)pChunk->free_slot_bitset);
    pEntity->layout_id = layout_id;
    pEntity->gen = pChunk->gens[free_chunk_idx];
    pEntity->entity_idx = ENTITY_IDX(free_chunk_idx, free_slot_idx);
    PRP_BIT_CLR(pChunk->free_slot_bitset, BIT_MASK(free_chunk_idx));
    if (!pChunk->free_slot_bitset) {
        CONT_BitmapClrUnchecked(pLayout->pFree_chunk_bitset, free_chunk_idx);
    }

    return PRP_OK;
}

PRP_Result EntityGroupSpawn(FECS_World *pWorld, FECS_LayoutId layout_id,
                            PRP_Size entity_count,
                            FECS_EntityGroupId **ppGroup) {
    FECS_Layout *pLayout = &pWorld->pLayouts[layout_id];

    PRP_Size min_cap = (entity_count + CHUNK_CAP - 1) / CHUNK_CAP;
    FECS_EntityGroupId *pGroup = malloc(sizeof(FECS_EntityGroupId));
    if (!pGroup) {
        return PRP_ERR_OOM;
    }
    PRP_Result code = CONT_ArrCreateUnchecked(sizeof(ChunkView), min_cap,
                                              &pGroup->pChunk_views);
    if (code != PRP_OK) {
        free(pGroup);
        return code;
    }

    pGroup->layout_id = layout_id;
    PRP_Size alloc_count = 0;
    while (alloc_count != entity_count) {
        PRP_Size free_chunk_idx = CONT_BitmapFFS(pLayout->pFree_chunk_bitset);
        if (free_chunk_idx == PRP_INVALID_INDEX) {
            code = CreateChunk(pLayout);
            if (code != PRP_OK) {
                goto err_path;
            }
            free_chunk_idx = CONT_BitmapFFS(pLayout->pFree_chunk_bitset);
        }
        FECS_Chunk *pChunk = CHUNK(pLayout, free_chunk_idx);

        // This is correct since every free slot will now become occupied.
        FECS_ChunkFreeSlotType occupied_slots_mask = pChunk->free_slot_bitset;
        PRP_Size left = entity_count - alloc_count;
        FECS_ChunkFreeSlotType pop =
            (FECS_ChunkFreeSlotType)CONT_BitwordPopCnt(occupied_slots_mask);
        for (; pop > left; pop--) {
            occupied_slots_mask &= occupied_slots_mask - 1;
        }

        ChunkView view = {.chunk_idx = free_chunk_idx,
                          .occupied_slots = occupied_slots_mask};
        // Easier to copy the entire thing than parse it.
        memcpy(view.gens, pChunk->gens, CHUNK_CAP * sizeof(PRP_U32));

        code = CONT_ArrPushUnchecked(pGroup->pChunk_views, &view);
        if (code != PRP_OK) {
            goto err_path;
        }
        alloc_count += CONT_BitwordPopCnt(occupied_slots_mask);
        PRP_BIT_CLR(pChunk->free_slot_bitset, occupied_slots_mask);
        if (!pChunk->free_slot_bitset) {
            CONT_BitmapClrUnchecked(pLayout->pFree_chunk_bitset,
                                    free_chunk_idx);
        }
    }
    *ppGroup = pGroup;

    return PRP_OK;

err_path:
    if (alloc_count == 0) {
        CONT_ArrDeleteUnchecked(&pGroup->pChunk_views);
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

PRP_Bool EntityIsValid(FECS_World *pWorld, const FECS_EntityId entity) {
    if (entity.layout_id >= pWorld->layout_count) {
        return PRP_False;
    }
    FECS_Layout *pLayout = &pWorld->pLayouts[entity.layout_id];

    if (entity.entity_idx >= MAX_ENTITY_CAP(pLayout)) {
        return PRP_False;
    }
    PRP_Size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    PRP_U8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    if (pChunk->gens[slot_idx] != entity.gen ||
        PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot_idx))) {
        return PRP_False;
    }

    return PRP_True;
}

static PRP_Result EntityGroupValidityCb(void *pVal, void *pUser_data) {
    ChunkView *pChunk_view = pVal;
    FECS_Layout *pLayout = pUser_data;

    if (pChunk_view->chunk_idx >= CONT_ArrLen(pLayout->pChunk_ptrs)) {
        return PRP_ERR_INV_STATE;
    }
    FECS_Chunk *pChunk = CHUNK(pLayout, pChunk_view->chunk_idx);
    FECS_ChunkFreeSlotType mask = pChunk_view->occupied_slots;
    while (mask) {
        FECS_ChunkFreeSlotType slot =
            (FECS_ChunkFreeSlotType)CONT_BitwordCTZ(mask);
        if (pChunk_view->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot))) {
            return PRP_ERR_INV_STATE;
        }
        mask &= mask - 1;
    }

    return PRP_OK;
}

PRP_Bool EntityGroupIsValid(FECS_World *pWorld,
                            const FECS_EntityGroupId *pGroup) {
    if (pGroup->layout_id >= pWorld->layout_count) {
        return PRP_False;
    }
    FECS_Layout *pLayout = &pWorld->pLayouts[pGroup->layout_id];
    PRP_Result code = CONT_ArrForEachUnchecked(pGroup->pChunk_views,
                                               EntityGroupValidityCb, pLayout);

    return code == PRP_OK;
}

void EntityKill(FECS_World *pWorld, FECS_EntityId *pEntity) {
    FECS_Layout *pLayout = &pWorld->pLayouts[pEntity->layout_id];
    PRP_Size chunk_idx = pEntity->entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    PRP_U8 slot_idx = pEntity->entity_idx & ENTITY_SLOT_MASK;

    pChunk->gens[slot_idx]++;
    PRP_BIT_SET(pChunk->free_slot_bitset, BIT_MASK(slot_idx));
    CONT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset, chunk_idx);

    pEntity->layout_id = PRP_INVALID_INDEX;
    pEntity->entity_idx = PRP_INVALID_INDEX;
}

static PRP_Result EntityGroupKillCb(void *pVal, void *pUser_data) {
    ChunkView *pChunk_view = pVal;
    FECS_Layout *pLayout = pUser_data;

    if (pChunk_view->chunk_idx >= CONT_ArrLen(pLayout->pChunk_ptrs)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_Chunk *pChunk = CHUNK(pLayout, pChunk_view->chunk_idx);
    FECS_ChunkFreeSlotType mask = pChunk_view->occupied_slots;
    while (mask) {
        FECS_ChunkFreeSlotType slot =
            (FECS_ChunkFreeSlotType)CONT_BitwordCTZ(mask);
        if (pChunk_view->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot))) {
            if (mask != pChunk_view->occupied_slots) {
                // We deleted not all entities but now chunk has free spot.
                CONT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset,
                                        pChunk_view->chunk_idx);
            }
            return PRP_ERR_INV_ARG;
        }
        mask &= mask - 1;
        PRP_BIT_SET(pChunk->free_slot_bitset, BIT_MASK(slot));
        pChunk->gens[slot]++;
    }
    CONT_BitmapSetUnchecked(pLayout->pFree_chunk_bitset,
                            pChunk_view->chunk_idx);

    return PRP_OK;
}

PRP_Result EntityGroupKill(FECS_World *pWorld, FECS_EntityGroupId **ppGroup) {
    FECS_EntityGroupId *pGroup = *ppGroup;
    FECS_Layout *pLayout = &pWorld->pLayouts[pGroup->layout_id];
    PRP_Result code = CONT_ArrForEachUnchecked(pGroup->pChunk_views,
                                               EntityGroupKillCb, pLayout);
    if (code != PRP_OK) {
        return code;
    }
    CONT_ArrDeleteUnchecked(&pGroup->pChunk_views);
    free(pGroup);
    *ppGroup = NULL;

    return PRP_OK;
}

PRP_Result EntityGetComp(FECS_World *pWorld, const FECS_EntityId entity,
                         FECS_CompId comp_id, void **ppComp_ptr) {
    FECS_Layout *pLayout = &pWorld->pLayouts[entity.layout_id];
    if (!CONT_BitmapIsSetUnchecked(pLayout->pComp_set, comp_id)) {
        return PRP_ERR_INV_ARG;
    }

    PRP_Size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    PRP_U8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    PRP_Size comp_size =
        (*(PRP_Size *)CONT_ArrGetUnchecked(g_ctx->pComp_sizes, comp_id));

    PRP_Size _;
    const CONT_Bitword *pBitwords =
        CONT_BitmapRawUnchecked(pLayout->pComp_set, &_, &_);
    PRP_Size word_i = WORD_I(comp_id);
    PRP_Size prefix_popcnt = pLayout->pWord_prefix_popcnts[word_i];
    PRP_U16 rank_in_word = (PRP_U16)CONT_BitwordPopCnt(pBitwords[word_i] &
                                                       (BIT_MASK(comp_id) - 1));
    PRP_Size comp_stride =
        pLayout->pComp_arr_strides[prefix_popcnt + rank_in_word];

    *ppComp_ptr =
        (PRP_U8 *)pChunk->pChunk_mem + comp_stride + (slot_idx * comp_size);

    return PRP_OK;
}

PRP_Result EntitySetComp(FECS_World *pWorld, FECS_EntityId entity,
                         FECS_CompId comp_id, const void *pComp_data) {
    FECS_Layout *pLayout = &pWorld->pLayouts[entity.layout_id];
    if (!CONT_BitmapIsSetUnchecked(pLayout->pComp_set, comp_id)) {
        return PRP_ERR_INV_ARG;
    }

    PRP_Size chunk_idx = entity.entity_idx >> ENTITY_SLOT_BITS;
    FECS_Chunk *pChunk = CHUNK(pLayout, chunk_idx);
    PRP_U8 slot_idx = entity.entity_idx & ENTITY_SLOT_MASK;

    PRP_Size comp_size =
        (*(PRP_Size *)CONT_ArrGetUnchecked(g_ctx->pComp_sizes, comp_id));

    PRP_Size _;
    const CONT_Bitword *pBitwords =
        CONT_BitmapRawUnchecked(pLayout->pComp_set, &_, &_);
    PRP_Size word_i = WORD_I(comp_id);
    PRP_Size prefix_popcnt = pLayout->pWord_prefix_popcnts[word_i];
    PRP_U16 rank_in_word = (PRP_U16)CONT_BitwordPopCnt(pBitwords[word_i] &
                                                       (BIT_MASK(comp_id) - 1));
    PRP_Size comp_stride =
        pLayout->pComp_arr_strides[prefix_popcnt + rank_in_word];

    PRP_U8 *ptr =
        (PRP_U8 *)pChunk->pChunk_mem + comp_stride + (slot_idx * comp_size);
    memcpy(ptr, pComp_data, comp_size);

    return PRP_OK;
}

typedef struct {
    FECS_Layout *pLayout;
    PRP_Size comp_size;
    PRP_Size comp_stride;
    PRP_Result (*cb)(void *pComp_data, void *pUser_data);
    void *pUser_data;
} IterationData;

static PRP_Result EntityGroupIterationCb(void *pVal, void *pUser_data) {
    ChunkView *pChunk_view = pVal;
    IterationData *pI_data = pUser_data;

    if (pChunk_view->chunk_idx >= CONT_ArrLen(pI_data->pLayout->pChunk_ptrs)) {
        return PRP_ERR_INV_ARG;
    }
    FECS_Chunk *pChunk = CHUNK(pI_data->pLayout, pChunk_view->chunk_idx);
    FECS_ChunkFreeSlotType mask = pChunk_view->occupied_slots;
    while (mask) {
        FECS_ChunkFreeSlotType slot =
            (FECS_ChunkFreeSlotType)CONT_BitwordCTZ(mask);
        if (pChunk_view->gens[slot] != pChunk->gens[slot] ||
            PRP_BIT_IS_SET(pChunk->free_slot_bitset, BIT_MASK(slot))) {
            if (mask != pChunk_view->occupied_slots) {
                // We deleted not all entities but now chunk has free spot.
                CONT_BitmapSetUnchecked(pI_data->pLayout->pFree_chunk_bitset,
                                        pChunk_view->chunk_idx);
            }
            return PRP_ERR_INV_ARG;
        }
        mask &= mask - 1;

        PRP_U8 *ptr = (PRP_U8 *)pChunk->pChunk_mem + pI_data->comp_stride +
                      (slot * pI_data->comp_size);
        PRP_Result code = pI_data->cb(ptr, pI_data->pUser_data);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}

PRP_Result EntityGroupForEach(
    FECS_World *pWorld, FECS_EntityGroupId *pGroup, FECS_CompId comp_id,
    PRP_Result (*cb)(void *pComp_data, void *pUser_data), void *pUser_data) {
    IterationData i_data = {.cb = cb, .pUser_data = pUser_data};
    i_data.pLayout = &pWorld->pLayouts[pGroup->layout_id];
    if (!CONT_BitmapIsSetUnchecked(i_data.pLayout->pComp_set, comp_id)) {
        return PRP_ERR_INV_ARG;
    }

    i_data.comp_size =
        (*(PRP_Size *)CONT_ArrGetUnchecked(g_ctx->pComp_sizes, comp_id));

    PRP_Size _;
    const CONT_Bitword *pBitwords =
        CONT_BitmapRawUnchecked(i_data.pLayout->pComp_set, &_, &_);
    PRP_Size word_i = WORD_I(comp_id);
    PRP_Size prefix_popcnt = i_data.pLayout->pWord_prefix_popcnts[word_i];
    PRP_U16 rank_in_word = (PRP_U16)CONT_BitwordPopCnt(pBitwords[word_i] &
                                                       (BIT_MASK(comp_id) - 1));
    i_data.comp_stride =
        i_data.pLayout->pComp_arr_strides[prefix_popcnt + rank_in_word];

    return CONT_ArrForEachUnchecked(pGroup->pChunk_views,
                                    EntityGroupIterationCb, &i_data);
}
