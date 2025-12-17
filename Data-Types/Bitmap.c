#include "Bitmap.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _Bitmap {
    // The count of bits currently set.
    DT_size set_c;
    /*
     * A cache for the first set index of the bitmap.
     * We cache this particular thing is that we often times need FFS calls very
     * much in code, and caching to speed it up is worth it.
     *
     * NOTE:
     * This is a 1-based index stored. When we return ffs, we will subtract 1
     * from it to get actual index.
     */
    DT_size fs_pos;

    DT_size word_cap;
    // The words that will store the bits.
    DT_Bitword *words;
};

#define DEFAULT_BIT_CAP (64)

/**
 * Changes the word capacity of the given bitmap to the provided new cap safely.
 *
 * @param bmp: The bitmap to change the cap of.
 * @param new_cap: The new cap of the bitmap to change to.
 *
 * @return PRP_FN_MALLOC_ERROR if the reallocation fails, otherwise
 * PRP_FN_SUCCESS;
 */
static PRP_FnCode BitmapChangeSize(DT_Bitmap *bmp, DT_size new_word_cap);
/**
 * Recomputes the FFS pos for the given bitmap, updating the cached value.
 *
 * @param bmp: The bitmap to update the ffs pos of.
 */
static DT_void BitmapFFSPos(DT_Bitmap *bmp);

static PRP_FnCode BitmapChangeSize(DT_Bitmap *bmp, DT_size new_word_cap) {
    if (bmp->word_cap == new_word_cap) {
        return PRP_FN_SUCCESS;
    }

    DT_size set_c_neg = 0;
    if (new_word_cap < bmp->word_cap) {
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
        // Setting new words to zero.
        memset(&words[bmp->word_cap], 0,
               sizeof(DT_Bitword) * (new_word_cap - bmp->word_cap));
    }
    bmp->words = words;
    bmp->word_cap = new_word_cap;
    bmp->set_c -= set_c_neg;

    return PRP_FN_SUCCESS;
}

static DT_void BitmapFFSPos(DT_Bitmap *bmp) {
    if (!bmp->set_c) {
        bmp->fs_pos = PRP_INVALID_POS;
        return;
    }

    for (DT_size i = 0; i < bmp->word_cap; i++) {
        DT_Bitword word = bmp->words[i];
        if (!word) {
            continue;
        }
        bmp->fs_pos = DT_BitwordFFS(word) + (i * BITWORD_BITS);
        return;
    }
}

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
        return (DT_size)(i + 1);
    return 0; // GCC semantics: 0 means no bits set
#else
    return word ? (DT_size)__builtin_ctzll(word) + 1 : 0;
