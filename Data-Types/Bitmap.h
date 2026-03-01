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

#define DT_BITMAP_DEFAULT_BIT_CAP (64)
#define DT_BITMAP_MAX_BIT_CAP (DT_SIZE_MAX)

/**
 * @return The last error code set by the bitmap functions that don't return
 * PRP_Result explicitly.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapGetLastErrCode(DT_void);

/**
 * Creates the bitmap with user specified bit cap.
 *
 * @param bit_cap: The number of bits the user wants to consider for the bitmap
 * to have.
 *
 * @return The pointer to the bitmap.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateUnchecked(DT_size bit_cap);
/**
 * Creates the bitmap with user specified bit cap.
 *
 * @param bit_cap: The number of bits the user wants to consider for the bitmap
 * to have.
 *
 * @return The pointer to the bitmap.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateChecked(DT_size bit_cap);
/**
 * Clones the bitmap into a new bitmap, preserving all the contents of the
 * bitmap too.
 *
 * @param: bmp: The bitmap to clone.
 *
 * @return The pointer to the cloned bitmap.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCloneUnchecked(const DT_Bitmap *bmp);
/**
 * Clones the bitmap into a new bitmap, preserving all the contents of the
 * bitmap too.
 *
 * @param: bmp: The bitmap to clone.
 *
 * @return The pointer to the cloned bitmap.
 */
PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCloneChecked(const DT_Bitmap *bmp);

