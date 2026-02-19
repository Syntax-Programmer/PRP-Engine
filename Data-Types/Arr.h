#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

/**
 * Arr(Dynamic Array), is an implementation of dynamic array that self grows.
 * It is equivalent to Python's lists or C++'s Vectors.
 *
 * It holds elements of a single SIZE(not a single type, since C is type
 * unsafe).
 */
typedef struct _Arr DT_Arr;

#define DT_ARR_DEFAULT_CAP (16)
#define DT_ARR_MAX_CAP(memb_size) (DT_SIZE_MAX / (memb_size))

/**
 * @return The last error code set by the array functions that don't return
 * PRP_Result explicitly.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrGetLastErrCode(DT_void);

/**
 * Creates the dynamic array with user specified initial cap.
 *
 * @param memb_size: The size of the members of the array.
 * @param cap: The initial capacity of the array
 *
 * @return The pointer of the array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreateUnchecked(DT_size memb_size,
                                                     DT_size cap);
/**
 * Creates the dynamic array with user specified initial cap.
 *
 * @param memb_size: The size of the members of the array.
 * @param cap: The initial capacity of the array
 *
 * @return The pointer of the array.
 */
PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreateChecked(DT_size memb_size,
                                                   DT_size cap);
/**
 * Clones the array into a new array, preserving all the contents of the array
 * too.
 *
 * @param arr: The array to clone.
 *
 * @return The pointer to the cloned array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCloneUnchecked(const DT_Arr *arr);
/**
 * Clones the array into a new array, preserving all the contents of the array
 * too.
 *
 * @param arr: The array to clone.
 *
 * @return The pointer to the cloned array.
 */
PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCloneChecked(const DT_Arr *arr);

/**
 * Deletes the array and sets the original DT_Arr * to DT_null to prevent use
 * after free bugs.
 *
 * @param pArr: The pointer to the array pointer to delete.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrDeleteUnchecked(DT_Arr **pArr);
/**
 * Deletes the array and sets the original DT_Arr * to DT_null to prevent use
 * after free bugs.
 *
 * @param pArr: The pointer to the array pointer to delete.
 *
 * @return PRP_ERR_INV_ARG if the pArr or *pArr is DT_null, otherwise it
 * returns PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrDeleteChecked(DT_Arr **pArr);

/**
 * Returns the raw memory pointer of the array to the user.
 * This function returns a non-fixed pointer to the array mem. If an operation
 * is performed to the array after getting the raw data, the raw data is no
 * longer guaranteed to be valid.
 *
 * @param arr: The array to get the raw mem data of.
 * @param pLen: Pointer to where the len of the array will be stored to be used
 * by the caller to prevent unsafe usage.
 *
 * @return The memory pointer of the array's raw memory.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRawUnchecked(const DT_Arr *arr,
                                                         DT_size *pLen);
/**
 * Returns the raw memory pointer of the array to the user.
 * This function returns a non-fixed pointer to the array mem. If an operation
 * is performed to the array after getting the raw data, the raw data is no
 * longer guaranteed to be valid.
 *
 * @param arr: The array to get the raw mem data of.
 * @param pLen: Pointer to where the len of the array will be stored to be used
 * by the caller to prevent unsafe usage.
 *
 * @return DT_null if the array is invalid or pLen is DT_null, otherwise the
 * memory pointer of the array's raw memory.
 */
PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRawChecked(const DT_Arr *arr,
                                                       DT_size *pLen);
