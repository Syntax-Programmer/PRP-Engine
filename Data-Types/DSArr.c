#include "DSArr.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _DSArr {
    DT_size memb_size;
    /*
     * The callback used to free internal allocations of the elements of the
     * data we store in the data array. This can be DT_null if the array
     * elements don't have internal allocations.
     */
    PRP_Result (*elem_del_cb)(DT_void *pData_entry);
    /*
     * This holds the actual data the user stored.
     *
     * It also reverse maps the index of the data to an index into the id_layer
     * so that we can maintain the internal state and correctness.
     */
    struct {
        // The actual array the user stores the data into.
        DT_u8 *elems;
        // The reverse mapping table that maps back to id layer.
        DT_u32 *data_to_id_table;
        // The cap and len of both the arrays are ideally in sync.
        DT_u32 cap, len;
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
        DT_u64 *id_to_data_table;
        /*
         * This cap represents the actual cap of the id_to_data_table and also
         * the bit_cap of the free_slots table.
         *
         * The actual cap of the free slots can easily be derived from the cap
         * so it is not worth storing.
         */
        DT_u32 cap;
        /*
         * This is a variation of what DT_Pool uses internally called free_list.
         * This contrary to that implementation stores indices into the
         * id_to_data_table buffer and value at each index is the next free
         * index, until a sentinal value is hit marking the end of the chain.
         *
         * You can have a question in mind that, MEM_Pool is vulnerable to
         * double free, is it the same? And to that I will say both yes and no.
         * In technicality yes, but since this is internal bookkeeping, we can
         * extensively verify its validity through other checkes that indirectly
         * imply that this index chain is valid, in practice it is not
         * vulnerable to double free.
         */
        DT_u32 free_index;
        DT_u32 free_count;
    } id_layer;
};

/*
 * The DT_DSId also follows the same structure as id_layer.id_to_data_table.
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
#define INVALID_ID_LAYER_INDEX ((DT_u32)(-1))

#define UNPACK_INDEX(packed) ((DT_u32)(packed))
#define UNPACK_GEN(packed) ((DT_u32)(packed >> 32))
#define UNPACK_PACKED(packed, index, gen)                                      \
    do {                                                                       \
        (index) = UNPACK_INDEX(packed);                                        \
        (gen) = UNPACK_GEN(packed);                                            \
    } while (0)

#define PACK_UNPACKED(index, gen) (((DT_u64)(gen) << 32) | (DT_u64)(index))

#define INVARIANT_EXPR(ds_arr)                                                 \
    ((ds_arr) != DT_null && (ds_arr)->memb_size > 0 &&                         \
     (ds_arr)->data_layer.elems != DT_null &&                                  \
     (ds_arr)->data_layer.data_to_id_table != DT_null &&                       \
     (ds_arr)->data_layer.cap > 0 &&                                           \
     (ds_arr)->data_layer.len <= (ds_arr)->data_layer.cap &&                   \
     (ds_arr)->data_layer.cap <= DT_DS_ARR_MAX_CAP((ds_arr)->memb_size) &&     \
     (ds_arr)->id_layer.id_to_data_table != DT_null &&                         \
     (ds_arr)->id_layer.cap > 0 &&                                             \
     (ds_arr)->id_layer.cap <= DT_DS_ARR_MAX_CAP((ds_arr)->memb_size) &&       \
     (ds_arr)->id_layer.free_count <= (ds_arr)->id_layer.cap &&                \
     (ds_arr)->data_layer.len + (ds_arr)->id_layer.free_count ==               \
         (ds_arr)->id_layer.cap &&                                             \
     ((ds_arr)->id_layer.free_index == INVALID_ID_LAYER_INDEX ||               \
      (ds_arr)->id_layer.free_index < (ds_arr)->id_layer.cap) &&               \
     ((ds_arr)->id_layer.free_count == 0                                       \
          ? (ds_arr)->id_layer.free_index == INVALID_ID_LAYER_INDEX            \
          : 1))
#define ASSERT_INVARIANT_EXPR(ds_arr)                                          \
    DIAG_ASSERT_MSG(INVARIANT_EXPR(ds_arr),                                    \
                    "The given id array is either DT_null, or is corrupted.")

/**
 * Fetches the data that we can derive from an id, while simultaneously checking
 * for validity of the id.
 *
 * @param ds_arr: The array from which the id belongs from.
 * @param id: The id to index into the ds array.
 * @param pId_i: The index value packed inside the id itself.
 * @param pId_gen: The gen value packed inside the id itself.
 * @param pSlot_data_i: The index of the data the id actually refers to.
 * @param pSlot_gen: The actual current gen of the slot stored in the ds_arr.
 *
 * @return PRP_ERR_OOB if the id encodes an index that doesn't exist,
 * PRP_ERR_INV_STATE if the id is stale and freed earlier, otherwise PRP_OK.
 */
