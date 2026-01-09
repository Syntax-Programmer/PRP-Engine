#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

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

/* ----  BITMAP UTILS ---- */

/**
 * A dynamic bitmap that stores bits and auto grows on demand.
 * Used for creating compact bool arrays.
 */
typedef struct _Bitmap DT_Bitmap;

/**
 * Creates the bitmap with user specified bit cap.
 *
 * @param bit_cap: The number of bits the user wants to consider for the bitmap
 * to have.
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
 * NOTE: If any kind of changes user makes without using designated functions
 * will cause corruption instantly.
 *
 * @param bmp: The bitmap to get the raw mem data of.
 * @param pWord_cap: Pointer to where the cap of the bitmap will be stored to be
 * used by the caller to prevent unsafe usage.
 * @param pBit_cap: Pointer to where the bit cap of the bitmap the user set will
 * be stored, for the semantic correctness of what user picked.
 *
 * @return DT_null if the bitmap is invalid or pWord_cap is DT_null, otherwise
 * the memory pointer of the bitmap's raw memory.
 */
PRP_FN_API const DT_Bitword *PRP_FN_CALL DT_BitmapRaw(const DT_Bitmap *bmp,
                                                      DT_size *pWord_cap,
                                                      DT_size *pBit_cap);

/**
 * Returns the current set count of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the set count of.
 *
 * @return PRP_INVALID_SIZE if the bitmap is invalid, otherwise the actual set
 * count of the bitmap.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCount(const DT_Bitmap *bmp);
/**
 * Returns the current first set pos of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the fist set pos of.
 *
 * @return PRP_INVALID_INDEX if there is no index set.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFS(const DT_Bitmap *bmp);
/**
 * Returns the current bit cap of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the bit cap of.
 *
 * @return PRP_INVALID_SIZE if the bitmap is invalid, otherwise the actual bit
 * cap of the bitmap.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCap(const DT_Bitmap *bmp);

/**
 * Sets the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i is bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapSet(DT_Bitmap *bmp, DT_size i);
/**
 * Clears the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i is bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapClr(DT_Bitmap *bmp, DT_size i);
/**
 * Toggles the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i is bigger or equal to btimap's bit cap, otherwise
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
 * PRP_FN_OOB_ERROR i is bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSet(const DT_Bitmap *bmp,
                                                 DT_size i, DT_bool *pRslt);

/**
 * Sets the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i or j are bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapSetRange(DT_Bitmap *bmp, DT_size i,
                                                    DT_size j);
/**
 * Clears the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i or j are bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapClrRange(DT_Bitmap *bmp, DT_size i,
                                                    DT_size j);
/**
 * Toggles the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i or j are bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapToggleRange(DT_Bitmap *bmp,
                                                       DT_size i, DT_size j);
/**
 * Checks if in the given range of indices in the bitmap any bit is set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 * @param pRslt: The pointer that will store the boolean result of the
 * operation.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i or j are bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSetRangeAny(const DT_Bitmap *bmp,
                                                         DT_size i, DT_size j,
                                                         DT_bool *pRslt);
/**
 * Checks if in the given range of indices in the bitmap all bit are set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 * @param pRslt: The pointer that will store the boolean result of the
 * operation.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR i or j are bigger or equal to btimap's bit cap, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSetRangeAll(const DT_Bitmap *bmp,
                                                         DT_size i, DT_size j,
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
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsEmpty(const DT_Bitmap *bmp,
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
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsFull(const DT_Bitmap *bmp,
                                                  DT_bool *pRslt);
/**
 * Finds if the first bitmap is a subset of the second bitmap.
 *
 * @param bmp1: The first bitmap.
 * @param bmp2: The second bitmap.
 * @param pRslt: The pointer that will store the boolean result of IsSubset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSubset(const DT_Bitmap *bmp1,
                                                    const DT_Bitmap *bmp2,
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
                                               const DT_Bitmap *bmp2);
/**
 * ORs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the oring, and will store the result also.
 * @param bmp2: The bitmap2 of the oring.
 *
 * @return PRP_FN_INV_ARG_ERROR if any of the two bitmaps is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapOr(DT_Bitmap *bmp1,
                                              const DT_Bitmap *bmp2);

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
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapHasAll(const DT_Bitmap *bmp1,
                                                  const DT_Bitmap *bmp2,
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
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapHasAny(const DT_Bitmap *bmp1,
                                                  const DT_Bitmap *bmp2,
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
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapCmp(const DT_Bitmap *bmp1,
                                               const DT_Bitmap *bmp2,
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
/**
 * Changes the bit cap of the given bitmap to the provided new cap safely.
 *
 * @param bitmap: The bitmap to change the cap of.
 * @param new_bit_cap: The new bit cap of the bitmap to change to.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way
 * PRP_FN_MALLOC_ERROR if the reallocation fails, otherwise PRP_FN_SUCCESS;
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapChangeSize(DT_Bitmap *bmp,
                                                      DT_size new_bit_cap);

#ifdef __cplusplus
}
#endif
