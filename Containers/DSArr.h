#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * CONT_DSArr (Dense-Sparse Array)
 *
 * Hybrid container combining:
 * - Dense storage (cache-friendly, contiguous memory)
 * - Sparse indexing via stable IDs
 *
 * Characteristics:
 * - Elements are stored densely
 * - Access via stable IDs (CONT_DSId)
 * - Deletion does NOT preserve order
 * - Prevents stale references via ID validation
 *
 * Limitations:
 * - Maximum capacity limited to U32_MAX elements
 */
typedef struct _DSArr CONT_DSArr;
/**
 * Stable identifier used to access elements.
 */
typedef PRP_U64 CONT_DSId;

#define CONT_DS_INVALID_ID ((CONT_DSId)(-1))

#define CONT_DS_ARR_DEFAULT_CAP (16)
#define CONT_DS_ARR_MAX_CAP(memb_size)                                         \
    ((PRP_U32)PRP_MIN((PRP_SIZE_MAX / (memb_size)), (PRP_U32)~0))

#define CONT_DS_INVALID_SIZE ((PRP_U32)(-1))

/**
 * Checks whether the given ds array is structurally valid.
 *
 * @param ds_arr Pointer to the ds array.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL CONT_DSArrIsValid(const CONT_DSArr *ds_arr);

/**
 * Creates a dynamic array.
 *
 * @param memb_size   Size (in bytes) of each element.
 * @param elem_del_cb Optional cb to free internall alloc of element..
 * @param pDs_arr     Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_DSArrCreateUnchecked(
    PRP_Size memb_size, PRP_Result (*elem_del_cb)(void *elem),
    CONT_DSArr **pDs_arr);
/**
 * Creates a dynamic array.
 *
 * @param memb_size   Size (in bytes) of each element.
 * @param elem_del_cb Optional cb to free internall alloc of element..
 * @param pDs_arr     Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_DSArrCreateChecked(
    PRP_Size memb_size, PRP_Result (*elem_del_cb)(void *elem),
    CONT_DSArr **pDs_arr);
/**
 * Deletes the ds array and nullifies the pointer.
 *
 * @param pDs_arr Pointer to ds array pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_DSArrDeleteUnchecked(CONT_DSArr **pDs_arr);
/**
 * Deletes the ds array and nullifies the pointer.
 *
 * @param pDs_arr Pointer to ds array pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pDs_arr or *pDs_arr is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_DSArrDeleteChecked(CONT_DSArr **pDs_arr);
/**
 * Returns the number of elements currently stored.
 *
 * @param ds_arr DSArr instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_API PRP_U32 PRP_CALL CONT_DSArrLen(const CONT_DSArr *ds_arr);
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
PRP_API void *PRP_CALL CONT_DSIdToDataUnchecked(const CONT_DSArr *ds_arr,
                                                CONT_DSId id);
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
PRP_API PRP_Result PRP_CALL CONT_DSIdToDataChecked(const CONT_DSArr *ds_arr,
                                                   CONT_DSId id, void **dest);
/**
 * Checks if the given id is valid.
 *
 * @param ds_arr DSArr instance.
 * @param id     The id to check.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_DSIdIsValidUnchecked(const CONT_DSArr *ds_arr,
                                                    CONT_DSId id);
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
PRP_API PRP_Result PRP_CALL CONT_DSIdIsValidChecked(const CONT_DSArr *ds_arr,
                                                    CONT_DSId id,
                                                    PRP_Bool *pRslt);
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
PRP_API PRP_Result PRP_CALL CONT_DSArrAddUnchecked(CONT_DSArr *ds_arr,
                                                   void *data, CONT_DSId *pId);
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
PRP_API PRP_Result PRP_CALL CONT_DSArrAddChecked(CONT_DSArr *ds_arr, void *data,
                                                 CONT_DSId *pId);
/**
 * Remove the data associated by the given id and invalidates the id.
 *
 * @param ds_arr DSArr instance.
 * @param pid    The pointer of the id to delete.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_DSArrDelElemUnchecked(CONT_DSArr *ds_arr,
                                                 CONT_DSId *pId);
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
PRP_API PRP_Result PRP_CALL CONT_DSArrDelElemChecked(CONT_DSArr *ds_arr,
                                                     CONT_DSId *pId);
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
PRP_API PRP_Result PRP_CALL CONT_DSArrReserveUnchecked(CONT_DSArr *ds_arr,
                                                       PRP_U32 count);
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
PRP_API PRP_Result PRP_CALL CONT_DSArrReserveChecked(CONT_DSArr *ds_arr,
                                                     PRP_U32 count);
/**
 * Iterates over all elements of the array.
 *
 * @param arr        DSArr instance.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_DSArrForEachUnchecked(
    CONT_DSArr *ds_arr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data);
/**
 * Iterates over all elements of the array.
 *
 * @param arr        DSArr instance.
 * @param cb         Callback invoked per element.
 * @parampUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_DSArrForEachChecked(
    CONT_DSArr *ds_arr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data);

#ifdef __cplusplus
}
#endif
