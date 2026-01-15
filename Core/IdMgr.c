#include "IdMgr.h"
#include "../Data-Types/Arr.h"
#include "../Data-Types/Bffr.h"
#include "../Data-Types/Bitmap.h"
#include "../Utils/Logger.h"

struct _IdMgr {
    /*
     * The data array that is being represented by the id. This is a
     * densely packed array at all times and the indices are unstable in it.
     */
    DT_Arr *data;
    /*
     * An array of u32, in sync with the data array that tells which id_layer
     * index manages the data index. This is reverse mapping to repack the data
     * array after data is freed.
     */
    DT_Arr *data_layer;
    /*
     * Max cap of an id manager is hardcapped at u32 max. It can be even lower
     * if the DT_Arr *data's max cap is lower.
     */
    DT_u32 max_cap;
    /*
     * An buffer of u64s that is used in a way like:
     * The ids dispatched are index into this buffer. This buffer acts a
     * validation layer for the id to access the data. The data stored at the
     * index the id points to stores the index to the data we want to point at.
     *
     * What we store in the buffer:
     * Bit 0-31: The index of the data array we point to.
     * Bit 32-63: A 32 bit gen value of the id_layer's slot.
     */
    DT_Bffr *id_layer;
    DT_Bffr *id_gen;
    /*
     * An on bit in this bitmap corresponds to a free slot in the id_layer array
     * that can be used to dispatch the id.
     */
    DT_Bitmap *free_id_slots;
    /*
     * The callback used to free internal allocations of the elements of the
     * data we store in the data array. This can be DT_null if the array
     * elements don't have internal allocations.
     */
    PRP_FnCode (*data_del_cb)(DT_void *pData_entry);
};

/*
 * Interesting point to look at:
 * If the gen of a slot doesn't match the gen of an id, the slot is surely
 * pointing to an index that has nothing to do with the id, so we can skip that
 * check and similarly if the gens match the data index is surely valid.
 */

#define GET_GEN_FROM_PACKED(packed) ((DT_u32)(packed >> 32))
#define GET_INDEX_FROM_PACKED(packed) ((DT_u32)packed)
#define UNPACK_GEN_INDEX_PACKING(packed, index, gen)                           \
    do {                                                                       \
        (index) = GET_INDEX_FROM_PACKED(packed);                               \
        (gen) = GET_GEN_FROM_PACKED(packed);                                   \
    } while (0)

#define PACK_GEN_INDEX(index, gen) (((DT_u64)(gen) << 32) | (DT_u64)(index))

// By convention the gen of a new slot is 0.
#define NEW_ID_LAYER_VAL ((DT_u64)CORE_INVALID_INDEX);

/*
 * This is returned by the IsIdValid function that gives all the info about the
 * id for further processing.
 */
typedef struct {
    DT_u32 id_i;
    DT_u32 id_gen;
    DT_u32 data_i;
    DT_u32 slot_gen;
    // If this is not PRP_FN_SUCCESS the id is invalid.
    PRP_FnCode validity_code;
} IdState;

/**
 * This acts as a api layer b/w the data del function that shouldn't have a user
 * data, and the arr api requirements.
 *
 * @param pVal: The pointer to the value to delete.
 * @param user_data: This will be the actual delete callback we will pass
 * through.
 *
 * @return Whatever the data_del func will return.
 */
static inline PRP_FnCode DataDelForEachWrapper(DT_void *pVal,
                                               DT_void *user_data);
/**
 * Fetches all of the data that an id can reveal while at the same time checking
 * if it the id is valid or not.
 *
 * @param id_mgr: The id manager that supposedly dispatched the concerned id.
 * @param id: The id to check validity of an get data from.
 *
 * @return The IdState containing all the data the id can give.
 */
static inline IdState GetIdData(const CORE_IdMgr *id_mgr, CORE_Id id);
/**
 * Grows the id_mgr maintaining same cap for each bffr. Also fills initial
 * values for the id_layer and free_id_slots.
 *
 * @param id_mgr: The id manager to grow.
 * @param new_cap: The new cap the id manager wants to set.
 *
 * @return PRP_FN_MALLOC_ERROR if any resizing failed, otherwise PRP_FN_SUCCESS.
 */
static PRP_FnCode GrowIdMgr(CORE_IdMgr *id_mgr, DT_size new_cap);

#define ID_MGR_INIT_ERROR_CHECK(x)                                             \
    do {                                                                       \
        if (!x) {                                                              \
            CORE_IdMgrDelete(&id_mgr);                                         \
            PRP_LOG_FN_MALLOC_ERROR(x);                                        \
            return DT_null;                                                    \
        }                                                                      \
    } while (0);

