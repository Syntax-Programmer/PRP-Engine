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

static DT_void CalcCompArStrides(Layout *layout) {
    DT_size size_len;
    DT_size *sizes = DT_ArrRaw(g_state->comp_registry.comp_sizes, &size_len);
    DT_size word_cap, bit_cap;
    DT_Bitword *behaviors = DT_BitmapRaw(layout->b_set, &word_cap, &bit_cap);
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
            layout->comp_arr_strides[j++] = stride;
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
    if (DT_ArrPush(layout->chunk_ptrs, &chunk)) {
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
     * This also means starting gen of any entity is 255, nbot zero which is
     * fine since int wrap around is permitted.
     *
     *  We can use sizeof(Chunk) inthis bcuz the chunk data is a flex array memb
     * and doesn't count in the size of struct.
     */
    memset(chunk, 0XFF, sizeof(Chunk));
    DT_size push_idx = DT_ArrLen(layout->chunk_ptrs);
    DT_size bit_cap = DT_BitmapBitCap(layout->free_chunks);
    if (push_idx >= bit_cap &&
        DT_BitmapChangeSize(layout->free_chunks, bit_cap * 2) !=
            PRP_FN_SUCCESS) {
        PRP_LOG_FN_MALLOC_ERROR(layout->free_chunks);
        DT_ArrPop(layout->chunk_ptrs, DT_null);
        free(chunk);
        return PRP_FN_MALLOC_ERROR;
    }
    // Marking the new chunk as free
    DT_BitmapSet(layout->free_chunks, push_idx);

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

CORE_Id LayoutCreate(CORE_Id b_set_id) {
    DT_Bitmap *b_set = CORE_IdToData(g_state->behavior_set_id_mgr, b_set_id);
    if (!b_set) {
        PRP_LOG_FN_INV_ARG_ERROR(b_set_id);
        return CORE_INVALID_ID;
    }
    if (!DT_BitmapSetCount(b_set)) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Attach components to the behavior set before trying "
                        "to create a layout with it.");
        return CORE_INVALID_ID;
    }

    DT_size len;
    Layout *existing = CORE_IdMgrRaw(g_state->layout_id_mgr, &len);
    for (DT_size i = 0; i < len; i++) {
        DT_bool rslt;
        if (DT_BitmapCmp(existing[i].b_set, b_set, &rslt) == PRP_FN_SUCCESS &&
            rslt) {
            /*TODO: Instead of throwing error return a valid id. */
            PRP_LOG_FN_CODE(
                PRP_FN_INV_ARG_ERROR,
                "Layout with the same behavior set already exists.");
            return CORE_INVALID_ID;
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
    if (AddLayoutChunk(&layout)) {
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
    PRP_NULL_ARG_CHECK(pLayout_id, PRP_FN_INV_ARG_ERROR);

    // We don't do id validation since the below function will do it anyways.
    return CORE_IdMgrDeleteData(g_state->layout_id_mgr, pLayout_id);
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
        DT_size len;
        DT_void **chunks = DT_ArrRaw(l->chunk_ptrs, &len);
        for (DT_size i = 0; i < len; i++) {
            free(chunks[i]);
        }
        DT_ArrDelete(&l->chunk_ptrs);
    }
    l->chunk_size = 0;

    return PRP_FN_SUCCESS;
}

// DT_u64 LayoutGetSlot(CORE_Id layout_id);
// PRP_FnCode LayoutFreeSlot(CORE_Id layout_id, DT_size chunk_i, DT_u8 slot);
// PRP_FnCode LayoutIsEntityIdValid(CORE_Id layout_id, DT_size chunk_i, DT_u8
// slot, DT_u8 gen);
