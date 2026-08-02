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
typedef struct CONT_Bffr CONT_Bffr;

#define CONT_BFFR_DEFAULT_CAP (16)
#define CONT_BFFR_MAX_CAP(memb_size) (PRP_SIZE_MAX / (memb_size))

/**
 * Checks whether the given buffer is structurally valid.
 *
 * @param pBffr Pointer to the buffer.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL CONT_BffrIsValid(const CONT_Bffr *pBffr);

/**
 * Creates a dynamic buffer.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param ppBffr    Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrCreateUnchecked(PRP_Size memb_size,
                                                     PRP_Size cap,
                                                     CONT_Bffr **ppBffr);
/**
 * Creates a dynamic buffer.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param ppBffr    Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrCreateChecked(PRP_Size memb_size,
                                                   PRP_Size cap,
                                                   CONT_Bffr **ppBffr);
/**
 * Deep clones the given buffer.
 *
 * @param pBffr  The buffer to clone.
 * @param ppBffr Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrCloneUnchecked(const CONT_Bffr *pBffr,
                                                    CONT_Bffr **ppBffr);
/**
 * Deep clones the given buffer with validation.
 *
 * @param pBffr  The buffer to clone.
 * @param ppBffr Output pointer receiving the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrCloneChecked(const CONT_Bffr *pBffr,
                                                  CONT_Bffr **ppBffr);

/**
 * Deletes the buffer and nullifies the pointer.
 *
 * @param ppBffr Pointer to buffer pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BffrDeleteUnchecked(CONT_Bffr **ppBffr);
/**
 * Deletes the buffer and nullifies the pointer.
 *
 * @param ppBffr Pointer to buffer pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if ppBffr or *ppBffr is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrDeleteChecked(CONT_Bffr **ppBffr);

/**
 * Returns the raw memory pointer of the buffer contents.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param pBffr Buffer instance.
 * @param pCap  Pointer to where to store cap of the buffer.
 *
 * @return The raw memory pointer of the buffer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API const void *PRP_CALL CONT_BffrRawUnchecked(const CONT_Bffr *pBffr,
                                                   PRP_Size *pCap);
/**
 * Returns the raw memory pointer of the buffer contents.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param pBffr Buffer instance.
 * @param pCap  Pointer to where to store cap of the buffer.
 * @param pRaw  The pointer to where the raw mem will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrRawChecked(const CONT_Bffr *pBffr,
                                                PRP_Size *pCap, void **pRaw);
/**
 * Returns the capacity (number of elements) of the buffer.
 *
 * @param pBffr Buffer instance.
 *
 * @return Buffer capacity.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_BffrCap(const CONT_Bffr *pBffr);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param pBffr Buffer instance.
 *
 * @return Element size.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_BffrMembSize(const CONT_Bffr *pBffr);
/**
 * Returns the maximum possible capacity for this buffer configuration.
 *
 * @param pBffr instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_BffrMaxCap(const CONT_Bffr *pBffr);

/**
 * Retrieves the value of the given index.
 *
 * @param pBffr Buffer instance.
 * @param i     The index into the buffer.
 *
 * @return The value pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void *PRP_CALL CONT_BffrGetUnchecked(const CONT_Bffr *pBffr,
                                             PRP_Size i);
/**
 * Retrieves the value of the given index.
 *
 * @param pBffr  Buffer instance.
 * @param i      The index into the buffer.
 * @param ppDest The pointer to the value pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrGetChecked(const CONT_Bffr *pBffr,
                                                PRP_Size i, void **ppDest);
/**
 * Sets the value of the given index.
 *
 * @param pBffr  Buffer instance.
 * @param i      The index into the buffer.
 * @param pData  The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BffrSetUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                            const void *pData);
/**
 * Sets the value of the given index.
 *
 * @param pBffr  Buffer instance.
 * @param i      The index into the buffer.
 * @param pData  The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrSetChecked(CONT_Bffr *pBffr, PRP_Size i,
                                                const void *pData);
/**
 * Sets element in the range to the given data.
 *
 * @param pBffr   Buffer instance.
 * @param i       The index 1 into the array.
 * @param j       The index 2 into the array.
 * @param pData   The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BffrSetRangeUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                                 PRP_Size j, const void *pData);
/**
 * Sets element in the range to the given data.
 *
 * @param pBffr   Buffer instance.
 * @param i       The index 1 into the array.
 * @param j       The index 2 into the array.
 * @param pData   The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if the indices are out of bounds of the buffer.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrSetRangeChecked(CONT_Bffr *pBffr,
                                                     PRP_Size i, PRP_Size j,
                                                     const void *pData);
/**
 * Copies len number of elements from data_arr into pBffr starting from index i.
 *
 * @param pBffr     Buffer instance.
 * @param i         The index into the array to start from.
 * @param pData_arr The array of data to set.
 * @param len       The len of the data_arr.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BffrSetManyUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                                const void *pData_arr,
                                                PRP_Size len);
/**
 * Copies len number of elements from data_arr into pBffr starting from index i.
 *
 * @param pBffr     Buffer instance.
 * @param i         The index into the array to start from.
 * @param pData_arr The array of data to set.
 * @param len       The len of the data_arr.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if i is out of bounds of the buffer.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrSetManyChecked(CONT_Bffr *pBffr,
                                                    PRP_Size i,
                                                    const void *pData_arr,
                                                    PRP_Size len);

/**
 * Comares the contents of the buffer.
 *
 * @param pBffr1 Buffer instance 1.
 * @param pBffr2 Buffer instance 2.
 *
 * @return PRP_True if equal, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BffrCmpUnchecked(const CONT_Bffr *pBffr1,
                                                const CONT_Bffr *pBffr2);
/**
 * Comares the contents of the array.
 *
 * @param pBffr1 Buffer instance 1.
 * @param pBffr2 Buffer instance 2.
 * @param pRslt  The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrCmpChecked(const CONT_Bffr *pBffr1,
                                                const CONT_Bffr *pBffr2,
                                                PRP_Bool *pRslt);
/**
 * Extends pBffr2 into pBffr1.
 *
 * @param pBffr1 Buffer instance 1.
 * @param pBffr2 Buffer instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrExtendUnchecked(CONT_Bffr *pBffr1,
                                                     const CONT_Bffr *pBffr2);
/**
 * Extends pBffr2 into pBffr1.
 *
 * @param pBffr1 Buffer instance 1.
 * @param pBffr2 Buffer instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrExtendChecked(CONT_Bffr *pBffr1,
                                                   const CONT_Bffr *pBffr2);
/**
 * Swaps the elements in the given indices.
 *
 * @param pBffr      Buffer instance.
 * @param i          The first index.
 * @param j          The second index.
 * @param pSwap_bffr A temp buffer for swapping. Must be equal pBffr's memb
 *                   size.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BffrSwapUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                             PRP_Size j, void *pSwap_bffr);
/**
 * Swaps the elements in the given indices.
 *
 * @param pBffr      Buffer instance.
 * @param i          The first index.
 * @param j          The second index.
 * @param pSwap_bffr A temp buffer for swapping. Must be equal pBffr's memb
 *                   size.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrSwapChecked(CONT_Bffr *pBffr, PRP_Size i,
                                                 PRP_Size j, void *pSwap_bffr);
/**
 * Clears the buffer to 0.
 *
 * @param pBffr Buffer instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BffrClearUnchecked(CONT_Bffr *pBffr);
/**
 * Clears the buffer to 0.
 *
 * @param pBffr Buffer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrClearChecked(CONT_Bffr *pBffr);
/**
 * Safely change size of the buffer.
 *
 * @param pBffr   Buffer instance.
 * @param new_cap The new cap of the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrChangeSizeUnchecked(CONT_Bffr *pBffr,
                                                         PRP_Size new_cap);
/**
 * Safely change size of the buffer.
 *
 * @param pBffr   Buffer instance.
 * @param new_cap The new cap of the buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BffrChangeSizeChecked(CONT_Bffr *pBffr,
                                                       PRP_Size new_cap);

#ifdef __cplusplus
}
#endif
