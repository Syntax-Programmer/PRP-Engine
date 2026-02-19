#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

/**
 * This is sot of a middle ground between an array's data locality and a
 * hashmap's O(1) access time. This is a dense-sparse array.
 *
 * In this we manage data entries mapped to by stable ids. It maintains a linear
 * memory order like an array internally which can be indexed by ids that we
 * dispatch.
 *
 * This also prevents use after free bugs and stale references bugs.
 *
 * Limitation of this implementation is that it can hold a theoretical maximum
 * of U32_MAX number of elements rather than every other container having
 * U64_MAX as the theoretical maximum.
 *
 * NOTE: This doesn't preserve the order of the elements pushed in and the order
 * can change.
 */
typedef struct _DSArr DT_DSArr;
/**
 * It will be used to index into the id arr to retrieve the data.
 */
typedef DT_u64 DT_DSId;

#define DT_DS_INVALID_ID ((DT_DSId)(-1))

#define DT_DS_ARR_DEFAULT_CAP (16)
#define DT_DS_ARR_MAX_CAP(memb_size)                                           \
    PRP_MIN((DT_SIZE_MAX / (memb_size)), (DT_u32)~0)

#define DT_DS_INVALID_SIZE ((DT_u32)(-1))

/**
 * @return The last error code set by the id array's functions that don't
 * return PRP_Result explicitly.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrGetLastErrCode(DT_void);

/**
 * Creates the ds array wiht the user specified memb size.
 *
 * @param memb_size: The size of the members of the array.
 * @pram elem_del_cb: If the elems inside the array have internal allocations
 * this is reffered when freeing one or more elements.
 *
 * @return The pointer of the ds array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_DSArr *PRP_FN_CALL DT_DSArrCreateUnchecked(
    DT_size memb_size, PRP_Result (*elem_del_cb)(DT_void *elem));
/**
 * Creates the ds array wiht the user specified memb size.
 *
 * @param memb_size: The size of the members of the array.
 * @pram elem_del_cb: If the elems inside the array have internal allocations
 * this is reffered when freeing one or more elements.
 *
 * @return The pointer of the ds array.
 */
PRP_FN_API DT_DSArr *PRP_FN_CALL DT_DSArrCreateChecked(
    DT_size memb_size, PRP_Result (*elem_del_cb)(DT_void *elem));
/**
 * Deletes the ds array and sets the original DT_DsArr * to DT_null to prevent
 * use after free bugs.
 *
 * @param pDs_arr: The pointer to the ds array pointer to delete.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_DSArrDeleteUnchecked(DT_DSArr **pDs_arr);
/**
 * Deletes the ds array and sets the original DT_DsArr * to DT_null to prevent
 * use after free bugs.
 *
 * @param pDs_arr: The pointer to the ds array pointer to delete.
 *
 * @return PRP_ERR_INV_ARG if the pDs_arr or *pDs_arr is DT_null, otherwise it
 * returns PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrDeleteChecked(DT_DSArr **pDs_arr);
/**
 * Returns the current len of the ds array that is passed to it.
 *
 * @param ds_arr: The ds array to get the len of.
 *
 * @return The len of the ds array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_u32 PRP_FN_CALL DT_DSArrLenUnchecked(const DT_DSArr *ds_arr);
/**
 * Returns the current len of the ds array that is passed to it.
 *
 * @param ds_arr: The ds array to get the len of.
 *
 * @return DT_DS_INVALID_SIZE if the ds array is invalid, otherwise the actual
 * len of the ds array.
 */
