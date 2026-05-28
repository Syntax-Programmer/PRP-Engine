#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Utils/Defs.h"

/* ----  BITWORD UTILS ---- */

// The number of bits in the bitword
#define BITWORD_BITS (sizeof(DT_Bitword) * 8)

// Which word index the global i falls into.
#define WORD_I(i) ((i) >> 6)

// Which bit position in its word the global i falls into.
#define BIT_I(i) ((i) & 63)

// Word mask of the global i.
#define BIT_MASK(i) ((DT_Bitword)1 << BIT_I(i))

/**
 * A singular word of the bitmap that can hold 64 bits at once.
 * These are used in the DT_Bitmap, and can be used separately as there own
 * util.
 */
typedef DT_u64 DT_Bitword;

/**
 * Counts the trailing zeros of bitword.
 *
 * @param word The word to count in.
 *
 * @return The CTZ count of word if word != 0.
 * @return PRP_INVALID_INDEX if word == 0.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCTZ(DT_Bitword word);
/**
 * Counts the leading zeros of bitword.
 *
 * @param word The word to count in.
 *
 * @return The CLZ count of word if word != 0.
 * @return PRP_INVALID_INDEX if word == 0.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCLZ(DT_Bitword word);
/**
 * Counts the total number of bits set in a word.
 *
 * @param word The word to count in.
 *
 * @return The number of set bits in word.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordPopCnt(DT_Bitword word);
/**
 * Finds the first set bit of the word.
 *
 * @param word The word to find FFS in.
 *
 * @return 0-based index of the first set bit.
 * @return PRP_INVALID_POS if word == 0.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordFFS(DT_Bitword word);

/* ----  BITMAP UTILS ---- */

/**
 * DT_Bitmap
 *
 * A dynamically resizing bitmap.
 *
 * - Bits are stored in linear memory.
 * - Capacity grows is controlled by user.
 */
typedef struct _Bitmap DT_Bitmap;

#define DT_BITMAP_DEFAULT_BIT_CAP (64)
#define DT_BITMAP_MAX_BIT_CAP (DT_SIZE_MAX)

