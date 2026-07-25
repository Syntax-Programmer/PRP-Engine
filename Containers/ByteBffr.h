#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * A dynamically resizing byte raw memory block.
 *
 * - Elements are stored contiguously in memory.
 * - Size grows is controlled by user.
 * - Not type-safe; user must ensure correct usage.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Buffer must be deleted using CONT_ByteBffrDelete* APIs.
 */
typedef struct CONT_ByteBffr CONT_ByteBffr;

#define CONT_BYTE_BFFR_MAX_SIZE (PRP_SIZE_MAX)
#define CONT_BYTE_BFFR_DEFAULT_SIZE (128)

/**
 * Checks whether the given  byte-buffer is structurally valid.
 *
 * @param b_bffr Pointer to the byte-buffer.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL CONT_ByteBffrIsValid(const CONT_ByteBffr *b_bffr);

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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCreateUnchecked(PRP_Size size, CONT_ByteBffr **pB_bffr);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrCreateChecked(PRP_Size size,
                                                       CONT_ByteBffr **pB_bffr);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrCloneUnchecked(
    const CONT_ByteBffr *b_bffr, CONT_ByteBffr **pB_bffr);
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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCloneChecked(const CONT_ByteBffr *b_bffr, CONT_ByteBffr **pB_bffr);

/**
 * Deletes the byte-buffer and nullifies the pointer.
 *
 * @param pB_bffr Pointer to byte-buffer pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ByteBffrDeleteUnchecked(CONT_ByteBffr **pB_bffr);
/**
 * Deletes the byte-buffer and nullifies the pointer.
 *
 * @param pB_bffr Pointer to byte-buffer pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pB_bffr or *pB_bffr is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ByteBffrDeleteChecked(CONT_ByteBffr **pB_bffr);

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
PRP_API const void *PRP_CALL
CONT_ByteBffrRawUnchecked(const CONT_ByteBffr *b_bffr, PRP_Size *pSize);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrRawChecked(const CONT_ByteBffr *b_bffr,
                                                    PRP_Size *pSize,
                                                    void **pRaw);

/**
 * Returns the size(in bytes) of the byte-buffer.
 *
 * @param b_bffr Byte-Buffer instance.
 *
 * @return Byte-Buffer size.
 *
 * @note Assumes valid bute-buffer (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_ByteBffrSize(const CONT_ByteBffr *b_bffr);

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
PRP_API void *PRP_CALL CONT_ByteBffrGetUnchecked(const CONT_ByteBffr *b_bffr,
                                                 PRP_Size ofs);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrGetChecked(const CONT_ByteBffr *b_bffr,
                                                    PRP_Size ofs,
                                                    void **ppDest);
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
PRP_API void PRP_CALL CONT_ByteBffrUploadUnchecked(CONT_ByteBffr *b_bffr,
                                                   PRP_Size ofs, PRP_Size size,
                                                   void *pData);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrUploadChecked(CONT_ByteBffr *b_bffr,
                                                       PRP_Size ofs,
                                                       PRP_Size size,
                                                       void *pData);
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
PRP_API void PRP_CALL CONT_ByteBffrCopyUnchecked(const CONT_ByteBffr *b_bffr1,
                                                 PRP_Size ofs1,
                                                 CONT_ByteBffr *b_bffr2,
                                                 PRP_Size ofs2, PRP_Size size);
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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCopyChecked(const CONT_ByteBffr *b_bffr1, PRP_Size ofs1,
                         CONT_ByteBffr *b_bffr2, PRP_Size ofs2, PRP_Size size);
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
PRP_API void PRP_CALL CONT_ByteBffrFillUnchecked(CONT_ByteBffr *b_bffr,
                                                 PRP_Size ofs, PRP_Size size,
                                                 PRP_U8 byte);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrFillChecked(CONT_ByteBffr *b_bffr,
                                                     PRP_Size ofs,
                                                     PRP_Size size,
                                                     PRP_U8 byte);

/**
 * Comares the contents of the byte-buffer.
 *
 * @param b_bffr1 Byte-Buffer instance 1.
 * @param b_bffr2 Byte-Buffer instance 2.
 *
 * @return PRP_True if equal, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_ByteBffrCmpUnchecked(
    const CONT_ByteBffr *b_bffr1, const CONT_ByteBffr *b_bffr2);
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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCmpChecked(const CONT_ByteBffr *b_bffr1,
                        const CONT_ByteBffr *b_bffr2, PRP_Bool *pRslt);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrExtendUnchecked(
    CONT_ByteBffr *b_bffr1, const CONT_ByteBffr *b_bffr2);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrExtendChecked(
    CONT_ByteBffr *b_bffr1, const CONT_ByteBffr *b_bffr2);
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
PRP_API void PRP_CALL CONT_ByteBffrSwapRegionUnchecked(CONT_ByteBffr *b_bffr,
                                                       PRP_Size ofs1,
                                                       PRP_Size ofs2,
                                                       PRP_Size size,
                                                       void *pSwap_bffr);
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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrSwapRegionChecked(CONT_ByteBffr *b_bffr, PRP_Size ofs1,
                               PRP_Size ofs2, PRP_Size size, void *pSwap_bffr);

/**
 * Clears the byte-buffer to 0.
 *
 * @param b_bffr Byte-Buffer instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_ByteBffrClearUnchecked(CONT_ByteBffr *b_bffr);
/**
 * Clears the byte-buffer to 0.
 *
 * @param b_bffr Byte-Buffer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_ByteBffrClearChecked(CONT_ByteBffr *b_bffr);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrReserveUnchecked(CONT_ByteBffr *b_bffr,
                                                          PRP_Size ofs,
                                                          PRP_Size size);
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
PRP_API PRP_Result PRP_CALL CONT_ByteBffrReserveChecked(CONT_ByteBffr *b_bffr,
                                                        PRP_Size ofs,
                                                        PRP_Size size);
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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrChangeSizeUnchecked(CONT_ByteBffr *b_bffr, PRP_Size new_size);
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
PRP_API PRP_Result PRP_CALL
CONT_ByteBffrChangeSizeChecked(CONT_ByteBffr *b_bffr, PRP_Size new_size);

#ifdef __cplusplus
}
#endif
