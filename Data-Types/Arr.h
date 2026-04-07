#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

/**
 * DT_Arr
 *
 * A dynamically resizing array storing elements of fixed size.
 *
 * - Elements are stored contiguously in memory.
 * - Capacity grows automatically on demand.
 * - Not type-safe; user must ensure correct usage.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Array must be deleted using DT_ArrDelete* APIs.
 */
typedef struct _Arr DT_Arr;

#define DT_ARR_DEFAULT_CAP (16)
#define DT_ARR_MAX_CAP(memb_size) (DT_SIZE_MAX / (memb_size))

/**
 * Checks whether the given array is structurally valid.
 *
 * @param arr Pointer to the array.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_ArrIsValid(const DT_Arr *arr);

/**
 * Creates a dynamic array.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param out       Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCreateUnchecked(DT_size memb_size,
                                                        DT_size cap,
                                                        DT_Arr **out);
/**
 * Creates a dynamic array with validation.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param out       Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCreateChecked(DT_size memb_size,
                                                      DT_size cap,
                                                      DT_Arr **out);
/**
 * Deep clones the given array.
 *
 * @param arr The array to clone.
 * @param out Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCloneUnchecked(const DT_Arr *arr,
                                                       DT_Arr **out);
/**
 * Deep clones the given array with validation.
 *
 * @param arr The array to clone.
 * @param out Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCloneChecked(const DT_Arr *arr,
                                                     DT_Arr **out);
/**
 * Creates the dynamic array with specified data.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param membs     The array of data to initialize with.
 * @param len       Len of the membs array.
 * @param out       Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCreateWithDataUnchecked(
    DT_size memb_size, const DT_void *membs, DT_size len, DT_Arr **out);
/**
 * Creates the dynamic array with specified data.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param membs     The array of data to initialize with.
 * @param len       Len of the membs array.
 * @param out       Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCreateWithDataChecked(
    DT_size memb_size, const DT_void *membs, DT_size len, DT_Arr **out);

/**
 * Deletes the array and nullifies the pointer.
 *
 * @param pArr Pointer to array pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrDeleteUnchecked(DT_Arr **pArr);
/**
 * Deletes the array and nullifies the pointer.
 *
 * @param pArr Pointer to array pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArr or *pArr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrDeleteChecked(DT_Arr **pArr);

/**
 * Returns the raw memory pointer of the array contenets.
 *
 * The pointer is not guaranteed to be valid after a mutation operation
 *
 * @param arr  Array instance.
 * @param pLen Pointer to where to store len of the array.
 *
 * @return The raw memory pointer of the array.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRawUnchecked(const DT_Arr *arr,
                                                         DT_size *pLen);
/**
 * Returns the raw memory pointer of the array contenets.
 *
 * The pointer is not guaranteed to be valid after a mutation operation
 *
 * @param arr  Array instance.
 * @param pLen Pointer to where to store len of the array.
 * @param pRaw Pointer to where to store raw mem of the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrRawChecked(const DT_Arr *arr,
                                                   DT_size *pLen,
                                                   const DT_void **pRaw);
/**
 * Returns the number of elements currently stored.
 *
 * @param arr Array instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrLen(const DT_Arr *arr);
/**
 * Returns the capacity (number of elements) of the array.
 *
 * @param arr Array instance.
 *
 * @return Array capacity.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrCap(const DT_Arr *arr);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param arr Array instance.
 *
 * @return Element size.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSize(const DT_Arr *arr);
/**
 * Returns the maximum possible capacity for this array configuration.
 *
 * @param arr Array instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCap(const DT_Arr *arr);

/**
 * Retrieves the value of the given index.
 *
 * @param arr Array instance.
 * @param i   The index into the array.
 *
 * @return The value pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGetUnchecked(const DT_Arr *arr,
                                                   DT_size i);
/**
 * Retrieves the value of the given index.
 *
 * @param arr  Array instance.
 * @param i    The index into the array.
 * @param dest The pointer to the value pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrGetChecked(const DT_Arr *arr, DT_size i,
                                                   DT_void **dest);
/**
 * Sets the value of the given index.
 *
 * @param arr   Array instance.
 * @param i     The index into the array.
 * @param pData The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrSetUnchecked(DT_Arr *arr, DT_size i,
                                                  const DT_void *pData);
/**
 * Sets the value of the given index.
 *
 * @param arr   Array instance.
 * @param i     The index into the array.
 * @param pData The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSetChecked(DT_Arr *arr, DT_size i,
                                                   const DT_void *pData);
/**
 * Pushes a new element at the end of the array.
 *
 * @param arr   Array instance.
 * @param pData The pointer to the data to push.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPushUnchecked(DT_Arr *arr,
                                                      const DT_void *pData);
/**
 * Pushes a new element at the end of the array.
 *
 * @param arr   Array instance.
 * @param pData The pointer to the data to push.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPushChecked(DT_Arr *arr,
                                                    const DT_void *pData);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr   Array instance.
 * @param count Number of elements to reserve.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserveUnchecked(DT_Arr *arr,
                                                         DT_size count);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr   Array instance.
 * @param count Number of elements to reserve.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserveChecked(DT_Arr *arr,
                                                       DT_size count);
/**
 * Inserts the value to a given index.
 *
 * @param arr   Array instance.
 * @param pData The pointer to the data to push.
 * @param i     The index into the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsertUnchecked(DT_Arr *arr,
                                                        const DT_void *pData,
                                                        DT_size i);
/**
 * Inserts the value to a given index.
 *
 * @param arr   Array instance.
 * @param pData The pointer to the data to push.
 * @param i     The index into the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsertChecked(DT_Arr *arr,
                                                      const DT_void *pData,
                                                      DT_size i);

/**
 * Pops the last element from the array
 *
 * @param arr   Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no elements to pop.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPopUnchecked(DT_Arr *arr,
                                                     DT_void *pDest);
/**
 * Pops the last element from the array
 *
 * @param arr   Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no elements to pop.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPopChecked(DT_Arr *arr, DT_void *pDest);
/**
 * Removes the given index and repack the array.
 *
 * @param arr   Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 * @param i     The index to remove.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrRemoveUnchecked(DT_Arr *arr,
                                                     DT_void *pDest, DT_size i);
/**
 * Removes the given index and repack the array.
 *
 * @param arr   Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 * @param i     The index to remove.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrRemoveChecked(DT_Arr *arr,
                                                      DT_void *pDest,
                                                      DT_size i);

/**
 * Comares the contents of the array.
 *
 * @param arr1 Array instance 1.
 * @param arr2 Array instance 2.
 *
 * @return DT_true if equal, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_ArrCmpUnchecked(const DT_Arr *arr1,
                                                  const DT_Arr *arr2);
/**
 * Comares the contents of the array.
 *
 * @param arr1  Array instance 1.
 * @param arr2  Array instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCmpChecked(const DT_Arr *arr1,
                                                   const DT_Arr *arr2,
                                                   DT_bool *pRslt);
/**
 * Extends arr2 into arr1.
 *
 * @param arr1 Array instance 1.
 * @param arr2 Array instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtendUnchecked(DT_Arr *arr1,
                                                        const DT_Arr *arr2);
/**
 * Extends arr2 into arr1.
 *
 * @param arr1 Array instance 1.
 * @param arr2 Array instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtendChecked(DT_Arr *arr1,
                                                      const DT_Arr *arr2);
/**
 * Swaps the elements in the given indices.
 *
 * @param arr       Array instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal arr's memb size.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrSwapUnchecked(DT_Arr *arr, DT_size i,
                                                   DT_size j,
                                                   DT_void *swap_bffr);
/**
 * Swaps the elements in the given indices.
 *
 * @param arr       Array instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal arr's memb size.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSwapChecked(DT_Arr *arr, DT_size i,
                                                    DT_size j,
                                                    DT_void *swap_bffr);
/**
 * Resets the array.
 *
 * Only sets len to 0 and 0s out everything, mem leak will happen if elements
 * have internal allocations.
 *
 * @param arr Array instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrResetUnchecked(DT_Arr *arr);
/**
 * Resets the array.
 *
 * Only sets len to 0 and 0s out everything, mem leak will happen if elements
 * have internal allocations.
 *
 * @param arr Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrResetChecked(DT_Arr *arr);
/**
 * Shrinks the array cap to match its len.
 *
 * @param arr Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFitUnchecked(DT_Arr *arr);
/**
 * Shrinks the array cap to match its len.
 *
 * @param arr Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFitChecked(DT_Arr *arr);
/**
 * Iterates over all elements of the array.
 *
 * @param arr       Array instance.
 * @param cb        Callback invoked per element.
 * @param user_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrForEachUnchecked(
    DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);
/**
 * Iterates over all elements of the array.
 *
 * @param arr       Array instance.
 * @param cb        Callback invoked per element.
 * @param user_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrForEachChecked(
    DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
