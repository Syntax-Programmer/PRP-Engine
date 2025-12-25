#include "IdMgr.h"
#include "../Data-Types/Arr.h"
#include "../Data-Types/Bffr.h"
#include "../Data-Types/Bitmap.h"
#include "../Utils/Logger.h"

struct _IdMgr {
    /*
     * The array of the data that is being represented by the id. This is a
     * dense array and the indices are unstable in it.
     */
    DT_Arr *data;
    /*
     * An arr of u32, in sync with the data array that tells which id_layer
     * index manages the data index. This is reverse mapping to repack the data
     * array after data is freed.
     */
    DT_Arr *data_layer;
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
    /*
     * An on bit in this bitmap corresponds to a free slot in the id_layer array
     * that can be used to dispatch the id.
     */
    DT_Bitmap *free_id_slots;
    /*
     * The callback used to free internal allocations of the elements of the
     * array. This can be DT_null if the array elements don't have internal
     * allocations.
     */
    PRP_FnCode (*data_del_cb)(DT_void *data_entry);
};

/*
 * Interesting point to look at:
 * If the gen of a slot doesn't match the gen of an id, the slot is surely
 * pointing to an index that has nothing to do with the id, so we can skip that
 * check and similarly if the gens match the data index is surely valid.
 */

/*
 * This will be used to pack and unpack the ids we dispatch, and also the
 * id_layer's val, since it is also packed in the same way.
 */
#define UNPACK_GEN_INDEX_PACKING(packed, index, gen)                           \
    do {                                                                       \
        (index) = (DT_u32)((packed) & ((DT_u64)0xFFFFFFFF));                   \
        (gen) = (DT_u32)((packed) >> 32);                                      \
    } while (0)

#define PACK_GEN_INDEX(index, gen, packed)                                     \
    do {                                                                       \
        (packed) = (((DT_u64)(gen) << 32) | (DT_u64)(index));                  \
    } while (0)

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
 * Fetches all of the data that an id can reveal while at the same time checking
 * if it the id is valid or not.
 *
 * @param id_mgr: The id_mgr that supposedly dispatched the concerned id.
 * @param id: The id to check validity of an get data from.
 *
 * @return The IdState containing all the data the id can give.
 */
static inline IdState GetIdData(CORE_IdMgr *id_mgr, CORE_Id id

#define ID_MGR_INIT_ERROR_CHECK(x)                                             \
    do {                                                                       \
        if (!x) {                                                              \
            CORE_IdMgrDelete(&id_mgr);                                         \
            PRP_LOG_FN_MALLOC_ERROR(x);                                        \
            return DT_null;                                                    \
        }                                                                      \
    } while (0);
);

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

    DT_size data_cap = DT_ArrCap(id_mgr->data);

    id_mgr->id_layer = DT_BffrCreate(sizeof(DT_u64), data_cap);
    ID_MGR_INIT_ERROR_CHECK(id_mgr->id_layer);

    id_mgr->free_id_slots = DT_BitmapCreate(data_cap);
    ID_MGR_INIT_ERROR_CHECK(id_mgr->free_id_slots);

    id_mgr->data_del_cb = data_del_cb;

    // These can't fail.
    DT_u64 x = NEW_ID_LAYER_VAL;
    DT_BffrSetRange(id_mgr->id_layer, 0, data_cap, &x);
    DT_BitmapSetRange(id_mgr->free_id_slots, 0, data_cap);

    return id_mgr;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDelete(CORE_IdMgr **pId_mgr) {
    PRP_NULL_ARG_CHECK(pId_mgr, PRP_FN_INV_ARG_ERROR);
    CORE_IdMgr *id_mgr = *pId_mgr;
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);

    if (id_mgr->data) {
        if (id_mgr->data_del_cb) {
            DT_ArrForEach(id_mgr->data, id_mgr->data_del_cb);
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

static inline IdState GetIdData(CORE_IdMgr *id_mgr, CORE_Id id) {
    IdState state;

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
    if (state.id_gen != state.slot_gen ||
        state.data_i >= DT_ArrLen(id_mgr->data)) {
        state.validity_code = PRP_FN_UAF_ERROR;
        PRP_LOG_FN_CODE(state.validity_code,
                        "Given id has already been freed, stale id detected.");
        return state;
    }
    state.validity_code = PRP_FN_SUCCESS;

    return state;
}

PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdToIndex(CORE_IdMgr *id_mgr, CORE_Id id) {
    PRP_NULL_ARG_CHECK(id_mgr, CORE_INVALID_INDEX);
    IdState state = GetIdData(id_mgr, id);
    if (state.validity_code != PRP_FN_SUCCESS) {
        return CORE_INVALID_INDEX;
    }

    return state.data_i;
}

PRP_FN_API DT_void *PRP_FN_CALL CORE_IdToData(CORE_IdMgr *id_mgr, CORE_Id id) {
    PRP_NULL_ARG_CHECK(id_mgr, DT_null);
    IdState state = GetIdData(id_mgr, id);
    if (state.validity_code != PRP_FN_SUCCESS) {
        return DT_null;
    }

    return DT_ArrGet(id_mgr->data, state.data_i);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdIsValid(CORE_IdMgr *id_mgr, CORE_Id id,
                                                 DT_bool *pRslt) {
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

PRP_FN_API CORE_Id PRP_FN_CALL CORE_IdMgrAddData(CORE_IdMgr *id_mgr,
                                                 DT_void *data) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(data, PRP_FN_INV_ARG_ERROR);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDeleteData(CORE_IdMgr *id_mgr,
                                                       CORE_Id *pId) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pId, PRP_FN_INV_ARG_ERROR);
    IdState state = GetIdData(id_mgr, *pId);
    if (state.validity_code != PRP_FN_SUCCESS) {
        return state.validity_code;
    }

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

    return PRP_FN_SUCCESS;
}

// PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrShrinkFit(CORE_IdMgr *id_mgr) {
//     PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);

//     // We can do this since PRP_FN_SUCCESS is defined as 0.
//     return DT_ArrShrinkFit(id_mgr->data) ||
//     DT_ArrShrinkFit(id_mgr->data_layer);
// }

PRP_FN_API PRP_FnCode PRP_FN_CALL
CORE_IdMgrForEach(CORE_IdMgr *id_mgr, PRP_FnCode (*cb)(DT_void *val)) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(cb, PRP_FN_INV_ARG_ERROR);

    DT_ArrForEach(id_mgr->data, cb);

    return PRP_FN_SUCCESS;
}
