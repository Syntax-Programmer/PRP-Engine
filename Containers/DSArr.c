#include "DSArr.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct CONT_DSArr {
    PRP_Size memb_size;
    /*
     * The callback used to free internal allocations of the elements of the
     * data we store in the data array. This can be NULL if the array
     * elements don't have internal allocations.
     */
    PRP_Result (*elem_del_cb)(void *pData_entry);
    /*
     * This holds the actual data the user stored.
     *
     * It also reverse maps the index of the data to an index into the id_layer
     * so that we can maintain the internal state and correctness.
     */
    struct {
        // The actual array the user stores the data into.
        PRP_U8 *elems;
        // The reverse mapping table that maps back to id layer.
        PRP_U32 *data_to_id_table;
        // The cap and len of both the arrays are ideally in sync.
        PRP_U32 cap, len;
    } data_layer;
    /*
     * This layers helps decode the id the user provides us into usable index in
     * the data layer to get to the data.
     */
    struct {
        /*
         * Maps the user given ids to indices into the data layer.
         * This stores:
         *
         *    <gen>      <index>
         * [Bit 32-64] [Bit 0-31]
         * Where <index> = 0XFFFFFFFF means an invalid value.
         *
         * The <gen> is the current gen of the slot in this table. This is what
         * prevents stale references.
         * The <index> is the index into the data layer to access the data.
         */
        PRP_U64 *id_to_data_table;
        /*
         * This cap represents the actual cap of the id_to_data_table and also
         * the bit_cap of the free_slots table.
         *
         * The actual cap of the free slots can easily be derived from the cap
         * so it is not worth storing.
         */
        PRP_U32 cap;
        /*
         * This is a variation of what CONT_Pool uses internally called
         * free_list. This contrary to that implementation stores indices into
         * the id_to_data_table buffer and value at each index is the next free
         * index, until a sentinal value is hit marking the end of the chain.
         *
         * You can have a question in mind that, MEM_Pool is vulnerable to
         * double free, is it the same? And to that I will say both yes and no.
         * In technicality yes, but since this is internal bookkeeping, we can
         * extensively verify its validity through other checkes that indirectly
         * imply that this index chain is valid, in practice it is not
         * vulnerable to double free.
         */
        PRP_U32 free_index;
        PRP_U32 free_count;
    } id_layer;
};

/*
 * The CONT_DSId also follows the same structure as id_layer.id_to_data_table.
 * This stores:
 *
 *    <gen>      <index>
 * [Bit 32-64] [Bit 0-31]
 * Where <index> = 0XFFFFFFFF means an invalid value.
 *
 * The <gen> is the gen of the slot at the time of creation of the id.
 * The <index> is the index into the id layer that validates the id before
 * letting it go through to the id.
 */

/*
 * If this value is encountered through an id, we know that the id was invalid
 * since the slot is not in use.
 * If this value is enountered throught free index we know it marks the end of
 * the free chain.
 * Either way it is invalid, and by invariants, only one of this shall exists in
 * the id_layer at all times(the one marking the end of the free chain). This is
 * because a slot is either actively in use or in the free index chain.
 */
#define INVALID_ID_LAYER_INDEX ((PRP_U32)(-1))

#define UNPACK_INDEX(packed) ((PRP_U32)(packed))
#define UNPACK_GEN(packed) ((PRP_U32)(packed >> 32))
#define UNPACK_PACKED(packed, index, gen)                                      \
    do {                                                                       \
        (index) = UNPACK_INDEX(packed);                                        \
        (gen) = UNPACK_GEN(packed);                                            \
    } while (0)

#define PACK_UNPACKED(index, gen) (((PRP_U64)(gen) << 32) | (PRP_U64)(index))

#define ASSERT_INVARIANT_EXPR(ds_arr)                                          \
    PRP_DIAG_ASSERT_MSG(CONT_DSArrIsValid(ds_arr),                             \
                        "The given id array is either NULL, or is corrupted.")