static inline PRP_Result GetIdData(const DT_DSArr *ds_arr, DT_DSId id,
                                   DT_u32 *pId_i, DT_u32 *pId_gen,
                                   DT_u32 *pSlot_data_i, DT_u32 *pSlot_gen);
/**
 * This is a helper function that deletes the data associated with the given id
 * and mark the id as invalid.
 *
 * @param ds_arr: The array to delete the element from.
 * @param pId: The id to delete.
 * @param id_i: The index encoded in the *pId.
 * @param slot_data_i: The index of the data that the id refers to.
 * @param slot_gen: The current gen of the slot in the ds_arr.
 */
static inline DT_void DelElem(DT_DSArr *ds_arr, DT_DSId *pId, DT_u32 id_i,
                              DT_u32 slot_data_i, DT_u32 slot_gen);
/**
 * Grows the data layer of a given ds array by <to_add> amount.
 *
 * @param ds_arr: The array to grow the data layer of.
 * @param to_add: The count to add the ds_arr's data layer by.
 *
 * @return PRP_ERR_RES_EXHAUSTED if the data layer is already at max cap,
 * PRP_ERR_OOM if the reallocation of the arrays fail, otherwise PRP_OK.
 */
static PRP_Result GrowDataLayer(DT_DSArr *ds_arr, DT_u32 to_add);
/**
 * Grows the id layer of a given ds array by <to_add> amount.
 *
 * @param ds_arr: The array to grow the id layer of.
 * @param to_add: The count to add the ds_arr's id layer by.
 *
 * @return PRP_ERR_RES_EXHAUSTED if the id layer is already at max cap,
 * PRP_ERR_OOM if the reallocation of the array fail, otherwise PRP_OK.
 */
static PRP_Result GrowIdLayer(DT_DSArr *ds_arr, DT_u32 to_add);

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrGetLastErrCode(DT_void) {
    return last_err_code;
}

#define ID_ARR_INIT_ERR_ROUTINE(x)                                             \
    do {                                                                       \
        if (!x) {                                                              \
            SET_LAST_ERR_CODE(PRP_ERR_OOM);                                    \
            goto free_internals;                                               \
        }                                                                      \
    } while (0);

PRP_FN_API DT_DSArr *PRP_FN_CALL DT_DSArrCreateUnchecked(
    DT_size memb_size, PRP_Result (*elem_del_cb)(DT_void *elem)) {
    DIAG_ASSERT(memb_size > 0);

    DT_DSArr *ds_arr = calloc(1, sizeof(DT_DSArr));
    if (!ds_arr) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    ds_arr->elem_del_cb = elem_del_cb;
    ds_arr->memb_size = memb_size;
    DT_u32 cap = PRP_MIN(DT_DS_ARR_MAX_CAP(memb_size), DT_DS_ARR_DEFAULT_CAP);

    ds_arr->data_layer.elems = malloc(memb_size * cap);
    ID_ARR_INIT_ERR_ROUTINE(ds_arr->data_layer.elems);
    ds_arr->data_layer.data_to_id_table = malloc(sizeof(DT_u32) * cap);
    ID_ARR_INIT_ERR_ROUTINE(ds_arr->data_layer.data_to_id_table);
    ds_arr->data_layer.cap = cap;
    ds_arr->data_layer.len = 0;

    ds_arr->id_layer.id_to_data_table = malloc(sizeof(DT_u64) * cap);
    ID_ARR_INIT_ERR_ROUTINE(ds_arr->id_layer.id_to_data_table);
    ds_arr->id_layer.cap = cap;
    // These set the gen to 0(the upper 32 bits), that is fine and intended.
    for (DT_u32 i = 0; i < cap - 1; i++) {
        ds_arr->id_layer.id_to_data_table[i] = i + 1;
    }
    ds_arr->id_layer.id_to_data_table[cap - 1] = INVALID_ID_LAYER_INDEX;
    ds_arr->id_layer.free_index = 0;
    ds_arr->id_layer.free_count = cap;

    return ds_arr;

free_internals:
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
    return DT_null;
}