#endif
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreate(DT_size bit_cap) {
    if (!bit_cap) {
        bit_cap = DEFAULT_BIT_CAP;
    }

    DT_Bitmap *bmp = calloc(1, sizeof(DT_Bitmap));
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
    bmp->fs_pos = PRP_INVALID_POS;

    return bmp;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapCreateDefault(DT_void) {
    return DT_BitmapCreate(DEFAULT_BIT_CAP);
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapClone(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, DT_null);

    DT_Bitmap *cpy = DT_BitmapCreate(bmp->word_cap * BITWORD_BITS);
    if (!cpy) {
        PRP_LOG_FN_MALLOC_ERROR(cpy);
        return DT_null;
    }
    cpy->set_c = bmp->set_c;
    cpy->fs_pos = bmp->fs_pos;
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
    bmp->word_cap = 0;
    bmp->set_c = 0;
    bmp->fs_pos = PRP_INVALID_POS;
    free(bmp);
    *pBmp = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_Bitword *PRP_FN_CALL DT_BitmapRaw(DT_Bitmap *bmp,
                                                DT_size *pWord_cap) {
    PRP_NULL_ARG_CHECK(bmp, DT_null);
    PRP_NULL_ARG_CHECK(pWord_cap, DT_null);

    *pWord_cap = bmp->word_cap;

    return bmp->words;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapSetCount(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_INVALID_SIZE);

    return bmp->set_c;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BitmapFFS(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_INVALID_POS);

    return bmp->fs_pos;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapSet(DT_Bitmap *bmp, DT_size i) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

    DT_size word_i = WORD_I(i);
    if (word_i >= bmp->word_cap &&
        BitmapChangeSize(bmp, word_i + 1) != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot set the bit: %u to the bitmap.", i);
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }

    if (PRP_BIT_IS_SET(bmp->words[word_i], BIT_MASK(i))) {
        return PRP_FN_SUCCESS;
    }
    // Setting the bit we were told to.
    PRP_BIT_SET(bmp->words[word_i], BIT_MASK(i));
    // i + 1 since fs_i is a one based index.
    if (bmp->fs_pos > PRP_I_TO_POS(i)) {
        // If the new set is smaller than curr fs_i, it becomes the new fs_i.
        bmp->fs_pos = PRP_I_TO_POS(i);
    }
    bmp->set_c++;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapClr(DT_Bitmap *bmp, DT_size i) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    DT_size word_i = WORD_I(i);
    if (word_i >= bmp->word_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried to access index: %zu, in a bitmap with bit cap: %zu", i,
            bmp->word_cap * BITWORD_BITS);
        return PRP_FN_OOB_ERROR;
    }

    if (!PRP_BIT_IS_SET(bmp->words[word_i], BIT_MASK(i))) {
        return PRP_FN_SUCCESS;
    }
    // Clearing the bit we were told to.
    PRP_BIT_CLR(bmp->words[word_i], BIT_MASK(i));
    // i + 1 since fs_i is a one based index.
    if (bmp->fs_pos == PRP_I_TO_POS(i)) {
        // If we cleared fs_i, we recompute it.
        BitmapFFSPos(bmp);
    }
    bmp->set_c--;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapToggle(DT_Bitmap *bmp, DT_size i) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    DT_size word_i = WORD_I(i);
    if (word_i >= bmp->word_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried to access index: %zu, in a bitmap with bit cap: %zu", i,
            bmp->word_cap * BITWORD_BITS);
        return PRP_FN_OOB_ERROR;
    }

    PRP_BIT_TOGGLE(bmp->words[word_i], BIT_MASK(i));
    if (PRP_BIT_IS_SET(bmp->words[word_i], BIT_MASK(i))) {
        if (bmp->fs_pos > PRP_I_TO_POS(i)) {
            // If the new set is smaller than curr fs_i, it becomes the new
            // fs_i.
            bmp->fs_pos = PRP_I_TO_POS(i);
        }
        bmp->set_c++;
    } else {
        if (bmp->fs_pos == PRP_I_TO_POS(i)) {
            // If we cleared fs_i, we recompute it.
            BitmapFFSPos(bmp);
        }
        bmp->set_c--;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsSet(DT_Bitmap *bmp, DT_size i,
                                                 DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);
    DT_size word_i = WORD_I(i);
    if (word_i >= bmp->word_cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried to access index: %zu, in a bitmap with bit cap: %zu", i,
            bmp->word_cap * BITWORD_BITS);
        return PRP_FN_OOB_ERROR;
    }

    *pRslt = PRP_BIT_IS_SET(bmp->words[word_i], BIT_MASK(i));

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsEmpty(DT_Bitmap *bmp,
                                                   DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    *pRslt = !bmp->set_c;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapIsFull(DT_Bitmap *bmp,
                                                  DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    *pRslt = (bmp->set_c == (bmp->word_cap * BITWORD_BITS));

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapNot(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

    bmp->fs_pos = PRP_INVALID_POS;
    for (DT_size i = 0; i < bmp->word_cap; i++) {
        bmp->words[i] = ~(bmp->words[i]);
        // Resetting the fs_pos to new conditions.
        if (bmp->fs_pos == PRP_INVALID_POS && bmp->words[i]) {
            bmp->fs_pos = DT_BitwordFFS(bmp->words[i]) + (i * BITWORD_BITS);
        }
    }
    bmp->set_c = (bmp->word_cap * BITWORD_BITS) - bmp->set_c;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapAnd(DT_Bitmap *bmp1,
                                               DT_Bitmap *bmp2) {
    PRP_NULL_ARG_CHECK(bmp1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bmp2, PRP_FN_INV_ARG_ERROR);

    bmp1->set_c = 0;
    bmp1->fs_pos = PRP_INVALID_POS;
    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_cap; i++) {
        bmp1->words[i] &= bmp2->words[i];

        DT_size pc = DT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (bmp1->fs_pos == PRP_INVALID_POS && pc) {
            bmp1->fs_pos = DT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
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
    bmp1->fs_pos = PRP_INVALID_POS;
    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    for (DT_size i = 0; i < min_cap; i++) {
        bmp1->words[i] |= bmp2->words[i];

        DT_size pc = DT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (bmp1->fs_pos == PRP_INVALID_POS && pc) {
            bmp1->fs_pos = DT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }
    for (DT_size i = min_cap; i < bmp1->word_cap; i++) {
        // Still updating set_c and fs_pos since in OR they can increase.
        DT_size pc = DT_BitwordPopCnt(bmp1->words[i]);
        bmp1->set_c += pc;
        // Resetting the fs_pos to new conditions.
        if (bmp1->fs_pos == PRP_INVALID_POS && pc) {
            bmp1->fs_pos = DT_BitwordFFS(bmp1->words[i]) + (i * BITWORD_BITS);
        }
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapNotNew(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, DT_null);

    DT_Bitmap *new_bmp = DT_BitmapCreate(bmp->word_cap * BITWORD_BITS);
    if (!new_bmp) {
        PRP_LOG_FN_MALLOC_ERROR(new_bmp);
        return DT_null;
    }
    for (DT_size i = 0; i < new_bmp->word_cap; i++) {
        new_bmp->words[i] = ~(bmp->words[i]);
        // Resetting the fs_pos to new conditions.
        if (new_bmp->fs_pos == PRP_INVALID_POS && new_bmp->words[i]) {
            new_bmp->fs_pos =
                DT_BitwordFFS(new_bmp->words[i]) + (i * BITWORD_BITS);
        }
    }

    new_bmp->set_c = (bmp->word_cap * BITWORD_BITS) - bmp->set_c;

    return new_bmp;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapAndNew(DT_Bitmap *bmp1,
                                                  DT_Bitmap *bmp2) {
    PRP_NULL_ARG_CHECK(bmp1, DT_null);
    PRP_NULL_ARG_CHECK(bmp2, DT_null);

    DT_size max_cap = PRP_MAX(bmp1->word_cap, bmp2->word_cap);
    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    DT_Bitmap *new_bmp = DT_BitmapCreate(max_cap * BITWORD_BITS);
    if (!new_bmp) {
        PRP_LOG_FN_MALLOC_ERROR(new_bmp);
        return DT_null;
    }
    for (DT_size i = 0; i < min_cap; i++) {
        new_bmp->words[i] = bmp1->words[i] & bmp2->words[i];

        DT_size pc = DT_BitwordPopCnt(new_bmp->words[i]);
        bmp1->set_c += pc;

        if (new_bmp->fs_pos == PRP_INVALID_POS && pc) {
            new_bmp->fs_pos =
                DT_BitwordFFS(new_bmp->words[i]) + (i * BITWORD_BITS);
        }
    }
    // This is not needed as the words are calloced when new created.
    // for (DT_size i = min_cap; i < max_cap; i++) {
    //     new_bmp->words[i] = 0
    // }

    return new_bmp;
}

PRP_FN_API DT_Bitmap *PRP_FN_CALL DT_BitmapOrNew(DT_Bitmap *bmp1,
                                                 DT_Bitmap *bmp2) {
    PRP_NULL_ARG_CHECK(bmp1, DT_null);
    PRP_NULL_ARG_CHECK(bmp2, DT_null);

    DT_size max_cap = PRP_MAX(bmp1->word_cap, bmp2->word_cap);
    DT_size min_cap = PRP_MIN(bmp1->word_cap, bmp2->word_cap);
    DT_Bitmap *new_bmp = DT_BitmapCreate(max_cap * BITWORD_BITS);
    if (!new_bmp) {
        PRP_LOG_FN_MALLOC_ERROR(new_bmp);
        return DT_null;
    }
    for (DT_size i = 0; i < min_cap; i++) {
        new_bmp->words[i] = bmp1->words[i] | bmp2->words[i];

        DT_size pc = DT_BitwordPopCnt(new_bmp->words[i]);
        bmp1->set_c += pc;

        if (new_bmp->fs_pos == PRP_INVALID_POS && pc) {
            new_bmp->fs_pos =
                DT_BitwordFFS(new_bmp->words[i]) + (i * BITWORD_BITS);
        }
    }
    // This will not run for edge cases since loops.
    DT_Bitmap *mx_bmp = (bmp1->word_cap == max_cap) ? bmp1 : bmp2;
    for (DT_size i = min_cap; i < max_cap; i++) {
        new_bmp->words[i] = mx_bmp->words[i];

        DT_size pc = DT_BitwordPopCnt(new_bmp->words[i]);
        bmp1->set_c += pc;

        if (new_bmp->fs_pos == PRP_INVALID_POS && pc) {
            new_bmp->fs_pos =
                DT_BitwordFFS(new_bmp->words[i]) + (i * BITWORD_BITS);
        }
    }

    return new_bmp;
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

    if (bmp1->set_c != bmp2->set_c || bmp1->fs_pos != bmp2->fs_pos) {
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
    bmp->fs_pos = PRP_INVALID_POS;
    memset(bmp->words, 0, sizeof(DT_Bitword) * bmp->word_cap);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BitmapShrinkFit(DT_Bitmap *bmp) {
    PRP_NULL_ARG_CHECK(bmp, PRP_FN_INV_ARG_ERROR);

    DT_size i;
    // Finding last i that has a bit on. Till then shrink will happen.
    for (i = bmp->word_cap; i-- > 0 && !bmp->words[i];)
        ;

    return BitmapChangeSize(bmp, i + 1);
}