/**
 * Fetches the data that we can derive from an id, while simultaneously checking
 * for validity of the id.
 *
 * @param ds_arr       The array from which the id belongs from.
 * @param id           The id to index into the ds array.
 * @param pId_i        The index value packed inside the id itself.
 * @param pId_gen      The gen value packed inside the id itself.
 * @param pSlot_data_i The index of the data the id actually refers to.
 * @param pSlot_gen    The actual current gen of the slot stored in the ds_arr.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if id encodes invalid index.
 * @return PRP_ERR_INV_STATE if the id is stale or freed earlier.
 */
static inline PRP_Result GetIdData(const CONT_DSArr *ds_arr, CONT_DSId id,
                                   PRP_U32 *pId_i, PRP_U32 *pId_gen,
                                   PRP_U32 *pSlot_data_i, PRP_U32 *pSlot_gen);
/**
 * This is a helper function that deletes the data associated with the given id
 * and mark the id as invalid.
 *
 * @param ds_arr      The array to delete the element from.
 * @param pId         The id to delete.
 * @param id_i        The index encoded in the *pId.
 * @param slot_data_i The index of the data that the id refers to.
 * @param slot_gen    The current gen of the slot in the ds_arr.
 */
static inline void DelElem(CONT_DSArr *ds_arr, CONT_DSId *pId, PRP_U32 id_i,
                           PRP_U32 slot_data_i, PRP_U32 slot_gen);
