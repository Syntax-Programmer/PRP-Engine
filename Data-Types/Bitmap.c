#include "Bitmap.h"
#include "../Utils/Logger.h"
#include <string.h>

/* ----  BITWORD UTILS ---- */

PRP_FN_API DT_size PRP_FN_CALL DT_BitwordCTZ(DT_Bitword word) {
    if (!word) {
        return PRP_INVALID_INDEX;
    }
#ifdef _MSC_VER
    unsigned long i;
    _BitScanForward64(&i, mask);
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
    return 0; // GCC semantics: 0 means no bits set
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

#define DEFAULT_BIT_CAP (64)

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

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreate(DT_size bit_cap) {
    if (!bit_cap) {
        bit_cap = DEFAULT_BIT_CAP;
    }

    DT_Bitmap *bmp = malloc(sizeof(DT_Bitmap));
    if (!bmp) {
        PRP_LOG_FN_MALLOC_ERROR(bmp);
        return DT_null;
    }
    bmp->word_cap = WORD_I(bit_cap) + 1;
    bmp->words = calloc(1, sizeof(DT_Bitword) * bmp->word_cap);
    if (!bmp->words) {
        free(bmp);
        PRP_LOG_FN_MALLOC_ERROR(bmp->words);
        return DT_null;
    }
    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    bmp->bit_cap = bit_cap;

    return bmp;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateDefault(DT_void) {
    return DT_BitmapCreate(DEFAULT_BIT_CAP);
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapClone(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, DT_null);

    DT_Bitmap *cpy = DT_BitmapCreate(bmp->bit_cap);
    if (!cpy) {
        PRP_LOG_FN_MALLOC_ERROR(cpy);
        return DT_null;
    }
    cpy->set_c = bmp->set_c;
    cpy->first_set = bmp->first_set;
    memcpy(cpy->words, bmp->words, sizeof(DT_Bitword) * cpy->word_cap);

    return cpy;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapDelete(DT_Bitmap **pBmp) {
    PRP_NULL_ARG_CHECK(pBmp, PRP_FN_INV_ARG_ERROR);
    DT_Bitmap *bmp = *pBmp;
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

    if (bmp->words) {
        free(bmp->words);
        bmp->words = DT_null;
    }
    bmp->bit_cap = 0;
    bmp->word_cap = 0;
    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    free(bmp);
    *pBmp = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_Bitword *PRP_FN_CALL DT_BitmapRaw(DT_Bitmap *bmp,
                                                DT_size *pWord_cap,
                                                DT_size *pBit_cap) {
    PRP_NULL_ARG_CHECK(bmp, DT_null);
    PRP_NULL_ARG_CHECK(pWord_cap, DT_null);
    PRP_NULL_ARG_CHECK(pBit_cap, DT_null);

    *pWord_cap = bmp->word_cap;
    *pBit_cap = bmp->bit_cap;

    return bmp->words;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCount(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_INVALID_SIZE);

    return bmp->set_c;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFS(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_INVALID_INDEX);

    return bmp->first_set;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapBitCap(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_INVALID_SIZE);

    return bmp->bit_cap;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapSet(DT_Bitmap *bmp, DT_size i) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    if (i >= bmp->bit_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the bit index: %zu, of bitmap with bit cap: %zu",
            i, bmp->bit_cap);
        return PRP_FN_OOB_ERROR;
    }

    DT_size word_i = WORD_I(i);
    DT_Bitword mask = BIT_MASK(i);
    if (bmp->words[word_i] & mask) {
        return PRP_FN_SUCCESS;
    }
    bmp->words[word_i] |= mask;
    bmp->set_c++;
    if (i < bmp->first_set) {
        // New first_set found.
        bmp->first_set = i;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapClr(DT_Bitmap *bmp, DT_size i) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    if (i >= bmp->bit_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the bit index: %zu, of bitmap with bit cap: %zu",
            i, bmp->bit_cap);
        return PRP_FN_OOB_ERROR;
    }

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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapToggle(DT_Bitmap *bmp, DT_size i) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    if (i >= bmp->bit_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the bit index: %zu, of bitmap with bit cap: %zu",
            i, bmp->bit_cap);
        return PRP_FN_OOB_ERROR;
    }

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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSet(DT_Bitmap *bmp, DT_size i,
                                                 DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);
    if (i >= bmp->bit_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the bit index: %zu, of bitmap with bit cap: %zu",
            i, bmp->bit_cap);
        return PRP_FN_OOB_ERROR;
    }

    *pRslt = ((bmp->words[WORD_I(i)] & BIT_MASK(i)) != 0);

    return PRP_FN_SUCCESS;
}

#define RANGE_OPS_VALIDITY_CHECK(bmp, i, j)                                    \
    do {                                                                       \
        PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);                         \
        if (i >= j) {                                                          \
            PRP_LOG_FN_CODE(                                                   \
                PRP_FN_INV_ARG_ERROR,                                          \
                "i can't be greater than or equal to j for this operation.");  \
            return PRP_FN_INV_ARG_ERROR;                                       \
        }                                                                      \
        if (i >= bmp->bit_cap || j > bmp->bit_cap) {                           \
            PRP_LOG_FN_CODE(                                                   \
                PRP_FN_OOB_ERROR,                                              \
                "Tried accessing the bit index: %zu-%zu, of a bitmap "         \
                "with bit cap: %zu",                                           \
                i, j, bmp->bit_cap);                                           \
            return PRP_FN_OOB_ERROR;                                           \
        }                                                                      \
    } while (0);