/**
 * Checks whether the given bitmap is structurally valid.
 *
 * @param bmp Pointer to the bitmap.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsValid(const DT_Bitmap *bmp);

/**
 * Creates a dynamic bitmap.
 *
 * @param bit_cap Initial bit capacity.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCreateUnchecked(DT_size bit_cap,
                                                           DT_Bitmap **out);
/**
 * Creates a dynamic bitmap.
 *
 * @param bit_cap Initial bit capacity.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCreateChecked(DT_size bit_cap,
                                                         DT_Bitmap **out);
/**
 * Deep clones the given bitmap.
 *
 * @param bmp The bitmap to clone.
 * @param out Output pointer receiving the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCloneUnchecked(const DT_Bitmap *bmp,
                                                          DT_Bitmap **out);
/**
 * Deep clones the given bitmap.
 *
 * @param bmp The bitmap to clone.
 * @param out Output pointer receiving the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCloneChecked(const DT_Bitmap *bmp,
                                                        DT_Bitmap **out);

/**
 * Deletes the bitmap and nullifies the pointer.
 *
 * @param pBmp Pointer to bitmap pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapDeleteUnchecked(DT_Bitmap **pBmp);
/**
 * Deletes the bitmap and nullifies the pointer.
 *
 * @param pBmp Pointer to bitmap pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArr or *pArr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapDeleteChecked(DT_Bitmap **pBmp);

/**
 * Returns the raw memory pointer of the bitmap contenets.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param bmp       Bitmap instance.
 * @param pWord_cap Pointer to where to store word cap of bitmap.
 * @param pBit_cap  Pointer to where to store word cap of bitmap.
 *
 * @return The raw memory pointer of the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API const DT_Bitword *PRP_FN_CALL DT_BitmapRawUnchecked(
    const DT_Bitmap *bmp, DT_size *pWord_cap, DT_size *pBit_cap);
/**
 * Returns the raw memory pointer of the bitmap contenets.
 *
 * The pointer is not guaranteed to be valid after a growth operation
 *
 * @param bmp       Bitmap instance.
 * @param pWord_cap Pointer to where to store word cap of bitmap.
 * @param pBit_cap  Pointer to where to store word cap of bitmap.
 * @param pRaw      The pointer to where the raw mem will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapRawChecked(const DT_Bitmap *bmp,
                                                      DT_size *pWord_cap,
                                                      DT_size *pBit_cap,
                                                      DT_void **pRaw);

/**
 * Returns the number of set bits currently stored.
 *
 * @param bmp Bitmap instance.
 *
 * @return Number of bits set.
 *
 * @note Assumes valid bitmap (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCount(const DT_Bitmap *bmp);
/**
 * Returns the first set bit of the bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return 0-based index of the first bit set.
 *
 * @note Assumes valid bitmap (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFS(const DT_Bitmap *bmp);
/**
 * Returns the number of bit cap of bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return Bit cap of bitmap.
 *
 * @note Assumes valid bitmap (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCap(const DT_Bitmap *bmp);

/**
 * Finds exclusive rank of the given bit.
 *
 * @param bmp Bitmap instance.
 * @param i   Bit to get rank of.
 *
 * @return Rank of the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitRankUnchecked(const DT_Bitmap *bmp,
                                                         DT_size i);
/**
 * Finds exclusive rank of the given bit.
 *
 * @param bmp   Bitmap instance.
 * @param i     Bit to get rank of.
 * @param pRank Output pointer to the rank of given bit.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOB if index out of bounds.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapBitRankChecked(const DT_Bitmap *bmp,
                                                          DT_size i,
                                                          DT_size *pRank);
/**
 * Sets bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapSetUnchecked(DT_Bitmap *bmp, DT_size i);
/**
 * Sets bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOB if index out of bounds.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapSetChecked(DT_Bitmap *bmp,
                                                      DT_size i);
/**
 * Clears bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapClrUnchecked(DT_Bitmap *bmp, DT_size i);
/**
 * Clears bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOB if index out of bounds.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapClrChecked(DT_Bitmap *bmp,
                                                      DT_size i);
/**
 * Toggles bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapToggleUnchecked(DT_Bitmap *bmp,
                                                        DT_size i);
/**
 * Toggles bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOB if index out of bounds.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapToggleChecked(DT_Bitmap *bmp,
                                                         DT_size i);
/**
 * Checks if bit set on given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @return DT_true if bit set, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsSetUnchecked(const DT_Bitmap *bmp,
                                                       DT_size i);
/**
 * Checks if bit set on given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOB if index out of bounds.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetChecked(const DT_Bitmap *bmp,
                                                        DT_size i,
                                                        DT_bool *pRslt);

/**
 * Sets bits in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapSetRangeUnchecked(DT_Bitmap *bmp,
                                                          DT_size i, DT_size j);
/**
 * Sets bits in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapSetRangeChecked(DT_Bitmap *bmp,
                                                           DT_size i,
                                                           DT_size j);
/**
 * Clears bits in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapClrRangeUnchecked(DT_Bitmap *bmp,
                                                          DT_size i, DT_size j);
/**
 * Clears bits in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapClrRangeChecked(DT_Bitmap *bmp,
                                                           DT_size i,
                                                           DT_size j);
/**
 * Toggles bits in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapToggleRangeUnchecked(DT_Bitmap *bmp,
                                                             DT_size i,
                                                             DT_size j);
/**
 * Toggles bits in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapToggleRangeChecked(DT_Bitmap *bmp,
                                                              DT_size i,
                                                              DT_size j);
/**
 * Checks if any bits are set in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return DT_true if any bit is set in range, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSetRangeAnyUnchecked(const DT_Bitmap *bmp, DT_size i, DT_size j);
/**
 * Checks if any bits are set in the range.
 *
 * @param bmp   Bitmap instance.
 * @param i     The index 1 into the bitmap.
 * @param j     The index 2 into the bitmap.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if any indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetRangeAnyChecked(
    const DT_Bitmap *bmp, DT_size i, DT_size j, DT_bool *pRslt);
/**
 * Checks if all bits are set in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return DT_true if all bit is set in range, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSetRangeAllUnchecked(const DT_Bitmap *bmp, DT_size i, DT_size j);
/**
 * Checks if all bits are set in the range.
 *
 * @param bmp   Bitmap instance.
 * @param i     The index 1 into the bitmap.
 * @param j     The index 2 into the bitmap.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if all indices are out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetRangeAllChecked(
    const DT_Bitmap *bmp, DT_size i, DT_size j, DT_bool *pRslt);

/**
 * Checks if there is no bits set in bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return DT_true if bitmap is empty, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsEmptyUnchecked(const DT_Bitmap *bmp);
/**
 * Checks if there is no bits set in bitmap.
 *
 * @param bmp   Bitmap instance.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsEmptyChecked(const DT_Bitmap *bmp,
                                                          DT_bool *pRslt);
/**
 * Checks if the bitmap is full.
 *
 * @param bmp Bitmap instance.
 *
 * @return DT_true if bitmap is full, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsFullUnchecked(const DT_Bitmap *bmp);
/**
 * Checks if the bitmap is full.
 *
 * @param bmp   Bitmap instance.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsFullChecked(const DT_Bitmap *bmp,
                                                         DT_bool *pRslt);

/**
 * Performs a NOT operation on every bit in bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapNotUnchecked(DT_Bitmap *bmp);
/**
 * Performs a NOT operation on every bit in bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapNotChecked(DT_Bitmap *bmp);
/**
 * Performs an AND operation b/w bmp1 and bmp2.
 *
 * Stores the result in bmp1 itself.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapAndUnchecked(DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2);
/**
 * Performs an AND operation b/w bmp1 and bmp2.
 *
 * Stores the result in bmp1 itself.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapAndChecked(DT_Bitmap *bmp1,
                                                      const DT_Bitmap *bmp2);
/**
 * Performs an OR operation b/w bmp1 and bmp2.
 *
 * Stores the result in bmp1 itself.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapOrUnchecked(DT_Bitmap *bmp1,
                                                    const DT_Bitmap *bmp2);
/**
 * Performs an OR operation b/w bmp1 and bmp2.
 *
 * Stores the result in bmp1 itself.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapOrChecked(DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2);

/**
 * Checks if bmp1 & bmp2 == bmp2.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @return DT_true if has all condition is met, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapHasAllUnchecked(const DT_Bitmap *bmp1,
                                                        const DT_Bitmap *bmp2);
/**
 * Checks if bmp1 & bmp2 == bmp2.
 *
 * @param bmp1  Bitmap instance 1.
 * @param bmp2  Bitmap instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapHasAllChecked(const DT_Bitmap *bmp1,
                                                         const DT_Bitmap *bmp2,
                                                         DT_bool *pRslt);
/**
 * Checks if bmp1 & bmp2 != 0.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @return DT_true if has any condition is met, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapHasAnyUnchecked(const DT_Bitmap *bmp1,
                                                        const DT_Bitmap *bmp2);
/**
 * Checks if bmp1 & bmp2 != 0.
 *
 * @param bmp1  Bitmap instance 1.
 * @param bmp2  Bitmap instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapHasAnyChecked(const DT_Bitmap *bmp1,
                                                         const DT_Bitmap *bmp2,
                                                         DT_bool *pRslt);

/**
 * Comares if bits of bmp1 are equal to bmp2.
 *
 * @param bmp1  Bitmap instance 1.
 * @param bmp2  Bitmap instance 2.
 *
 * @return DT_true if equal, DT_false otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapCmpUnchecked(const DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2);
/**
 * Comares if bits of bmp1 are equal to bmp2.
 *
 * @param bmp1  Bitmap instance 1.
 * @param bmp2  Bitmap instance 2.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCmpChecked(const DT_Bitmap *bmp1,
                                                      const DT_Bitmap *bmp2,
                                                      DT_bool *pRslt);
/**
 * Resets the bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapResetUnchecked(DT_Bitmap *bmp);
/**
 * Resets the bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapResetChecked(DT_Bitmap *bmp);
/**
 * Shrinks the bitmap to its MSB word.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapShrinkFitUnchecked(DT_Bitmap *bmp);
/**
 * Shrinks the bitmap to its MSB word.
 *
 * @param arr Bitmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapShrinkFitChecked(DT_Bitmap *bmp);
/**
 * Safely change size of the bitmap.
 *
 * @param bmp         Bitmap instance.
 * @param new_bit_cap The new bit cap of the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max bit cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_BitmapChangeSizeUnchecked(DT_Bitmap *bmp, DT_size new_bit_cap);
/**
 * Safely change size of the bitmap.
 *
 * @param bmp         Bitmap instance.
 * @param new_bit_cap The new bit cap of the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max bit cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_BitmapChangeSizeChecked(DT_Bitmap *bmp, DT_size new_bit_cap);

#ifdef __cplusplus
}
#endif