/**
 * Grows the data layer of a given ds array by <to_add> amount.
 *
 * @param ds_arr The array to grow the data layer of.
 * @param to_add The count to add the ds_arr's data layer by.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result GrowDataLayer(CONT_DSArr *ds_arr, PRP_U32 to_add);
/**
 * Grows the id layer of a given ds array by <to_add> amount.
 *
 * @param ds_arr The array to grow the id layer of.
 * @param to_add The count to add the ds_arr's id layer by.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result GrowIdLayer(CONT_DSArr *ds_arr, PRP_U32 to_add);

PRP_API PRP_Bool PRP_CALL CONT_DSArrIsValid(const CONT_DSArr *ds_arr) {
    return (ds_arr != NULL && ds_arr->memb_size > 0 &&
            ds_arr->data_layer.elems != NULL &&
            ds_arr->data_layer.data_to_id_table != NULL &&
            ds_arr->data_layer.cap > 0 &&
            ds_arr->data_layer.len <= ds_arr->data_layer.cap &&
            ds_arr->data_layer.cap <= CONT_DS_ARR_MAX_CAP(ds_arr->memb_size) &&
            ds_arr->id_layer.id_to_data_table != NULL &&
            ds_arr->id_layer.cap > 0 &&
            ds_arr->id_layer.cap <= CONT_DS_ARR_MAX_CAP(ds_arr->memb_size) &&
            ds_arr->id_layer.free_count <= ds_arr->id_layer.cap &&
            ds_arr->data_layer.len + ds_arr->id_layer.free_count ==
                ds_arr->id_layer.cap &&
            (ds_arr->id_layer.free_index == INVALID_ID_LAYER_INDEX ||
             ds_arr->id_layer.free_index < ds_arr->id_layer.cap) &&
            (ds_arr->id_layer.free_count == 0
                 ? ds_arr->id_layer.free_index == INVALID_ID_LAYER_INDEX
                 : 1));
}

#define ID_ARR_INIT_ERR_ROUTINE(x)                                             \
    do {                                                                       \
        if (!x) {                                                              \
            goto err_path;                                                     \
        }                                                                      \
    } while (0);

PRP_API PRP_Result PRP_CALL CONT_DSArrCreateUnchecked(
    PRP_Size memb_size, PRP_Result (*elem_del_cb)(void *elem),
    CONT_DSArr **pDs_arr) {
    PRP_DIAG_ASSERT(memb_size > 0);
    PRP_DIAG_ASSERT(pDs_arr != NULL);

    CONT_DSArr *ds_arr = calloc(1, sizeof(CONT_DSArr));
    if (!ds_arr) {
        return PRP_ERR_OOM;
    }
    ds_arr->elem_del_cb = elem_del_cb;
    ds_arr->memb_size = memb_size;
    PRP_U32 cap =
        PRP_MIN(CONT_DS_ARR_MAX_CAP(memb_size), CONT_DS_ARR_DEFAULT_CAP);

    ds_arr->data_layer.elems = malloc(memb_size * cap);
    ID_ARR_INIT_ERR_ROUTINE(ds_arr->data_layer.elems);
    ds_arr->data_layer.data_to_id_table = malloc(sizeof(PRP_U32) * cap);
    ID_ARR_INIT_ERR_ROUTINE(ds_arr->data_layer.data_to_id_table);
    ds_arr->data_layer.cap = cap;
    ds_arr->data_layer.len = 0;

    ds_arr->id_layer.id_to_data_table = malloc(sizeof(PRP_U64) * cap);
    ID_ARR_INIT_ERR_ROUTINE(ds_arr->id_layer.id_to_data_table);
    ds_arr->id_layer.cap = cap;
    // These set the gen to 0(the upper 32 bits), that is fine and intended.
    for (PRP_U32 i = 0; i < cap - 1; i++) {
        ds_arr->id_layer.id_to_data_table[i] = i + 1;
    }
    ds_arr->id_layer.id_to_data_table[cap - 1] = INVALID_ID_LAYER_INDEX;
    ds_arr->id_layer.free_index = 0;
    ds_arr->id_layer.free_count = cap;

    *pDs_arr = ds_arr;

    return PRP_OK;

err_path:
    if (ds_arr->data_layer.elems) {
        free(ds_arr->data_layer.elems);
    }
    if (ds_arr->data_layer.data_to_id_table) {
        free(ds_arr->data_layer.data_to_id_table);
    }
    if (ds_arr->id_layer.id_to_data_table) {
        free(ds_arr->id_layer.id_to_data_table);
    }
    free(ds_arr);
    return PRP_ERR_OOM;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrCreateChecked(
    PRP_Size memb_size, PRP_Result (*elem_del_cb)(void *elem),
    CONT_DSArr **pDs_arr) {
    if (!memb_size || !pDs_arr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrCreateUnchecked(memb_size, elem_del_cb, pDs_arr);
}

PRP_API void PRP_CALL CONT_DSArrDeleteUnchecked(CONT_DSArr **pDs_arr) {
    PRP_DIAG_ASSERT(pDs_arr != NULL);
    PRP_DIAG_ASSERT(*pDs_arr != NULL);
    PRP_DIAG_ASSERT((*pDs_arr)->data_layer.elems != NULL);
    PRP_DIAG_ASSERT((*pDs_arr)->data_layer.data_to_id_table != NULL);
    PRP_DIAG_ASSERT((*pDs_arr)->id_layer.id_to_data_table != NULL);

    CONT_DSArr *ds_arr = *pDs_arr;

    if (ds_arr->elem_del_cb) {
        for (PRP_U32 i = 0; i < ds_arr->data_layer.len; i++) {
            void *ptr = ds_arr->data_layer.elems + (i * ds_arr->memb_size);
            ds_arr->elem_del_cb(ptr);
        }
    }
    free(ds_arr->data_layer.elems);
    free(ds_arr->data_layer.data_to_id_table);
    free(ds_arr->id_layer.id_to_data_table);

#ifdef PRP_DEBUG_MODE
    ds_arr->memb_size = 0;
    ds_arr->data_layer.elems = NULL;
    ds_arr->data_layer.data_to_id_table = NULL;
    ds_arr->data_layer.cap = ds_arr->data_layer.len = 0;
    ds_arr->id_layer.id_to_data_table = NULL;
    ds_arr->id_layer.cap = ds_arr->id_layer.free_index =
        ds_arr->id_layer.free_count = 0;
    ds_arr->elem_del_cb = NULL;
#endif

    free(ds_arr);
    *pDs_arr = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrDeleteChecked(CONT_DSArr **pDs_arr) {
    if (!pDs_arr || !(*pDs_arr)) {
        return PRP_ERR_INV_ARG;
    }
    if (!(*pDs_arr)->data_layer.elems ||
        !(*pDs_arr)->data_layer.data_to_id_table ||
        !(*pDs_arr)->id_layer.id_to_data_table) {
        return PRP_ERR_INV_ARG;
    }

    CONT_DSArrDeleteUnchecked(pDs_arr);

    return PRP_OK;
}

PRP_API PRP_U32 PRP_CALL CONT_DSArrLen(const CONT_DSArr *ds_arr) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    return ds_arr->data_layer.len;
}

static inline PRP_Result GetIdData(const CONT_DSArr *ds_arr, CONT_DSId id,
                                   PRP_U32 *pId_i, PRP_U32 *pId_gen,
                                   PRP_U32 *pSlot_data_i, PRP_U32 *pSlot_gen) {
    UNPACK_PACKED(id, *pId_i, *pId_gen);
    if (*pId_i >= ds_arr->id_layer.cap) {
        return PRP_ERR_OOB;
    }
    PRP_U64 id_val = ds_arr->id_layer.id_to_data_table[*pId_i];
    UNPACK_PACKED(id_val, *pSlot_data_i, *pSlot_gen);
    if (*pId_gen != *pSlot_gen || *pSlot_data_i >= ds_arr->data_layer.len) {
        return PRP_ERR_INV_STATE;
    }

    return PRP_OK;
}

PRP_API void *PRP_CALL CONT_DSIdToDataUnchecked(const CONT_DSArr *ds_arr,
                                                CONT_DSId id) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    // Initializing the slot_data_i doesn't change shit but we do it to satisfy
    // compiler.
    PRP_U32 dummy1, dummy2, slot_data_i = 0, dummy3;
    PRP_Result code =
        GetIdData(ds_arr, id, &dummy1, &dummy2, &slot_data_i, &dummy3);
    PRP_DIAG_ASSERT_MSG(code == PRP_OK,
                        "The given id is invalid or used after free.");
    (void)code;

    return ds_arr->data_layer.elems + (ds_arr->memb_size * slot_data_i);
}

PRP_API PRP_Result PRP_CALL CONT_DSIdToDataChecked(const CONT_DSArr *ds_arr,
                                                   CONT_DSId id, void **dest) {
    if (!CONT_DSArrIsValid(ds_arr) || !dest) {
        return PRP_ERR_INV_ARG;
    }

    PRP_U32 dummy1, dummy2, slot_data_i, dummy3;
    PRP_Result code =
        GetIdData(ds_arr, id, &dummy1, &dummy2, &slot_data_i, &dummy3);
    if (code != PRP_OK) {
        return code;
    }

    *dest = ds_arr->data_layer.elems + (ds_arr->memb_size * slot_data_i);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_DSIdIsValidUnchecked(const CONT_DSArr *ds_arr,
                                                    CONT_DSId id) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    PRP_U32 dummy1, dummy2, dummy3, dummy4;
    PRP_Result code = GetIdData(ds_arr, id, &dummy1, &dummy2, &dummy3, &dummy4);

    return code == PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSIdIsValidChecked(const CONT_DSArr *ds_arr,
                                                    CONT_DSId id,
                                                    PRP_Bool *pRslt) {
    if (!CONT_DSArrIsValid(ds_arr) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_DSIdIsValidUnchecked(ds_arr, id);

    return PRP_OK;
}

static PRP_Result GrowDataLayer(CONT_DSArr *ds_arr, PRP_U32 to_add) {
    /*
     * This is not suseptible to overflow bugs since the max cap of an ds array
     * is hard capped at U32_MAX, so storing the new_cap in a u64 prevents any
     * overflow that can happen.
     */
    PRP_U64 new_cap = (PRP_U64)(ds_arr->data_layer.cap) + to_add;
    PRP_U32 max_cap = CONT_DS_ARR_MAX_CAP(ds_arr->memb_size);
    if (ds_arr->data_layer.cap == max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > max_cap) {
        new_cap = max_cap;
    }

    void *elems =
        realloc(ds_arr->data_layer.elems, ds_arr->memb_size * new_cap);
    if (!elems) {
        return PRP_ERR_OOM;
    }
    PRP_U32 *data_to_id_table =
        realloc(ds_arr->data_layer.data_to_id_table, sizeof(PRP_U32) * new_cap);
    if (!data_to_id_table) {
        /*
         * The extra mem allocated to elems will be considered over
         * allocated(since the cap is still the same one) till we again try to
         * grow the data layer, during when that lost memory will be
         * regained(granted it doesn't fail for the second time.).
         */
        ds_arr->data_layer.elems = elems;
        return PRP_ERR_OOM;
    }

    ds_arr->data_layer.elems = elems;
    ds_arr->data_layer.data_to_id_table = data_to_id_table;
    ds_arr->data_layer.cap = (PRP_U32)new_cap;

    return PRP_OK;
}