PRP_FN_API DT_DSArr *PRP_FN_CALL DT_DSArrCreateChecked(
    DT_size memb_size, PRP_Result (*elem_del_cb)(DT_void *elem)) {
    if (!memb_size) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_DSArrCreateUnchecked(memb_size, elem_del_cb);
}

PRP_FN_API DT_void PRP_FN_CALL DT_DSArrDeleteUnchecked(DT_DSArr **pDs_arr) {
    DIAG_ASSERT(pDs_arr != DT_null);
    DIAG_ASSERT(*pDs_arr != DT_null);
    DIAG_ASSERT((*pDs_arr)->data_layer.elems != DT_null);
    DIAG_ASSERT((*pDs_arr)->data_layer.data_to_id_table != DT_null);
    DIAG_ASSERT((*pDs_arr)->id_layer.id_to_data_table != DT_null);

    DT_DSArr *ds_arr = *pDs_arr;

    if (ds_arr->elem_del_cb) {
        for (DT_u32 i = 0; i < ds_arr->data_layer.len; i++) {
            DT_void *ptr = ds_arr->data_layer.elems + (i * ds_arr->memb_size);
            ds_arr->elem_del_cb(ptr);
        }
    }
    free(ds_arr->data_layer.elems);
    free(ds_arr->data_layer.data_to_id_table);
    free(ds_arr->id_layer.id_to_data_table);

#if !defined(PRP_NDEBUG)
    ds_arr->memb_size = 0;
    ds_arr->data_layer.elems = DT_null;
    ds_arr->data_layer.data_to_id_table = DT_null;
    ds_arr->data_layer.cap = ds_arr->data_layer.len = 0;
    ds_arr->id_layer.id_to_data_table = DT_null;
    ds_arr->id_layer.cap = ds_arr->id_layer.free_index =
        ds_arr->id_layer.free_count = 0;
    ds_arr->elem_del_cb = DT_null;
#endif

    free(ds_arr);
    *pDs_arr = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrDeleteChecked(DT_DSArr **pDs_arr) {
    if (!pDs_arr || !(*pDs_arr)) {
        return PRP_ERR_INV_ARG;
    }
    if (!(*pDs_arr)->data_layer.elems ||
        !(*pDs_arr)->data_layer.data_to_id_table ||
        !(*pDs_arr)->id_layer.id_to_data_table) {
        return PRP_ERR_INV_ARG;
    }

    DT_DSArrDeleteUnchecked(pDs_arr);

    return PRP_OK;
}

PRP_FN_API DT_u32 PRP_FN_CALL DT_DSArrLenUnchecked(const DT_DSArr *ds_arr) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    return ds_arr->data_layer.len;
}

PRP_FN_API DT_u32 PRP_FN_CALL DT_DSArrLenChecked(const DT_DSArr *ds_arr) {
    if (!INVARIANT_EXPR(ds_arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_DS_INVALID_SIZE;
    }

    return ds_arr->data_layer.len;
}

static inline PRP_Result GetIdData(const DT_DSArr *ds_arr, DT_DSId id,
                                   DT_u32 *pId_i, DT_u32 *pId_gen,
                                   DT_u32 *pSlot_data_i, DT_u32 *pSlot_gen) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    DIAG_ASSERT(pId_i != DT_null);
    DIAG_ASSERT(pId_gen != DT_null);
    DIAG_ASSERT(pSlot_data_i != DT_null);
    DIAG_ASSERT(pSlot_gen != DT_null);

    UNPACK_PACKED(id, *pId_i, *pId_gen);
    if (*pId_i >= ds_arr->id_layer.cap) {
        return PRP_ERR_OOB;
    }
    DT_u64 id_val = ds_arr->id_layer.id_to_data_table[*pId_i];
    UNPACK_PACKED(id_val, *pSlot_data_i, *pSlot_gen);
    if (*pId_gen != *pSlot_gen || *pSlot_data_i >= ds_arr->data_layer.len) {
        return PRP_ERR_INV_STATE;
    }

    return PRP_OK;
}

