#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Utils/Defs.h"

/**
 * DT_Bffr
 *
 * A dynamically resizing buffer storing elements of fixed size.
 *
 * - Elements are stored contiguously in memory.
 * - Capacity grows is controlled by user.
 * - Not type-safe; user must ensure correct usage.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Buffer  must be deleted using DT_BffrDelete* APIs.
 */
typedef struct _Bffr DT_Bffr;

#define DT_BFFR_DEFAULT_CAP (16)
#define DT_BFFR_MAX_CAP(memb_size) (DT_SIZE_MAX / (memb_size))

/**
 * Checks whether the given buffer is structurally valid.
 *
 * @param bffr Pointer to the buffer.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BffrIsValid(const DT_Bffr *bffr);

/**
 * Creates a dynamic buffer.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param out       Output pointer receiving the arraybuffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCreateUnchecked(DT_size memb_size,
                                                         DT_size cap,
                                                         DT_Bffr **out);
/**
 * Creates a dynamic buffer.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Initial capacity.
 * @param out       Output pointer receiving the arraybuffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCreateChecked(DT_size memb_size,
                                                       DT_size cap,
                                                       DT_Bffr **out);
/**
 * Deep clones the given buffer.
 *
 * @param bffr The buffer to clone.
 * @param out  Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCloneUnchecked(const DT_Bffr *bffr,
                                                        DT_Bffr **out);
/**
 * Deep clones the given buffer with validation.
 *
 * @param bffr The buffer to clone.
 * @param out  Output pointer receiving the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCloneChecked(const DT_Bffr *bffr,
                                                      DT_Bffr **out);

/**
 * Deletes the buffer and nullifies the pointer.
 *
 * @param pBffr Pointer to buffer pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BffrDeleteUnchecked(DT_Bffr **pBffr);
/**
 * Deletes the buffer and nullifies the pointer.
 *
 * @param pBffr Pointer to buffer pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArr or *pArr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrDeleteChecked(DT_Bffr **pBffr);

/**
 * Returns the raw memory pointer of the array contenets.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param bffr Buffer instance.
 * @param pCap Pointer to where to store cap of the buffer.
 *
 * @return The raw memory pointer of the array.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API const DT_void *PRP_FN_CALL DT_BffrRawUnchecked(const DT_Bffr *bffr,
                                                          DT_size *pCap);
/**
 * Returns the raw memory pointer of the array contenets.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param bffr Buffer instance.
 * @param pCap Pointer to where to store cap of the buffer.
 * @param pRaw The pointer to where the raw mem will be stored.
 *
 * @return The raw memory pointer of the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrRawChecked(const DT_Bffr *bffr,
                                                    DT_size *pCap,
                                                    DT_void **pRaw);
/**
 * Returns the capacity (number of elements) of the buffer.
 *
 * @param bffr Buffer instance.
 *
 * @return Buffer capacity.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BffrCap(const DT_Bffr *bffr);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param bffr Buffer instance.
 *
 * @return Element size.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BffrMembSize(const DT_Bffr *bffr);
/**
 * Returns the maximum possible capacity for this buffer configuration.
 *
 * @param bffr instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid buffer (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BffrMaxCap(const DT_Bffr *bffr);

/**
 * Retrieves the value of the given index.
 *
 * @param bffr Buffer instance.
 * @param i    The index into the array.
 *
 * @return The value pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_BffrGetUnchecked(const DT_Bffr *bffr,
                                                    DT_size i);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrGetChecked(const DT_Bffr *bffr,
                                                    DT_size i, DT_void **dest);
/**
 * Sets the value of the given index.
 *
 * @param bffr  Buffer instance.
 * @param i     The index into the array.
 * @param pData The pointer to the data to set.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BffrSetUnchecked(DT_Bffr *bffr, DT_size i,
                                                   const DT_void *pData);
/**
 * Sets the value of the given index.
 *
 * @param bffr  Buffer instance.
 * @param i     The index into the array.
 * @param pData The pointer to the data to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetChecked(DT_Bffr *bffr, DT_size i,
                                                    const DT_void *pData);
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
PRP_FN_API DT_void PRP_FN_CALL DT_BffrSetRangeUnchecked(DT_Bffr *bffr,
                                                        DT_size i, DT_size j,
                                                        const DT_void *pData);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetRangeChecked(DT_Bffr *bffr,
                                                         DT_size i, DT_size j,
                                                         const DT_void *pData);
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
PRP_FN_API DT_void PRP_FN_CALL DT_BffrSetManyUnchecked(DT_Bffr *bffr, DT_size i,
                                                       const DT_void *data_arr,
                                                       DT_size len);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetManyChecked(DT_Bffr *bffr,
                                                        DT_size i,
                                                        const DT_void *data_arr,
                                                        DT_size len);

/**
 * Comares the contents of the buffer.
 *
 * @param bffr1 Buffer instance 1.
 * @param bffr2 Buffer instance 2.
 *
 * @return DT_true if equal, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BffrCmpUnchecked(const DT_Bffr *bffr1,
                                                   const DT_Bffr *bffr2);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCmpChecked(const DT_Bffr *bffr1,
                                                    const DT_Bffr *bffr2,
                                                    DT_bool *pRslt);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrExtendUnchecked(DT_Bffr *bffr1,
                                                         const DT_Bffr *bffr2);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrExtendChecked(DT_Bffr *bffr1,
                                                       const DT_Bffr *bffr2);
/**
 * Swaps the elements in the given indices.
 *
 * @param bffr      Buffer instance.
 * @param i         The first index.
 * @param j         The second index.
 * @param swap_bffr A temp buffer for swapping. Must be equal arr's memb size.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BffrSwapUnchecked(DT_Bffr *bffr, DT_size i,
                                                    DT_size j,
                                                    DT_void *swap_bffr);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSwapChecked(DT_Bffr *bffr, DT_size i,
                                                     DT_size j,
                                                     DT_void *swap_bffr);
/**
 * Clears the buffer to 0.
 *
 * @param bffr Buffer instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BffrClearUnchecked(DT_Bffr *bffr);
/**
 * Clears the buffer to 0.
 *
 * @param bffr Buffer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrClearChecked(DT_Bffr *bffr);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrChangeSizeUnchecked(DT_Bffr *bffr,
                                                             DT_size new_cap);
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
PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrChangeSizeChecked(DT_Bffr *bffr,
                                                           DT_size new_cap);

#ifdef __cplusplus
}
#endif