static PRP_Result GrowIdLayer(CONT_DSArr *ds_arr, PRP_U32 to_add) {
    /*
     * This is not suseptible to overflow bugs since the max cap of an ds array
     * is hard capped at U32_MAX, so storing the new_cap in a u64 prevents any
     * overflow that can happen.
     */
    PRP_U64 new_cap = (PRP_U64)(ds_arr->id_layer.cap) + to_add;
    PRP_U32 max_cap = CONT_DS_ARR_MAX_CAP(ds_arr->memb_size);
    if (ds_arr->id_layer.cap == max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > max_cap) {
        new_cap = max_cap;
    }

    PRP_U64 *id_to_data_table =
        realloc(ds_arr->id_layer.id_to_data_table, sizeof(PRP_U64) * new_cap);
    if (!id_to_data_table) {
        return PRP_ERR_OOM;
    }
    for (PRP_U32 i = ds_arr->id_layer.cap; i < new_cap - 1; i++) {
        id_to_data_table[i] = i + 1;
    }
    id_to_data_table[new_cap - 1] = ds_arr->id_layer.free_index;
    ds_arr->id_layer.free_index = ds_arr->id_layer.cap;
    ds_arr->id_layer.free_count += (PRP_U32)new_cap - ds_arr->id_layer.cap;

    ds_arr->id_layer.id_to_data_table = id_to_data_table;
    ds_arr->id_layer.cap = (PRP_U32)new_cap;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrAddUnchecked(CONT_DSArr *ds_arr,
                                                   void *data, CONT_DSId *pId) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    PRP_DIAG_ASSERT(data != NULL);
    PRP_DIAG_ASSERT(pId != NULL);

    PRP_U32 free_index = ds_arr->id_layer.free_index;
    if (free_index == INVALID_ID_LAYER_INDEX) {
        PRP_Result code = GrowIdLayer(ds_arr, ds_arr->data_layer.cap);
        if (code != PRP_OK) {
            return code;
        }
        free_index = ds_arr->id_layer.free_index;
    }
    if (ds_arr->data_layer.len == ds_arr->data_layer.cap) {
        PRP_Result code = GrowDataLayer(ds_arr, ds_arr->id_layer.cap);
        if (code != PRP_OK) {
            return code;
        }
    }
    ds_arr->id_layer.free_index =
        (PRP_U32)ds_arr->id_layer.id_to_data_table[free_index];

    PRP_U32 len = ds_arr->data_layer.len;
    void *dest = ds_arr->data_layer.elems + (len * ds_arr->memb_size);
    memcpy(dest, data, ds_arr->memb_size);
    ds_arr->data_layer.data_to_id_table[len] = free_index;

    PRP_U32 curr_gen =
        UNPACK_GEN(ds_arr->id_layer.id_to_data_table[free_index]);
    ds_arr->id_layer.id_to_data_table[free_index] =
        PACK_UNPACKED(len, curr_gen);
    ds_arr->data_layer.len++;
    ds_arr->id_layer.free_count--;

    *pId = PACK_UNPACKED(free_index, curr_gen);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrAddChecked(CONT_DSArr *ds_arr, void *data,
                                                 CONT_DSId *pId) {
    if (!CONT_DSArrIsValid(ds_arr) || !data || !pId) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrAddUnchecked(ds_arr, data, pId);
}

static inline void DelElem(CONT_DSArr *ds_arr, CONT_DSId *pId, PRP_U32 id_i,
                           PRP_U32 slot_data_i, PRP_U32 slot_gen) {
    /*
     * NOTE: This function will corrupt the free index chain if there is a 2 ^32
     * generation wrap and there is still a stale id.
     * Although rare, it can still happen. But for now I won't handle for
     * intentional stupidity.
     */
    // Incrementing the gen of the deleted slot and marking it free.
    ds_arr->id_layer.id_to_data_table[id_i] =
        PACK_UNPACKED(ds_arr->id_layer.free_index, slot_gen + 1);
    ds_arr->id_layer.free_index = id_i;

    // Metadata for O(1) deletion.
    PRP_U32 len = ds_arr->data_layer.len;
    void *to_del = ds_arr->data_layer.elems + (slot_data_i * ds_arr->memb_size);
    void *last_elem =
        ds_arr->data_layer.elems + ((len - 1) * ds_arr->memb_size);
    PRP_U32 last_elem_id_i = ds_arr->data_layer.data_to_id_table[len - 1];

    if (ds_arr->elem_del_cb) {
        ds_arr->elem_del_cb(to_del);
    }
    // The condition is an early exit if last elem is removed.
    if (slot_data_i != len - 1) {
        // Packing the data_layer arrays by shifting last elem in the new hole.
        memcpy(to_del, last_elem, ds_arr->memb_size);
        ds_arr->data_layer.data_to_id_table[slot_data_i] =
            ds_arr->data_layer.data_to_id_table[len - 1];
        /*
         * Updating the id layer metadata of the last elem so it points to the
         * new data index after the packing.
         */
        PRP_U32 gen =
            UNPACK_GEN(ds_arr->id_layer.id_to_data_table[last_elem_id_i]);
        ds_arr->id_layer.id_to_data_table[last_elem_id_i] =
            PACK_UNPACKED(slot_data_i, gen);
    }
    ds_arr->data_layer.len--;
    ds_arr->id_layer.free_count++;

    *pId = CONT_DS_INVALID_ID;
}

PRP_API void PRP_CALL CONT_DSArrDelElemUnchecked(CONT_DSArr *ds_arr,
                                                 CONT_DSId *pId) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    PRP_DIAG_ASSERT(pId != NULL);

    // Initializing the doesn't change shit but we do it to satisfy compiler.
    PRP_U32 id_i = 0, dummy1, slot_data_i = 0, slot_gen = 0;
    PRP_Result code =
        GetIdData(ds_arr, *pId, &id_i, &dummy1, &slot_data_i, &slot_gen);
    PRP_DIAG_ASSERT_MSG(code == PRP_OK,
                        "The given id is invalid or used after free.");
    (void)code;

    DelElem(ds_arr, pId, id_i, slot_data_i, slot_gen);
}

