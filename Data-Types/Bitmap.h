#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

// The number of bits in the bitword
#define BITWORD_BITS (sizeof(DT_Bitword) * 8)

// Which word index the global i falls into.
#define WORD_I(i) ((i) >> 6)

// Which bit position in its word the global i falls into.
#define BIT_I(i) ((i) & 63)

// Word mask of the global i.
#define BIT_MASK(i) (1UL << BIT_I(i))

/**
 * A singular word of the bitmap that can hold 64 bits at once.
 * These are used in the DT_Bitmap, and can be used separately as there own
 * util.
 */
typedef DT_u64 DT_Bitword;

/**
 * Counts the trailing zeros of a bitword.
 *
 * @param word: The word to count the trailing zeros for.
 *
 * @return PRP_INVALID_INDEX if word is 0, otherwise the zero-based index of the
 * LSB.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCTZ(DT_Bitword word);
/**
 * Counts the leading zeros of a bitword.
 *
 * @param word: The word to count the leading zeros for.
 *
 * @return PRP_INVALID_INDEX if word is 0, otherwise the number of zeros after
 * the MSB.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCLZ(DT_Bitword word);
/**
 * Counts the total number of bits set in a bitword.
 *
 * @param word: The word to count the set bits for.
 *
 * @return The number of set bits in the given word.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordPopCnt(DT_Bitword word);
/**
 * Finds the first set bit of the word.
 *
 * @param word: The word to find FFS for.
 *
 * @return PRP_INVALID_POS if word is 0, otherwise a 1-based ffs pos.
 *
 * @note: This returns from 1-64 not 0-63. So handle accordingly.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitwordFFS(DT_Bitword word);

/**
 * A dynamic bitmap that stores bits and auto grows on demand.
 * Used for creating compact bool arrays.
 */
typedef struct _Bitmap DT_Bitmap;

/**
 * Creates the bitmap with user specified bit cap.
 *
 * @param bit_cap: The number of bits for the bitmap to have.
 * These get rounded up to the perfect word size. So a bit_cap of say 100 gets
 * rounded up to 128 for dev ease of life.
 *
 * @return The pointer to the bitmap.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreate(DT_size bit_cap);
/**
 * Creates the bitmap with default bit cap of 64.
 *
 * @return The pointer to the bitmap.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateDefault(DT_void);
/**
 * Clones the bitmap into a new bitmap, preserving all the contents of the
 * bitmap too.
 *
 * @param: bmp: The bitmap to clone.
 *
 * @return The pointer to the cloned bitmap.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapClone(DT_Bitmap *bmp);

/**
 * Deletes the bitmap and sets the original DT_Bitmap * to DT_null to prevent
 * use after free bugs.
 *
 * @param pBmp: The pointer to the bitmap pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pBmp is DT_null or the bitmap it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapDelete(DT_Bitmap **pBmp);

/**
 * Returns the raw memory pointer of the bitmap to the user.
 * This function returns a non-fixed pointer to the bitmap mem. If an operation
 * is performed to the bitmap after getting the raw data, the raw data is no
 * longer guaranteed to be valid.
 *
 * @param bmp: The bitmap to get the raw mem data of.
 * @param pWord_cap: Pointer to where the cap of the bitmap will be stored to be
 * used by the caller to prevent unsafe usage.
 *
 * @return DT_null if the bitmap is invalid or pWord_cap is DT_null, otherwise
 * the memory pointer of the bitmap's raw memory.
 */
PRP_FN_API DT_Bitword *PRP_FN_CALL DT_BitmapRaw(DT_Bitmap *bmp,
                                                DT_size *pWord_cap);