#define MAKE_SAME_WORD_MASK(mask, i, last)                                     \
    do {                                                                       \
        mask = ((DT_Bitword)~0 << BIT_I(i));                                   \
        /* This prevents UB edge case where (<< 64) is undefined. */           \
        if (BIT_I(last) < 63) {                                                \
            mask &= ~((DT_Bitword)~0 << (BIT_I(last) + 1));                    \
        }                                                                      \
    } while (0);

#define MAKE_PARTIAL_FIRST_WORD_MASK(mask, i)                                  \
    mask = ((DT_Bitword)(~0) << BIT_I(i));
// This ternary prevents UB edge case where (<< 64) is undefined.
#define MAKE_PARTIAL_LAST_WORD_MASK(mask, last)                                \
    mask = (BIT_I(last) == 63) ? (DT_Bitword)~0 : (BIT_MASK(last + 1) - 1);

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapSetRange(DT_Bitmap *bmp, DT_size i,
                                                    DT_size j) {
    RANGE_OPS_VALIDITY_CHECK(bmp, i, j);

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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapClrRange(DT_Bitmap *bmp, DT_size i,
                                                    DT_size j) {
    RANGE_OPS_VALIDITY_CHECK(bmp, i, j);

    if (!bmp->set_c) {
        return PRP_FN_SUCCESS;
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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapToggleRange(DT_Bitmap *bmp,
                                                       DT_size i, DT_size j) {
    RANGE_OPS_VALIDITY_CHECK(bmp, i, j);

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

    if (bmp->first_set > i) {
        // Since during the toggle the i(clear bit) will be turned on.
        bmp->first_set = i;
    } else if (bmp->first_set == i) {
        BitmapCalcFirstSet(bmp, i + 1);
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSetRangeAny(DT_Bitmap *bmp,
                                                         DT_size i, DT_size j,
                                                         DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);
    RANGE_OPS_VALIDITY_CHECK(bmp, i, j);

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        *pRslt = ((bmp->words[wi] & mask) != 0);
        return PRP_FN_SUCCESS;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((bmp->words[wi] & mask) != 0) {
            goto true_condition;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((bmp->words[wj] & mask) != 0) {
            goto true_condition;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (bmp->words[wi]) {
                goto true_condition;
            }
        }
    }
    *pRslt = DT_false;

    return PRP_FN_SUCCESS;

true_condition:
    *pRslt = DT_true;
    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSetRangeAll(DT_Bitmap *bmp,
                                                         DT_size i, DT_size j,
                                                         DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);
    RANGE_OPS_VALIDITY_CHECK(bmp, i, j);

    DT_size last = j - 1;
    DT_size wi = WORD_I(i), wj = WORD_I(last);
    DT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        *pRslt = ((bmp->words[wi] & mask) == mask);
        return PRP_FN_SUCCESS;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((bmp->words[wi] & mask) != mask) {
            goto false_condition;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((bmp->words[wj] & mask) != mask) {
            goto false_condition;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (bmp->words[wi] != (DT_Bitword)~0) {
                goto false_condition;
            }
        }
    }
    *pRslt = DT_true;

    return PRP_FN_SUCCESS;

false_condition:
    *pRslt = DT_false;
    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsEmpty(DT_Bitmap *bmp,
                                                   DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    *pRslt = (bmp->set_c == 0);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsFull(DT_Bitmap *bmp,
                                                  DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    *pRslt = (bmp->set_c == bmp->bit_cap);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSubset(DT_Bitmap *bmp1,
                                                    DT_Bitmap *bmp2,
                                                    DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_cap; i++) {
        if ((bmp1->words[i] & bmp2->words[i]) != bmp2->words[i]) {
            *pRslt = DT_false;
            return PRP_FN_SUCCESS;
        }
    }
    for (DT_size i = min_cap; i < bmp2->word_cap; i++) {
        if (bmp2->words[i]) {
            *pRslt = DT_false;
            return PRP_FN_SUCCESS;
        }
    }
    *pRslt = DT_true;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapNot(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapAnd(DT_Bitmap *bmp1,
                                               DT_Bitmap *bmp2) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);

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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapOr(DT_Bitmap *bmp1,
                                              DT_Bitmap *bmp2) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);

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

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapHasAll(DT_Bitmap *bmp1,
                                                  DT_Bitmap *bmp2,
                                                  DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    DT_size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_words; i++) {
        if ((bmp1->words[i] & bmp2->words[i]) != bmp2->words[i]) {
            *pRslt = DT_false;
            return PRP_FN_SUCCESS;
        }
    }
    for (DT_size i = min_words; i < bmp2->word_cap; i++) {
        if (bmp2->words[i]) {
            *pRslt = DT_false;
            return PRP_FN_SUCCESS;
        }
    }
    *pRslt = DT_true;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapHasAny(DT_Bitmap *bmp1,
                                                  DT_Bitmap *bmp2,
                                                  DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    DT_size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_words; i++) {
        if ((bmp1->words[i] & bmp2->words[i])) {
            *pRslt = DT_true;
            return PRP_FN_SUCCESS;
        }
    }
    // Not checking beyond min_words as in this case they are irrelevant.
    *pRslt = DT_false;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapCmp(DT_Bitmap *bmp1, DT_Bitmap *bmp2,
                                               DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    if (bmp1->set_c != bmp2->set_c || bmp1->first_set != bmp2->first_set) {
        *pRslt = DT_false;
        return PRP_FN_SUCCESS;
    }

    DT_size min_words = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    DT_size max_words = PRP_MAX(bmp1->word_cap, bmp2->word_cap);
    if (memcmp(bmp1->words, bmp2->words, sizeof(DT_Bitword) * min_words) != 0) {
        *pRslt = DT_false;
        return PRP_FN_SUCCESS;
    }
    // This will not run for edge cases since loops.
    DT_Bitmap *mx_bmp = (bmp1->word_cap == max_words) ? bmp1 : bmp2;
    for (DT_size i = min_words; i < max_words; i++) {
        if (mx_bmp->words[i]) {
            *pRslt = DT_false;
            return PRP_FN_SUCCESS;
        }
    }
    *pRslt = DT_true;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapReset(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

    bmp->set_c = 0;
    bmp->first_set = PRP_INVALID_INDEX;
    memset(bmp->words, 0, sizeof(DT_Bitword) * bmp->word_cap);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapShrinkFit(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

    if (bmp->set_c) {
        DT_size i = bmp->word_cap;
        // Finding last i that has a bit on. Till then shrink will happen.
        for (; i-- > 0 && !bmp->words[i];)
            ;
        return DT_BitmapChangeSize(bmp, (i + 1) * BITWORD_BITS);
    } else {
        return DT_BitmapChangeSize(bmp, DEFAULT_BIT_CAP);
    }
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapChangeSize(DT_Bitmap *bmp,
                                                      DT_size new_bit_cap) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    if (!new_bit_cap) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Cannot change size of the bitmap to 0 bits.");
        return PRP_FN_INV_ARG_ERROR;
    }

    DT_size new_word_i = WORD_I(new_bit_cap);
    DT_size new_word_cap = new_word_i + 1;
    if (bmp->word_cap == new_word_cap) {
        bmp->bit_cap = new_bit_cap;
        return PRP_FN_SUCCESS;
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
        PRP_LOG_FN_MALLOC_ERROR(words);
        return PRP_FN_MALLOC_ERROR;
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

    return PRP_FN_SUCCESS;
}