PRP_FN_API DT_u32 PRP_FN_CALL DT_DSArrLenChecked(const DT_DSArr *ds_arr);
/**
 * Takes in the id and fetches the actual data it represents.
 *
 * @param ds_arr: The ds array to get the data from.
 * @param id: The id that represents the data we need to fetch.
 *
 * @return The pointer to the data element.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_DSIdToDataUnchecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id);
/**
 * Takes in the id and fetches the actual data it represents.
 *
 * @param ds_arr: The ds array to get the data from.
 * @param id: The id that represents the data we need to fetch.
 *
 * @return PRP_ERR_INV_ARG if the ds_arr is DT_null,
 * PRP_ERR_OOB/PRP_ERR_INV_STATE if the id given is not a valid id in the
 * ds_arr, otherwise the pointer to the data element.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_DSIdToDataChecked(const DT_DSArr *ds_arr,
                                                     DT_DSId id);
/**
 * Checks if the given id is a valid id inside the ds array.
 *
 * @param ds_arr: The array to check the id with.
 * @param id: The id to check the validity of.
 *
 * @return DT_false if the id is not valid, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_DSIdIsValidUnchecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id);
/**
 * Compares the given two array to see if their contents are exactly equal.
 *
 * @param ds_arr: The array to check the id with.
 * @param id: The id to check the validity of.
 * @param pRslt: The pointer to the variable where the boolean result will be
 * stored.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSIdIsValidChecked(const DT_DSArr *ds_arr,
                                                        DT_DSId id,
                                                        DT_bool *pRslt);
/**
 * Adds the given data to the ds array.
 *
 * @param ds_arr: The ds array to add the data to.
 * @param data: The data to add to the ds array.
 *
 * @return DT_DS_INVALID_ID if we cannot allocate enough memory for the data.
 * otherwise the id to the data.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_DSId PRP_FN_CALL DT_DSArrAddUnchecked(DT_DSArr *ds_arr,
                                                    DT_void *data);
/**
 * Adds the given data to the ds array.
 *
 * @param ds_arr: The ds array to add the data to.
 * @param data: The data to add to the ds array.
 *
 * @return DT_DS_INVALID_ID if the parameters are invalid in any way or we
 * cannot allocate enough memory for the data. otherwise the id to the data.
 */
PRP_FN_API DT_DSId PRP_FN_CALL DT_DSArrAddChecked(DT_DSArr *ds_arr,
                                                  DT_void *data);
/**
 * Remove the data represented by the id from the given ds array.
 *
 * @param ds_arr: The ds array to remove the element from.
 * @param pId: The pointer to the id of the data to remove.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_DSArrDelElemUnchecked(DT_DSArr *ds_arr,
                                                        DT_DSId *pId);
/**
 * Remove the data represented by the id from the given ds array.
 *
 * @param ds_arr: The ds array to remove the element from.
 * @param pId: The pointer to the id of the data to remove.
 *
 * @return PRP_ERR_INV_AG if the paramters are invalid in any way, PRP_ERR_OOB
 * or PRP_ERR_INV_STATE if the given id is not a valid id from the given ds
 * array, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrDelElemChecked(DT_DSArr *ds_arr,
                                                         DT_DSId *pId);
/**
 * Reserves <count> number of elements in the ds array.
 *
 * @param ds_arr: The ds array to reserve into.
 * @param count: The number of elements to reserve.
 *
 * @return PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if reserving <count> elements into
 * the ds array is not possible, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrReserveUnchecked(DT_DSArr *ds_arr,
                                                           DT_u32 count);
/**
 * Reserves <count> number of elements in the ds array.
 *
 * @param ds_arr: The ds array to reserve into.
 * @param count: The number of elements to reserve.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if reserving <count> elements into the ds
 * array is not possible, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrReserveChecked(DT_DSArr *ds_arr,
                                                         DT_u32 count);
/**
 * Performs a foreach operation of each of the element of the ds array. Calling
 * cb per element.
 *
 * @param ds_arr: The ds array on which the foreach will happen.
 * @param cb: The callback to be called per element. If this doesn't return
 * PRP_OK, further execution will be halted.
 * @param user_data: The data user wants to pass in as additional context.
 *
 * @return PRP_OK, or the callback error code.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrForEachUnchecked(
    DT_DSArr *ds_arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);
/**
 * Performs a foreach operation of each of the element of the ds array. Calling
 * cb per element.
 *
 * @param ds_arr: The ds array on which the foreach will happen.
 * @param cb: The callback to be called per element. If this doesn't return
 * PRP_OK, further execution will be halted.
 * @param user_data: The data user wants to pass in as additional context.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in some way, otherwise
 * PRP_OK, or the callback error code.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrForEachChecked(
    DT_DSArr *ds_arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
