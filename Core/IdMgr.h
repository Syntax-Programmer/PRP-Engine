#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Utils/Defs.h"

typedef DT_u64 CORE_Id;
#define CORE_INVALID_ID ((CORE_Id)(~0))

#define CORE_INVALID_DATA_INDEX ((DT_u32)(-1))

/**
 * This stores the array of data the user want to represent with the id. Manages
 * its addition and deletion and dispatches stable ids that link to specific
 * data.
 *
 * It also manages use after free bugs and stale reference bugs by essentially
 * never dispatching the same id. (unless the dev using this is stupid and
 * managed to generate 2 ^ 8 stale refs. ...Have I given too much technical
 * details...you will never know.)
 */
typedef struct _IdMgr CORE_IdMgr;

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
                                                    DT_u32 count);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDeleteData(CORE_IdMgr *id_mgr,
                                                       CORE_Id *pId);
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrShrinkFit(CORE_IdMgr *id_mgr);
PRP_FN_API PRP_FnCode PRP_FN_CALL
CORE_IdMgrForEach(CORE_IdMgr *id_mgr, PRP_FnCode (*cb)(DT_void *val));

#ifdef __cplusplus
}
#endif
