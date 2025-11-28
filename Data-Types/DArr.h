#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Utils/Defs.h"

/**
 * DArr(Dynamic Array), is an implementation of dynamic array that self grows.
 * It is equivalent to Python's lists or C++'s Vectors.
 *
 * It holds elements of a single SIZE(not a single type, since C is type
 * unsafe).
 */
typedef struct _DArr PRP_DArr;

/**
 * Creates the dynamic array with user specified initial cap.
 *
 * @param memb_size: The size of the members of the array.
 * @param cap: The initial capacity of the array
 *
 * @return The pointer of the array.
 */
PRP_FN_API PRP_DArr *PRP_FN_CALL PRP_DArrCreate(PRP_size memb_size,
                                                PRP_size cap);
/**
 * Creates the dynamic array with default initial cap of 16.
 *
 * @param memb_size: The size of the members of the array.
 *
 * @return The pointer of the array.
 */
PRP_FN_API PRP_DArr *PRP_FN_CALL PRP_DArrCreateDefault(PRP_size memb_size);
/**
 * Clones the array into a new array, preserving all the contents of the array
 * too.
 *
 * @param arr: The array to clone.
 *
 * @return The pointer to the cloned array.
 */
PRP_FN_API PRP_DArr *PRP_FN_CALL PRP_DArrClone(PRP_DArr *arr);

/**
 * Deletes the array and sets the original PRP_DArr * to PRP_null to prevent use
 * after free bugs.
 *
 * @param pArr: The pointer to the array pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the array is pArr is PRP_null or the array it
 * points to is invalid. Otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrDelete(PRP_DArr **pArr);

/**
 * Returns the raw memory pointer of the array to the user.
 * This function returns a non-fixed pointer to the array mem. If an operation
 * is performed to the array after getting the raw data, the raw data is no
 * longer guranteed to be valid.
 *
 * @param arr: The array to get the raw mem data of.
 * @param pLen: Pointer to where the len of the array will be stored to be used
 * by the caller to prevent unsafe usage.
 *
 * @return PRP_null if the array is invalid or pLen is PRP_null. Otherwise the
 * memory pointer of the array's raw memory.
 */
PRP_FN_API PRP_void *PRP_FN_CALL PRP_DArrRaw(PRP_DArr *arr, PRP_size *pLen);
/**
 * Returns the current len of the array that is passed to it.
 *
 * @param arr: The array to get the len of.
 *
 * @return PRP_INVALID_SIZE if the array is invalid, otherwise the actual len of
 * the array.
 */
PRP_FN_API PRP_size PRP_FN_CALL PRP_DArrLen(PRP_DArr *arr);
/**
 * Returns the current allocated cap of the array that is passed to it.
 *
 * @param arr: The array to get the cap of.
 *
 * @return PRP_INVALID_SIZE if the array is invalid, otherwise the actual cap of
 * the array.
 */
PRP_FN_API PRP_size PRP_FN_CALL PRP_DArrCap(PRP_DArr *arr);

/**
 * Gets the pointer to the element of the given index of the array.
 *
 * @param arr: The array to operate on.
 * @param i: The index to get the pointer to.
 *
 * @return PRP_null if array is invalid or the i is out of bound, otherwise the
 * pointer of the requested index.
 */
PRP_FN_API PRP_void *PRP_FN_CALL PRP_DArrGet(PRP_DArr *arr, PRP_size i);
/**
 * Sets the given index of the array with the given data.
 *
 * @param arr: The array to operate on.
 * @param i: The index to set the value of.
 * @param data: The pointer to the data that will be set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_OOB_ERROR if i is out of bounds of the array, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrSet(PRP_DArr *arr, PRP_size i,
                                              PRP_void *data);

/**
 * Pushes a new element into the given array, auto growing to accomodate for new
 * elements.
 *
 * @param arr: The array to push the element into.
 * @param data: The pointer to the data to be pushed into the array.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if pushing into the array is not possible,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrPush(PRP_DArr *arr, PRP_void *data);
/**
 * Reserves <count> number of elements in the array.
 *
 * @param arr: The array to reserve into.
 * @param count: The number of elements to reserve.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_MALLOC_ERROR if reserving failed due to realloc failure, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrReserve(PRP_DArr *arr,
                                                  PRP_size count);
/**
 * Inserts the given data into index <i> of the array.
 *
 * @param arr: The array to insert into.
 * @param data: The pointer to the data to insert.
 * @param i: The index to insert the data into.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_OOB_ERROR if i is out of bounds of the array,
 * PRP_FN_RES_EXHAUSTED_ERROR if accomodating an insert op is not possible,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrInsert(PRP_DArr *arr, PRP_void *data,
                                                 PRP_size i);

/**
 * Pops the last element of the array from the array, and optionally gives the
 * popped data to dest.
 *
 * @param arr: The array to pop data from.
 * @param dest: If not PRP_null, the popped data is copied into it.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if no elements remain to pop, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrPop(PRP_DArr *arr, PRP_void *dest);
/**
 * Removes the given index from the array and repacks the array.
 *
 * @param arr: The array to remove from.
 * @param dest: If not PRP_null, the removed data is copied into it.
 * @param i: The index that is to be removed from the array.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_OOB_ERROR if the given index is out of bounds of the array, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrRemove(PRP_DArr *arr, PRP_void *dest,
                                                 PRP_size i);

/**
 * Compares the given two array to see if their contents are exactly equal.
 *
 * @param arr1: The first array that is to be compared.
 * @param arr2: The other array that is to be compared.
 *
 * @return PRP_false if the arrays are dissimilar or are invalid, otherwise
 * PRP_true.
 */
PRP_FN_API PRP_bool PRP_FN_CALL PRP_DArrCmp(PRP_DArr *arr1, PRP_DArr *arr2);
/**
 * Joins the content of arr2 with arr1.
 *
 * @param arr1: The first array that will contain the final extended array.
 * @param arr2: The array that is to be merged with arr2.
 *
 * @return PRP_FN_INV_ARG_ERROR if the paramerters are invalid in any way,
 * PRP_FN_MALLOC_ERROR if realloc failed on arr1, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrExtend(PRP_DArr *arr1,
                                                 PRP_DArr *arr2);
/**
 * Resets the array to make it behave like a brand new array.
 *
 * @param arr: The array to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the array is invalid in some way, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrReset(PRP_DArr *arr);
/**
 * Shrinks the array to make its cap = its len.
 *
 * @param arr: The array to shrink fit.
 *
 * @return PRP_FN_INV_ARG_ERROR if the array is invalid in some way,
 * PRP_FN_MALLOC_ERROR if realloc failed on arr, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrShrinkFit(PRP_DArr *arr);
/**
 * Performs a foreach operation of each of the element of the array. Calling cb
 * per element.
 *
 * @param arr: The array on which the foreach will happen.
 * @param cb: The callback to be called per element. If this doesn't return
 * PRP_FN_SUCCESS, further execution will be halted.
 *
 * @return RP_FN_INV_ARG_ERROR if the array is invalid in some way,
 * PRP_FN_MALLOC_ERROR if realloc failed on arr, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL
PRP_DArrForEach(PRP_DArr *arr, PRP_FnCode (*cb)(PRP_void *val));

#ifdef __cplusplus
}
#endif