PRP_FN_API CORE_IdMgr *PRP_FN_CALL CORE_IdMgrCreate(
    DT_size data_size, PRP_FnCode (*data_del_cb)(DT_void *data_entry)) {
    if (!data_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "CORE_IdMgr can't be made with data_size=0.");
        return DT_null;
    }

    CORE_IdMgr *id_mgr = malloc(sizeof(CORE_IdMgr));
    if (!id_mgr) {
        PRP_LOG_FN_MALLOC_ERROR(id_mgr);
        return DT_null;
    }
    id_mgr->data = DT_ArrCreateDefault(data_size);
    ID_MGR_INIT_ERROR_CHECK(id_mgr->data);

    id_mgr->data_layer = DT_ArrCreateDefault(sizeof(DT_u32));
    ID_MGR_INIT_ERROR_CHECK(id_mgr->data_layer);

    id_mgr->id_layer = DT_BffrCreateDefault(sizeof(DT_u64));
    ID_MGR_INIT_ERROR_CHECK(id_mgr->id_layer);

    DT_size cap = DT_BffrCap(id_mgr->id_layer);
    id_mgr->free_id_slots = DT_BitmapCreate(cap);
    ID_MGR_INIT_ERROR_CHECK(id_mgr->free_id_slots);

    id_mgr->data_del_cb = data_del_cb;
    DT_size max_data_cap = DT_ArrMaxCap(id_mgr->data);
    // Hard constraint of 32 bit max due to packing of data.
    id_mgr->max_cap = (max_data_cap > (DT_u32)~0) ? (DT_u32)~0 : max_data_cap;

    // These can't fail.
    DT_u64 x = NEW_ID_LAYER_VAL;
    DT_BffrSetRange(id_mgr->id_layer, 0, cap, &x);
    DT_BitmapSetRange(id_mgr->free_id_slots, 0, cap);

    return id_mgr;
}

