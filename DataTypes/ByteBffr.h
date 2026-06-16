#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Utils/Defs.h"

/**
 * A dynamically resizing byte raw memory block.
 *
 * - Elements are stored contiguously in memory.
 * - Size grows is controlled by user.
 * - Not type-safe; user must ensure correct usage.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Buffer must be deleted using DT_ByteBffrDelete* APIs.
 */
typedef struct _ByteBffr DT_ByteBffr;

#define DT_BYTE_BFFR_MAX_SIZE (DT_SIZE_MAX)
#define DT_BYTE_BFFR_DEFAULT_SIZE (128)

/**
 * Checks whether the given  byte-buffer is structurally valid.
 *
 * @param b_bffr Pointer to the byte-buffer.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_ByteBffrIsValid(const DT_ByteBffr *b_bffr);

/**
 * Creates a dynamic byte-buffer.
 *
 * @param size    Initial size (in bytes) of the byte-buffer.
 * @param pB_bffr Output pointer receiving the byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCreateUnchecked(DT_size size, DT_ByteBffr **pB_bffr);
/**
 * Creates a dynamic byte-buffer.
 *
 * @param size    Initial size (in bytes) of the byte-buffer.
 * @param pB_bffr Output pointer receiving the byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCreateChecked(DT_size size, DT_ByteBffr **pB_bffr);
/**
 * Deep clones the given byte-buffer.
 *
 * @param b_bffr  The byte-buffer to clone.
 * @param pB_bffr Output pointer receiving the byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCloneUnchecked(const DT_ByteBffr *b_bffr, DT_ByteBffr **pB_bffr);
/**
 * Deep clones the given byte-buffer.
 *
 * @param b_bffr  The byte-buffer to clone.
 * @param pB_bffr Output pointer receiving the byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCloneChecked(const DT_ByteBffr *b_bffr, DT_ByteBffr **pB_bffr);

/**
 * Deletes the byte-buffer and nullifies the pointer.
 *
 * @param pB_bffr Pointer to byte-buffer pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL
DT_ByteBffrDeleteUnchecked(DT_ByteBffr **pB_bffr);
/**
 * Deletes the byte-buffer and nullifies the pointer.
 *
 * @param pB_bffr Pointer to byte-buffer pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pB_bffr or *pB_bffr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrDeleteChecked(DT_ByteBffr **pB_bffr);

/**
 * Returns the raw memory pointer of the byte-buffer contents.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param b_bffr Byte-Buffer instance.
 * @param pSize  Pointer to where to store size of the byte-buffer.
 *
 * @return The raw memory pointer of the byte-buffer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API const DT_void *PRP_FN_CALL
DT_ByteBffrRawUnchecked(const DT_ByteBffr *b_bffr, DT_size *pSize);
/**
 * Returns the raw memory pointer of the byte-buffer contents.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param b_bffr Byte-Buffer instance.
 * @param pSize  Pointer to where to store size of the byte-buffer.
 * @param pRaw   The pointer to where the raw mem will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrRawChecked(
    const DT_ByteBffr *b_bffr, DT_size *pSize, DT_void **pRaw);

/**
 * Returns the size(in bytes) of the byte-buffer.
 *
 * @param b_bffr Byte-Buffer instance.
 *
 * @return Byte-Buffer size.
 *
 * @note Assumes valid bute-buffer (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_ByteBffrSize(const DT_ByteBffr *b_bffr);

/**
 * Retrieves the memory pointer of the given ofset.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The offset into the byte-buffer.
 *
 * @return The value pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void *PRP_FN_CALL
DT_ByteBffrGetUnchecked(const DT_ByteBffr *b_bffr, DT_size ofs);
/**
 * Retrieves the memory pointer of the given ofset.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The offset into the byte-buffer.
 * @param ppDest The pointer to the value pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if offset out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrGetChecked(const DT_ByteBffr *b_bffr, DT_size ofs, DT_void **ppDest);
/**
 * Uploads a byte stream into the byte-buffer.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The ofset into the byte-buffer.
 * @param size   The size of the data to upload.
 * @param pData  The pointer to the data to upload.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ByteBffrUploadUnchecked(DT_ByteBffr *b_bffr,
                                                          DT_size ofs,
                                                          DT_size size,
                                                          DT_void *pData);
/**
 * Uploads a byte stream into the byte-buffer.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The ofset into the byte-buffer.
 * @param size   The size of the data to upload.
 * @param pData  The pointer to the data to upload.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if offset and/or size out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrUploadChecked(DT_ByteBffr *b_bffr,
                                                           DT_size ofs,
                                                           DT_size size,
                                                           DT_void *pData);
/**
 * Copies a region from one byte-buffer to another.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param ofs1    The ofset into the first byte-buffer.
 * @param b_bffr2 Byte-Buffer instance 2.
 * @param ofs2    The ofset into the second byte-buffer.
 * @param size    The size of the data to copy.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL
DT_ByteBffrCopyUnchecked(const DT_ByteBffr *b_bffr1, DT_size ofs1,
                         DT_ByteBffr *b_bffr2, DT_size ofs2, DT_size size);
/**
 * Copies a region from one byte-buffer to another.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param ofs1    The ofset into the first byte-buffer.
 * @param b_bffr2 Byte-Buffer instance 2.
 * @param ofs2    The ofset into the second byte-buffer.
 * @param size    The size of the data to copy.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if offset and/or size out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCopyChecked(const DT_ByteBffr *b_bffr1, DT_size ofs1,
                       DT_ByteBffr *b_bffr2, DT_size ofs2, DT_size size);
/**
 * Fills a region of byte-buffer with the specified byte.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The ofset into the byte-buffer.
 * @param size   The size of the reegion to fill.
 * @param byte   The byte of data to fill.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ByteBffrFillUnchecked(DT_ByteBffr *b_bffr,
                                                        DT_size ofs,
                                                        DT_size size,
                                                        DT_u8 byte);
/**
 * Fills a region of byte-buffer with the specified byte.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The ofset into the byte-buffer.
 * @param size   The size of the reegion to fill.
 * @param byte   The byte of data to fill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if offset and/or size out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrFillChecked(DT_ByteBffr *b_bffr,
                                                         DT_size ofs,
                                                         DT_size size,
                                                         DT_u8 byte);

/**
 * Comares the contents of the byte-buffer.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param b_bffr2 Byte-Buffer instance 2.
 *
 * @return DT_true if equal, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL
DT_ByteBffrCmpUnchecked(const DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2);
/**
 * Comares the contents of the byte-buffer.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param b_bffr2 Byte-Buffer instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrCmpChecked(
    const DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2, DT_bool *pRslt);
/**
 * Extends b_bffr2 into b_bffr1.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param b_bffr2 Byte-Buffer instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrExtendUnchecked(DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2);
/**
 * Extends b_bffr2 into b_bffr1.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param b_bffr2 Byte-Buffer instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrExtendChecked(DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2);
/**
 * Swaps the contents in the given non overlapping regions.
 *
 * @param b_bffr     Byte-Buffer instance.
 * @param ofs1       Offset of the first memory region.
 * @param ofs2       Offset of the second memory region.
 * @param size       Size of the region to swap.
 * @param pSwap_bffr A temp buffer for swapping. Must be equal to the size
 *                   given to swap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL
DT_ByteBffrSwapRegionUnchecked(DT_ByteBffr *b_bffr, DT_size ofs1, DT_size ofs2,
                               DT_size size, DT_void *pSwap_bffr);
/**
 * Swaps the contents in the given non overlapping regions.
 *
 * @param b_bffr     Byte-Buffer instance.
 * @param ofs1       Offset of the first memory region.
 * @param ofs2       Offset of the second memory region.
 * @param size       Size of the region to swap.
 * @param pSwap_bffr A temp buffer for swapping. Must be equal to the size
 *                   given to swap.
 *
 * @return PRP_OK on success, or if ofs1 == ofs2.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_UNSUPPORTED if memory regions overlap but are NOT equal.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrSwapRegionChecked(DT_ByteBffr *b_bffr, DT_size ofs1, DT_size ofs2,
                             DT_size size, DT_void *pSwap_bffr);

/**
 * Clears the byte-buffer to 0.
 *
 * @param b_bffr Byte-Buffer instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_ByteBffrClearUnchecked(DT_ByteBffr *b_bffr);
/**
 * Clears the byte-buffer to 0.
 *
 * @param b_bffr Byte-Buffer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrClearChecked(DT_ByteBffr *b_bffr);
/**
 * Reserves size bytes into the byte-buffer from the given offset.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The offset from which to reserve the memory. Can be equal to
 *               size of the byte-bffr, for new memory chunk addition entirely.
 * @param size   The size to reserve into byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max size is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrReserveUnchecked(DT_ByteBffr *b_bffr, DT_size ofs, DT_size size);
/**
 * Reserves size bytes into the byte-buffer from the given offset.
 *
 * @param b_bffr Byte-Buffer instance.
 * @param ofs    The offset from which to reserve the memory.Can be equal to
 *               size of the byte-bffr, for new memory chunk addition entirely.
 * @param size   The size to reserve into byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max size is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_OOB if offset is bigger than the byte-bffr size.
 * @return PRP_ERR_INV_ARG if arguments are invalid..
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrReserveChecked(DT_ByteBffr *b_bffr,
                                                            DT_size ofs,
                                                            DT_size size);
/**
 * Safely change size of the byte-buffer.
 *
 * @param b_bffr   Byte-Buffer instance.
 * @param new_size The new size of the byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max size is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrChangeSizeUnchecked(DT_ByteBffr *b_bffr, DT_size new_size);
/**
 * Safely change size of the byte-buffer.
 *
 * @param b_bffr   Byte-Buffer instance.
 * @param new_size The new size of the byte-buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max size is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrChangeSizeChecked(DT_ByteBffr *b_bffr, DT_size new_size);

#ifdef __cplusplus
}
#endif
