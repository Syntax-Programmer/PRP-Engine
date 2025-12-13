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
};

#define ID_MGR_VALIDITY_CHECK(id_mgr, ret)                                     \
    do {                                                                       \
        if (!id_mgr) {                                                         \
            PRP_LOG_FN_INV_ARG_ERROR(id_mgr);                                  \
            return ret;                                                        \
        }                                                                      \
    } while (0)

PRP_FN_API CORE_IdMgr *PRP_FN_CALL CORE_IdMgrCreate(
    DT_size data_size, PRP_FnCode (*data_del_cb)(DT_void *data_entry));
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDelete(CORE_IdMgr **pId_mgr);
PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdToIndex(CORE_IdMgr *id_mgr, CORE_Id id);
PRP_FN_API DT_void *PRP_FN_CALL CORE_IdToData(CORE_IdMgr *id_mgr, CORE_Id id);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdIsValid(CORE_IdMgr *id_mgr, CORE_Id id,
                                                 DT_bool *pRslt);
PRP_FN_API CORE_Id PRP_FN_CALL CORE_IdMgrAddData(CORE_IdMgr *id_mgr,
                                                 DT_void *data);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrReserve(CORE_IdMgr *id_mgr,
                                                    DT_size count);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDeleteData(CORE_IdMgr *id_mgr,
                                                       CORE_Id *pId);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrShrinkToFit(CORE_IdMgr *mgr);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrForEach(
    CORE_IdMgr *id_mgr, PRP_FnCode (*cb)(CORE_Id id, DT_void *val));
