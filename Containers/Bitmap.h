#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/* ----  BITWORD UTILS ---- */

// The number of bits in the bitword
#define BITWORD_BITS (sizeof(CONT_Bitword) * 8)

// Which word index the global i falls into.
#define WORD_I(i) ((i) >> 6)

// Which bit position in its word the global i falls into.
#define BIT_I(i) ((i) & 63)

// Word mask of the global i.
#define BIT_MASK(i) ((CONT_Bitword)1 << BIT_I(i))

/**
 * A singular word of the bitmap that can hold 64 bits at once.
 * These are used in the CONT_Bitmap, and can be used separately as there own
 * util.
 */
typedef PRP_U64 CONT_Bitword;

/**
 * Counts the trailing zeros of bitword.
 *
 * @param word The word to count in.
 *
 * @return The CTZ count of word if word != 0.
 * @return PRP_INVALID_INDEX if word == 0.
 */
PRP_API PRP_Size PRP_CALL CONT_BitwordCTZ(CONT_Bitword word);
/**
 * Counts the leading zeros of bitword.
 *
 * @param word The word to count in.
 *
 * @return The CLZ count of word if word != 0.
 * @return PRP_INVALID_INDEX if word == 0.
 */
PRP_API PRP_Size PRP_CALL CONT_BitwordCLZ(CONT_Bitword word);
/**
 * Counts the total number of bits set in a word.
 *
 * @param word The word to count in.
 *
 * @return The number of set bits in word.
 */
PRP_API PRP_Size PRP_CALL CONT_BitwordPopCnt(CONT_Bitword word);
/**
 * Finds the first set bit of the word.
 *
 * @param word The word to find FFS in.
 *
 * @return 0-based index of the first set bit.
 * @return PRP_INVALID_POS if word == 0.
 */
PRP_API PRP_Size PRP_CALL CONT_BitwordFFS(CONT_Bitword word);

/* ----  BITMAP UTILS ---- */

/**
 * CONT_Bitmap
 *
 * A dynamically resizing bitmap.
 *
 * - Bits are stored in linear memory.
 * - Capacity grows is controlled by user.
 */
typedef struct CONT_Bitmap CONT_Bitmap;

#define CONT_BITMAP_DEFAULT_BIT_CAP (64)
#define CONT_BITMAP_MAX_BIT_CAP (PRP_SIZE_MAX)