/**
 * Deletes the bitmap and sets the original DT_Bitmap * to DT_null to prevent
 * use after free bugs.
 *
 * @param pBmp: The pointer to the bitmap pointer to delete.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapDeleteUnchecked(DT_Bitmap **pBmp);
/**
 * Deletes the bitmap and sets the original DT_Bitmap * to DT_null to prevent
 * use after free bugs.
 *
 * @param pBmp: The pointer to the bitmap pointer to delete.
 *
 * @return PRP_ERR_INV_ARG if the pBmp or *pBmp is DT_null, otherwise it
 * returns PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapDeleteChecked(DT_Bitmap **pBmp);

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
 * @return The memory pointer of the bitmap's raw memory.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API const DT_Bitword *PRP_FN_CALL DT_BitmapRawUnchecked(
    const DT_Bitmap *bmp, DT_size *pWord_cap, DT_size *pBit_cap);
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
PRP_FN_API const DT_Bitword *PRP_FN_CALL DT_BitmapRawChecked(
    const DT_Bitmap *bmp, DT_size *pWord_cap, DT_size *pBit_cap);

/**
 * Returns the current set count of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the set count of.
 *
 * @return The set count fo the bitmap.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCountUnchecked(const DT_Bitmap *bmp);
/**
 * Returns the current set count of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the set count of.
 *
 * @return PRP_INVALID_SIZE if the bitmap is invalid, otherwise the actual set
 * count of the bitmap.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCountChecked(const DT_Bitmap *bmp);
/**
 * Returns the current first set pos of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the fist set pos of.
 *
 * @return PRP_INVALID_INDEX if there is no bit set, otherwise the actual
 * index of the first set bit.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFSUnchecked(const DT_Bitmap *bmp);
/**
 * Returns the current first set pos of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the fist set pos of.
 *
 * @return PRP_INVALID_INDEX if the bitmap is invalid, PRP_INVALID_INDEX if
 * there is no bit set, otherwise the actual index of the first set bit.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFSChecked(const DT_Bitmap *bmp);
/**
 * Returns the current bit cap of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the bit cap of.
 *
 * @return The bit cap of the bitmap.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCapUnchecked(const DT_Bitmap *bmp);
/**
 * Returns the current bit cap of the bitmap that is passed to it.
 *
 * @param bmp: The bitmap to get the bit cap of.
 *
 * @return PRP_INVALID_SIZE if the bitmap is invalid, otherwise the actual bit
 * cap of the bitmap.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCapChecked(const DT_Bitmap *bmp);

/**
 * Sets the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapSetUnchecked(DT_Bitmap *bmp, DT_size i);
/**
 * Sets the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i is bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapSetChecked(DT_Bitmap *bmp,
                                                      DT_size i);
/**
 * Clears the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapClrUnchecked(DT_Bitmap *bmp, DT_size i);
/**
 * Clears the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i is bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapClrChecked(DT_Bitmap *bmp,
                                                      DT_size i);
/**
 * Toggles the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapToggleUnchecked(DT_Bitmap *bmp,
                                                        DT_size i);
/**
 * Toggles the given index bit in the bitmap.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to set.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i is bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapToggleChecked(DT_Bitmap *bmp,
                                                         DT_size i);
/**
 * Checks the given index bit in the bitmap is set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to check for.
 *
 * @return DT_false if the given bit is not set, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsSetUnchecked(const DT_Bitmap *bmp,
                                                       DT_size i);
/**
 * Checks the given index bit in the bitmap is set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to check for.
 * @param pRslt: The pointer that will store the boolean result of IsSet.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i is bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetChecked(const DT_Bitmap *bmp,
                                                        DT_size i,
                                                        DT_bool *pRslt);

/**
 * Sets the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapSetRangeUnchecked(DT_Bitmap *bmp,
                                                          DT_size i, DT_size j);
/**
 * Sets the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i or j are bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapSetRangeChecked(DT_Bitmap *bmp,
                                                           DT_size i,
                                                           DT_size j);
/**
 * Clears the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapClrRangeUnchecked(DT_Bitmap *bmp,
                                                          DT_size i, DT_size j);
/**
 * Clears the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i or j are bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapClrRangeChecked(DT_Bitmap *bmp,
                                                           DT_size i,
                                                           DT_size j);
/**
 * Toggles the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapToggleRangeUnchecked(DT_Bitmap *bmp,
                                                             DT_size i,
                                                             DT_size j);
/**
 * Toggles the given range of bits in the bitmap. From i to j excluding j.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i or j are bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapToggleRangeChecked(DT_Bitmap *bmp,
                                                              DT_size i,
                                                              DT_size j);
/**
 * Checks if in the given range of indices in the bitmap any bit is set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return DT_false if no bits are set in the given range, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSetRangeAnyUnchecked(const DT_Bitmap *bmp, DT_size i, DT_size j);
/**
 * Checks if in the given range of indices in the bitmap any bit is set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 * @param pRslt: The pointer that will store the boolean result of the
 * operation.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i or j are bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetRangeAnyChecked(
    const DT_Bitmap *bmp, DT_size i, DT_size j, DT_bool *pRslt);
/**
 * Checks if in the given range of indices in the bitmap all bit are set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 *
 * @return DT_false if no bits are set in the given range, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSetRangeAllUnchecked(const DT_Bitmap *bmp, DT_size i, DT_size j);
/**
 * Checks if in the given range of indices in the bitmap all bit are set.
 *
 * @param bmp: The bitmap to operate on.
 * @param i: The index to start from.
 * @param j: The index to end at.
 * @param pRslt: The pointer that will store the boolean result of the
 * operation.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * PRP_ERR_OOB i or j are bigger or equal to btimap's bit cap, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetRangeAllChecked(
    const DT_Bitmap *bmp, DT_size i, DT_size j, DT_bool *pRslt);

/**
 * Checks the given bitmap is empty.
 *
 * @param bmp: The bitmap to check for.
 *
 * @return DT_false if the bitmap has at least one bit set, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsEmptyUnchecked(const DT_Bitmap *bmp);
/**
 * Checks the given bitmap is empty.
 *
 * @param bmp: The bitmap to check for.
 * @param pRslt: The pointer that will store the boolean result of IsEmpty.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsEmptyChecked(const DT_Bitmap *bmp,
                                                          DT_bool *pRslt);
/**
 * Checks the given bitmap is full.
 *
 * @param bmp: The bitmap to check for.
 *
 * @return DT_false if the bitmap is not completly full, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsFullUnchecked(const DT_Bitmap *bmp);
/**
 * Checks the given bitmap is full.
 *
 * @param bmp: The bitmap to check for.
 * @param pRslt: The pointer that will store the boolean result of IsFull.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsFullChecked(const DT_Bitmap *bmp,
                                                         DT_bool *pRslt);
/**
 * Finds if the first bitmap is a subset of the second bitmap.
 *
 * @param bmp1: The first bitmap.
 * @param bmp2: The second bitmap.
 *
 * @return DT_false if the bmp2 is not a subset of bmp1, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSubsetUnchecked(const DT_Bitmap *bmp1, const DT_Bitmap *bmp2);
/**
 * Finds if the first bitmap is a subset of the second bitmap.
 *
 * @param bmp1: The first bitmap.
 * @param bmp2: The second bitmap.
 * @param pRslt: The pointer that will store the boolean result of IsSubset.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSubsetChecked(
    const DT_Bitmap *bmp1, const DT_Bitmap *bmp2, DT_bool *pRslt);

/**
 * Inverses the bits in the given bitmap.
 *
 * @param bmp; The bitmap to operate on.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapNotUnchecked(DT_Bitmap *bmp);
/**
 * Inverses the bits in the given bitmap.
 *
 * @param bmp; The bitmap to operate on.
 *
 * @return PRP_ERR_INV_ARG if the bitmap is invalid, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapNotChecked(DT_Bitmap *bmp);
/**
 * ANDs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the anding, and will store the result also.
 * @param bmp2: The bitmap2 of the anding.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapAndUnchecked(DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2);
/**
 * ANDs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the anding, and will store the result also.
 * @param bmp2: The bitmap2 of the anding.
 *
 * @return PRP_ERR_INV_ARG if any of the two bitmaps is invalid, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapAndChecked(DT_Bitmap *bmp1,
                                                      const DT_Bitmap *bmp2);
/**
 * ORs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the oring, and will store the result also.
 * @param bmp2: The bitmap2 of the oring.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapOrUnchecked(DT_Bitmap *bmp1,
                                                    const DT_Bitmap *bmp2);
/**
 * ORs b/w bitmap1 and bitmap2 and stores the result in bmp1.
 *
 * @param bmp1: The bitmap1 of the oring, and will store the result also.
 * @param bmp2: The bitmap2 of the oring.
 *
 * @return PRP_ERR_INV_ARG if any of the two bitmaps is invalid, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapOrChecked(DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2);

/**
 * Checks if bitmap1 has all of the bits of bitmap2.
 *
 * @param bmp1: The bitmap which will be checked.
 * @param bmp2: The bitmap against which bmp1 will be checked.
 *
 * @return DT_false if the bmp1 doesn't have all the bits set that are in bmp2,
 * otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapHasAllUnchecked(const DT_Bitmap *bmp1,
                                                        const DT_Bitmap *bmp2);
/**
 * Checks if bitmap1 has all of the bits of bitmap2.
 *
 * @param bmp1: The bitmap which will be checked.
 * @param bmp2: The bitmap against which bmp1 will be checked.
 * @param pRslt: The pointer that will store the boolean result of HasAll.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapHasAllChecked(const DT_Bitmap *bmp1,
                                                         const DT_Bitmap *bmp2,
                                                         DT_bool *pRslt);
/**
 * Checks if bitmap1 has any of the bits of bitmap2.
 *
 * @param bmp1: The bitmap which will be checked.
 * @param bmp2: The bitmap against which bmp1 will be checked.
 *
 * @return DT_false if the bmp1 doesn't have any bits set that are in bmp2,
 * otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapHasAnyUnchecked(const DT_Bitmap *bmp1,
                                                        const DT_Bitmap *bmp2);
/**
 * Checks if bitmap1 has any of the bits of bitmap2.
 *
 * @param bmp1: The bitmap which will be checked.
 * @param bmp2: The bitmap against which bmp1 will be checked.
 * @param pRslt: The pointer that will store the boolean result of HasAny.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapHasAnyChecked(const DT_Bitmap *bmp1,
                                                         const DT_Bitmap *bmp2,
                                                         DT_bool *pRslt);

/**
 * Compares the bit structure of the two bitmaps, if they are exactly equal.
 *
 * @param bmp1: First bitmap.
 * @param bmp2: Second bitmap.
 *
 * @return DT_false if the bitmaps are equal, otherwise DT_true.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapCmpUnchecked(const DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2);
/**
 * Compares the bit structure of the two bitmaps, if they are exactly equal.
 *
 * @param bmp1: First bitmap.
 * @param bmp2: Second bitmap.
 * @param pRslt: The pointer that will store the boolean result of Cmp.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, otherwise
 * PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCmpChecked(const DT_Bitmap *bmp1,
                                                      const DT_Bitmap *bmp2,
                                                      DT_bool *pRslt);
/**
 * Resets the bitmap to make it behave like a brand new bitmap.
 *
 * @param bmp: The bitmap to reset.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_BitmapResetUnchecked(DT_Bitmap *bmp);
/**
 * Resets the bitmap to make it behave like a brand new bitmap.
 *
 * @param bmp: The bitmap to reset.
 *
 * @return PRP_ERR_INV_ARG if the bitmap is invalid, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapResetChecked(DT_Bitmap *bmp);
/**
 * Shrinks the bitmap to its MSB word.
 *
 * @param bmp: The bitmap to shrink fit.
 *
 * @return PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if we can't accomodate the
 * shrinking, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapShrinkFitUnchecked(DT_Bitmap *bmp);
/**
 * Shrinks the bitmap to its MSB word.
 *
 * @param bmp: The bitmap to shrink fit.
 *
 * @return PRP_ERR_INV_ARG if the bitmap is invalid,
 * PRP_ERR_RES_EXHAUSTED/PRP_ERR_OOM if we can't accomodate the shrinking,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapShrinkFitChecked(DT_Bitmap *bmp);
/**
 * Changes the bit cap of the given bitmap to the provided new cap safely.
 *
 * @param bitmap: The bitmap to change the cap of.
 * @param new_bit_cap: The new bit cap of the bitmap to change to.
 *
 * @return PRP_ERR_OOM if the reallocation fails, PRP_ERR_RES_EXHAUSTED if bffr
 * cap reaches a max, otherwise PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_BitmapChangeSizeUnchecked(DT_Bitmap *bmp, DT_size new_bit_cap);
/**
 * Changes the bit cap of the given bitmap to the provided new cap safely.
 *
 * @param bitmap: The bitmap to change the cap of.
 * @param new_bit_cap: The new bit cap of the bitmap to change to.
 *
 * @return PRP_ERR_INV_ARG if the parameters are invalid in any way, PRP_ERR_OOM
 * if the reallocation fails, PRP_ERR_RES_EXHAUSTED if bffr cap reaches a max,
 * otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
DT_BitmapChangeSizeChecked(DT_Bitmap *bmp, DT_size new_bit_cap);

#ifdef __cplusplus
}
#endif