PRP_FN_API DT_void *PRP_FN_CALL DT_DSIdToDataUnchecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    DT_u32 dummy1, dummy2, slot_data_i, dummy3;
    PRP_Result code =
        GetIdData(ds_arr, id, &dummy1, &dummy2, &slot_data_i, &dummy3);
    DIAG_ASSERT_MSG(code == PRP_OK,
                    "The given id is invalid or used after free.");

    return ds_arr->data_layer.elems + (ds_arr->memb_size * slot_data_i);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_DSIdToDataChecked(const DT_DSArr *ds_arr,
                                                     DT_DSId id) {
    if (!INVARIANT_EXPR(ds_arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    DT_u32 dummy1, dummy2, slot_data_i, dummy3;
    PRP_Result code =
        GetIdData(ds_arr, id, &dummy1, &dummy2, &slot_data_i, &dummy3);
    if (code != PRP_OK) {
        SET_LAST_ERR_CODE(code);
        return DT_null;
    }

    return ds_arr->data_layer.elems + (ds_arr->memb_size * slot_data_i);
}

PRP_FN_API DT_bool PRP_FN_CALL DT_DSIdIsValidUnchecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    DT_u32 dummy1, dummy2, dummy3, dummy4;
    PRP_Result code = GetIdData(ds_arr, id, &dummy1, &dummy2, &dummy3, &dummy4);

    return code == PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSIdIsValidChecked(const DT_DSArr *ds_arr,
                                                        DT_DSId id,
                                                        DT_bool *pRslt) {
    if (!INVARIANT_EXPR(ds_arr) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_DSIdIsValidUnchecked(ds_arr, id);

    return PRP_OK;
}

static PRP_Result GrowDataLayer(DT_DSArr *ds_arr, DT_u32 to_add) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    /*
     * This is not suseptible to overflow bugs since the max cap of an ds array
     * is hard capped at U32_MAX, so storing the new_cap in a u64 prevents any
     * overflow that can happen.
     */
    DT_u64 new_cap = (DT_u64)(ds_arr->data_layer.cap) + to_add;
    DT_u32 max_cap = DT_DS_ARR_MAX_CAP(ds_arr->memb_size);
    if (ds_arr->data_layer.cap == max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > max_cap) {
        new_cap = max_cap;
    }

    DT_void *elems =
        realloc(ds_arr->data_layer.elems, ds_arr->memb_size * new_cap);
    if (!elems) {
        return PRP_ERR_OOM;
    }
    DT_u32 *data_to_id_table =
        realloc(ds_arr->data_layer.data_to_id_table, sizeof(DT_u32) * new_cap);
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
    ds_arr->data_layer.cap = new_cap;

    return PRP_OK;
}

static PRP_Result GrowIdLayer(DT_DSArr *ds_arr, DT_u32 to_add) {
    ASSERT_INVARIANT_EXPR(ds_arr);

    /*
     * This is not suseptible to overflow bugs since the max cap of an ds array
     * is hard capped at U32_MAX, so storing the new_cap in a u64 prevents any
     * overflow that can happen.
     */
    DT_u64 new_cap = (DT_u64)(ds_arr->id_layer.cap) + to_add;
    DT_u32 max_cap = DT_DS_ARR_MAX_CAP(ds_arr->memb_size);
    if (ds_arr->id_layer.cap == max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > max_cap) {
        new_cap = max_cap;
    }

    DT_u64 *id_to_data_table =
        realloc(ds_arr->id_layer.id_to_data_table, sizeof(DT_u64) * new_cap);
    if (!id_to_data_table) {
        return PRP_ERR_OOM;
    }
    for (DT_u32 i = ds_arr->id_layer.cap; i < new_cap - 1; i++) {
        id_to_data_table[i] = i + 1;
    }
    id_to_data_table[new_cap - 1] = ds_arr->id_layer.free_index;
    ds_arr->id_layer.free_index = ds_arr->id_layer.cap;
    ds_arr->id_layer.free_count += new_cap - ds_arr->id_layer.cap;

    ds_arr->id_layer.id_to_data_table = id_to_data_table;
    ds_arr->id_layer.cap = new_cap;

    return PRP_OK;
}

PRP_FN_API DT_DSId PRP_FN_CALL DT_DSArrAddUnchecked(DT_DSArr *ds_arr,
                                                    DT_void *data) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    DIAG_ASSERT(data != DT_null);

    DT_u32 free_index = ds_arr->id_layer.free_index;
    if (free_index == INVALID_ID_LAYER_INDEX) {
        PRP_Result code = GrowIdLayer(ds_arr, ds_arr->data_layer.cap);
        if (code != PRP_OK) {
            SET_LAST_ERR_CODE(code);
            return DT_DS_INVALID_ID;
        }
        free_index = ds_arr->id_layer.free_index;
    }
    if (ds_arr->data_layer.len == ds_arr->data_layer.cap) {
        PRP_Result code = GrowDataLayer(ds_arr, ds_arr->id_layer.cap);
        if (code != PRP_OK) {
            SET_LAST_ERR_CODE(code);
            return DT_DS_INVALID_ID;
        }
    }
    ds_arr->id_layer.free_index = ds_arr->id_layer.id_to_data_table[free_index];

    DT_u32 len = ds_arr->data_layer.len;
    DT_void *dest = ds_arr->data_layer.elems + (len * ds_arr->memb_size);
    memcpy(dest, data, ds_arr->memb_size);
    ds_arr->data_layer.data_to_id_table[len] = free_index;

    DT_u32 curr_gen = UNPACK_GEN(ds_arr->id_layer.id_to_data_table[free_index]);
    ds_arr->id_layer.id_to_data_table[free_index] =
        PACK_UNPACKED(len, curr_gen);
    ds_arr->data_layer.len++;
    ds_arr->id_layer.free_count--;

    return PACK_UNPACKED(free_index, curr_gen);
}

PRP_FN_API DT_DSId PRP_FN_CALL DT_DSArrAddChecked(DT_DSArr *ds_arr,
                                                  DT_void *data) {
    if (!INVARIANT_EXPR(ds_arr) || !data) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_DS_INVALID_ID;
    }

    return DT_DSArrAddUnchecked(ds_arr, data);
}

