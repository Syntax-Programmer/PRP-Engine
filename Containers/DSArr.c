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
    PRP_Result (*pElem_del_cb)(void *pData_entry);
    /*
     * This holds the actual data the user stored.
     *
     * It also reverse maps the index of the data to an index into the id_layer
     * so that we can maintain the internal state and correctness.
     */
    struct {
        // The actual array the user stores the data into.
        PRP_U8 *pElems;
        // The reverse mapping table that maps back to id layer.
        PRP_U32 *pData_to_id_table;
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
        PRP_U64 *pId_to_data_table;
        /*
         * This cap represents the actual cap of the pId_to_data_table and also
         * the bit_cap of the free_slots table.
         *
         * The actual cap of the free slots can easily be derived from the cap
         * so it is not worth storing.
         */
        PRP_U32 cap;
        /*
         * This is a variation of what CONT_Pool uses internally called
         * free_list. This contrary to that implementation stores indices into
         * the pId_to_data_table buffer and value at each index is the next free
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
 * The CONT_DSId also follows the same structure as id_layer.pId_to_data_table.
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

#define ASSERT_INVARIANT_EXPR(pDs_arr)                                         \
    PRP_DIAG_ASSERT_MSG(CONT_DSArrIsValid(pDs_arr),                            \
                        "The given pDs_arr is invalid.")

/**
 * Fetches the data that we can derive from an id, while simultaneously checking
 * for validity of the id.
 *
 * @param pDs_arr      The array from which the id belongs from.
 * @param id           The id to index into the ds array.
 * @param pId_i        The index value packed inside the id itself.
 * @param pId_gen      The gen value packed inside the id itself.
 * @param pSlot_data_i The index of the data the id actually refers to.
 * @param pSlot_gen    The actual current gen of the slot stored in the pDs_arr.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if id encodes invalid index.
 * @return PRP_ERR_INV_STATE if the id is stale or freed earlier.
 */
static inline PRP_Result GetIdData(const CONT_DSArr *pDs_arr, CONT_DSId id,
                                   PRP_U32 *pId_i, PRP_U32 *pId_gen,
                                   PRP_U32 *pSlot_data_i, PRP_U32 *pSlot_gen);
/**
 * This is a helper function that deletes the data associated with the given id
 * and mark the id as invalid.
 *
 * @param pDs_arr     The array to delete the element from.
 * @param pId         The id to delete.
 * @param id_i        The index encoded in the *pId.
 * @param slot_data_i The index of the data that the id refers to.
 * @param slot_gen    The current gen of the slot in the pDs_arr.
 */
static inline void DelElem(CONT_DSArr *pDs_arr, CONT_DSId *pId, PRP_U32 id_i,
                           PRP_U32 slot_data_i, PRP_U32 slot_gen);
/**
 * Grows the data layer of a given ds array by <to_add> amount.
 *
 * @param pDs_arr The array to grow the data layer of.
 * @param to_add  The count to add the pDs_arr's data layer by.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result GrowDataLayer(CONT_DSArr *pDs_arr, PRP_U32 to_add);
/**
 * Grows the id layer of a given ds array by <to_add> amount.
 *
 * @param pDs_arr The array to grow the id layer of.
 * @param to_add  The count to add the pDs_arr's id layer by.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result GrowIdLayer(CONT_DSArr *pDs_arr, PRP_U32 to_add);

PRP_API PRP_Bool PRP_CALL CONT_DSArrIsValid(const CONT_DSArr *pDs_arr) {
    return (pDs_arr != NULL && pDs_arr->memb_size > 0 &&
            pDs_arr->data_layer.pElems != NULL &&
            pDs_arr->data_layer.pData_to_id_table != NULL &&
            pDs_arr->data_layer.cap > 0 &&
            pDs_arr->data_layer.len <= pDs_arr->data_layer.cap &&
            pDs_arr->data_layer.cap <=
                CONT_DS_ARR_MAX_CAP(pDs_arr->memb_size) &&
            pDs_arr->id_layer.pId_to_data_table != NULL &&
            pDs_arr->id_layer.cap > 0 &&
            pDs_arr->id_layer.cap <= CONT_DS_ARR_MAX_CAP(pDs_arr->memb_size) &&
            pDs_arr->id_layer.free_count <= pDs_arr->id_layer.cap &&
            pDs_arr->data_layer.len + pDs_arr->id_layer.free_count ==
                pDs_arr->id_layer.cap &&
            (pDs_arr->id_layer.free_index == INVALID_ID_LAYER_INDEX ||
             pDs_arr->id_layer.free_index < pDs_arr->id_layer.cap) &&
            (pDs_arr->id_layer.free_count == 0
                 ? pDs_arr->id_layer.free_index == INVALID_ID_LAYER_INDEX
                 : 1));
}

#define ID_ARR_INIT_ERR_ROUTINE(x)                                             \
    do {                                                                       \
        if (!x) {                                                              \
            goto err_path;                                                     \
        }                                                                      \
    } while (0);

PRP_API PRP_Result PRP_CALL CONT_DSArrCreateUnchecked(
    PRP_Size memb_size, PRP_Result (*pElem_del_cb)(void *pElem),
    CONT_DSArr **ppDs_arr) {
    PRP_DIAG_ASSERT_MSG(memb_size > 0,
                        "The memb_size of the ds_arr must be > 0.");
    PRP_DIAG_ASSERT(ppDs_arr != NULL);

    *ppDs_arr = NULL;
    CONT_DSArr *pDs_arr = calloc(1, sizeof(CONT_DSArr));
    if (!pDs_arr) {
        return PRP_ERR_OOM;
    }
    pDs_arr->pElem_del_cb = pElem_del_cb;
    pDs_arr->memb_size = memb_size;
    PRP_U32 cap =
        PRP_MIN(CONT_DS_ARR_MAX_CAP(memb_size), CONT_DS_ARR_DEFAULT_CAP);

    pDs_arr->data_layer.pElems = malloc(memb_size * cap);
    ID_ARR_INIT_ERR_ROUTINE(pDs_arr->data_layer.pElems);
    pDs_arr->data_layer.pData_to_id_table = malloc(sizeof(PRP_U32) * cap);
    ID_ARR_INIT_ERR_ROUTINE(pDs_arr->data_layer.pData_to_id_table);
    pDs_arr->data_layer.cap = cap;
    pDs_arr->data_layer.len = 0;

    pDs_arr->id_layer.pId_to_data_table = malloc(sizeof(PRP_U64) * cap);
    ID_ARR_INIT_ERR_ROUTINE(pDs_arr->id_layer.pId_to_data_table);
    pDs_arr->id_layer.cap = cap;
    // These set the gen to 0(the upper 32 bits), that is fine and intended.
    for (PRP_U32 i = 0; i < cap - 1; i++) {
        pDs_arr->id_layer.pId_to_data_table[i] = i + 1;
    }
    pDs_arr->id_layer.pId_to_data_table[cap - 1] = INVALID_ID_LAYER_INDEX;
    pDs_arr->id_layer.free_index = 0;
    pDs_arr->id_layer.free_count = cap;

    *ppDs_arr = pDs_arr;

    return PRP_OK;

err_path:
    if (pDs_arr->data_layer.pElems) {
        free(pDs_arr->data_layer.pElems);
    }
    if (pDs_arr->data_layer.pData_to_id_table) {
        free(pDs_arr->data_layer.pData_to_id_table);
    }
    if (pDs_arr->id_layer.pId_to_data_table) {
        free(pDs_arr->id_layer.pId_to_data_table);
    }
    free(pDs_arr);
    return PRP_ERR_OOM;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrCreateChecked(
    PRP_Size memb_size, PRP_Result (*pElem_del_cb)(void *pElem),
    CONT_DSArr **ppDs_arr) {
    if (!memb_size || !ppDs_arr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrCreateUnchecked(memb_size, pElem_del_cb, ppDs_arr);
}

PRP_API void PRP_CALL CONT_DSArrDeleteUnchecked(CONT_DSArr **ppDs_arr) {
    PRP_DIAG_ASSERT(ppDs_arr != NULL);
    PRP_DIAG_ASSERT(*ppDs_arr != NULL);
    PRP_DIAG_ASSERT_MSG((*ppDs_arr)->data_layer.pElems != NULL &&
                            (*ppDs_arr)->data_layer.pData_to_id_table != NULL &&
                            (*ppDs_arr)->id_layer.pId_to_data_table != NULL,
                        "The given *ppDs_arr is invalid.");

    CONT_DSArr *pDs_arr = *ppDs_arr;

    if (pDs_arr->pElem_del_cb) {
        for (PRP_U32 i = 0; i < pDs_arr->data_layer.len; i++) {
            void *ptr = pDs_arr->data_layer.pElems + (i * pDs_arr->memb_size);
            pDs_arr->pElem_del_cb(ptr);
        }
    }
    free(pDs_arr->data_layer.pElems);
    free(pDs_arr->data_layer.pData_to_id_table);
    free(pDs_arr->id_layer.pId_to_data_table);

#ifdef PRP_DEBUG_MODE
    pDs_arr->memb_size = 0;
    pDs_arr->data_layer.pElems = NULL;
    pDs_arr->data_layer.pData_to_id_table = NULL;
    pDs_arr->data_layer.cap = pDs_arr->data_layer.len = 0;
    pDs_arr->id_layer.pId_to_data_table = NULL;
    pDs_arr->id_layer.cap = pDs_arr->id_layer.free_index =
        pDs_arr->id_layer.free_count = 0;
    pDs_arr->pElem_del_cb = NULL;
#endif

    free(pDs_arr);
    *ppDs_arr = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrDeleteChecked(CONT_DSArr **ppDs_arr) {
    if (!ppDs_arr || !(*ppDs_arr)) {
        return PRP_ERR_INV_ARG;
    }
    if (!(*ppDs_arr)->data_layer.pElems ||
        !(*ppDs_arr)->data_layer.pData_to_id_table ||
        !(*ppDs_arr)->id_layer.pId_to_data_table) {
        return PRP_ERR_INV_ARG;
    }

    CONT_DSArrDeleteUnchecked(ppDs_arr);

    return PRP_OK;
}

PRP_API PRP_U32 PRP_CALL CONT_DSArrLen(const CONT_DSArr *pDs_arr) {
    ASSERT_INVARIANT_EXPR(pDs_arr);

    return pDs_arr->data_layer.len;
}

static inline PRP_Result GetIdData(const CONT_DSArr *pDs_arr, CONT_DSId id,
                                   PRP_U32 *pId_i, PRP_U32 *pId_gen,
                                   PRP_U32 *pSlot_data_i, PRP_U32 *pSlot_gen) {
    UNPACK_PACKED(id, *pId_i, *pId_gen);
    if (*pId_i >= pDs_arr->id_layer.cap) {
        return PRP_ERR_OOB;
    }
    PRP_U64 id_val = pDs_arr->id_layer.pId_to_data_table[*pId_i];
    UNPACK_PACKED(id_val, *pSlot_data_i, *pSlot_gen);
    if (*pId_gen != *pSlot_gen || *pSlot_data_i >= pDs_arr->data_layer.len) {
        return PRP_ERR_INV_STATE;
    }

    return PRP_OK;
}

PRP_API void *PRP_CALL CONT_DSIdToDataUnchecked(const CONT_DSArr *pDs_arr,
                                                CONT_DSId id) {
    ASSERT_INVARIANT_EXPR(pDs_arr);

    // Initializing the slot_data_i doesn't change shit but we do it to satisfy
    // compiler.
    PRP_U32 dummy1, dummy2, slot_data_i = 0, dummy3;
    PRP_Result code =
        GetIdData(pDs_arr, id, &dummy1, &dummy2, &slot_data_i, &dummy3);
    PRP_DIAG_ASSERT_MSG(code == PRP_OK,
                        "The given id is invalid or used after free.");
    (void)code;

    return pDs_arr->data_layer.pElems + (pDs_arr->memb_size * slot_data_i);
}

PRP_API PRP_Result PRP_CALL CONT_DSIdToDataChecked(const CONT_DSArr *pDs_arr,
                                                   CONT_DSId id,
                                                   void **ppDest) {
    if (!CONT_DSArrIsValid(pDs_arr) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }

    PRP_U32 dummy1, dummy2, slot_data_i, dummy3;
    PRP_Result code =
        GetIdData(pDs_arr, id, &dummy1, &dummy2, &slot_data_i, &dummy3);
    if (code != PRP_OK) {
        return code;
    }

    *ppDest = pDs_arr->data_layer.pElems + (pDs_arr->memb_size * slot_data_i);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_DSIdIsValidUnchecked(const CONT_DSArr *pDs_arr,
                                                    CONT_DSId id) {
    ASSERT_INVARIANT_EXPR(pDs_arr);

    PRP_U32 dummy1, dummy2, dummy3, dummy4;
    PRP_Result code =
        GetIdData(pDs_arr, id, &dummy1, &dummy2, &dummy3, &dummy4);

    return code == PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSIdIsValidChecked(const CONT_DSArr *pDs_arr,
                                                    CONT_DSId id,
                                                    PRP_Bool *pRslt) {
    if (!CONT_DSArrIsValid(pDs_arr) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_DSIdIsValidUnchecked(pDs_arr, id);

    return PRP_OK;
}

static PRP_Result GrowDataLayer(CONT_DSArr *pDs_arr, PRP_U32 to_add) {
    /*
     * This is not suseptible to overflow bugs since the max cap of an ds array
     * is hard capped at U32_MAX, so storing the new_cap in a u64 prevents any
     * overflow that can happen.
     */
    PRP_U64 new_cap = (PRP_U64)(pDs_arr->data_layer.cap) + to_add;
    PRP_U32 max_cap = CONT_DS_ARR_MAX_CAP(pDs_arr->memb_size);
    if (pDs_arr->data_layer.cap == max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > max_cap) {
        new_cap = max_cap;
    }

    void *pElems =
        realloc(pDs_arr->data_layer.pElems, pDs_arr->memb_size * new_cap);
    if (!pElems) {
        return PRP_ERR_OOM;
    }
    PRP_U32 *pData_to_id_table = realloc(pDs_arr->data_layer.pData_to_id_table,
                                         sizeof(PRP_U32) * new_cap);
    if (!pData_to_id_table) {
        /*
         * The extra mem allocated to elems will be considered over
         * allocated(since the cap is still the same one) till we again try to
         * grow the data layer, during when that lost memory will be
         * regained(granted it doesn't fail for the second time.).
         */
        pDs_arr->data_layer.pElems = pElems;
        return PRP_ERR_OOM;
    }

    pDs_arr->data_layer.pElems = pElems;
    pDs_arr->data_layer.pData_to_id_table = pData_to_id_table;
    pDs_arr->data_layer.cap = (PRP_U32)new_cap;

    return PRP_OK;
}