/**
 * Returns the current len of the array that is passed to it.
 *
 * @param arr: The array to get the len of.
 *
 * @return The len of the array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrLenUnchecked(const DT_Arr *arr);
/**
 * Returns the current len of the array that is passed to it.
 *
 * @param arr: The array to get the len of.
 *
 * @return PRP_INVALID_SIZE if the array is invalid, otherwise the actual len of
 * the array.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrLenChecked(const DT_Arr *arr);
/**
 * Returns the current allocated cap of the array that is passed to it.
 *
 * @param arr: The array to get the cap of.
 *
 * @return The cap of the array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrCapUnchecked(const DT_Arr *arr);
/**
 * Returns the current allocated cap of the array that is passed to it.
 *
 * @param arr: The array to get the cap of.
 *
 * @return PRP_INVALID_SIZE if the array is invalid, otherwise the actual cap of
 * the array.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrCapChecked(const DT_Arr *arr);
/**
 * Returns the member size of the array that is passed to it.
 *
 * @param arr: The array to get the memb_size of.
 *
 * @return The memb size of the array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSizeUnchecked(const DT_Arr *arr);
/**
 * Returns the member size of the array that is passed to it.
 *
 * @param arr: The array to get the memb_size of.
 *
 * @return PRP_INVALID_SIZE if the array is invalid, otherwise the actual
 * memb_size of the array.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSizeChecked(const DT_Arr *arr);
/**
 * Returns the max capacity of the array that is passed to it based on its
 * memb_size.
 *
 * @param arr: The array to get the max capacity of.
 *
 * @return The max capacity of the array..
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCapUnchecked(const DT_Arr *arr);
/**
 * Returns the max capacity of the array that is passed to it based on its
 * memb_size.
 *
 * @param arr: The array to get the max capacity of.
 *
 * @return PRP_INVALID_SIZE if the array is invalid, otherwise the actual
 * max capacity of the array.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCapChecked(const DT_Arr *arr);

/**
 * Gets the pointer to the element of the given index of the array.
 *
 * @param arr: The array to operate on.
 * @param i: The index to get the pointer to.
 *
 * @return The pointer of the requested index.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGetUnchecked(const DT_Arr *arr,
                                                   DT_size i);
/**
 * Gets the pointer to the element of the given index of the array.
 *
 * @param arr: The array to operate on.
 * @param i: The index to get the pointer to.
 *
 * @return DT_null if array is invalid or the i is out of bound, otherwise the
 * pointer of the requested index.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGetChecked(const DT_Arr *arr, DT_size i);
/**
 * Sets the given index of the array with the given data.
 *
 * @param arr: The array to operate on.
 * @param i: The index to set the value of.
 * @param pData: The pointer to the data that will be set.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrSetUnchecked(DT_Arr *arr, DT_size i,
                                                  const DT_void *pData);
/**
 * Sets the given index of the array with the given data.
 *
 * @param arr: The array to operate on.
 * @param i: The index to set the value of.
 * @param pData: The pointer to the data that will be set.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, PRP_ERR_OOB
 * if i is out of bounds of the array, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSetChecked(DT_Arr *arr, DT_size i,
                                                   const DT_void *pData);
/**
 * Pushes a new element into the given array, auto growing to accommodate for
 * new elements.
 *
 * @param arr: The array to push the element into.
 * @param pData: The pointer to the data to be pushed into the array.
 *
 * @return PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if pushing into the array is not
 * possible, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPushUnchecked(DT_Arr *arr,
                                                      const DT_void *pData);
/**
 * Pushes a new element into the given array, auto growing to accommodate for
 * new elements.
 *
 * @param arr: The array to push the element into.
 * @param pData: The pointer to the data to be pushed into the array.
 *
 * @return PRP_INV_ARG_ERR if the parameters are invalid in any way,
 * PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if pushing into the array is not possible,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPushChecked(DT_Arr *arr,
                                                    const DT_void *pData);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr: The array to reserve into.
 * @param count: The number of elements to reserve.
 *
 * @return PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if reserving <count> elements into
 * the array is not possible, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserveUnchecked(DT_Arr *arr,
                                                         DT_size count);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr: The array to reserve into.
 * @param count: The number of elements to reserve.
 *
 * @return PRP_INV_ARG_ERR if the parameters are invalid in any way,
 * PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if reserving <count> elements into the
 * array is not possible, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserveChecked(DT_Arr *arr,
                                                       DT_size count);
/**
 * Inserts the given data into index <i> of the array.
 *
 * @param arr: The array to insert into.
 * @param pData: The pointer to the data to insert.
 * @param i: The index to insert the data into.
 *
 * @return PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if pushing into the array is not
 * possible, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsertUnchecked(DT_Arr *arr,
                                                        const DT_void *pData,
                                                        DT_size i);
/**
 * Inserts the given data into index <i> of the array.
 *
 * @param arr: The array to insert into.
 * @param pData: The pointer to the data to insert.
 * @param i: The index to insert the data into.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, PRP_ERR_OOB
 * if i is out of bounds of the array, PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if
 * pushing into the array is not possible, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsertChecked(DT_Arr *arr,
                                                      const DT_void *pData,
                                                      DT_size i);

/**
 * Pops the last element of the array from the array, and optionally gives the
 * popped data to dest.
 *
 * @param arr: The array to pop data from.
 * @param pDest: If not DT_null, the popped data is copied into it.
 *
 * @return PRP_ERR_RES_EXHAUSTED if no elements remain to pop, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPopUnchecked(DT_Arr *arr,
                                                     DT_void *pDest);
/**
 * Pops the last element of the array from the array, and optionally gives the
 * popped data to dest.
 *
 * @param arr: The array to pop data from.
 * @param pDest: If not DT_null, the popped data is copied into it.
 *
 * @return PRP_ERR_INV_ARG if the arr invalid, PRP_ERR_RES_EXHAUSTED if no
 * elements remain to pop, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPopChecked(DT_Arr *arr, DT_void *pDest);
/**
 * Removes the given index from the array and repacks the array.
 *
 * @param arr: The array to remove from.
 * @param pDest: If not DT_null, the removed data is copied into it.
 * @param i: The index that is to be removed from the array.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrRemoveUnchecked(DT_Arr *arr,
                                                     DT_void *pDest, DT_size i);
/**
 * Removes the given index from the array and repacks the array.
 *
 * @param arr: The array to remove from.
 * @param pDest: If not DT_null, the removed data is copied into it.
 * @param i: The index that is to be removed from the array.
 *
 * @return PRP_ERR_INV_ARG if the arr invalid, PRP_ERR_OOB if the given index is
 * out of bounds of the array, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrRemoveChecked(DT_Arr *arr,
                                                      DT_void *pDest,
                                                      DT_size i);

/**
 * Compares the given two array to see if their contents are exactly equal.
 *
 * @param arr1: The first array that is to be compared.
 * @param arr2: The other array that is to be compared.
 *
 * @return DT_false if the arrays are equal, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_ArrCmpUnchecked(const DT_Arr *arr1,
                                                  const DT_Arr *arr2);
/**
 * Compares the given two array to see if their contents are exactly equal.
 *
 * @param arr1: The first array that is to be compared.
 * @param arr2: The other array that is to be compared.
 * @param pRslt: The pointer to the variable where the boolean result will be
 * stored.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCmpChecked(const DT_Arr *arr1,
                                                   const DT_Arr *arr2,
                                                   DT_bool *pRslt);
/**
 * Joins the content of arr2 with arr1.
 *
 * @param arr1: The first array that will contain the final extended array.
 * @param arr2: The array that is to be merged with arr1.
 *
 * @return PRP_ERR_RES_EXHAUSTED if the combined size exceeds the max cap of the
 * array, PRP_ERR_OOM if realloc failed on arr1, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtendUnchecked(DT_Arr *arr1,
                                                        const DT_Arr *arr2);
/**
 * Joins the content of arr2 with arr1.
 *
 * @param arr1: The first array that will contain the final extended array.
 * @param arr2: The array that is to be merged with arr1.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_RES_EXHAUSTED if the combined size exceeds the max cap of the array,
 * PRP_ERR_OOM if realloc failed on arr1, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtendChecked(DT_Arr *arr1,
                                                      const DT_Arr *arr2);
/**
 * Swaps the elements of the given two indices.
 *
 * @param arr: The array to operate on.
 * @param i: The first index.
 * @param j: The second index.
 * @param swap_bffr: A buffer which will hold temp data tell the swap is
 * happening. its size = memb_size.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrSwapUnchecked(DT_Arr *arr, DT_size i,
                                                   DT_size j,
                                                   DT_void *swap_bffr);
/**
 * Swaps the elements of the given two indices.
 *
 * @param arr: The array to operate on.
 * @param i: The first index.
 * @param j: The second index.
 * @param swap_bffr: A buffer which will hold temp data tell the swap is
 * happening. its size = memb_size.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB if the indices are out of bound, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSwapChecked(DT_Arr *arr, DT_size i,
                                                    DT_size j,
                                                    DT_void *swap_bffr);
/**
 * Resets the array to make it behave like a brand new array.
 *
 * @param arr: The array to reset.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ArrResetUnchecked(DT_Arr *arr);
/**
 * Resets the array to make it behave like a brand new array.
 *
 * @param arr: The array to reset.
 *
 * @return PRP_ERR_INV_ARG if the array is invalid in some way, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrResetChecked(DT_Arr *arr);
/**
 * Shrinks the array to make its cap = len.
 *
 * @param arr: The array to shrink fit.
 *
 * @return PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if shrinking the array is not
 * possible, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFitUnchecked(DT_Arr *arr);
/**
 * Shrinks the array to make its cap = len.
 *
 * @param arr: The array to shrink fit.
 *
 * @return PRP_INV_ARG_ERR if the parameters are invalid in any way,
 * PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if shrinking the array is not possible,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFitChecked(DT_Arr *arr);
/**
 * Performs a foreach operation of each of the element of the array. Calling cb
 * per element.
 *
 * @param arr: The array on which the foreach will happen.
 * @param cb: The callback to be called per element. If this doesn't return
 * PRP_OK, further execution will be halted.
 * @param user_data: The data user wants to pass in as additional context.
 *
 * @return PRP_OK, or the callback error code.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrForEachUnchecked(
    DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);
/**
 * Performs a foreach operation of each of the element of the array. Calling cb
 * per element.
 *
 * @param arr: The array on which the foreach will happen.
 * @param cb: The callback to be called per element. If this doesn't return
 * PRP_OK, further execution will be halted.
 * @param user_data: The data user wants to pass in as additional context.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in some way,
 * otherwise PRP_OK, or the callback error code.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrForEachChecked(
    DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data);

#ifdef __cplusplus
}
#endif
