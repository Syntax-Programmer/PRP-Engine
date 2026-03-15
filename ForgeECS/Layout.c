#include "../Diagnostics/Assert.h"
#include "Internals.h"
#include <string.h>

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

    World *world = DT_DSIdToDataChecked(g_ctx->worlds, world_id);
    if (!world) {
        PRP_Result code = DT_DSArrGetLastErrCode();
        DIAG_ASSERT(code != PRP_ERR_INV_ARG);
        if (code == PRP_ERR_INV_STATE || code == PRP_ERR_OOB) {
            SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        } else if (code != PRP_OK) {
            SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
        }
        return PRP_INVALID_INDEX;
    }
    DIAG_ASSERT(behavior_idx < DT_ArrLenUnchecked(g_ctx->behaviors));

    DT_size len;
    const Layout *layouts = DT_ArrRawUnchecked(world->layouts, &len);
    for (DT_size i = 0; i < len; i++) {
        if (behavior_idx == layouts[i].behavior_idx) {
            return i;
        }
    }
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

    return len;
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