static inline PRP_FnCode DataDelForEachWrapper(DT_void *pVal,
                                               DT_void *user_data) {
    PRP_FnCode (*data_del_cb)(DT_void *) = (PRP_FnCode (*)(DT_void *))user_data;

    return data_del_cb(pVal);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDelete(CORE_IdMgr **pId_mgr) {
    PRP_NULL_ARG_CHECK(pId_mgr, PRP_FN_INV_ARG_ERROR);
    CORE_IdMgr *id_mgr = *pId_mgr;
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);

    if (id_mgr->data) {
        if (id_mgr->data_del_cb) {
            DT_ArrForEach(id_mgr->data, DataDelForEachWrapper,
                          id_mgr->data_del_cb);
        }
        DT_ArrDelete(&id_mgr->data);
    }
    if (id_mgr->data_layer) {
        DT_ArrDelete(&id_mgr->data_layer);
    }
    if (id_mgr->id_layer) {
        DT_BffrDelete(&id_mgr->id_layer);
    }
    if (id_mgr->free_id_slots) {
        DT_BitmapDelete(&id_mgr->free_id_slots);
    }
    free(id_mgr);
    *pId_mgr = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API const DT_void *PRP_FN_CALL CORE_IdMgrRaw(const CORE_IdMgr *id_mgr,
                                                    DT_u32 *pLen) {
    PRP_NULL_ARG_CHECK(id_mgr, DT_null);

    DT_size len;
    const DT_void *data = DT_ArrRaw(id_mgr->data, &len);
    *pLen = (DT_u32)len;

    return data;
}

PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdMgrLen(const CORE_IdMgr *id_mgr) {
    PRP_NULL_ARG_CHECK(id_mgr, CORE_INVALID_SIZE);

    DT_size len = DT_ArrLen(id_mgr->data);

    return (DT_u32)len;
}

PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdMgrMaxCap(const CORE_IdMgr *id_mgr) {
    PRP_NULL_ARG_CHECK(id_mgr, CORE_INVALID_SIZE);

    return id_mgr->max_cap;
}

static inline IdState GetIdData(const CORE_IdMgr *id_mgr, CORE_Id id) {
    IdState state = {0};

    UNPACK_GEN_INDEX_PACKING(id, state.id_i, state.id_gen);
    if (state.id_i >= DT_BffrCap(id_mgr->id_layer)) {
        state.validity_code = PRP_FN_OOB_ERROR;
        PRP_LOG_FN_CODE(
            state.validity_code,
            "Given id is not possible to be dispatched through valid means.");
        return state;
    }

    DT_u64 id_val = *(DT_u64 *)DT_BffrGet(id_mgr->id_layer, state.id_i);
    UNPACK_GEN_INDEX_PACKING(id_val, state.data_i, state.slot_gen);
    DT_size len = DT_ArrLen(id_mgr->data);

    if (state.id_gen != state.slot_gen || state.data_i >= (DT_u32)len) {
        state.validity_code = PRP_FN_UAF_ERROR;
        PRP_LOG_FN_CODE(state.validity_code,
                        "Given id has already been freed, stale id detected.");
        return state;
    }
    state.validity_code = PRP_FN_SUCCESS;

    return state;
}

PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdToIndex(const CORE_IdMgr *id_mgr,
                                             CORE_Id id) {
    PRP_NULL_ARG_CHECK(id_mgr, CORE_INVALID_INDEX);
    IdState state = GetIdData(id_mgr, id);
    if (state.validity_code != PRP_FN_SUCCESS) {
        return CORE_INVALID_INDEX;
    }

    return state.data_i;
}

PRP_FN_API DT_void *PRP_FN_CALL CORE_IdToData(const CORE_IdMgr *id_mgr,
                                              CORE_Id id) {
    PRP_NULL_ARG_CHECK(id_mgr, DT_null);
    IdState state = GetIdData(id_mgr, id);
    if (state.validity_code != PRP_FN_SUCCESS) {
        return DT_null;
    }

    return DT_ArrGet(id_mgr->data, state.data_i);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdIsValid(const CORE_IdMgr *id_mgr,
                                                 CORE_Id id, DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    IdState state = GetIdData(id_mgr, id);
    if (state.validity_code == PRP_FN_SUCCESS) {
        *pRslt = DT_true;
    } else {
        *pRslt = DT_false;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API CORE_Id PRP_FN_CALL CORE_DataIToId(const CORE_IdMgr *id_mgr,
                                              DT_size data_i) {
    PRP_NULL_ARG_CHECK(id_mgr, CORE_INVALID_ID);

    DT_size len = DT_ArrLen(id_mgr->data);
    if (data_i >= (DT_u32)len) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried to access data index: %zu, with id manager of len: %zu",
            data_i, (DT_u32)len);
        return CORE_INVALID_ID;
    }
    DT_u32 id_i = *(DT_u32 *)DT_ArrGet(id_mgr->data_layer, data_i);
    DT_u64 id_val = *(DT_u64 *)DT_BffrGet(id_mgr->id_layer, id_i);

    return (CORE_Id)PACK_GEN_INDEX(id_i, GET_GEN_FROM_PACKED(id_val));
}

static PRP_FnCode GrowIdMgr(CORE_IdMgr *id_mgr, DT_size new_cap) {
    DT_size old_cap = DT_BffrCap(id_mgr->id_layer);

    if (DT_BitmapChangeSize(id_mgr->free_id_slots, new_cap) != PRP_FN_SUCCESS ||
        DT_BffrChangeSize(id_mgr->id_layer, new_cap) != PRP_FN_SUCCESS) {
        /*
         * Trying to revert to old cap if possible since we need to maintain cap
         * sync.
         * If the below op also fail we can't do anything to save the mem leak
         * about to happen.
         */
        DT_BitmapChangeSize(id_mgr->free_id_slots, old_cap);
        DT_BffrChangeSize(id_mgr->id_layer, old_cap);
        return PRP_FN_MALLOC_ERROR;
    }
    // These can't fail.
    DT_u64 x = NEW_ID_LAYER_VAL;
    DT_BffrSetRange(id_mgr->id_layer, old_cap, new_cap, &x);
    DT_BitmapSetRange(id_mgr->free_id_slots, old_cap, new_cap);

    DT_size reserve_count = new_cap - DT_ArrCap(id_mgr->data);
    if (DT_ArrReserve(id_mgr->data, reserve_count) != PRP_FN_SUCCESS ||
        DT_ArrReserve(id_mgr->data_layer, reserve_count) != PRP_FN_SUCCESS) {
        /*
         * By the nature of the invariants, if these fail the push ops will also
         * fail in the future the caps of these two arrays might go out of sync.
         * This is fine and not a UB, but we can't continue adding to the
         * id_mgr.
         */
        return PRP_FN_MALLOC_ERROR;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API CORE_Id PRP_FN_CALL CORE_IdMgrAddData(CORE_IdMgr *id_mgr,
                                                 const DT_void *pData) {
    PRP_NULL_ARG_CHECK(id_mgr, CORE_INVALID_ID);
    PRP_NULL_ARG_CHECK(pData, CORE_INVALID_ID);

    DT_size len = DT_ArrLen(id_mgr->data);
    if ((DT_u32)len == id_mgr->max_cap) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "The max capacity of elements a CORE_IdMgr can managed "
                        "has been reached.");
        return CORE_INVALID_ID;
    }
    if (!DT_BitmapSetCount(id_mgr->free_id_slots)) {
        /*
         * The bffr_Cap * 2 can't overflow since the max cap of the id layer
         * array is wayyyyyy bigger than the max cap of the id manager and the
         * above condition will stop us wayyyy before we even come close to
         * overflowing.
         */
        DT_size new_cap = DT_BffrCap(id_mgr->id_layer) * 2;
        PRP_FnCode code = GrowIdMgr(id_mgr, new_cap);
        if (code != PRP_FN_SUCCESS) {
            PRP_LOG_FN_CODE(
                code, "Cannot add any more elements to the CORE_IdMgr due to "
                      "memory limitations.");
            return CORE_INVALID_ID;
        }
    }

    // Since the grow above didn't fail, everything below can't fail ever.
    // Get free id_layer index.
    DT_u32 id_i = DT_BitmapFFS(id_mgr->free_id_slots);

    // Mark it as in use.
    DT_BitmapClr(id_mgr->free_id_slots, id_i);

    // Update the data_i of the id_val at id_i index.
    DT_u64 id_val = *(DT_u64 *)DT_BffrGet(id_mgr->id_layer, id_i);
    // Data index is len++ since the data arr is densely packed.
    DT_u32 data_i = (DT_u32)len, gen = GET_GEN_FROM_PACKED(id_val);
    id_val = PACK_GEN_INDEX(data_i, gen);
    DT_BffrSet(id_mgr->id_layer, id_i, &id_val);

    // Sets the data and reverse indices.
    DT_ArrPush(id_mgr->data_layer, &id_i);
    DT_ArrPush(id_mgr->data, pData);

    // Crafts the id.
    return (CORE_Id)PACK_GEN_INDEX(id_i, gen);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDeleteData(CORE_IdMgr *id_mgr,
                                                       CORE_Id *pId) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pId, PRP_FN_INV_ARG_ERROR);
    IdState state = GetIdData(id_mgr, *pId);
    if (state.validity_code != PRP_FN_SUCCESS) {
        return state.validity_code;
    }

    // Marking the slot to be free.
    DT_BitmapSet(id_mgr->free_id_slots, state.id_i);
    // Incrementing the gen and setting invalid data index to id_i.
    DT_u64 id_val = PACK_GEN_INDEX(CORE_INVALID_INDEX, ++state.slot_gen);
    DT_BffrSet(id_mgr->id_layer, state.id_i, &id_val);

    if (id_mgr->data_del_cb) {
        id_mgr->data_del_cb(DT_ArrGet(id_mgr->data, state.data_i));
    }

    DT_size last_i = DT_ArrLen(id_mgr->data) - 1;
    if (state.data_i != last_i) {
        // data of the last element in the dense buffer.
        DT_u32 id_i = *(DT_u32 *)DT_ArrGet(id_mgr->data_layer, last_i);

        // Repacking the dense buffers.
        DT_ArrSwap(id_mgr->data, state.data_i, last_i);
        DT_ArrSwap(id_mgr->data_layer, state.data_i, last_i);

        // Updating the id_layer val of the last element that was moved.
        id_val = *(DT_u64 *)DT_BffrGet(id_mgr->id_layer, id_i);
        DT_u32 gen = GET_GEN_FROM_PACKED(id_val);
        id_val = PACK_GEN_INDEX(state.data_i, gen);
        DT_BffrSet(id_mgr->id_layer, id_i, &id_val);
    }
    // Invalidating the original ptr.
    *pId = CORE_INVALID_ID;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrReserve(CORE_IdMgr *id_mgr,
                                                    DT_u32 count) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    if (!count) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Cannot reserve 0 members in CORE_IdMgr.");
        return PRP_FN_INV_ARG_ERROR;
    }

    if (count > (id_mgr->max_cap - (DT_u32)(DT_ArrLen(id_mgr->data)))) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot reserve %u elements into the CORE_IdMgr "
                        "because it exceeds max capacity of CORE_IdMgr.",
                        count);
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }
    DT_size free_slots = DT_BitmapSetCount(id_mgr->free_id_slots);
    if (count <= free_slots) {
        return PRP_FN_SUCCESS;
    }
    DT_size new_cap = DT_BffrCap(id_mgr->id_layer) + (count - free_slots);

    PRP_FnCode code = GrowIdMgr(id_mgr, new_cap);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code,
                        "Cannot add any more elements to the CORE_IdMgr due to "
                        "memory limitations.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

// PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrShrinkFit(CORE_IdMgr *id_mgr) {
//     PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);

//     // We can do this since PRP_FN_SUCCESS is defined as 0.
//     return DT_ArrShrinkFit(id_mgr->data) ||
//     DT_ArrShrinkFit(id_mgr->data_layer);
// }

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrForEach(
    CORE_IdMgr *id_mgr, PRP_FnCode (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);

    return DT_ArrForEach(id_mgr->data, cb, user_data);
}