static inline DT_void DelElem(DT_DSArr *ds_arr, DT_DSId *pId, DT_u32 id_i,
                              DT_u32 slot_data_i, DT_u32 slot_gen) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    DIAG_ASSERT(pId != DT_null);
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
    DT_u32 len = ds_arr->data_layer.len;
    DT_void *to_del =
        ds_arr->data_layer.elems + (slot_data_i * ds_arr->memb_size);
    DT_void *last_elem =
        ds_arr->data_layer.elems + ((len - 1) * ds_arr->memb_size);
    DT_u32 last_elem_id_i = ds_arr->data_layer.data_to_id_table[len - 1];

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
        DT_u32 gen =
            UNPACK_GEN(ds_arr->id_layer.id_to_data_table[last_elem_id_i]);
        ds_arr->id_layer.id_to_data_table[last_elem_id_i] =
            PACK_UNPACKED(slot_data_i, gen);
    }
    ds_arr->data_layer.len--;
    ds_arr->id_layer.free_count++;

    *pId = DT_DS_INVALID_ID;
}

PRP_FN_API DT_void PRP_FN_CALL DT_DSArrDelElemUnchecked(DT_DSArr *ds_arr,
                                                        DT_DSId *pId) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    DIAG_ASSERT(pId != DT_null);

    DT_u32 id_i, dummy1, slot_data_i, slot_gen;
    PRP_Result code =
        GetIdData(ds_arr, *pId, &id_i, &dummy1, &slot_data_i, &slot_gen);
    DIAG_ASSERT_MSG(code == PRP_OK,
                    "The given id is invalid or used after free.");

    DelElem(ds_arr, pId, id_i, slot_data_i, slot_gen);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrDelElemChecked(DT_DSArr *ds_arr,
                                                         DT_DSId *pId) {
    if (!INVARIANT_EXPR(ds_arr) || !pId) {
        return PRP_ERR_INV_ARG;
    }

    DT_u32 id_i, dummy1, slot_data_i, slot_gen;
    PRP_Result code =
        GetIdData(ds_arr, *pId, &id_i, &dummy1, &slot_data_i, &slot_gen);
    if (code != PRP_OK) {
        return code;
    }

    DelElem(ds_arr, pId, id_i, slot_data_i, slot_gen);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrReserveUnchecked(DT_DSArr *ds_arr,
                                                           DT_u32 count) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    DIAG_ASSERT(count > 0);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrReserveChecked(DT_DSArr *ds_arr,
                                                         DT_u32 count) {
    if (!INVARIANT_EXPR(ds_arr) || !count) {
        return PRP_ERR_INV_ARG;
    }

    return DT_DSArrReserveUnchecked(ds_arr, count);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrForEachUnchecked(
    DT_DSArr *ds_arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data) {
    ASSERT_INVARIANT_EXPR(ds_arr);
    DIAG_ASSERT(cb != DT_null);

    DT_u8 *mem = ds_arr->data_layer.elems;
    for (DT_u32 i = 0; i < ds_arr->data_layer.len; i++) {
        PRP_Result code = cb(mem, user_data);
        if (code != PRP_OK) {
            return code;
        }
        mem += ds_arr->memb_size;
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrForEachChecked(
    DT_DSArr *ds_arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data) {
    if (!INVARIANT_EXPR(ds_arr) || !cb) {
        return PRP_ERR_INV_ARG;
    }

    return DT_DSArrForEachUnchecked(ds_arr, cb, user_data);
}
