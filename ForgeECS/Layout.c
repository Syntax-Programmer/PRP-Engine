#include "Shared-Internals.h"
#include <string.h>

/**
 * Calculates the stride of each component array in the layout, and also set the
 * correct chunk size for the layout.
 *
 * @param layout: The layout to calculate the stride for.
 */
static DT_void CalcCompArStrides(Layout *layout);
/**
 * Initializes a new chunk for the layout.
 *
 * @param layout: The layout to create new chunk for.
 *
 * @return PRP_FN_MALLOC_ERROR if allocation fails, PRP_FN_RES_EXHAUSTED_ERROR
 * if the chunk arr can't hold anymore chunks, otherwise PRP_FN_SUCCESS.
 */
static PRP_FnCode AddLayoutChunk(Layout *layout);
/**
 * Converts a component ID to its corresponding index in the component array.
 *
 * @param layout: The layout to convert the component ID for.
 * @param comp_id: The component ID to convert.
 *
 * @return The index of the component stride array of the layout corresponding
 * to the component ID.
 */
static DT_size CompIdToCompArrStrideI(Layout *layout, FECS_CompId comp_id);
/**
 * Just a callback to free chunks pointers stored inside the layout.
 *
 * @param data: The arr should ideally give it Chunk **.
 *
 * @return PRP_FN_SUCCESS.
 */
static inline PRP_FnCode FreeChunkPtrs(DT_void *data);

static DT_void CalcCompArStrides(Layout *layout) {
    DT_size size_len;
    const DT_size *sizes =
        DT_ArrRaw(g_state->comp_registry.comp_sizes, &size_len);
    DT_size word_cap, bit_cap;
    const DT_Bitword *behaviors =
        DT_BitmapRaw(layout->b_set, &word_cap, &bit_cap);
    DT_size stride = 0;

    /*
     * Since word_cap and bit cap are very much in sync all the time, and the
     * api doesn't allow any bits to be set beyond bit_cap, it is fine to use
     * id_raw in such a way since we are not mutating it.
     */
    for (DT_size i = 0, j = 0; i < word_cap; i++) {
        DT_Bitword word = behaviors[i];
        while (word) {
            DT_Bitword mask = word & -word;
            layout->comp_arr_strides[j++] = stride * CHUNK_CAP;
            stride += sizes[DT_BitwordCTZ(mask) + (i * BITWORD_BITS)];
            word ^= mask;
        }
    }
    layout->chunk_size = (stride * CHUNK_CAP) + sizeof(Chunk);
}

static PRP_FnCode AddLayoutChunk(Layout *layout) {
    Chunk *chunk = malloc(layout->chunk_size);
    if (!chunk) {
        PRP_LOG_FN_MALLOC_ERROR(chunk);
        return PRP_FN_MALLOC_ERROR;
    }
    if (DT_ArrPush(layout->chunk_ptrs, &chunk) != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot accommodate a new slot for the new chunk.");
        free(chunk);
        return PRP_FN_RES_EXHAUSTED_ERROR;
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
    DT_size push_i = DT_ArrLen(layout->chunk_ptrs) - 1;
    DT_size bit_cap = DT_BitmapBitCap(layout->free_chunks);
    if (push_i >= bit_cap &&
        DT_BitmapChangeSize(layout->free_chunks, bit_cap * 2) !=
            PRP_FN_SUCCESS) {
        PRP_LOG_FN_MALLOC_ERROR(layout->free_chunks);
        DT_ArrPop(layout->chunk_ptrs, DT_null);
        free(chunk);
        return PRP_FN_MALLOC_ERROR;
    }
    // Marking the new chunk as free
    DT_BitmapSet(layout->free_chunks, push_i);

    return PRP_FN_SUCCESS;
}

#define LAYOUT_INIT_ERROR_CHECK(x)                                             \
    do {                                                                       \
        if (!x) {                                                              \
            LayoutDelCb(&layout);                                              \
            PRP_LOG_FN_MALLOC_ERROR(x);                                        \
            return CORE_INVALID_ID;                                            \
        }                                                                      \
    } while (0);

CORE_Id LayoutCreate(CORE_Id b_set_id, DT_bool *pIsDuplicate) {
    DT_Bitmap **pB_set = CORE_IdToData(g_state->b_set_id_mgr, b_set_id);
    if (!pB_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return CORE_INVALID_ID;
    }
    DT_Bitmap *b_set = *pB_set;
    if (!DT_BitmapSetCount(b_set)) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Attach components to the behavior set before trying "
                        "to create a layout with it.");
        return CORE_INVALID_ID;
    }

    *pIsDuplicate = DT_false;
    DT_u32 len;
    const Layout *existing = CORE_IdMgrRaw(g_state->layout_id_mgr, &len);
    for (DT_u32 i = 0; i < len; i++) {
        DT_bool rslt;
        if (DT_BitmapCmp(existing[i].b_set, b_set, &rslt) == PRP_FN_SUCCESS &&
            rslt) {
            *pIsDuplicate = DT_true;

            return CORE_DataIToId(g_state->layout_id_mgr, i);
        }
    }

    Layout layout = {0};

    // The number of components in the behavior set.
    layout.comp_arr_strides =
        malloc(sizeof(DT_size) * DT_BitmapSetCount(b_set));
    LAYOUT_INIT_ERROR_CHECK(layout.comp_arr_strides);
    layout.free_chunks = DT_BitmapCreateDefault();
    LAYOUT_INIT_ERROR_CHECK(layout.free_chunks);
    layout.chunk_ptrs = DT_ArrCreateDefault(sizeof(Chunk *));
    LAYOUT_INIT_ERROR_CHECK(layout.chunk_ptrs);
    layout.b_set = DT_BitmapClone(b_set);
    LAYOUT_INIT_ERROR_CHECK(layout.b_set);

    CalcCompArStrides(&layout);
    if (AddLayoutChunk(&layout) != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(PRP_FN_MALLOC_ERROR,
                        "Cannot add initial first chunk to the layout.");
        LayoutDelCb(&layout);
        return CORE_INVALID_ID;
    }

    CORE_Id layout_id = CORE_IdMgrAddData(g_state->layout_id_mgr, &layout);
    if (layout_id == CORE_INVALID_ID) {
        LayoutDelCb(&layout);
        return CORE_INVALID_ID;
    }

    return layout_id;
}

