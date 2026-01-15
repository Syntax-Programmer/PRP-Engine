#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

typedef struct _Bffr DT_Bffr;

/**
 * Creates the dynamic buffer with user specified initial cap.
 *
 * @param memb_size: The size of the members of the buffer.
 * @param cap: The initial capacity of the buffer
 *
 * @return The pointer of the buffer.
 */
PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreate(DT_size memb_size, DT_size cap);
/**
 * Creates the dynamic buffer with default initial cap of 16.
 *
 * @param memb_size: The size of the members of the buffer.
 *
 * @return The pointer of the buffer.
 */
PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreateDefault(DT_size memb_size);
/**
 * Clones the buffer into a new buffer, preserving all the contents of the
 * buffer too.
 *
 * @param bffr: The buffer to clone.
 *
 * @return The pointer to the cloned buffer.
 */
PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrClone(DT_Bffr *bffr);

/**
 * Deletes the buffer and sets the original DT_Bffr * to DT_null to prevent use
 * after free bugs.
 *
 * @param pBffr: The pointer to the buffer pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pBffr is DT_null or the array it points to is
 * invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrDelete(DT_Bffr **pBffr);

/**
 * Returns the raw memory pointer of the buffer to the user.
 * This function returns a non-fixed pointer to the buffer mem. If the buffer is
 * grown after getting the raw data, the raw data is no longer guaranteed to be
 * valid.
 *
 * @param bffr: The buffer to get the raw mem data of.
 * @param pCap: Pointer to where the cap of the buffer will be stored to be used
 * by the caller to prevent unsafe usage.
 *
 * @return DT_null if the array is invalid or pCap is DT_null, otherwise the
 * memory pointer of the buffer's raw memory.
 */
PRP_FN_API const DT_void *PRP_FN_CALL DT_BffrRaw(const DT_Bffr *bffr,
                                                 DT_size *pCap);
/**
 * Returns the current allocated cap of the buffer that is passed to it.
 *
 * @param bffr: The buffer to get the cap of.
 *
 * @return PRP_INVALID_SIZE if buffer is invalid, otherwise the actual cap of
 * the buffer.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BffrCap(const DT_Bffr *bffr);
/**
 * Returns the member size of the buffer that is passed to it.
 *
 * @param bffr: The buffer to get the memb_size of.
 *
 * @return PRP_INVALID_SIZE if buffer is invalid, otherwise the actual memb_size
 * of the buffer.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BffrMembSize(const DT_Bffr *bffr);
/**
 * Returns the max capacity of the buffer that is passed to it based on its
 * memb_size.
 *
 * @param bffr: The buffer to get the max capacity of.
 *
 * @return PRP_INVALID_SIZE if the buffer is invalid, otherwise the actual
 * max capacity of the buffer.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BffrMaxCap(const DT_Bffr *bffr);

/**
 * Gets the pointer to the element of the given index of the buffer.
 *
 * @param arr: The buffer to operate on.
 * @param i: The index to get the pointer to.
 *
 * @return DT_null if buffer is invalid or the i is out of bound, otherwise the
 * pointer of the requested index.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_BffrGet(const DT_Bffr *bffr, DT_size i);
/**
 * Sets the given index of the buffer with the given data.
 *
 * @param bffr: The buffer to operate on.
 * @param i: The index to set the value of.
 * @param pData: The pointer to the data that will be set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if i is out of bounds of the buffer, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSet(DT_Bffr *bffr, DT_size i,
                                             const DT_void *pData);
/**
 * Sets the same element 'data' to all the indices b/w i and j excluding j.
 *
 * @param bffr: The buffer to operate on.
 * @param i: The index to start from.
 * @param j: The index to stop on.
 * @param pData: The poinbter to the data to set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way or i >
 * j, PRP_FN_OOB_ERROR if i or j is out of bounds of the buffer, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSetRange(DT_Bffr *bffr, DT_size i,
                                                  DT_size j,
                                                  const DT_void *pData);
/**
 * Sets count number of elements in order starting from index i.
 *
 * @param bffr: The buffer to operate on.
 * @param i: The index to start from.
 * @param data_arr: The array of data to set.
 * @param len: The len of the data_arr.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if i or i + len is out of bounds of the buffer, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSetMany(DT_Bffr *bffr, DT_size i,
                                                 const DT_void *data_arr,
                                                 DT_size len);

/**
 * Compares the given two buffer to see if their contents are exactly equal.
 *
 * @param bffr1: The first buffer that is to be compared.
 * @param bffr2: The other buffer that is to be compared.
 * @param pRslt: The pointer to the variable where the boolean result will be
 * stored.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrCmp(const DT_Bffr *bffr1,
                                             const DT_Bffr *bffr2,
                                             DT_bool *pRslt);
/**
 * Joins the content of bffr2 with bffr1.
 *
 * @param bffr1: The first buffer that will contain the final extended buffer.
 * @param bffr2: The buffer that is to be merged with bffr1.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_MALLOC_ERROR if realloc failed on bffr1, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrExtend(DT_Bffr *bffr1,
                                                const DT_Bffr *bffr2);
/**
 * Swaps the elements of the given two indices.
 *
 * @param bffr: The buffer to operate on.
 * @param i: The first index.
 * @param j: The second index.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if the indices are out of bound, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSwap(DT_Bffr *bffr, DT_size i,
                                              DT_size j);
/**
 * Clears the buffer to 0.
 *
 * @param bffr: The buffer to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the buffer is invalid in some way, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrClear(DT_Bffr *bffr);
/**
 * Changes the capacity of the given buffer to the provided new cap safely.
 *
 * @param bffr: The buffer to change the cap of.
 * @param new_cap: The new cap of the buffer to change to.
 *
 * @return PRP_FN_MALLOC_ERROR if the reallocation fails, otherwise
 * PRP_FN_SUCCESS;
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrChangeSize(DT_Bffr *bffr,
                                                    DT_size new_cap);

#ifdef __cplusplus
}
#endif
