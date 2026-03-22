#include "Bitmap.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

/* ----  BITWORD UTILS ---- */

PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCTZ(DT_Bitword word) {
    if (!word) {
        return PRP_INVALID_INDEX;
    }
#ifdef _MSC_VER
    unsigned long i;
    _BitScanForward64(&i, word);
    return (DT_size)i;
#else
    return (DT_size)__builtin_ctzll(word);
#endif
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCLZ(DT_Bitword word) {
    if (!word) {
        return PRP_INVALID_INDEX;
    }
#ifdef _MSC_VER
    unsigned long i;
    _BitScanReverse64(&i, word);
    return (DT_size)(63 - i);
#else
    return (DT_size)__builtin_clzll(word);
#endif
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitwordPopCnt(DT_Bitword word) {
#ifdef _MSC_VER
    return (DT_size)__popcnt64(word);
#else
    return (DT_size)__builtin_popcountll(word);
#endif
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitwordFFS(DT_Bitword word) {
#ifdef _MSC_VER
    unsigned long i;
    if (_BitScanForward64(&i, word))
        return (DT_size)(i);
    return PRP_INVALID_INDEX;
#else
    return word ? (DT_size)__builtin_ctzll(word) : PRP_INVALID_INDEX;
#endif
}

/* ----  BITMAP UTILS ---- */

struct _Bitmap {
    // The count of bits currently set.
    DT_size set_c;
    /*
     * A cache for the first set index of the bitmap.
     * We cache this particular thing is that we often times need FFS calls very
     * much in code, and caching to speed it up is worth it.
     */
    DT_size first_set;
    /*
     * The semantic max cap the user has set. No operations will be performed
     * beyond this cap.
     */
    DT_size bit_cap;

    // The cap of the number of words allocated.
    DT_size word_cap;
    // The words that will store the bits.
    DT_Bitword *words;
};

#define INVARIANT_EXPR(bmp)                                                    \
    ((bmp) != DT_null && (bmp)->words != DT_null &&                            \
     (bmp)->bit_cap <= DT_BITMAP_MAX_BIT_CAP &&                                \
     (bmp)->set_c <= (bmp)->bit_cap &&                                         \
     WORD_I((bmp)->bit_cap) == (bmp)->word_cap - 1)
#define ASSERT_INVARIANT_EXPR(bmp)                                             \
    DIAG_ASSERT_MSG(INVARIANT_EXPR(bmp),                                       \
                    "The given bitmap is either DT_null, or is corrupted.")

/**
 * Recomputes the first set index for the given bitmap, updating the cached
 * value.
 *
 * @param bmp: The bitmap to update the first set index of.
 * @param start: There are certain conditions where we are confirmed the first
 * set to be beyond or equal to start index. So we take in that for easier
 * computation.
 */
static DT_void BitmapCalcFirstSet(DT_Bitmap *bmp, DT_size start);

static DT_void BitmapCalcFirstSet(DT_Bitmap *bmp, DT_size start) {
    ASSERT_INVARIANT_EXPR(bmp);

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
    DT_size i = (start != PRP_INVALID_INDEX)          ? WORD_I(start)
                : bmp->first_set != PRP_INVALID_INDEX ? WORD_I(bmp->first_set)
                                                      : 0;
    for (; i < bmp->word_cap; i++) {
        DT_Bitword word = bmp->words[i];
        if (!word) {
            continue;
        }
        bmp->first_set = DT_BitwordFFS(word) + (i * BITWORD_BITS);
        return;
    }
    /*
     * This is just a "just in case" line because according to my
     intuition it
     * will never execute ever.
     */
    bmp->first_set = PRP_INVALID_INDEX;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapGetLastErrCode(DT_void) {
    return last_err_code;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateUnchecked(DT_size bit_cap) {
    DIAG_ASSERT(bit_cap > 0 && bit_cap <= DT_BITMAP_MAX_BIT_CAP);

    DT_Bitmap *bmp = malloc(sizeof(DT_Bitmap));
    if (!bmp) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    bmp->word_cap = WORD_I(bit_cap) + 1;
    bmp->words = calloc(1, sizeof(DT_Bitword) * bmp->word_cap);
    if (!bmp->words) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        free(bmp);
        return DT_null;
    }
    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    bmp->bit_cap = bit_cap;

    return bmp;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateChecked(DT_size bit_cap) {
    if (!bit_cap || bit_cap > DT_BITMAP_MAX_BIT_CAP) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_BitmapCreateUnchecked(bit_cap);
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL
DT_BitmapCloneUnchecked(const DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    DT_Bitmap *cpy = DT_BitmapCreateUnchecked(bmp->bit_cap);
    if (!cpy) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    cpy->set_c = bmp->set_c;
    cpy->first_set = bmp->first_set;
    memcpy(cpy->words, bmp->words, sizeof(DT_Bitword) * cpy->word_cap);

    return cpy;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCloneChecked(const DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_BitmapCloneUnchecked(bmp);
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapDeleteUnchecked(DT_Bitmap **pBmp) {
    DIAG_ASSERT(pBmp != DT_null);
    DIAG_ASSERT(*pBmp != DT_null && (*pBmp)->words != DT_null);

    DT_Bitmap *bmp = *pBmp;

    free(bmp->words);

#if !defined(PRP_NDEBUG)
    bmp->words = DT_null;
    bmp->bit_cap = bmp->word_cap = bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
#endif

    free(bmp);
    *pBmp = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapDeleteChecked(DT_Bitmap **pBmp) {
    if (!pBmp || !(*pBmp) || !(*pBmp)->words) {
        return PRP_ERR_INV_ARG;
    }

    DT_BitmapDeleteUnchecked(pBmp);

    return PRP_OK;
}

PRP_FN_API const DT_Bitword *PRP_FN_CALL DT_BitmapRawUnchecked(
    const DT_Bitmap *bmp, DT_size *pWord_cap, DT_size *pBit_cap) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(pWord_cap != DT_null);
    DIAG_ASSERT(pBit_cap != DT_null);

    *pWord_cap = bmp->word_cap;
    *pBit_cap = bmp->bit_cap;

    return bmp->words;
}

PRP_FN_API const DT_Bitword *PRP_FN_CALL DT_BitmapRawChecked(
    const DT_Bitmap *bmp, DT_size *pWord_cap, DT_size *pBit_cap) {
    if (!INVARIANT_EXPR(bmp) || !pWord_cap || !pBit_cap) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    *pWord_cap = bmp->word_cap;
    *pBit_cap = bmp->bit_cap;

    return bmp->words;
}

PRP_FN_API DT_size PRP_FN_CALL
DT_BitmapSetCountUnchecked(const DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return bmp->set_c;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCountChecked(const DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return bmp->set_c;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFSUnchecked(const DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return bmp->first_set;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFSChecked(const DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_INDEX;
    }

    return bmp->first_set;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCapUnchecked(const DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return bmp->bit_cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCapChecked(const DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return bmp->bit_cap;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapSetUnchecked(DT_Bitmap *bmp,
                                                     DT_size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    DT_size word_i = WORD_I(i);
    DT_Bitword mask = BIT_MASK(i);
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapSetChecked(DT_Bitmap *bmp,
                                                      DT_size i) {
    if (!INVARIANT_EXPR(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    DT_BitmapSetUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapClrUnchecked(DT_Bitmap *bmp,
                                                     DT_size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    DT_size word_i = WORD_I(i);
    DT_Bitword mask = BIT_MASK(i);
    if (bmp->words[word_i] & mask) {
        bmp->words[word_i] &= ~mask;
        bmp->set_c--;
        if (i == bmp->first_set) {
            // Recomputing fist set if we just cleared it.
            BitmapCalcFirstSet(bmp, i + 1);
        }
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapClrChecked(DT_Bitmap *bmp,
                                                      DT_size i) {
    if (!INVARIANT_EXPR(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    DT_BitmapClrUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapToggleUnchecked(DT_Bitmap *bmp,
                                                        DT_size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    DT_size word_i = WORD_I(i);
    DT_Bitword mask = BIT_MASK(i);
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapToggleChecked(DT_Bitmap *bmp,
                                                         DT_size i) {
    if (!INVARIANT_EXPR(bmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    DT_BitmapToggleUnchecked(bmp, i);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsSetUnchecked(const DT_Bitmap *bmp,
                                                       DT_size i) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(i < bmp->bit_cap);

    return ((bmp->words[WORD_I(i)] & BIT_MASK(i)) != 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetChecked(const DT_Bitmap *bmp,
                                                        DT_size i,
                                                        DT_bool *pRslt) {
    if (!INVARIANT_EXPR(bmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    *pRslt = DT_BitmapIsSetUnchecked(bmp, i);

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
        if (!INVARIANT_EXPR((bmp)) || (i) >= (j)) {                            \
            return PRP_ERR_INV_ARG;                                            \
        }                                                                      \
        if ((i) >= (bmp)->bit_cap || j > (bmp)->bit_cap) {                     \
            return PRP_ERR_OOB;                                                \
        }                                                                      \
    } while (0)

#define MAKE_SAME_WORD_MASK(mask, i, last)                                     \
    do {                                                                       \
        mask = ((DT_Bitword)~0 << BIT_I(i));                                   \
        /* This prevents UB edge case where (<< 64) is undefined. */           \
        if (BIT_I(last) < 63) {                                                \
            mask &= ~((DT_Bitword)~0 << (BIT_I(last) + 1));                    \
        }                                                                      \
    } while (0);

#define MAKE_PARTIAL_FIRST_WORD_MASK(mask, i)                                  \
    do {                                                                       \
        mask = ((DT_Bitword)(~0) << BIT_I(i));                                 \
    } while (0)

// This ternary prevents UB edge case where (<< 64) is undefined.
#define MAKE_PARTIAL_LAST_WORD_MASK(mask, last)                                \
    do {                                                                       \
        mask =                                                                 \
            (BIT_I(last) == 63) ? (DT_Bitword)~0 : (BIT_MASK(last + 1) - 1);   \
    } while (0)

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapSetRangeUnchecked(DT_Bitmap *bmp,
                                                          DT_size i,
                                                          DT_size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        bmp->set_c +=
            (DT_BitwordPopCnt(mask) - DT_BitwordPopCnt(bmp->words[wi] & mask));
        bmp->words[wi] |= mask;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        bmp->set_c +=
            DT_BitwordPopCnt(mask) - DT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] |= mask;

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        bmp->set_c +=
            DT_BitwordPopCnt(mask) - DT_BitwordPopCnt(bmp->words[wj] & mask);
        bmp->words[wj] |= mask;

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            bmp->set_c += BITWORD_BITS - DT_BitwordPopCnt(bmp->words[wi]);
            bmp->words[wi] = (DT_Bitword)~0;
        }
    }

    if (bmp->first_set == PRP_INVALID_INDEX || i < bmp->first_set) {
        bmp->first_set = i;
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapSetRangeChecked(DT_Bitmap *bmp,
                                                           DT_size i,
                                                           DT_size j) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_BitmapSetRangeUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapClrRangeUnchecked(DT_Bitmap *bmp,
                                                          DT_size i,
                                                          DT_size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    if (!bmp->set_c) {
        return;
    }

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        bmp->set_c -= DT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] &= ~mask;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        bmp->set_c -= DT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] &= ~mask;

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        bmp->set_c -= DT_BitwordPopCnt(bmp->words[wj] & mask);
        bmp->words[wj] &= ~mask;

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            bmp->set_c -= DT_BitwordPopCnt(bmp->words[wi]);
            bmp->words[wi] = 0;
        }
    }

    if (bmp->first_set >= i && bmp->first_set < j) {
        BitmapCalcFirstSet(bmp, j);
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapClrRangeChecked(DT_Bitmap *bmp,
                                                           DT_size i,
                                                           DT_size j) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_BitmapClrRangeUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapToggleRangeUnchecked(DT_Bitmap *bmp,
                                                             DT_size i,
                                                             DT_size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        bmp->set_c -= DT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] ^= mask;
        bmp->set_c += DT_BitwordPopCnt(bmp->words[wi] & mask);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        bmp->set_c -= DT_BitwordPopCnt(bmp->words[wi] & mask);
        bmp->words[wi] ^= mask;
        bmp->set_c += DT_BitwordPopCnt(bmp->words[wi] & mask);

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        bmp->set_c -= DT_BitwordPopCnt(bmp->words[wj] & mask);
        bmp->words[wj] ^= mask;
        bmp->set_c += DT_BitwordPopCnt(bmp->words[wj] & mask);

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            bmp->set_c -= DT_BitwordPopCnt(bmp->words[wi]);
            bmp->words[wi] = ~bmp->words[wi];
            bmp->set_c += DT_BitwordPopCnt(bmp->words[wi]);
        }
    }

    BitmapCalcFirstSet(bmp, i);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapToggleRangeChecked(DT_Bitmap *bmp,
                                                              DT_size i,
                                                              DT_size j) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_BitmapToggleRangeUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSetRangeAnyUnchecked(const DT_Bitmap *bmp, DT_size i, DT_size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        return ((bmp->words[wi] & mask) != 0);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((bmp->words[wi] & mask) != 0) {
            return DT_true;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((bmp->words[wj] & mask) != 0) {
            return DT_true;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (bmp->words[wi]) {
                return DT_true;
            }
        }
    }

    return DT_false;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetRangeAnyChecked(
    const DT_Bitmap *bmp, DT_size i, DT_size j, DT_bool *pRslt) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_BitmapIsSetRangeAnyUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL
DT_BitmapIsSetRangeAllUnchecked(const DT_Bitmap *bmp, DT_size i, DT_size j) {
    ASSERT_RANGE_OPS_VALIDITY(bmp, i, j);

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        return ((bmp->words[wi] & mask) == mask);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((bmp->words[wi] & mask) != mask) {
            return DT_false;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((bmp->words[wj] & mask) != mask) {
            return DT_false;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (bmp->words[wi] != (DT_Bitword)~0) {
                return DT_false;
            }
        }
    }

    return DT_true;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsSetRangeAllChecked(
    const DT_Bitmap *bmp, DT_size i, DT_size j, DT_bool *pRslt) {
    CHECK_RANGE_OPS_VALIDITY(bmp, i, j);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_BitmapIsSetRangeAllUnchecked(bmp, i, j);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsEmptyUnchecked(const DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return (bmp->set_c == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsEmptyChecked(const DT_Bitmap *bmp,
                                                          DT_bool *pRslt) {
    if (!INVARIANT_EXPR(bmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = (bmp->set_c == 0);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapIsFullUnchecked(const DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    return (bmp->set_c == bmp->bit_cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapIsFullChecked(const DT_Bitmap *bmp,
                                                         DT_bool *pRslt) {
    if (!INVARIANT_EXPR(bmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = (bmp->set_c == bmp->bit_cap);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapNotUnchecked(DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    bmp->first_set = PRP_INVALID_INDEX;
    for (DT_size i = 0; i < bmp->word_cap; i++) {
        bmp->words[i] = ~(bmp->words[i]);
        // Resetting the fs_pos to new conditions.
        if (bmp->first_set == PRP_INVALID_INDEX && bmp->words[i]) {
            bmp->first_set = DT_BitwordFFS(bmp->words[i]) + (i * BITWORD_BITS);
        }
    }

    bmp->set_c = bmp->bit_cap - bmp->set_c;

    // Clearing the bits over bit_cap that were also set to 1 by not operation.
    DT_size r = bmp->bit_cap & (BITWORD_BITS - 1);
    DT_Bitword mask =
        ~((DT_Bitword)0) >> ((BITWORD_BITS - r) & (BITWORD_BITS - 1));
    bmp->words[bmp->word_cap - 1] &= mask;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapNotChecked(DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    DT_BitmapNotUnchecked(bmp);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapAndUnchecked(DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    bmp1->set_c = 0;
    bmp1->first_set = PRP_INVALID_INDEX;
    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_cap; i++) {
        bmp1->words[i] &= bmp2->words[i];

        DT_size pc = DT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (bmp1->first_set == PRP_INVALID_INDEX && pc) {
            bmp1->first_set =
                DT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
    for (DT_size i = min_cap; i < bmp1->word_cap; i++) {
        bmp1->words[i] = 0;
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapAndChecked(DT_Bitmap *bmp1,
                                                      const DT_Bitmap *bmp2) {
    if (!INVARIANT_EXPR(bmp1) || !INVARIANT_EXPR(bmp2)) {
        return PRP_ERR_INV_ARG;
    }

    DT_BitmapAndUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapOrUnchecked(DT_Bitmap *bmp1,
                                                    const DT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    bmp1->set_c = 0;
    bmp1->first_set = PRP_INVALID_INDEX;
    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_cap; i++) {
        bmp1->words[i] |= bmp2->words[i];

        DT_size pc = DT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (bmp1->first_set == PRP_INVALID_INDEX && pc) {
            bmp1->first_set =
                DT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
    for (DT_size i = min_cap; i < bmp1->word_cap; i++) {
        // Still updating set_c and fs_pos since in OR they can increase.
        DT_size pc = DT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;
        // Resetting the fs_pos to new conditions.
        if (bmp1->first_set == PRP_INVALID_INDEX && pc) {
            bmp1->first_set =
                DT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapOrChecked(DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2) {
    if (!INVARIANT_EXPR(bmp1) || !INVARIANT_EXPR(bmp2)) {
        return PRP_ERR_INV_ARG;
    }

    DT_BitmapOrUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapHasAllUnchecked(const DT_Bitmap *bmp1,
                                                        const DT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    DT_size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_words; i++) {
        if ((bmp1->words[i] & bmp2->words[i]) != bmp2->words[i]) {
            return DT_false;
        }
    }
    for (DT_size i = min_words; i < bmp2->word_cap; i++) {
        if (bmp2->words[i]) {
            return DT_false;
        }
    }

    return DT_true;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapHasAllChecked(const DT_Bitmap *bmp1,
                                                         const DT_Bitmap *bmp2,
                                                         DT_bool *pRslt) {
    if (!INVARIANT_EXPR(bmp1) || !INVARIANT_EXPR(bmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_BitmapHasAllUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapHasAnyUnchecked(const DT_Bitmap *bmp1,
                                                        const DT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    DT_size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_words; i++) {
        if ((bmp1->words[i] & bmp2->words[i])) {
            return DT_true;
        }
    }

    // Not checking beyond min_words as in this case they are irrelevant.
    return DT_false;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapHasAnyChecked(const DT_Bitmap *bmp1,
                                                         const DT_Bitmap *bmp2,
                                                         DT_bool *pRslt) {
    if (!INVARIANT_EXPR(bmp1) || !INVARIANT_EXPR(bmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_BitmapHasAnyUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BitmapCmpUnchecked(const DT_Bitmap *bmp1,
                                                     const DT_Bitmap *bmp2) {
    ASSERT_INVARIANT_EXPR(bmp1);
    ASSERT_INVARIANT_EXPR(bmp2);

    if (bmp1->set_c != bmp2->set_c || bmp1->first_set != bmp2->first_set) {
        return DT_false;
    }

    DT_size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    DT_size max_words = PRP_MAX(bmp1->word_cap, bmp2->word_cap);
    if (memcmp(bmp1->words, bmp2->words, sizeof(DT_Bitword) * min_words) != 0) {
        return DT_false;
    }
    // This will not run for edge cases since loops.
    const DT_Bitmap *mx_bmp = (bmp1->word_cap == max_words) ? bmp1 : bmp2;
    for (DT_size i = min_words; i < max_words; i++) {
        if (mx_bmp->words[i]) {
            return DT_false;
        }
    }

    return DT_true;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapCmpChecked(const DT_Bitmap *bmp1,
                                                      const DT_Bitmap *bmp2,
                                                      DT_bool *pRslt) {
    if (!INVARIANT_EXPR(bmp1) || !INVARIANT_EXPR(bmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_BitmapCmpUnchecked(bmp1, bmp2);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BitmapResetUnchecked(DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    memset(bmp->words, 0, sizeof(DT_Bitword) * bmp->word_cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapResetChecked(DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    DT_BitmapResetUnchecked(bmp);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapShrinkFitUnchecked(DT_Bitmap *bmp) {
    ASSERT_INVARIANT_EXPR(bmp);

    if (bmp->set_c) {
        DT_size i = bmp->word_cap;
        // Finding last i that has a bit on. Till then shrink will happen.
        for (; i-- > 0 && !bmp->words[i];)
            ;
        return DT_BitmapChangeSizeUnchecked(bmp, (i + 1) * BITWORD_BITS);
    } else {
        return DT_BitmapChangeSizeUnchecked(bmp, DT_BITMAP_DEFAULT_BIT_CAP);
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BitmapShrinkFitChecked(DT_Bitmap *bmp) {
    if (!INVARIANT_EXPR(bmp)) {
        return PRP_ERR_INV_ARG;
    }

    return DT_BitmapShrinkFitUnchecked(bmp);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_BitmapChangeSizeUnchecked(DT_Bitmap *bmp, DT_size new_bit_cap) {
    ASSERT_INVARIANT_EXPR(bmp);
    DIAG_ASSERT(new_bit_cap > 0 && new_bit_cap <= DT_BITMAP_MAX_BIT_CAP);

    DT_size new_word_i = WORD_I(new_bit_cap);
    DT_size new_word_cap = new_word_i + 1;
    if (bmp->word_cap == new_word_cap) {
        bmp->bit_cap = new_bit_cap;
        return PRP_OK;
    }

    DT_size set_c_neg = 0;
    if (new_word_cap < bmp->word_cap) {
        // Clearing the bits over bit_cap that were also set to 1 by not
        // operation.
        DT_size r = bmp->bit_cap & (BITWORD_BITS - 1);
        DT_Bitword mask =
            ~((DT_Bitword)0) >> ((BITWORD_BITS - r) & (BITWORD_BITS - 1));
        // This calcs the partial word set_c reduction count on cao red.
        set_c_neg += DT_BitwordPopCnt(bmp->words[new_word_cap - 1] & mask);
        for (DT_size i = new_word_cap; i < bmp->word_cap; i++) {
            set_c_neg += DT_BitwordPopCnt(bmp->words[i]);
        }
    }

    DT_Bitword *words = realloc(bmp->words, sizeof(DT_Bitword) * new_word_cap);
    if (!words) {
        return PRP_ERR_OOM;
    }

    if (new_word_cap > bmp->word_cap) {
        memset(&(words[bmp->word_cap]), 0,
               sizeof(DT_Bitword) * (new_word_cap - bmp->word_cap));
    }
    bmp->words = words;
    bmp->word_cap = new_word_cap;
    bmp->bit_cap = new_bit_cap;
    bmp->set_c -= set_c_neg;
    if (bmp->first_set >= bmp->bit_cap) {
        /*
         * Bcuz if first_set index is weeded out in the size change the set_c is
         * 0 and we can just skip the function calls entirely.
         */
        bmp->first_set = PRP_INVALID_INDEX;
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_BitmapChangeSizeChecked(DT_Bitmap *bmp, DT_size new_bit_cap) {
    if (!INVARIANT_EXPR(bmp) || !new_bit_cap ||
        new_bit_cap > DT_BITMAP_MAX_BIT_CAP) {
        return PRP_ERR_INV_ARG;
    }

    return DT_BitmapChangeSizeUnchecked(bmp, new_bit_cap);
}