static PRP_Result GrowIdLayer(CONT_DSArr *pDs_arr, PRP_U32 to_add) {
    /*
     * This is not suseptible to overflow bugs since the max cap of an ds array
     * is hard capped at U32_MAX, so storing the new_cap in a u64 prevents any
     * overflow that can happen.
     */
    PRP_U64 new_cap = (PRP_U64)(pDs_arr->id_layer.cap) + to_add;
    PRP_U32 max_cap = CONT_DS_ARR_MAX_CAP(pDs_arr->memb_size);
    if (pDs_arr->id_layer.cap == max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > max_cap) {
        new_cap = max_cap;
    }

    PRP_U64 *pId_to_data_table =
        realloc(pDs_arr->id_layer.pId_to_data_table, sizeof(PRP_U64) * new_cap);
    if (!pId_to_data_table) {
        return PRP_ERR_OOM;
    }
    for (PRP_U32 i = pDs_arr->id_layer.cap; i < new_cap - 1; i++) {
        pId_to_data_table[i] = i + 1;
    }
    pId_to_data_table[new_cap - 1] = pDs_arr->id_layer.free_index;
    pDs_arr->id_layer.free_index = pDs_arr->id_layer.cap;
    pDs_arr->id_layer.free_count += (PRP_U32)new_cap - pDs_arr->id_layer.cap;

    pDs_arr->id_layer.pId_to_data_table = pId_to_data_table;
    pDs_arr->id_layer.cap = (PRP_U32)new_cap;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrAddUnchecked(CONT_DSArr *pDs_arr,
                                                   void *pData,
                                                   CONT_DSId *pId) {
    ASSERT_INVARIANT_EXPR(pDs_arr);
    PRP_DIAG_ASSERT(pData != NULL);
    PRP_DIAG_ASSERT(pId != NULL);

    PRP_U32 free_index = pDs_arr->id_layer.free_index;
    if (free_index == INVALID_ID_LAYER_INDEX) {
        PRP_Result code = GrowIdLayer(pDs_arr, pDs_arr->data_layer.cap);
        if (code != PRP_OK) {
            return code;
        }
        free_index = pDs_arr->id_layer.free_index;
    }
    if (pDs_arr->data_layer.len == pDs_arr->data_layer.cap) {
        PRP_Result code = GrowDataLayer(pDs_arr, pDs_arr->id_layer.cap);
        if (code != PRP_OK) {
            return code;
        }
    }
    pDs_arr->id_layer.free_index =
        (PRP_U32)pDs_arr->id_layer.pId_to_data_table[free_index];

    PRP_U32 len = pDs_arr->data_layer.len;
    void *pDest = pDs_arr->data_layer.pElems + (len * pDs_arr->memb_size);
    memcpy(pDest, pData, pDs_arr->memb_size);
    pDs_arr->data_layer.pData_to_id_table[len] = free_index;

    PRP_U32 curr_gen =
        UNPACK_GEN(pDs_arr->id_layer.pId_to_data_table[free_index]);
    pDs_arr->id_layer.pId_to_data_table[free_index] =
        PACK_UNPACKED(len, curr_gen);
    pDs_arr->data_layer.len++;
    pDs_arr->id_layer.free_count--;

    *pId = PACK_UNPACKED(free_index, curr_gen);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrAddChecked(CONT_DSArr *pDs_arr,
                                                 void *pData, CONT_DSId *pId) {
    if (!CONT_DSArrIsValid(pDs_arr) || !pData || !pId) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrAddUnchecked(pDs_arr, pData, pId);
}

static inline void DelElem(CONT_DSArr *pDs_arr, CONT_DSId *pId, PRP_U32 id_i,
                           PRP_U32 slot_data_i, PRP_U32 slot_gen) {
    /*
     * NOTE: This function will corrupt the free index chain if there is a 2 ^32
     * generation wrap and there is still a stale id.
     * Although rare, it can still happen. But for now I won't handle for
     * intentional stupidity.
     */
    // Incrementing the gen of the deleted slot and marking it free.
    pDs_arr->id_layer.pId_to_data_table[id_i] =
        PACK_UNPACKED(pDs_arr->id_layer.free_index, slot_gen + 1);
    pDs_arr->id_layer.free_index = id_i;

    // Metadata for O(1) deletion.
    PRP_U32 len = pDs_arr->data_layer.len;
    void *pTo_del =
        pDs_arr->data_layer.pElems + (slot_data_i * pDs_arr->memb_size);
    void *pLast_elem =
        pDs_arr->data_layer.pElems + ((len - 1) * pDs_arr->memb_size);
    PRP_U32 last_elem_id_i = pDs_arr->data_layer.pData_to_id_table[len - 1];

    if (pDs_arr->pElem_del_cb) {
        pDs_arr->pElem_del_cb(pTo_del);
    }
    // The condition is an early exit if last elem is removed.
    if (slot_data_i != len - 1) {
        // Packing the data_layer arrays by shifting last elem in the new hole.
        memcpy(pTo_del, pLast_elem, pDs_arr->memb_size);
        pDs_arr->data_layer.pData_to_id_table[slot_data_i] =
            pDs_arr->data_layer.pData_to_id_table[len - 1];
        /*
         * Updating the id layer metadata of the last elem so it points to the
         * new data index after the packing.
         */
        PRP_U32 gen =
            UNPACK_GEN(pDs_arr->id_layer.pId_to_data_table[last_elem_id_i]);
        pDs_arr->id_layer.pId_to_data_table[last_elem_id_i] =
            PACK_UNPACKED(slot_data_i, gen);
    }
    pDs_arr->data_layer.len--;
    pDs_arr->id_layer.free_count++;

    *pId = CONT_DS_INVALID_ID;
}

PRP_API void PRP_CALL CONT_DSArrDelElemUnchecked(CONT_DSArr *pDs_arr,
                                                 CONT_DSId *pId) {
    ASSERT_INVARIANT_EXPR(pDs_arr);
    PRP_DIAG_ASSERT(pId != NULL);

    // Initializing the doesn't change shit but we do it to satisfy compiler.
    PRP_U32 id_i = 0, dummy1, slot_data_i = 0, slot_gen = 0;
    PRP_Result code =
        GetIdData(pDs_arr, *pId, &id_i, &dummy1, &slot_data_i, &slot_gen);
    PRP_DIAG_ASSERT_MSG(code == PRP_OK,
                        "The given id is invalid or used after free.");
    (void)code;

    DelElem(pDs_arr, pId, id_i, slot_data_i, slot_gen);
}

PRP_API PRP_Result PRP_CALL CONT_DSArrDelElemChecked(CONT_DSArr *pDs_arr,
                                                     CONT_DSId *pId) {
    if (!CONT_DSArrIsValid(pDs_arr) || !pId) {
        return PRP_ERR_INV_ARG;
    }

    // Initializing doesn't change shit but we do it to satisfy compiler.
    PRP_U32 id_i = 0, dummy1, slot_data_i = 0, slot_gen = 0;
    PRP_Result code =
        GetIdData(pDs_arr, *pId, &id_i, &dummy1, &slot_data_i, &slot_gen);
    if (code != PRP_OK) {
        return code;
    }

    DelElem(pDs_arr, pId, id_i, slot_data_i, slot_gen);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrReserveUnchecked(CONT_DSArr *pDs_arr,
                                                       PRP_U32 count) {
    ASSERT_INVARIANT_EXPR(pDs_arr);
    PRP_DIAG_ASSERT_MSG(
        count > 0, "At least one element to reserve the ds array with needed.");

    if (pDs_arr->data_layer.cap - pDs_arr->data_layer.len < count) {
        PRP_Result code = GrowDataLayer(pDs_arr, count);
        if (code != PRP_OK) {
            return code;
        }
    }
    if (pDs_arr->id_layer.free_count < count) {
        PRP_Result code = GrowIdLayer(pDs_arr, count);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}
PRP_API PRP_Result PRP_CALL CONT_DSArrReserveChecked(CONT_DSArr *pDs_arr,
                                                     PRP_U32 count) {
    if (!CONT_DSArrIsValid(pDs_arr) || !count) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrReserveUnchecked(pDs_arr, count);
}

PRP_API PRP_Result PRP_CALL CONT_DSArrForEachUnchecked(
    CONT_DSArr *pDs_arr, PRP_Result (*pCb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    ASSERT_INVARIANT_EXPR(pDs_arr);
    PRP_DIAG_ASSERT(pCb != NULL);

    PRP_U8 *pMem = pDs_arr->data_layer.pElems;
    for (PRP_U32 i = 0; i < pDs_arr->data_layer.len; i++) {
        PRP_Result code = pCb(pMem, pUser_data);
        if (code != PRP_OK) {
            return code;
        }
        pMem += pDs_arr->memb_size;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_DSArrForEachChecked(
    CONT_DSArr *pDs_arr, PRP_Result (*pCb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    if (!CONT_DSArrIsValid(pDs_arr) || !pCb) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_DSArrForEachUnchecked(pDs_arr, pCb, pUser_data);
}