PRP_FnCode LayoutDelete(CORE_Id *pLayout_id) {
    // We don't do id validation since the below function will do it anyways.
    return CORE_IdMgrDeleteData(g_state->layout_id_mgr, pLayout_id);
}

static inline PRP_FnCode FreeChunkPtrs(DT_void *data) {
    Chunk **pChunk_ptr = data;
    free(*pChunk_ptr);

    return PRP_FN_SUCCESS;
}

PRP_FnCode LayoutDelCb(DT_void *layout) {
    PRP_NULL_ARG_CHECK(layout, PRP_FN_INV_ARG_ERROR);

    Layout *l = layout;
    if (l->comp_arr_strides) {
        free(l->comp_arr_strides);
        l->comp_arr_strides = DT_null;
    }
    if (l->b_set) {
        DT_BitmapDelete(&l->b_set);
    }
    if (l->free_chunks) {
        DT_BitmapDelete(&l->free_chunks);
    }
    if (l->chunk_ptrs) {
        DT_ArrForEach(l->chunk_ptrs, FreeChunkPtrs);
        DT_ArrDelete(&l->chunk_ptrs);
    }
    l->chunk_size = 0;

    return PRP_FN_SUCCESS;
}

// Utility macros for brevity and less points of failure if something changes.
#define CHUNK(layout, chunk_i)                                                 \
    (*(Chunk **)DT_ArrGet((layout)->chunk_ptrs, (chunk_i)))
#define SLOT_BIT_MASK(slot) (1U << (slot))

PRP_FnCode LayoutCreateEntity(CORE_Id layout_id, FECS_EntityId *entity_id) {
    PRP_NULL_ARG_CHECK(entity_id, PRP_FN_INV_ARG_ERROR);
    Layout *layout = CORE_IdToData(g_state->layout_id_mgr, layout_id);
    if (!layout) {
        PRP_LOG_FN_INV_ARG_ERROR(layout_id);
        return PRP_FN_INV_ARG_ERROR;
    }

    PRP_FnCode code;
    if (!DT_BitmapSetCount(layout->free_chunks) &&
        (code = AddLayoutChunk(layout)) != PRP_FN_SUCCESS) {
        return code;
    }

    entity_id->layout_id = layout_id;
    // Since we grew the layout earlier, the below ops can't fail.
    entity_id->chunk_i = DT_BitmapFFS(layout->free_chunks);
    Chunk *chunk = CHUNK(layout, entity_id->chunk_i);

    entity_id->slot = DT_BitwordFFS((DT_Bitword)chunk->free_slot);
    PRP_BIT_CLR(chunk->free_slot, SLOT_BIT_MASK(entity_id->slot));
    if (!chunk->free_slot) {
        // Removing the chunk if we just filled it up.
        DT_BitmapClr(layout->free_chunks, entity_id->chunk_i);
    }

    entity_id->gen = chunk->gens[entity_id->slot];

    return PRP_FN_SUCCESS;
}

