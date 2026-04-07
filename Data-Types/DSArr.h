#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

/**
 * DT_DSArr (Dense-Sparse Array)
 *
 * Hybrid container combining:
 * - Dense storage (cache-friendly, contiguous memory)
 * - Sparse indexing via stable IDs
 *
 * Characteristics:
 * - Elements are stored densely
 * - Access via stable IDs (DT_DSId)
 * - Deletion does NOT preserve order
 * - Prevents stale references via ID validation
 *
 * Limitations:
 * - Maximum capacity limited to U32_MAX elements
 */
typedef struct _DSArr DT_DSArr;
/**
 * Stable identifier used to access elements.
 */
typedef DT_u64 DT_DSId;

#define DT_DS_INVALID_ID ((DT_DSId)(-1))

#define DT_DS_ARR_DEFAULT_CAP (16)
#define DT_DS_ARR_MAX_CAP(memb_size)                                           \
    ((DT_u32)PRP_MIN((DT_SIZE_MAX / (memb_size)), (DT_u32)~0))

#define DT_DS_INVALID_SIZE ((DT_u32)(-1))

/**
 * Checks whether the given ds array is structurally valid.
 *
 * @param ds_arr Pointer to the ds array.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_DSArrIsValid(const DT_DSArr *ds_arr);

/**
 * Creates a dynamic array.
 *
 * @param memb_size   Size (in bytes) of each element.
 * @param elem_del_cb Optional cb to free internall alloc of element..
 * @param out         Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrCreateUnchecked(
    DT_size memb_size, PRP_Result (*elem_del_cb)(DT_void *elem),
    DT_DSArr **out);
/**
 * Creates a dynamic array.
 *
 * @param memb_size   Size (in bytes) of each element.
 * @param elem_del_cb Optional cb to free internall alloc of element..
 * @param out         Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_DSArrCreateChecked(DT_size memb_size,
                      PRP_Result (*elem_del_cb)(DT_void *elem), DT_DSArr **out);
/**
 * Deletes the ds array and nullifies the pointer.
 *
 * @param pDs_arr Pointer to ds array pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_DSArrDeleteUnchecked(DT_DSArr **pDs_arr);
/**
 * Deletes the ds array and nullifies the pointer.
 *
 * @param pDs_arr Pointer to ds array pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pDs_arr or *pDs_arr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrDeleteChecked(DT_DSArr **pDs_arr);
/**
 * Returns the number of elements currently stored.
 *
 * @param ds_arr DSArr instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_FN_API DT_u32 PRP_FN_CALL DT_DSArrLen(const DT_DSArr *ds_arr);
/**
 * Fetches the data corresponding to the given id.
 *
 * @param ds_arr DSArr instance.
 * @param id     The id to the data.
 *
 * @return The pointer to the data element.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_DSIdToDataUnchecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id);
/**
 * Fetches the data corresponding to the given id.
 *
 * @param ds_arr DSArr instance.
 * @param id     The id to the data.
 * @param dest   The pointer to the value pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if id contains invalid index embedding.
 * @return PRP_ERR_INV_STATE if the id is stale/already deleted.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSIdToDataChecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id,
                                                       DT_void **dest);
/**
 * Checks if the given id is valid.
 *
 * @param ds_arr DSArr instance.
 * @param id     The id to check.
 *
 * @return DT_true if valid, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_DSIdIsValidUnchecked(const DT_DSArr *ds_arr,
                                                       DT_DSId id);
/**
 * Checks if the given id is valid.
 *
 * @param ds_arr DSArr instance.
 * @param id     The id to check.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSIdIsValidChecked(const DT_DSArr *ds_arr,
                                                        DT_DSId id,
                                                        DT_bool *pRslt);
/**
 * Adds the given data to the ds array.
 *
 * @param ds_arr DSArr instance.
 * @param data   The data to add.
 * @param pid    The pointer to hold the id to the data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if the max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrAddUnchecked(DT_DSArr *ds_arr,
                                                       DT_void *data,
                                                       DT_DSId *pId);
/**
 * Adds the given data to the ds array.
 *
 * @param ds_arr DSArr instance.
 * @param data   The data to add.
 * @param pid    The pointer to hold the id to the data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if the max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrAddChecked(DT_DSArr *ds_arr,
                                                     DT_void *data,
                                                     DT_DSId *pId);
/**
 * Remove the data associated by the given id and invalidates the id.
 *
 * @param ds_arr DSArr instance.
 * @param pid    The pointer of the id to delete.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_DSArrDelElemUnchecked(DT_DSArr *ds_arr,
                                                        DT_DSId *pId);
/**
 * Remove the data associated by the given id and invalidates the id.
 *
 * @param ds_arr DSArr instance.
 * @param pid    The pointer of the id to delete.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if id contains invalid index embedding.
 * @return PRP_ERR_INV_STATE if the id is stale/already deleted.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrDelElemChecked(DT_DSArr *ds_arr,
                                                         DT_DSId *pId);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr   DSArr instance.
 * @param count Number of elements to reserve.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrReserveUnchecked(DT_DSArr *ds_arr,
                                                           DT_u32 count);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr   DSArr instance.
 * @param count Number of elements to reserve.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrReserveChecked(DT_DSArr *ds_arr,
                                                         DT_u32 count);
/**
 * Iterates over all elements of the array.
 *
 * @param arr       DSArr instance.
 * @param cb        Callback invoked per element.
 * @param user_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrForEachUnchecked(
    DT_DSArr *ds_arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);
/**
 * Iterates over all elements of the array.
 *
 * @param arr       DSArr instance.
 * @param cb        Callback invoked per element.
 * @param user_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_DSArrForEachChecked(
    DT_DSArr *ds_arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
