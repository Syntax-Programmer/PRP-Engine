#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

typedef DT_u64 CORE_Id;
#define CORE_INVALID_ID ((CORE_Id)(~0))

/*
 * A 32 bit index is only dispatched by the id mgr so we make a special
 * sentinel for it.
 */
#define CORE_INVALID_INDEX ((DT_u32)(-1))
#define CORE_INVALID_SIZE ((DT_u32)(-1))

/**
 * This stores the array of data the user want to represent with the id. Manages
 * its addition and deletion and dispatches stable ids that link to specific
 * data.
 *
 * It also manages use after free bugs and stale reference bugs by essentially
 * never dispatching the same id. So if a old id shows up we know its used after
 * being freed.
 */
typedef struct _IdMgr CORE_IdMgr;

/**
 * Creates the dynamic id manager of an array of elements of size specified by
 * user..
 *
 * @param data_size: The size of the members of the array to manage.
 * @param data_del_cb: The callback that will free the memory of the element
 * correctly and safely when we delete the id manager, or remove an element from
 * the id manager.
 *
 * @return The pointer of the id manager.
 */
PRP_FN_API CORE_IdMgr *PRP_FN_CALL CORE_IdMgrCreate(
    DT_size data_size, PRP_FnCode (*data_del_cb)(DT_void *data_entry));
/**
 * Deletes the id manager and sets the original CORE_IdMgr * to DT_null to
 * prevent use after free bugs.
 *
 * @param pId_mgr: The pointer to the id manager pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pId_mgr is DT_null or the id manager it
 * points to is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDelete(CORE_IdMgr **pId_mgr);

/**
 * Returns the raw memory pointer of the data array to the user.
 * This function returns a non-fixed pointer to the array mem. If an operation
 * is performed to the id_mgr after getting the raw data, the raw data is no
 * longer guaranteed to be valid.
 *
 * @param id_mgr: The id manager to get the raw mem data of.
 * @param pLen: Pointer to where the len of the data array will be stored to be
 * used by the caller to prevent unsafe usage.
 *
 * @return DT_null if the id manager is invalid or pLen is DT_null, otherwise
 * the memory pointer of the id manager's data array raw memory.
 */
PRP_FN_API DT_void *PRP_FN_CALL CORE_IdMgrRaw(CORE_IdMgr *id_mgr, DT_u32 *pLen);
/**
 * Returns the current number of elements the id manager is managing.
 *
 * @param id_mgr: The id manager to get the len of.
 *
 * @return CORE_INVALID_SIZE if the id manager is invalid, otherwise the actual
 * len of the id manager.
 */
PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdMgrLen(CORE_IdMgr *id_mgr);

/**
 * Returns the index of where the actual data lives of the data id points to.
 *
 * @param id_mgr: The id manager to get the index from.
 * @param id: The id that points to the data.
 *
 * @return CORE_INVALID_INDEX if the id manager is invalid or the id is invalid
 * in some way, otherwise the actual index from the id manager of the id.
 */
PRP_FN_API DT_u32 PRP_FN_CALL CORE_IdToIndex(CORE_IdMgr *id_mgr, CORE_Id id);
/**
 * Returns the data pointer of the actual data id points to.
 *
 * @param id_mgr: The id manager to get the data pointer from.
 * @param id: The id that points to the data.
 *
 * @return CORE_INVALID_INDEX if the id manager is invalid or the id is invalid
 * in some way, otherwise the actual data pointer from the id manager of the id.
 */
PRP_FN_API DT_void *PRP_FN_CALL CORE_IdToData(CORE_IdMgr *id_mgr, CORE_Id id);
/**
 * Checks if the given id is valid id dispatched from id_mgr.
 *
 * @param id_mgr: The id manager that we check the id against.
 * @param id: The id to check validity of.
 * @param pRslt: The pointer to the variable where the boolean result will be
 * stored.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdIsValid(CORE_IdMgr *id_mgr, CORE_Id id,
                                                 DT_bool *pRslt);
/**
 * Converts a data index to an id that manages it.
 *
 * @param id_mgr: The id manager to get the data pointer from.
 * @param data_i: The data index to convert to an id.
 *
 * @return CORE_INVALID_ID if the parameters are invalid in any way,
 * CORE_INVALID_ID if the data index is out of bounds, otherwise a valid id to
 * the data at the given index.
 */
PRP_FN_API CORE_Id PRP_FN_CALL CORE_DataIdxToId(CORE_IdMgr *id_mgr,
                                                DT_size data_i);

/**
 * Pushes a new element into the given id manager, auto growing to accommodate
 * for new elements.
 *
 * @param id_mgr: The id manager to push data into.
 * @param data: The pointer to the data to be pushed into the id manager.
 *
 * @return CORE_INVALID_ID if the parameters are invalid in any way,
 * CORE_INVALID_ID if pushing into the id manager is not possible,
 * otherwise a valid id to the new data.
 */
PRP_FN_API CORE_Id PRP_FN_CALL CORE_IdMgrAddData(CORE_IdMgr *id_mgr,
                                                 DT_void *data);
/**
 * Deletes the data the given id points to and invalidate any copies of the id
 * passed.
 *
 * @param id_mgr: The id manager to delete the data from.
 * @param pId: The pointer to the id that points to the data to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR/PRP_FN_UAF_ERROR if the given id is invalid or stale,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrDeleteData(CORE_IdMgr *id_mgr,
                                                       CORE_Id *pId);
/**
 * Reserves <count> number of elements in the id manager for future insertion.
 *
 * @param id_mgr: The id manager to reserve into.
 * @param count: The number of elements to reserve.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if max cap of the id manager is reached,
 * PRP_FN_MALLOC_ERROR if reserving failed due to realloc failure, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrReserve(CORE_IdMgr *id_mgr,
                                                    DT_u32 count);

// PRP_FN_API PRP_FnCode PRP_FN_CALL CORE_IdMgrShrinkFit(CORE_IdMgr *id_mgr);

/**
 * Performs a foreach operation of each of the element of the id manager.
 * Calling cb per element.
 *
 * @param id_mgr: The id manager on which the foreach will happen.
 * @param cb: The callback to be called per element. If this doesn't return
 * PRP_FN_SUCCESS, further execution will be halted.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in some way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL
CORE_IdMgrForEach(CORE_IdMgr *id_mgr, PRP_FnCode (*cb)(DT_void *val));

#ifdef __cplusplus
}
#endif
