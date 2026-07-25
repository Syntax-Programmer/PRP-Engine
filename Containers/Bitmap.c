#include "Bitmap.h"
#include "Diagnostics/Assert.h"
#include <string.h>

/* ----  BITWORD UTILS ---- */

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitwordCTZ(CONT_Bitword word) {
    if (!word) {
        return PRP_INVALID_INDEX;
    }
#ifdef PRP_COMPILER_MSVC
    unsigned long i;
    _BitScanForward64(&i, word);
    return (PRP_Size)i;
#else
    return (PRP_Size)__builtin_ctzll(word);
#endif
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitwordCLZ(CONT_Bitword word) {
    if (!word) {
        return PRP_INVALID_INDEX;
    }
#ifdef PRP_COMPILER_MSVC
    unsigned long i;
    _BitScanReverse64(&i, word);
    return (PRP_Size)(63 - i);
#else
    return (PRP_Size)__builtin_clzll(word);
#endif
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitwordPopCnt(CONT_Bitword word) {
#ifdef PRP_COMPILER_MSVC
    return (PRP_Size)__popcnt64(word);
#else
    return (PRP_Size)__builtin_popcountll(word);
#endif
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitwordFFS(CONT_Bitword word) {
#ifdef PRP_COMPILER_MSVC
    unsigned long i;
    if (_BitScanForward64(&i, word))
        return (PRP_Size)(i);
    return PRP_INVALID_INDEX;
#else
    return word ? (PRP_Size)__builtin_ctzll(word) : PRP_INVALID_INDEX;
#endif
}

/* ----  BITMAP UTILS ---- */

struct _Bitmap {
    // The count of bits currently set.
    PRP_Size set_c;
    /*
     * A cache for the first set index of the bitmap.
     * We cache this particular thing is that we often times need FFS calls very
     * much in code, and caching to speed it up is worth it.
     */
    PRP_Size first_set;
    /*
     * The semantic max cap the user has set. No operations will be performed
     * beyond this cap.
     */
    PRP_Size bit_cap;

    // The cap of the number of words allocated.
    PRP_Size word_cap;
    // The words that will store the bits.
    CONT_Bitword *words;
};

#define ASSERT_INVARIANT_EXPR(bmp)                                             \
    DIAG_ASSERT_MSG(CONT_BitmapIsValid(bmp),                                   \
                    "The given bitmap is either NULL, or is corrupted.")

/**
 * Recomputes the first set index for the given bitmap, updating the cached
 * value.
 *
 * @param bmp   The bitmap to update the first set index of.
 * @param start There are certain conditions where we are confirmed the first
 * set to be beyond or equal to start index. So we take in that for easier
 * computation.
 */
static void BitmapCalcFirstSet(CONT_Bitmap *bmp, PRP_Size start);

static void BitmapCalcFirstSet(CONT_Bitmap *bmp, PRP_Size start) {
    if (!bmp->set_c) {
        bmp->first_set = PRP_INVALID_INDEX;
        return;
    }

    /*
     * Since this function is only used after we clear the fs bit, doing
     this is
     * always valid, why?
     * Because we are sure that there is no bit set before the fs_pos,
     due to
     * the active updation we do during the bitmap operations.
     */
    PRP_Size i = (start != PRP_INVALID_INDEX)          ? WORD_I(start)
                 : bmp->first_set != PRP_INVALID_INDEX ? WORD_I(bmp->first_set)
                                                       : 0;
    for (; i < bmp->word_cap; i++) {
        CONT_Bitword word = bmp->words[i];
        if (!word) {
            continue;
        }
        bmp->first_set = CONT_BitwordFFS(word) + (i * BITWORD_BITS);
        return;
    }
    /*
     * This is just a "just in case" line because according to my
     intuition it
     * will never execute ever.
     */
    bmp->first_set = PRP_INVALID_INDEX;
}

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BitmapIsValid(const CONT_Bitmap *bmp) {
    return (bmp != NULL && bmp->words != NULL &&
            bmp->bit_cap <= CONT_BITMAP_MAX_BIT_CAP &&
            bmp->set_c <= bmp->bit_cap &&
            WORD_I(bmp->bit_cap) == bmp->word_cap - 1);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapCreateUnchecked(PRP_Size bit_cap, CONT_Bitmap **pBmp) {
    DIAG_ASSERT(bit_cap > 0 && bit_cap <= CONT_BITMAP_MAX_BIT_CAP);
    DIAG_ASSERT(pBmp != NULL);

    CONT_Bitmap *bmp = malloc(sizeof(CONT_Bitmap));
    if (!bmp) {
        return PRP_ERR_OOM;
    }
    bmp->word_cap = WORD_I(bit_cap) + 1;
    bmp->words = calloc(1, sizeof(CONT_Bitword) * bmp->word_cap);
    if (!bmp->words) {
        free(bmp);
        return PRP_ERR_OOM;
    }
    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    bmp->bit_cap = bit_cap;

    *pBmp = bmp;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapCreateChecked(PRP_Size bit_cap,
                                                           CONT_Bitmap **pBmp) {
    if (!bit_cap || bit_cap > CONT_BITMAP_MAX_BIT_CAP || !pBmp) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapCreateUnchecked(bit_cap, pBmp);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapCloneUnchecked(const CONT_Bitmap *bmp, CONT_Bitmap **pBmp) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(pBmp != NULL);

    PRP_Result code = CONT_BitmapCreateUnchecked(bmp->bit_cap, pBmp);
    if (code != PRP_OK) {
        return code;
    }
    CONT_Bitmap *cpy = *pBmp;
    cpy->set_c = bmp->set_c;
    cpy->first_set = bmp->first_set;
    memcpy(cpy->words, bmp->words, sizeof(CONT_Bitword) * cpy->word_cap);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapCloneChecked(const CONT_Bitmap *bmp, CONT_Bitmap **pBmp) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapCloneUnchecked(bmp, pBmp);
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapDeleteUnchecked(CONT_Bitmap **pBmp) {
    DIAG_ASSERT(pBmp != NULL);
    DIAG_ASSERT(*pBmp != NULL && (*pBmp)->words != NULL);

    CONT_Bitmap *bmp = *pBmp;

    free(bmp->words);

#if PRP_DEBUG_MODE
    bmp->words = NULL;
    bmp->bit_cap = bmp->word_cap = bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
#endif

    free(bmp);
    *pBmp = NULL;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapDeleteChecked(CONT_Bitmap **pBmp) {
    if (!pBmp || !(*pBmp) || !(*pBmp)->words) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapDeleteUnchecked(pBmp);

    return PRP_OK;
}

PRP_FN_API const CONT_Bitword *PRP_FN_CALL CONT_BitmapRawUnchecked(
    const CONT_Bitmap *bmp, PRP_Size *pWord_cap, PRP_Size *pBit_cap) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(pWord_cap != NULL);
    DIAG_ASSERT(pBit_cap != NULL);

    *pWord_cap = bmp->word_cap;
    *pBit_cap = bmp->bit_cap;

    return bmp->words;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapRawChecked(const CONT_Bitmap *bmp,
                                                        PRP_Size *pWord_cap,
                                                        PRP_Size *pBit_cap,
                                                        void **pRaw) {
    if (!CONT_BitmapIsValid(bmp) || !pWord_cap || !pBit_cap || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pWord_cap = bmp->word_cap;
    *pBit_cap = bmp->bit_cap;

    *pRaw = bmp->words;

    return PRP_OK;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitmapSetCount(const CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return bmp->set_c;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitmapFFS(const CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return bmp->first_set;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BitmapBitCap(const CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return bmp->bit_cap;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapSetUnchecked(CONT_Bitmap *bmp,
                                                    PRP_Size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    PRP_Size word_i = WORD_I(i);
    CONT_Bitword mask = BIT_MASK(i);
    if (bmp->words[word_i] & mask) {
        return;
    }
    bmp->words[word_i] |= mask;
    bmp->set_c++;
    if (i < bmp->first_set) {
        // New first_set found.
        bmp->first_set = i;
    }
}

PRP_FN_API PRP_Size PRP_FN_CALL
CONT_BitmapBitRankUnchecked(const CONT_Bitmap *bmp, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    PRP_Size idx = 0;
    for (PRP_Size j = 0; j < WORD_I(i); j++) {
        idx += CONT_BitwordPopCnt(bmp->words[j]);
    }
    idx += CONT_BitwordPopCnt(bmp->words[WORD_I(i)] & (BIT_MASK(i) - 1));

    return idx;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapBitRankChecked(const CONT_Bitmap *bmp, PRP_Size i, PRP_Size *pRank) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    *pRank = CONT_BitmapBitRankUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapSetChecked(CONT_Bitmap *bmp,
                                                        PRP_Size i) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    CONT_BitmapSetUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapClrUnchecked(CONT_Bitmap *bmp,
                                                    PRP_Size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    PRP_Size word_i = WORD_I(i);
    CONT_Bitword mask = BIT_MASK(i);
    if (bmp->words[word_i] & mask) {
        bmp->words[word_i] &= ~mask;
        bmp->set_c--;
        if (i == bmp->first_set) {
            // Recomputing fist set if we just cleared it.
            BitmapCalcFirstSet(bmp, i + 1);
        }
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapClrChecked(CONT_Bitmap *bmp,
                                                        PRP_Size i) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    CONT_BitmapClrUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapToggleUnchecked(CONT_Bitmap *bmp,
                                                       PRP_Size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    PRP_Size word_i = WORD_I(i);
    CONT_Bitword mask = BIT_MASK(i);
    bmp->words[word_i] ^= mask;

    if (bmp->words[word_i] & mask) {
        bmp->set_c++;
        if (i < bmp->first_set) {
            // New first_set found.
            bmp->first_set = i;
        }
    } else {
        bmp->set_c--;
        if (i == bmp->first_set) {
            // Recomputing fist set if we just cleared it.
            BitmapCalcFirstSet(bmp, i + 1);
        }
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapToggleChecked(CONT_Bitmap *bmp,
                                                           PRP_Size i) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    CONT_BitmapToggleUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_BitmapIsSetUnchecked(const CONT_Bitmap *bmp, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    return ((bmp->words[WORD_I(i)] & BIT_MASK(i)) != 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapIsSetChecked(const CONT_Bitmap *bmp, PRP_Size i, PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(bmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    *pRslt = CONT_BitmapIsSetUnchecked(bmp, i);

    return PRP_OK;
}

#define ASSERT_RANGE_OPS_VALIDITY(bmp, i, j)                                   \
    do {                                                                       \
        ASSERT_INVARIANT_EXPR((bmp));                                          \
        DIAG_ASSERT((i) < (j));                                                \
        DIAG_ASSERT((i) < (bmp)->bit_cap && (j) <= (bmp)->bit_cap);            \
    } while (0)

#define CHECK_RANGE_OPS_VALIDITY(bmp, i, j)                                    \
    do {                                                                       \
        if (!CONT_BitmapIsValid((bmp)) || (i) >= (j)) {                        \
            return PRP_ERR_INV_ARG;                                            \
        }                                                                      \
        if ((i) >= (bmp)->bit_cap || j > (bmp)->bit_cap) {                     \
            return PRP_ERR_OOB;                                                \
        }                                                                      \
    } while (0)

#define MAKE_SAME_WORD_MASK(mask, i, last)                                     \
    do {                                                                       \
        mask = ((CONT_Bitword)~0 << BIT_I(i));                                 \
        /* This prevents UB edge case where (<< 64) is undefined. */           \
        if (BIT_I(last) < 63) {                                                \
            mask &= ~((CONT_Bitword)~0 << (BIT_I(last) + 1));                  \
        }                                                                      \
    } while (0);

#define MAKE_PARTIAL_FIRST_WORD_MASK(mask, i)                                  \
    do {                                                                       \
        mask = ((CONT_Bitword)(~0) << BIT_I(i));                               \
    } while (0)

// This ternary prevents UB edge case where (<< 64) is undefined.
#define MAKE_PARTIAL_LAST_WORD_MASK(mask, last)                                \
    do {                                                                       \
        mask =                                                                 \
            (BIT_I(last) == 63) ? (CONT_Bitword)~0 : (BIT_MASK(last + 1) - 1); \
    } while (0)

PRP_FN_API void PRP_FN_CALL CONT_BitmapSetRangeUnchecked(CONT_Bitmap *bmp,
                                                         PRP_Size i,
                                                         PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        bmp->set_c += (CONT_BitwordPopCnt(mask) -
                       CONT_BitwordPopCnt(bmp->words[wi] & mask));
        bmp->words[wi] |= mask;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        bmp->set_c += CONT_BitwordPopCnt(mask) -
                      CONT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] |= mask;

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        bmp->set_c += CONT_BitwordPopCnt(mask) -
                      CONT_BitwordPopCnt(bmp->words[wj] & mask);
        bmp->words[wj] |= mask;

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            bmp->set_c += BITWORD_BITS - CONT_BitwordPopCnt(bmp->words[wi]);
            bmp->words[wi] = (CONT_Bitword)~0;
        }
    }

    if (bmp->first_set == PRP_INVALID_INDEX || i < bmp->first_set) {
        bmp->first_set = i;
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapSetRangeChecked(CONT_Bitmap *bmp,
                                                             PRP_Size i,
                                                             PRP_Size j) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);

    CONT_BitmapSetRangeUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapClrRangeUnchecked(CONT_Bitmap *bmp,
                                                         PRP_Size i,
                                                         PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    if (!bmp->set_c) {
        return;
    }

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] &= ~mask;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] &= ~mask;

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wj] & mask);
        bmp->words[wj] &= ~mask;

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wi]);
            bmp->words[wi] = 0;
        }
    }

    if (bmp->first_set >= i && bmp->first_set < j) {
        BitmapCalcFirstSet(bmp, j);
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapClrRangeChecked(CONT_Bitmap *bmp,
                                                             PRP_Size i,
                                                             PRP_Size j) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);

    CONT_BitmapClrRangeUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapToggleRangeUnchecked(CONT_Bitmap *bmp,
                                                            PRP_Size i,
                                                            PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] ^= mask;
        bmp->set_c += CONT_BitwordPopCnt(bmp->words[wi] & mask);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] ^= mask;
        bmp->set_c += CONT_BitwordPopCnt(bmp->words[wi] & mask);

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wj] & mask);
        bmp->words[wj] ^= mask;
        bmp->set_c += CONT_BitwordPopCnt(bmp->words[wj] & mask);

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            bmp->set_c -= CONT_BitwordPopCnt(bmp->words[wi]);
            bmp->words[wi] = ~bmp->words[wi];
            bmp->set_c += CONT_BitwordPopCnt(bmp->words[wi]);
        }
    }

    BitmapCalcFirstSet(bmp, i);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapToggleRangeChecked(CONT_Bitmap *bmp, PRP_Size i, PRP_Size j) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);

    CONT_BitmapToggleRangeUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BitmapIsSetRangeAnyUnchecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        return ((bmp->words[wi] & mask) != 0);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((bmp->words[wi] & mask) != 0) {
            return PRP_True;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((bmp->words[wj] & mask) != 0) {
            return PRP_True;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (bmp->words[wi]) {
                return PRP_True;
            }
        }
    }

    return PRP_False;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapIsSetRangeAnyChecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j, PRP_Bool *pRslt) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapIsSetRangeAnyUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BitmapIsSetRangeAllUnchecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        return ((bmp->words[wi] & mask) == mask);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((bmp->words[wi] & mask) != mask) {
            return PRP_False;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((bmp->words[wj] & mask) != mask) {
            return PRP_False;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (bmp->words[wi] != (CONT_Bitword)~0) {
                return PRP_False;
            }
        }
    }

    return PRP_True;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapIsSetRangeAllChecked(
    const CONT_Bitmap *bmp, PRP_Size i, PRP_Size j, PRP_Bool *pRslt) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapIsSetRangeAllUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_BitmapIsEmptyUnchecked(const CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return (bmp->set_c == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapIsEmptyChecked(const CONT_Bitmap *bmp, PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(bmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = (bmp->set_c == 0);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_BitmapIsFullUnchecked(const CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return (bmp->set_c == bmp->bit_cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapIsFullChecked(const CONT_Bitmap *bmp, PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(bmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = (bmp->set_c == bmp->bit_cap);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapNotUnchecked(CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    bmp->first_set = PRP_INVALID_INDEX;
    for (PRP_Size i = 0; i < bmp->word_cap; i++) {
        bmp->words[i] = ~(bmp->words[i]);
        // Resetting the fs_pos to new conditions.
        if (bmp->first_set == PRP_INVALID_INDEX && bmp->words[i]) {
            bmp->first_set =
                CONT_BitwordFFS(bmp->words[i]) + (i * BITWORD_BITS);
        }
    }

    bmp->set_c = bmp->bit_cap - bmp->set_c;

    // Clearing the bits over bit_cap that were also set to 1 by not operation.
    PRP_Size r = bmp->bit_cap & (BITWORD_BITS - 1);
    CONT_Bitword mask =
        ~((CONT_Bitword)0) >> ((BITWORD_BITS - r) & (BITWORD_BITS - 1));
    bmp->words[bmp->word_cap - 1] &= mask;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapNotChecked(CONT_Bitmap *bmp) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapNotUnchecked(bmp);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapAndUnchecked(CONT_Bitmap *bmp1,
                                                    const CONT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    bmp1->set_c = 0;
    bmp1->first_set = PRP_INVALID_INDEX;
    PRP_Size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (PRP_Size i = 0; i < min_cap; i++) {
        bmp1->words[i] &= bmp2->words[i];

        PRP_Size pc = CONT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (bmp1->first_set == PRP_INVALID_INDEX && pc) {
            bmp1->first_set =
                CONT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
    for (PRP_Size i = min_cap; i < bmp1->word_cap; i++) {
        bmp1->words[i] = 0;
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapAndChecked(CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2) {
    if (!CONT_BitmapIsValid(bmp1) || !CONT_BitmapIsValid(bmp2)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapAndUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapOrUnchecked(CONT_Bitmap *bmp1,
                                                   const CONT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    bmp1->set_c = 0;
    bmp1->first_set = PRP_INVALID_INDEX;
    PRP_Size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (PRP_Size i = 0; i < min_cap; i++) {
        bmp1->words[i] |= bmp2->words[i];

        PRP_Size pc = CONT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (bmp1->first_set == PRP_INVALID_INDEX && pc) {
            bmp1->first_set =
                CONT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
    for (PRP_Size i = min_cap; i < bmp1->word_cap; i++) {
        // Still updating set_c and fs_pos since in OR they can increase.
        PRP_Size pc = CONT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;
        // Resetting the fs_pos to new conditions.
        if (bmp1->first_set == PRP_INVALID_INDEX && pc) {
            bmp1->first_set =
                CONT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapOrChecked(CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2) {
    if (!CONT_BitmapIsValid(bmp1) || !CONT_BitmapIsValid(bmp2)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapOrUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_BitmapHasAllUnchecked(const CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    PRP_Size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (PRP_Size i = 0; i < min_words; i++) {
        if ((bmp1->words[i] & bmp2->words[i]) != bmp2->words[i]) {
            return PRP_False;
        }
    }
    for (PRP_Size i = min_words; i < bmp2->word_cap; i++) {
        if (bmp2->words[i]) {
            return PRP_False;
        }
    }

    return PRP_True;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapHasAllChecked(
    const CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2, PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(bmp1) || !CONT_BitmapIsValid(bmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapHasAllUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_BitmapHasAnyUnchecked(const CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    PRP_Size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (PRP_Size i = 0; i < min_words; i++) {
        if ((bmp1->words[i] & bmp2->words[i])) {
            return PRP_True;
        }
    }

    // Not checking beyond min_words as in this case they are irrelevant.
    return PRP_False;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapHasAnyChecked(
    const CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2, PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(bmp1) || !CONT_BitmapIsValid(bmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapHasAnyUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_BitmapCmpUnchecked(const CONT_Bitmap *bmp1, const CONT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    if (bmp1->set_c != bmp2->set_c || bmp1->first_set != bmp2->first_set) {
        return PRP_False;
    }

    PRP_Size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    PRP_Size max_words = PRP_MAX(bmp1->word_cap, bmp2->word_cap);
    if (memcmp(bmp1->words, bmp2->words, sizeof(CONT_Bitword) * min_words) !=
        0) {
        return PRP_False;
    }
    // This will not run for edge cases since loops.
    const CONT_Bitmap *mx_bmp = (bmp1->word_cap == max_words) ? bmp1 : bmp2;
    for (PRP_Size i = min_words; i < max_words; i++) {
        if (mx_bmp->words[i]) {
            return PRP_False;
        }
    }

    return PRP_True;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapCmpChecked(const CONT_Bitmap *bmp1,
                                                        const CONT_Bitmap *bmp2,
                                                        PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(bmp1) || !CONT_BitmapIsValid(bmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapCmpUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BitmapResetUnchecked(CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    memset(bmp->words, 0, sizeof(CONT_Bitword) * bmp->word_cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BitmapResetChecked(CONT_Bitmap *bmp) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapResetUnchecked(bmp);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapShrinkFitUnchecked(CONT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    if (bmp->set_c) {
        PRP_Size i = bmp->word_cap;
        // Finding last i that has a bit on. Till then shrink will happen.
        for (; i-- > 0 && !bmp->words[i];)
            ;
        return CONT_BitmapChangeSizeUnchecked(bmp, (i + 1) * BITWORD_BITS);
    } else {
        return CONT_BitmapChangeSizeUnchecked(bmp, CONT_BITMAP_DEFAULT_BIT_CAP);
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapShrinkFitChecked(CONT_Bitmap *bmp) {
    if (!CONT_BitmapIsValid(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapShrinkFitUnchecked(bmp);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapChangeSizeUnchecked(CONT_Bitmap *bmp, PRP_Size new_bit_cap) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(new_bit_cap > 0 && new_bit_cap <= CONT_BITMAP_MAX_BIT_CAP);

    PRP_Size new_word_i = WORD_I(new_bit_cap);
    PRP_Size new_word_cap = new_word_i + 1;
    if (bmp->word_cap == new_word_cap) {
        bmp->bit_cap = new_bit_cap;
        return PRP_OK;
    }

    PRP_Size set_c_neg = 0;
    if (new_word_cap < bmp->word_cap) {
        // Clearing the bits over bit_cap that were also set to 1 by not
        // operation.
        PRP_Size r = bmp->bit_cap & (BITWORD_BITS - 1);
        CONT_Bitword mask =
            ~((CONT_Bitword)0) >> ((BITWORD_BITS - r) & (BITWORD_BITS - 1));
        // This calcs the partial word set_c reduction count on cao red.
        set_c_neg += CONT_BitwordPopCnt(bmp->words[new_word_cap - 1] & mask);
        for (PRP_Size i = new_word_cap; i < bmp->word_cap; i++) {
            set_c_neg += CONT_BitwordPopCnt(bmp->words[i]);
        }
    }

    CONT_Bitword *words =
        realloc(bmp->words, sizeof(CONT_Bitword) * new_word_cap);
    if (!words) {
        return PRP_ERR_OOM;
    }

    if (new_word_cap > bmp->word_cap) {
        memset(&(words[bmp->word_cap]), 0,
               sizeof(CONT_Bitword) * (new_word_cap - bmp->word_cap));
    }
    bmp->words = words;
    bmp->word_cap = new_word_cap;
    bmp->bit_cap = new_bit_cap;
    bmp->set_c -= set_c_neg;
    if (bmp->first_set >= bmp->bit_cap) {
        /*
         * Bcuz if first_set index is weeded pBmp in the size change the set_c
         * is 0 and we can just skip the function calls entirely.
         */
        bmp->first_set = PRP_INVALID_INDEX;
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BitmapChangeSizeChecked(CONT_Bitmap *bmp, PRP_Size new_bit_cap) {
    if (!CONT_BitmapIsValid(bmp) || !new_bit_cap ||
        new_bit_cap > CONT_BITMAP_MAX_BIT_CAP) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapChangeSizeUnchecked(bmp, new_bit_cap);
}
