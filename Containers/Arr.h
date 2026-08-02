#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * CONT_Arr
 *
 * A dynamically resizing array storing elements of fixed size.
 *
 * - Elements are stored contiguously in memory.
 * - Capacity grows automatically on demand.
 * - Not type-safe; user must ensure correct usage.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Array must be deleted using CONT_ArrDelete* APIs.
 */
typedef struct CONT_Arr CONT_Arr;

#define CONT_ARR_DEFAULT_CAP (16)
#define CONT_ARR_MAX_CAP(memb_size) (PRP_SIZE_MAX / (memb_size))

/**
 * Checks whether the given array is structurally valid.
 *
 * @param pArr Pointer to the array.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL CONT_ArrIsValid(const CONT_Arr *pArr);

/**
 * Creates a dynamic array.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param ppArr     Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCreateUnchecked(PRP_Size memb_size,
                                                    PRP_Size cap,
                                                    CONT_Arr **ppArr);
/**
 * Creates a dynamic array with validation.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param ppArr     Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCreateChecked(PRP_Size memb_size,
                                                  PRP_Size cap,
                                                  CONT_Arr **ppArr);
/**
 * Deep clones the given array.
 *
 * @param pArr  The array to clone.
 * @param ppArr Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCloneUnchecked(const CONT_Arr *pArr,
                                                   CONT_Arr **ppArr);
/**
 * Deep clones the given array with validation.
 *
 * @param pArr  The array to clone.
 * @param ppArr Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCloneChecked(const CONT_Arr *pArr,
                                                 CONT_Arr **ppArr);
/**
 * Creates the dynamic array with specified data.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param membs     The array of data to initialize with.
 * @param len       Len of the membs array.
 * @param ppArr     Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCreateWithDataUnchecked(PRP_Size memb_size,
                                                            const void *membs,
                                                            PRP_Size len,
                                                            CONT_Arr **ppArr);
/**
 * Creates the dynamic array with specified data.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param membs     The array of data to initialize with.
 * @param len       Len of the membs array.
 * @param ppArr     Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCreateWithDataChecked(PRP_Size memb_size,
                                                          const void *membs,
                                                          PRP_Size len,
                                                          CONT_Arr **ppArr);

/**
 * Deletes the array and nullifies the pointer.
 *
 * @param ppArr Pointer to array pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ArrDeleteUnchecked(CONT_Arr **ppArr);
/**
 * Deletes the array and nullifies the pointer.
 *
 * @param ppArr Pointer to array pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if ppArr or *ppArr is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrDeleteChecked(CONT_Arr **ppArr);

/**
 * Returns the raw memory pointer of the array contenets.
 *
 * The pointer is not guaranteed to be valid after a mutation operation
 *
 * @param pArr Array instance.
 * @param pLen Pointer to where to store len of the array.
 *
 * @return The raw memory pointer of the array.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API const void *PRP_CALL CONT_ArrRawUnchecked(const CONT_Arr *pArr,
                                                  PRP_Size *pLen);
/**
 * Returns the raw memory pointer of the array contenets.
 *
 * The pointer is not guaranteed to be valid after a mutation operation
 *
 * @param pArr Array instance.
 * @param pLen Pointer to where to store len of the array.
 * @param pRaw Pointer to where to store raw mem of the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrRawChecked(const CONT_Arr *pArr,
                                               PRP_Size *pLen,
                                               const void **pRaw);
/**
 * Returns the number of elements currently stored.
 *
 * @param pArr Array instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_ArrLen(const CONT_Arr *pArr);
/**
 * Returns the capacity (number of elements) of the array.
 *
 * @param pArr Array instance.
 *
 * @return Array capacity.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_ArrCap(const CONT_Arr *pArr);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param pArr Array instance.
 *
 * @return Element size.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_ArrMembSize(const CONT_Arr *pArr);
/**
 * Returns the maximum possible capacity for this array configuration.
 *
 * @param pArr Array instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid array (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_ArrMaxCap(const CONT_Arr *pArr);

/**
 * Retrieves the value of the given index.
 *
 * @param pArr Array instance.
 * @param i    The index into the array.
 *
 * @return The value pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void *PRP_CALL CONT_ArrGetUnchecked(const CONT_Arr *pArr, PRP_Size i);
/**
 * Retrieves the value of the given index.
 *
 * @param pArr Array instance.
 * @param i    The index into the array.
 * @param dest The pointer to the value pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrGetChecked(const CONT_Arr *pArr, PRP_Size i,
                                               void **dest);
/**
 * Sets the value of the given index.
 *
 * @param pArr  Array instance.
 * @param i     The index into the array.
 * @param pData The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ArrSetUnchecked(CONT_Arr *pArr, PRP_Size i,
                                           const void *pData);
/**
 * Sets the value of the given index.
 *
 * @param pArr  Array instance.
 * @param i     The index into the array.
 * @param pData The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrSetChecked(CONT_Arr *pArr, PRP_Size i,
                                               const void *pData);
/**
 * Pushes a new element at the end of the array.
 *
 * @param pArr  Array instance.
 * @param pData The pointer to the data to push.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrPushUnchecked(CONT_Arr *pArr,
                                                  const void *pData);
/**
 * Pushes a new element at the end of the array.
 *
 * @param pArr  Array instance.
 * @param pData The pointer to the data to push.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrPushChecked(CONT_Arr *pArr,
                                                const void *pData);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param pArr  Array instance.
 * @param count Number of elements to reserve.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrReserveUnchecked(CONT_Arr *pArr,
                                                     PRP_Size count);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param pArr  Array instance.
 * @param count Number of elements to reserve.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrReserveChecked(CONT_Arr *pArr,
                                                   PRP_Size count);
/**
 * Inserts the value to a given index.
 *
 * @param pArr  Array instance.
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
PRP_API PRP_Result PRP_CALL CONT_ArrInsertUnchecked(CONT_Arr *pArr,
                                                    const void *pData,
                                                    PRP_Size i);
/**
 * Inserts the value to a given index.
 *
 * @param pArr  Array instance.
 * @param pData The pointer to the data to push.
 * @param i     The index into the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrInsertChecked(CONT_Arr *pArr,
                                                  const void *pData,
                                                  PRP_Size i);

/**
 * Pops the last element from the array
 *
 * @param pArr  Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no elements to pop.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrPopUnchecked(CONT_Arr *pArr, void *pDest);
/**
 * Pops the last element from the array
 *
 * @param pArr  Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no elements to pop.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrPopChecked(CONT_Arr *pArr, void *pDest);
/**
 * Removes the given index and repack the array.
 *
 * @param pArr  Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 * @param i     The index to remove.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ArrRemoveUnchecked(CONT_Arr *pArr, void *pDest,
                                              PRP_Size i);
/**
 * Removes the given index and repack the array.
 *
 * @param pArr  Array instance.
 * @param pDest The pointer to the poped data. Its an optional param.
 * @param i     The index to remove.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrRemoveChecked(CONT_Arr *pArr, void *pDest,
                                                  PRP_Size i);

/**
 * Comares the contents of the array.
 *
 * @param pArr1 Array instance 1.
 * @param pArr2 Array instance 2.
 *
 * @return PRP_True if equal, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_ArrCmpUnchecked(const CONT_Arr *pArr1,
                                               const CONT_Arr *pArr2);
/**
 * Comares the contents of the array.
 *
 * @param pArr1 Array instance 1.
 * @param pArr2 Array instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrCmpChecked(const CONT_Arr *pArr1,
                                               const CONT_Arr *pArr2,
                                               PRP_Bool *pRslt);
/**
 * Extends pArr2 into pArr1.
 *
 * @param pArr1 Array instance 1.
 * @param pArr2 Array instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrExtendUnchecked(CONT_Arr *pArr1,
                                                    const CONT_Arr *pArr2);
/**
 * Extends pArr2 into pArr1.
 *
 * @param pArr1 Array instance 1.
 * @param pArr2 Array instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrExtendChecked(CONT_Arr *pArr1,
                                                  const CONT_Arr *pArr2);
/**
 * Swaps the elements in the given indices.
 *
 * @param pArr      Array instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal pArr's memb size.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ArrSwapUnchecked(CONT_Arr *pArr, PRP_Size i,
                                            PRP_Size j, void *swap_bffr);
/**
 * Swaps the elements in the given indices.
 *
 * @param pArr      Array instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal pArr's memb size.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrSwapChecked(CONT_Arr *pArr, PRP_Size i,
                                                PRP_Size j, void *swap_bffr);
/**
 * Resets the array.
 *
 * Only sets len to 0 and 0s out everything, mem leak will happen if elements
 * have internal allocations.
 *
 * @param pArr Array instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ArrResetUnchecked(CONT_Arr *pArr);
/**
 * Resets the array.
 *
 * Only sets len to 0 and 0s out everything, mem leak will happen if elements
 * have internal allocations.
 *
 * @param pArr Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrResetChecked(CONT_Arr *pArr);
/**
 * Shrinks the array cap to match its len.
 *
 * @param pArr Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrShrinkFitUnchecked(CONT_Arr *pArr);
/**
 * Shrinks the array cap to match its len.
 *
 * @param pArr Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrShrinkFitChecked(CONT_Arr *pArr);
/**
 * Iterates over all elements of the array.
 *
 * @param pArr       Array instance.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrForEachUnchecked(
    CONT_Arr *pArr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data);
/**
 * Iterates over all elements of the array.
 *
 * @param pArr       Array instance.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ArrForEachChecked(
    CONT_Arr *pArr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data);

#ifdef __cplusplus
}
#endif