/**
 * Checks whether the given bitmap is structurally valid.
 *
 * @param bmp Pointer to the bitmap.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapIsValid(const CONT_Bitmap *bmp);

/**
 * Creates a dynamic bitmap.
 *
 * @param bit_cap Initial bit capacity.
 * @param pBmp    Output pointer receiving the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapCreateUnchecked(PRP_Size bit_cap,
                                                       CONT_Bitmap **pBmp);
/**
 * Creates a dynamic bitmap.
 *
 * @param bit_cap Initial bit capacity.
 * @param pBmp    Output pointer receiving the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapCreateChecked(PRP_Size bit_cap,
                                                     CONT_Bitmap **pBmp);
/**
 * Deep clones the given bitmap.
 *
 * @param bmp  The bitmap to clone.
 * @param pBmp Output pointer receiving the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapCloneUnchecked(const CONT_Bitmap *bmp,
                                                      CONT_Bitmap **pBmp);
/**
 * Deep clones the given bitmap.
 *
 * @param bmp  The bitmap to clone.
 * @param pBmp Output pointer receiving the bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapCloneChecked(const CONT_Bitmap *bmp,
                                                    CONT_Bitmap **pBmp);

/**
 * Deletes the bitmap and nullifies the pointer.
 *
 * @param pBmp Pointer to bitmap pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BitmapDeleteUnchecked(CONT_Bitmap **pBmp);
/**
 * Deletes the bitmap and nullifies the pointer.
 *
 * @param pBmp Pointer to bitmap pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArr or *pArr is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapDeleteChecked(CONT_Bitmap **pBmp);

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
PRP_API const CONT_Bitword *PRP_CALL CONT_BitmapRawUnchecked(
    const CONT_Bitmap *bmp, PRP_Size *pWord_cap, PRP_Size *pBit_cap);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapRawChecked(const CONT_Bitmap *bmp,
                                                  PRP_Size *pWord_cap,
                                                  PRP_Size *pBit_cap,
                                                  void **pRaw);

/**
 * Returns the number of set bits currently stored.
 *
 * @param bmp Bitmap instance.
 *
 * @return Number of bits set.
 *
 * @note Assumes valid bitmap (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_BitmapSetCount(const CONT_Bitmap *bmp);
/**
 * Returns the first set bit of the bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return 0-based index of the first bit set.
 *
 * @note Assumes valid bitmap (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_BitmapFFS(const CONT_Bitmap *bmp);
/**
 * Returns the number of bit cap of bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return Bit cap of bitmap.
 *
 * @note Assumes valid bitmap (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_BitmapBitCap(const CONT_Bitmap *bmp);

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
PRP_API PRP_Size PRP_CALL CONT_BitmapBitRankUnchecked(const CONT_Bitmap *bmp,
                                                      PRP_Size i);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapBitRankChecked(const CONT_Bitmap *bmp,
                                                      PRP_Size i,
                                                      PRP_Size *pRank);
/**
 * Sets bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BitmapSetUnchecked(CONT_Bitmap *bmp, PRP_Size i);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapSetChecked(CONT_Bitmap *bmp, PRP_Size i);
/**
 * Clears bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BitmapClrUnchecked(CONT_Bitmap *bmp, PRP_Size i);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapClrChecked(CONT_Bitmap *bmp, PRP_Size i);
/**
 * Toggles bit of the given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BitmapToggleUnchecked(CONT_Bitmap *bmp, PRP_Size i);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapToggleChecked(CONT_Bitmap *bmp,
                                                     PRP_Size i);
/**
 * Checks if bit set on given index.
 *
 * @param bmp Bitmap instance.
 * @param i   Index into the bitmap.
 *
 * @return PRP_True if bit set, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapIsSetUnchecked(const CONT_Bitmap *bmp,
                                                    PRP_Size i);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapIsSetChecked(const CONT_Bitmap *bmp,
                                                    PRP_Size i,
                                                    PRP_Bool *pRslt);

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
PRP_API void PRP_CALL CONT_BitmapSetRangeUnchecked(CONT_Bitmap *bmp, PRP_Size i,
                                                   PRP_Size j);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapSetRangeChecked(CONT_Bitmap *bmp,
                                                       PRP_Size i, PRP_Size j);
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
PRP_API void PRP_CALL CONT_BitmapClrRangeUnchecked(CONT_Bitmap *bmp, PRP_Size i,
                                                   PRP_Size j);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapClrRangeChecked(CONT_Bitmap *bmp,
                                                       PRP_Size i, PRP_Size j);
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
PRP_API void PRP_CALL CONT_BitmapToggleRangeUnchecked(CONT_Bitmap *bmp,
                                                      PRP_Size i, PRP_Size j);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapToggleRangeChecked(CONT_Bitmap *bmp,
                                                          PRP_Size i,
                                                          PRP_Size j);
/**
 * Checks if any bits are set in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return PRP_True if any bit is set in range, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapIsSetRangeAnyUnchecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapIsSetRangeAnyChecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j, PRP_Bool *pRslt);
/**
 * Checks if all bits are set in the range.
 *
 * @param bmp Bitmap instance.
 * @param i   The index 1 into the bitmap.
 * @param j   The index 2 into the bitmap.
 *
 * @return PRP_True if all bit is set in range, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapIsSetRangeAllUnchecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapIsSetRangeAllChecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j, PRP_Bool *pRslt);

/**
 * Checks if there is no bits set in bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_True if bitmap is empty, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapIsEmptyUnchecked(const CONT_Bitmap *bmp);
/**
 * Checks if there is no bits set in bitmap.
 *
 * @param bmp   Bitmap instance.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapIsEmptyChecked(const CONT_Bitmap *bmp,
                                                      PRP_Bool *pRslt);
/**
 * Checks if the bitmap is full.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_True if bitmap is full, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapIsFullUnchecked(const CONT_Bitmap *bmp);
/**
 * Checks if the bitmap is full.
 *
 * @param bmp   Bitmap instance.
 * @param pRslt The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapIsFullChecked(const CONT_Bitmap *bmp,
                                                     PRP_Bool *pRslt);

/**
 * Performs a NOT operation on every bit in bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BitmapNotUnchecked(CONT_Bitmap *bmp);
/**
 * Performs a NOT operation on every bit in bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapNotChecked(CONT_Bitmap *bmp);
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
PRP_API void PRP_CALL CONT_BitmapAndUnchecked(CONT_Bitmap *bmp1,
                                              const CONT_Bitmap *bmp2);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapAndChecked(CONT_Bitmap *bmp1,
                                                  const CONT_Bitmap *bmp2);
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
PRP_API void PRP_CALL CONT_BitmapOrUnchecked(CONT_Bitmap *bmp1,
                                             const CONT_Bitmap *bmp2);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapOrChecked(CONT_Bitmap *bmp1,
                                                 const CONT_Bitmap *bmp2);

/**
 * Checks if bmp1 & bmp2 == bmp2.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @return PRP_True if has all condition is met, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapHasAllUnchecked(const CONT_Bitmap *bmp1,
                                                     const CONT_Bitmap *bmp2);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapHasAllChecked(const CONT_Bitmap *bmp1,
                                                     const CONT_Bitmap *bmp2,
                                                     PRP_Bool *pRslt);
/**
 * Checks if bmp1 & bmp2 != 0.
 *
 * @param bmp1 Bitmap instance 1.
 * @param bmp2 Bitmap instance 2.
 *
 * @return PRP_True if has any condition is met, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapHasAnyUnchecked(const CONT_Bitmap *bmp1,
                                                     const CONT_Bitmap *bmp2);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapHasAnyChecked(const CONT_Bitmap *bmp1,
                                                     const CONT_Bitmap *bmp2,
                                                     PRP_Bool *pRslt);

/**
 * Comares if bits of bmp1 are equal to bmp2.
 *
 * @param bmp1  Bitmap instance 1.
 * @param bmp2  Bitmap instance 2.
 *
 * @return PRP_True if equal, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Bool PRP_CALL CONT_BitmapCmpUnchecked(const CONT_Bitmap *bmp1,
                                                  const CONT_Bitmap *bmp2);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapCmpChecked(const CONT_Bitmap *bmp1,
                                                  const CONT_Bitmap *bmp2,
                                                  PRP_Bool *pRslt);
/**
 * Resets the bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_BitmapResetUnchecked(CONT_Bitmap *bmp);
/**
 * Resets the bitmap.
 *
 * @param bmp Bitmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_BitmapResetChecked(CONT_Bitmap *bmp);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapShrinkFitUnchecked(CONT_Bitmap *bmp);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapShrinkFitChecked(CONT_Bitmap *bmp);
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
PRP_API PRP_Result PRP_CALL
CONT_BitmapChangeSizeUnchecked(CONT_Bitmap *bmp, PRP_Size new_bit_cap);
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
PRP_API PRP_Result PRP_CALL CONT_BitmapChangeSizeChecked(CONT_Bitmap *bmp,
                                                         PRP_Size new_bit_cap);

#ifdef __cplusplus
}
#endif