PRP_FnCode LayoutDeleteEntity(FECS_EntityId *entity_id) {
    PRP_NULL_ARG_CHECK(entity_id, PRP_FN_INV_ARG_ERROR);
    if (entity_id->slot >= CHUNK_CAP) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id->slot);
        return PRP_FN_INV_ARG_ERROR;
    }
    Layout *layout =
        CORE_IdToData(g_state->layout_id_mgr, entity_id->layout_id);
    if (!layout) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id->layout_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    if (entity_id->chunk_i >= DT_ArrLen(layout->chunk_ptrs)) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id->chunk_i);
        return PRP_FN_INV_ARG_ERROR;
    }
    Chunk *chunk = CHUNK(layout, entity_id->chunk_i);
    if (chunk->gens[entity_id->slot] != entity_id->gen) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id->gen);
        return PRP_FN_INV_ARG_ERROR;
    }

    chunk->gens[entity_id->slot]++;
    PRP_BIT_SET(chunk->free_slot, SLOT_BIT_MASK(entity_id->slot));
    DT_BitmapSet(layout->free_chunks, entity_id->chunk_i);
    // This invalidates the entity Id.
    memset(entity_id, 0XFF, sizeof(FECS_EntityId));

    return PRP_FN_SUCCESS;
}

FECS_EntityIdBatch *LayoutCreateEntityBatch(CORE_Id layout_id, DT_size count) {
    if (!count) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "FECS_EntityIdBatch can't be made with count=0.");
        return DT_null;
    }
    Layout *layout = CORE_IdToData(g_state->layout_id_mgr, layout_id);
    if (!layout) {
        PRP_LOG_FN_INV_ARG_ERROR(layout_id);
        return DT_null;
    }

    FECS_EntityIdBatch *entity_batch = malloc(
        sizeof(FECS_EntityIdBatch) + (sizeof(FECS_EntityBatchData) * count));
    if (!entity_batch) {
        PRP_LOG_FN_MALLOC_ERROR(entity_batch);
        return DT_null;
    }
    entity_batch->layout_id = layout_id;
    entity_batch->count = count;
    for (DT_size i = 0; i < count; i++) {
        if (!DT_BitmapSetCount(layout->free_chunks)) {
            DT_size remaining = count - i;
            DT_size chunks_needed = (remaining + CHUNK_CAP - 1) / CHUNK_CAP;
            // Allocating all the needed chunk in bulk.
            for (DT_size j = 0; j < chunks_needed; j++) {
                if (AddLayoutChunk(layout) != PRP_FN_SUCCESS) {
                    if (j > 0) {
                        // Create entities of the chunks added so far.
                        break;
                    }
                    /*
                     * We don't throw an error here because going back and
                     * refreeing each entity is expensive.
                     */
                    PRP_LOG_FN_CODE(
                        PRP_FN_RES_EXHAUSTED_ERROR,
                        "Only able to create %zu entities out of %zu. "
                        "Partially created entity batch will be returned.",
                        i, count);
                    entity_batch->count = i + 1;
                    return entity_batch;
                }
            }
        }
        DT_size chunk_i = DT_BitmapFFS(layout->free_chunks);
        Chunk *chunk = CHUNK(layout, chunk_i);
        DT_u8 slot = DT_BitwordFFS((DT_Bitword)chunk->free_slot);
        PRP_BIT_CLR(chunk->free_slot, SLOT_BIT_MASK(slot));
        if (!chunk->free_slot) {
            // Removing the chunk if we just filled it up.
            DT_BitmapClr(layout->free_chunks, chunk_i);
        }
        entity_batch->entities[i].chunk_i = chunk_i;
        entity_batch->entities[i].slot = slot;
        entity_batch->entities[i].gen = chunk->gens[slot];
    }

    return entity_batch;
}

PRP_FnCode LayoutDeleteEntityBatch(FECS_EntityIdBatch **pEntity_batch) {
    PRP_NULL_ARG_CHECK(pEntity_batch, PRP_FN_INV_ARG_ERROR);
    FECS_EntityIdBatch *entity_batch = *pEntity_batch;
    PRP_NULL_ARG_CHECK(entity_batch, PRP_FN_INV_ARG_ERROR);
    Layout *layout =
        CORE_IdToData(g_state->layout_id_mgr, entity_batch->layout_id);
    if (!layout) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_batch->layout_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    DT_size chunks_len = DT_ArrLen(layout->chunk_ptrs);

    for (DT_size i = 0; i < entity_batch->count; i++) {
        DT_size chunk_i = entity_batch->entities[i].chunk_i;
        DT_u8 slot = entity_batch->entities[i].slot,
              gen = entity_batch->entities[i].gen;
        if (slot >= CHUNK_CAP) {
            PRP_LOG_FN_INV_ARG_ERROR(slot);
            continue;
        }
        if (chunk_i >= chunks_len) {
            PRP_LOG_FN_INV_ARG_ERROR(chunk_i);
            continue;
        }
        Chunk *chunk = CHUNK(layout, chunk_i);
        if (chunk->gens[slot] != gen) {
            PRP_LOG_FN_INV_ARG_ERROR(gen);
            continue;
        }
        chunk->gens[slot]++;
        PRP_BIT_SET(chunk->free_slot, SLOT_BIT_MASK(slot));
        DT_BitmapSet(layout->free_chunks, chunk_i);
    }
    free(entity_batch);
    *pEntity_batch = DT_null;

    return PRP_FN_SUCCESS;
}

