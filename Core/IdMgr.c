#include "IdMgr.h"
#include "../Data-Types/Arr.h"
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
     * An array of u64s that is used in a way like:
     * The ids dispatched are index into this array. This array acts a
     * validation layer for the id to access the data. The data stored at the
     * index the id points to stores the index to the data we want to point at.
     *
     * Bit 0-31: The index of the data array we point to.
     * Bit 32-63: A 32 bit gen value of the id_layer's slot.
     */
    DT_Arr *id_layer;
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

    id_mgr->id_layer = DT_ArrCreateDefault(sizeof(DT_u64));
    ID_MGR_INIT_ERROR_CHECK(id_mgr->id_layer);

    id_mgr->free_id_slots = DT_BitmapCreateDefault();
    ID_MGR_INIT_ERROR_CHECK(id_mgr->free_id_slots);

    id_mgr->data_del_cb = data_del_cb;

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
        DT_ArrDelete(&id_mgr->id_layer);
    }
    if (id_mgr->free_id_slots) {
        DT_BitmapDelete(&id_mgr->free_id_slots);
    }
    free(id_mgr);
    *pId_mgr = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdToIndex(CORE_IdMgr *id_mgr, CORE_Id id);
PRP_FN_API DT_void *PRP_FN_CALL CORE_IdToData(CORE_IdMgr *id_mgr, CORE_Id id);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdIsValid(CORE_IdMgr *id_mgr, CORE_Id id,
                                                 DT_bool *pRslt);
PRP_FN_API CORE_Id PRP_FN_CALL CORE_IdMgrAddData(CORE_IdMgr *id_mgr,
                                                 DT_void *data);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrReserve(CORE_IdMgr *id_mgr,
                                                    DT_u32 count);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDeleteData(CORE_IdMgr *id_mgr,
                                                       CORE_Id *pId);

PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrShrinkFit(CORE_IdMgr *id_mgr) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);

    // We can do this since PRP_FN_SUCCESS is defined as 0.
    return DT_ArrShrinkFit(id_mgr->data) || DT_ArrShrinkFit(id_mgr->data_layer);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
CORE_IdMgrForEach(CORE_IdMgr *id_mgr, PRP_FnCode (*cb)(DT_void *val)) {
    PRP_NULL_ARG_CHECK(id_mgr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(cb, PRP_FN_INV_ARG_ERROR);

    DT_ArrForEach(id_mgr->data, cb);

    return PRP_FN_SUCCESS;
}
