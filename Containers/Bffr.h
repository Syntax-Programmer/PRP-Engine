#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * CONT_Bffr
 *
 * A dynamically resizing buffer storing elements of fixed size.
 *
 * - Elements are stored contiguously in memory.
 * - Capacity grows is controlled by user.
 * - Not type-safe; user must ensure correct usage.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Buffer  must be deleted using CONT_BffrDelete* APIs.
 */
typedef struct _Bffr CONT_Bffr;

#define CONT_BFFR_DEFAULT_CAP (16)
#define CONT_BFFR_MAX_CAP(memb_size) (PRP_SIZE_MAX / (memb_size))

/**
 * Checks whether the given buffer is structurally valid.
 *
 * @param bffr Pointer to the buffer.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BffrIsValid(const CONT_Bffr *bffr);

/**
 * Creates a dynamic buffer.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param pBffr     Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCreateUnchecked(PRP_Size memb_size,
                                                         PRP_Size cap,
                                                         CONT_Bffr **pBffr);
/**
 * Creates a dynamic buffer.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param pBffr     Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCreateChecked(PRP_Size memb_size,
                                                       PRP_Size cap,
                                                       CONT_Bffr **pBffr);
/**
 * Deep clones the given buffer.
 *
 * @param bffr  The buffer to clone.
 * @param pBffr Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCloneUnchecked(const CONT_Bffr *bffr,
                                                        CONT_Bffr **pBffr);
/**
 * Deep clones the given buffer with validation.
 *
 * @param bffr  The buffer to clone.
 * @param pBffr Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCloneChecked(const CONT_Bffr *bffr,
                                                      CONT_Bffr **pBffr);

/**
 * Deletes the buffer and nullifies the pointer.
 *
 * @param pBffr Pointer to buffer pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_BffrDeleteUnchecked(CONT_Bffr **pBffr);
/**
 * Deletes the buffer and nullifies the pointer.
 *
 * @param pBffr Pointer to buffer pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pBffr or *pBffr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrDeleteChecked(CONT_Bffr **pBffr);

/**
 * Returns the raw memory pointer of the buffer contents.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param bffr Buffer instance.
 * @param pCap Pointer to where to store cap of the buffer.
 *
 * @return The raw memory pointer of the buffer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API const void *PRP_FN_CALL CONT_BffrRawUnchecked(const CONT_Bffr *bffr,
                                                       PRP_Size *pCap);
/**
 * Returns the raw memory pointer of the buffer contents.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param bffr Buffer instance.
 * @param pCap Pointer to where to store cap of the buffer.
 * @param pRaw The pointer to where the raw mem will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrRawChecked(const CONT_Bffr *bffr,
                                                    PRP_Size *pCap,
                                                    void **pRaw);
/**
 * Returns the capacity (number of elements) of the buffer.
 *
 * @param bffr Buffer instance.
 *
 * @return Buffer capacity.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_FN_API PRP_Size PRP_FN_CALL CONT_BffrCap(const CONT_Bffr *bffr);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param bffr Buffer instance.
 *
 * @return Element size.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_FN_API PRP_Size PRP_FN_CALL CONT_BffrMembSize(const CONT_Bffr *bffr);
/**
 * Returns the maximum possible capacity for this buffer configuration.
 *
 * @param bffr instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_FN_API PRP_Size PRP_FN_CALL CONT_BffrMaxCap(const CONT_Bffr *bffr);

/**
 * Retrieves the value of the given index.
 *
 * @param bffr Buffer instance.
 * @param i    The index into the buffer.
 *
 * @return The value pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void *PRP_FN_CALL CONT_BffrGetUnchecked(const CONT_Bffr *bffr,
                                                 PRP_Size i);
/**
 * Retrieves the value of the given index.
 *
 * @param bffr Buffer instance.
 * @param i    The index into the buffer.
 * @param dest The pointer to the value pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrGetChecked(const CONT_Bffr *bffr,
                                                    PRP_Size i, void **dest);
/**
 * Sets the value of the given index.
 *
 * @param bffr  Buffer instance.
 * @param i     The index into the buffer.
 * @param pData The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_BffrSetUnchecked(CONT_Bffr *bffr, PRP_Size i,
                                                const void *pData);
/**
 * Sets the value of the given index.
 *
 * @param bffr  Buffer instance.
 * @param i     The index into the buffer.
 * @param pData The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSetChecked(CONT_Bffr *bffr, PRP_Size i,
                                                    const void *pData);
/**
 * Sets element in the range to the given data.
 *
 * @param bffr   Buffer instance.
 * @param i      The index 1 into the array.
 * @param j      The index 2 into the array.
 * @param pData: The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_BffrSetRangeUnchecked(CONT_Bffr *bffr, PRP_Size i,
                                                     PRP_Size j,
                                                     const void *pData);
/**
 * Sets element in the range to the given data.
 *
 * @param bffr   Buffer instance.
 * @param i      The index 1 into the array.
 * @param j      The index 2 into the array.
 * @param pData: The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if the indices are out of bounds of the buffer.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSetRangeChecked(CONT_Bffr *bffr,
                                                         PRP_Size i, PRP_Size j,
                                                         const void *pData);
/**
 * Copies len number of elements from data_arr into bffr starting from index i.
 *
 * @param bffr     Buffer instance.
 * @param i        The index into the array to start from.
 * @param data_arr The array of data to set.
 * @param len      The len of the data_arr.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_BffrSetManyUnchecked(CONT_Bffr *bffr, PRP_Size i,
                                                    const void *data_arr,
                                                    PRP_Size len);
/**
 * Copies len number of elements from data_arr into bffr starting from index i.
 *
 * @param bffr     Buffer instance.
 * @param i        The index into the array to start from.
 * @param data_arr The array of data to set.
 * @param len      The len of the data_arr.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if i is out of bounds of the buffer.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSetManyChecked(CONT_Bffr *bffr,
                                                        PRP_Size i,
                                                        const void *data_arr,
                                                        PRP_Size len);

/**
 * Comares the contents of the buffer.
 *
 * @param bffr1 Buffer instance 1.
 * @param bffr2 Buffer instance 2.
 *
 * @return PRP_True if equal, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BffrCmpUnchecked(const CONT_Bffr *bffr1,
                                                    const CONT_Bffr *bffr2);
/**
 * Comares the contents of the array.
 *
 * @param bffr1 Buffer instance 1.
 * @param bffr2 Buffer instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCmpChecked(const CONT_Bffr *bffr1,
                                                    const CONT_Bffr *bffr2,
                                                    PRP_Bool *pRslt);
/**
 * Extends bffr2 into bffr1.
 *
 * @param bffr1 Buffer instance 1.
 * @param bffr2 Buffer instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrExtendUnchecked(CONT_Bffr *bffr1,
                                                         const CONT_Bffr *bffr2);
/**
 * Extends bffr2 into bffr1.
 *
 * @param bffr1 Buffer instance 1.
 * @param bffr2 Buffer instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrExtendChecked(CONT_Bffr *bffr1,
                                                       const CONT_Bffr *bffr2);
/**
 * Swaps the elements in the given indices.
 *
 * @param bffr      Buffer instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal bffr's memb size.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_BffrSwapUnchecked(CONT_Bffr *bffr, PRP_Size i,
                                                 PRP_Size j, void *swap_bffr);
/**
 * Swaps the elements in the given indices.
 *
 * @param bffr      Buffer instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal arr's memb size.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSwapChecked(CONT_Bffr *bffr, PRP_Size i,
                                                     PRP_Size j,
                                                     void *swap_bffr);
/**
 * Clears the buffer to 0.
 *
 * @param bffr Buffer instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_BffrClearUnchecked(CONT_Bffr *bffr);
/**
 * Clears the buffer to 0.
 *
 * @param bffr Buffer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrClearChecked(CONT_Bffr *bffr);
/**
 * Safely change size of the buffer.
 *
 * @param bffr    Buffer instance.
 * @param new_cap The new cap of the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrChangeSizeUnchecked(CONT_Bffr *bffr,
                                                             PRP_Size new_cap);
/**
 * Safely change size of the buffer.
 *
 * @param bffr    Buffer instance.
 * @param new_cap The new cap of the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrChangeSizeChecked(CONT_Bffr *bffr,
                                                           PRP_Size new_cap);

#ifdef __cplusplus
}
#endif