static DT_size CompIdToCompArrStrideI(Layout *layout, FECS_CompId comp_id) {
    DT_size index = PRP_INVALID_INDEX;
    DT_size word_cap, bit_cap;
    const DT_Bitword *b_set_raw =
        DT_BitmapRaw(layout->b_set, &word_cap, &bit_cap);

    /*
     * Not respecting word cap and bit cap since the below is guaranteed to be
     * within bounds.
     */
    for (DT_size i = 0; i < WORD_I(comp_id); i++) {
        index += DT_BitwordPopCnt(b_set_raw[i]);
    }
    // Masking out higher bits and popcnt the bits below.
    index +=
        DT_BitwordPopCnt(b_set_raw[WORD_I(comp_id)] & (BIT_MASK(comp_id) - 1));

    return index;
}

PRP_FnCode LayoutEntityOperateComp(FECS_EntityId entity_id, FECS_CompId comp_id,
                                   PRP_FnCode (*fn)(DT_void *data,
                                                    DT_void *user_data),
                                   DT_void *user_data) {
    PRP_NULL_ARG_CHECK(fn, PRP_FN_INV_ARG_ERROR);
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);
    if (entity_id.slot >= CHUNK_CAP) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id.slot);
        return PRP_FN_INV_ARG_ERROR;
    }
    Layout *layout = CORE_IdToData(g_state->layout_id_mgr, entity_id.layout_id);
    if (!layout) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id.layout_id);
        return PRP_FN_INV_ARG_ERROR;
    }
    if (entity_id.chunk_i >= DT_ArrLen(layout->chunk_ptrs)) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id.chunk_i);
        return PRP_FN_INV_ARG_ERROR;
    }
    Chunk *chunk = CHUNK(layout, entity_id.chunk_i);
    if (chunk->gens[entity_id.slot] != entity_id.gen) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_id.gen);
        return PRP_FN_INV_ARG_ERROR;
    }

    DT_size comp_arr_stride =
        layout->comp_arr_strides[CompIdToCompArrStrideI(layout, comp_id)];
    DT_size comp_size =
        *(DT_size *)DT_ArrGet(g_state->comp_registry.comp_sizes, comp_id);
    DT_u8 *chunk_comp_arr = chunk->data + comp_arr_stride;
    DT_void *data = chunk_comp_arr + (entity_id.slot * comp_size);

    return fn(data, user_data);
}

PRP_FnCode LayoutEntityBatchOperateComp(
    FECS_EntityIdBatch *entity_batch, FECS_CompId comp_id,
    PRP_FnCode (*fn)(DT_void *data, DT_void *user_data), DT_void *user_data) {
    PRP_NULL_ARG_CHECK(fn, PRP_FN_INV_ARG_ERROR);
    COMP_ID_VALIDITY_CHECK(comp_id, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(entity_batch, PRP_FN_INV_ARG_ERROR);
    Layout *layout =
        CORE_IdToData(g_state->layout_id_mgr, entity_batch->layout_id);
    if (!layout) {
        PRP_LOG_FN_INV_ARG_ERROR(entity_batch->layout_id);
        return PRP_FN_INV_ARG_ERROR;
    }

    DT_size chunks_len = DT_ArrLen(layout->chunk_ptrs);
    DT_size comp_arr_stride =
        layout->comp_arr_strides[CompIdToCompArrStrideI(layout, comp_id)];
    DT_size comp_size =
        *(DT_size *)DT_ArrGet(g_state->comp_registry.comp_sizes, comp_id);

    for (DT_size i = 0; i < entity_batch->count; i++) {
        DT_size chunk_i = entity_batch->entities[i].chunk_i;
        DT_u8 slot = entity_batch->entities[i].slot,
              gen = entity_batch->entities[i].gen;
        if (slot >= CHUNK_CAP) {
            PRP_LOG_FN_INV_ARG_ERROR(slot);
            continue;
        }
        if (chunk_i >= chunks_len) {
            PRP_LOG_FN_INV_ARG_ERROR(chunk_i);
            continue;
        }
        Chunk *chunk = CHUNK(layout, chunk_i);
        if (chunk->gens[slot] != gen) {
            PRP_LOG_FN_INV_ARG_ERROR(gen);
            continue;
        }

        DT_u8 *chunk_comp_arr = chunk->data + comp_arr_stride;
        DT_void *data = chunk_comp_arr + (slot * comp_size);
        fn(data, user_data);
    }

    return PRP_FN_SUCCESS;
}