/**
 * Returns the current set count of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the set count of.
 *
 * @return PRP_INVALID_SIZE if the bitmap is invalid, otherwise the actual set
 * count of the bitmap.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCount(DT_Bitmap *bmp);
/**
 * Returns the current first set pos of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the fist set pos of.
 *
 * @return PRP_INVALID_POS if the bitmap is invalid, otherwise the actual
 * 1-based first set pos.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFS(DT_Bitmap *bmp);

/**
 * Sets the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if we cannot accommodate for index i, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapSet(DT_Bitmap *bmp, DT_size i);
/**
 * Clears the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to clear.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if i is out of bounds of the bitmap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapClr(DT_Bitmap *bmp, DT_size i);
/**
 * Toggles the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to toggle.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if i is out of bounds of the bitmap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapToggle(DT_Bitmap *bmp, DT_size i);
/**
 * Checks the given index bit in the bitmap is set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to check for.
 * @param pRslt: The pointer that will store the boolean result of IsSet.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if i is out of bounds of the bitmap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSet(DT_Bitmap *bmp, DT_size i,
                                                 DT_bool *pRslt);
/**
 * Checks the given bitmap is empty.
 *
 * @param bmp: The bitmap to check for.
 * @param pRslt: The pointer that will store the boolean result of IsEmpty.
 *
 * @return PRP_FN_INV_ARG_ERROR if the bitmap is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsEmpty(DT_Bitmap *bmp,
                                                   DT_bool *pRslt);
/**
 * Checks the given bitmap is full.
 *
 * @param bmp: The bitmap to check for.
 * @param pRslt: The pointer that will store the boolean result of IsFull.
 *
 * @return PRP_FN_INV_ARG_ERROR if the bitmap is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsFull(DT_Bitmap *bmp,
                                                  DT_bool *pRslt);

/**
 * Inverses the bits in the given bitmap.
 *
 * @param bmp; The bitmap to operate on.
 *
 * @return PRP_FN_INV_ARG_ERROR if the bitmap is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapNot(DT_Bitmap *bmp);
/**
 * ANDs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the anding, and will store the result also.
 * @param bmp2: The bitmap2 of the anding.
 *
 * @return PRP_FN_INV_ARG_ERROR if any of the two bitmaps is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapAnd(DT_Bitmap *bmp1,
                                               DT_Bitmap *bmp2);
/**
 * ORs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the oring, and will store the result also.
 * @param bmp2: The bitmap2 of the oring.
 *
 * @return PRP_FN_INV_ARG_ERROR if any of the two bitmaps is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapOr(DT_Bitmap *bmp1, DT_Bitmap *bmp2);

/**
 * Creates a new bitmap and stores inverted bits of the given bitmap.
 *
 * @param bmp; The bitmap to operate on.
 *
 * @return DT_null if the bitmap is invalid or creating of the new bitmap
 * failed, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapNotNew(DT_Bitmap *bmp);
/**
 * Creates a new bitmap which stores ANDs b/w bitmap1 and bitmap2.
 *
 * @param bmp1: The bitmap1 of the anding.
 * @param bmp2: The bitmap2 of the anding.
 *
 * @return DT_null if any of the two bitmaps is invalid or creating of the new
 * bitmap failed, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapAndNew(DT_Bitmap *bmp1,
                                                  DT_Bitmap *bmp2);
/**
 * Creates a new bitmap which stores ORs b/w bitmap1 and bitmap2.
 *
 * @param bmp1: The bitmap1 of the oring.
 * @param bmp2: The bitmap2 of the oring.
 *
 * @return DT_null if any of the two bitmaps is invalid or creating of the new
 * bitmap failed, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapOrNew(DT_Bitmap *bmp1,
                                                 DT_Bitmap *bmp2);

/**
 * Checks if bitmap1 has all of the bits of bitmap2.
 *
 * @param bmp1: The bitmap which will be checked.
 * @param bmp2: The bitmap against which bmp1 will be checked.
 * @param pRslt: The pointer that will store the boolean result of HasAll.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapHasAll(DT_Bitmap *bmp1,
                                                  DT_Bitmap *bmp2,
                                                  DT_bool *pRslt);
/**
 * Checks if bitmap1 has any of the bits of bitmap2.
 *
 * @param bmp1: The bitmap which will be checked.
 * @param bmp2: The bitmap against which bmp1 will be checked.
 * @param pRslt: The pointer that will store the boolean result of HasAny.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapHasAny(DT_Bitmap *bmp1,
                                                  DT_Bitmap *bmp2,
                                                  DT_bool *pRslt);

/**
 * Compares the bit structure of the two bitmaps, if they are exactly equal.
 *
 * @param bmp1: First bitmap.
 * @param bmp2: Second bitmap.
 * @param pRslt: The pointer that will store the boolean result of Cmp.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapCmp(DT_Bitmap *bmp1, DT_Bitmap *bmp2,
                                               DT_bool *pRslt);
/**
 * Resets the bitmap to make it behave like a brand new bitmap.
 *
 * @param bmp: The bitmap to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the bitmap is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapReset(DT_Bitmap *bmp);
/**
 * Shrinks the bitmap to its MSB word.
 *
 * @param bmp: The bitmap to shrink fit.
 *
 * @return PRP_FN_INV_ARG_ERROR if the bitmap is invalid, PRP_FN_MALLOC_ERROR if
 * realloc failed on bitmap, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapShrinkFit(DT_Bitmap *bmp);

#ifdef __cplusplus
}
#endif