PRP_API PRP_Result PRP_CALL CONT_DSArrDelElemChecked(CONT_DSArr *ds_arr,
                                                     CONT_DSId *pId) {
    if (!CONT_DSArrIsValid(ds_arr) || !pId) {
        return PRP_ERR_INV_ARG;
    }

    // Initializing doesn't change shit but we do it to satisfy compiler.
    PRP_U32 id_i = 0, dummy1, slot_data_i = 0, slot_gen = 0;
    PRP_Result code =
        GetIdData(ds_arr, *pId, &id_i, &dummy1, &slot_data_i, &slot_gen);
    if (code != PRP_OK) {
        return code;
    }

    DelElem(ds_arr, pId, id_i, slot_data_i, slot_gen);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrReserveUnchecked(CONT_DSArr *ds_arr,
                                                       PRP_U32 count) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    PRP_DIAG_ASSERT(count > 0);

    if (ds_arr->data_layer.cap - ds_arr->data_layer.len < count) {
        PRP_Result code = GrowDataLayer(ds_arr, count);
        if (code != PRP_OK) {
            return code;
        }
    }
    if (ds_arr->id_layer.free_count < count) {
        PRP_Result code = GrowIdLayer(ds_arr, count);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}
PRP_API PRP_Result PRP_CALL CONT_DSArrReserveChecked(CONT_DSArr *ds_arr,
                                                     PRP_U32 count) {
    if (!CONT_DSArrIsValid(ds_arr) || !count) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrReserveUnchecked(ds_arr, count);
}

PRP_API PRP_Result PRP_CALL CONT_DSArrForEachUnchecked(
    CONT_DSArr *ds_arr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    PRP_DIAG_ASSERT(cb != NULL);

    PRP_U8 *mem = ds_arr->data_layer.elems;
    for (PRP_U32 i = 0; i < ds_arr->data_layer.len; i++) {
        PRP_Result code = cb(mem, pUser_data);
        if (code != PRP_OK) {
            return code;
        }
        mem += ds_arr->memb_size;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrForEachChecked(
    CONT_DSArr *ds_arr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    if (!CONT_DSArrIsValid(ds_arr) || !cb) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrForEachUnchecked(ds_arr, cb, pUser_data);
}
