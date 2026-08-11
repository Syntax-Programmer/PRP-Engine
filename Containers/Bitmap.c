#include "Bitmap.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

/* ----  BITWORD UTILS ---- */

PRP_API PRP_Size PRP_CALL CONT_BitwordCTZ(CONT_Bitword word) {
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

PRP_API PRP_Size PRP_CALL CONT_BitwordCLZ(CONT_Bitword word) {
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

PRP_API PRP_Size PRP_CALL CONT_BitwordPopCnt(CONT_Bitword word) {
#ifdef PRP_COMPILER_MSVC
    return (PRP_Size)__popcnt64(word);
#else
    return (PRP_Size)__builtin_popcountll(word);
#endif
}

PRP_API PRP_Size PRP_CALL CONT_BitwordFFS(CONT_Bitword word) {
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

struct CONT_Bitmap {
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
    CONT_Bitword *pWords;
};

#define ASSERT_INVARIANT_EXPR(pBmp)                                            \
    PRP_DIAG_ASSERT_MSG(CONT_BitmapIsValid(pBmp), "The given pBmp is "         \
                                                  "invalid.")

/**
 * Recomputes the first set index for the given bitmap, updating the cached
 * value.
 *
 * @param pBmp  The bitmap to update the first set index of.
 * @param start There are certain conditions where we are confirmed the first
 * set to be beyond or equal to start index. So we take in that for easier
 * computation.
 */
static void BitmapCalcFirstSet(CONT_Bitmap *pBmp, PRP_Size start);

static void BitmapCalcFirstSet(CONT_Bitmap *pBmp, PRP_Size start) {
    if (!pBmp->set_c) {
        pBmp->first_set = PRP_INVALID_INDEX;
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
    PRP_Size i = (start != PRP_INVALID_INDEX) ? WORD_I(start)
                 : pBmp->first_set != PRP_INVALID_INDEX
                     ? WORD_I(pBmp->first_set)
                     : 0;
    for (; i < pBmp->word_cap; i++) {
        CONT_Bitword word = pBmp->pWords[i];
        if (!word) {
            continue;
        }
        pBmp->first_set = CONT_BitwordFFS(word) + (i * BITWORD_BITS);
        return;
    }
    /*
     * This is just a "just in case" line because according to my
     intuition it
     * will never execute ever.
     */
    pBmp->first_set = PRP_INVALID_INDEX;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapIsValid(const CONT_Bitmap *pBmp) {
    return (pBmp != NULL && pBmp->pWords != NULL &&
            pBmp->bit_cap <= CONT_BITMAP_MAX_BIT_CAP &&
            pBmp->set_c <= pBmp->bit_cap &&
            WORD_I(pBmp->bit_cap) == pBmp->word_cap - 1);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapCreateUnchecked(PRP_Size bit_cap,
                                                       CONT_Bitmap **ppBmp) {
    PRP_DIAG_ASSERT_MSG(bit_cap > 0, "The bit_cap of the bitmap must be > 0.");
    PRP_DIAG_ASSERT_MSG(
        bit_cap <= CONT_BITMAP_MAX_BIT_CAP,
        "The bit_cap of the bitmap must be <= CONT_BITMAP_MAX_BIT_CAP.");
    PRP_DIAG_ASSERT(ppBmp != NULL);

    CONT_Bitmap *pBmp = malloc(sizeof(CONT_Bitmap));
    if (!pBmp) {
        return PRP_ERR_OOM;
    }
    pBmp->word_cap = WORD_I(bit_cap) + 1;
    pBmp->pWords = calloc(1, sizeof(CONT_Bitword) * pBmp->word_cap);
    if (!pBmp->pWords) {
        free(pBmp);
        return PRP_ERR_OOM;
    }
    pBmp->set_c = 0;
    pBmp->first_set = PRP_INVALID_INDEX;
    pBmp->bit_cap = bit_cap;

    *ppBmp = pBmp;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapCreateChecked(PRP_Size bit_cap,
                                                     CONT_Bitmap **ppBmp) {
    if (!bit_cap || bit_cap > CONT_BITMAP_MAX_BIT_CAP || !ppBmp) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapCreateUnchecked(bit_cap, ppBmp);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapCloneUnchecked(const CONT_Bitmap *pBmp,
                                                      CONT_Bitmap **ppBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT(ppBmp != NULL);

    PRP_Result code = CONT_BitmapCreateUnchecked(pBmp->bit_cap, ppBmp);
    if (code != PRP_OK) {
        return code;
    }
    CONT_Bitmap *pCpy = *ppBmp;
    pCpy->set_c = pBmp->set_c;
    pCpy->first_set = pBmp->first_set;
    memcpy(pCpy->pWords, pBmp->pWords, sizeof(CONT_Bitword) * pCpy->word_cap);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapCloneChecked(const CONT_Bitmap *pBmp,
                                                    CONT_Bitmap **ppBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapCloneUnchecked(pBmp, ppBmp);
}

PRP_API void PRP_CALL CONT_BitmapDeleteUnchecked(CONT_Bitmap **ppBmp) {
    PRP_DIAG_ASSERT(ppBmp != NULL);
    PRP_DIAG_ASSERT_MSG(*ppBmp != NULL && (*ppBmp)->pWords != NULL,
                        "The given *ppBmp is invalid.");

    CONT_Bitmap *pBmp = *ppBmp;

    free(pBmp->pWords);

#if PRP_DEBUG_MODE
    pBmp->pWords = NULL;
    pBmp->bit_cap = pBmp->word_cap = pBmp->set_c = 0;
    pBmp->first_set = PRP_INVALID_INDEX;
#endif

    free(pBmp);
    *ppBmp = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapDeleteChecked(CONT_Bitmap **ppBmp) {
    if (!ppBmp || !(*ppBmp) || !(*ppBmp)->pWords) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapDeleteUnchecked(ppBmp);

    return PRP_OK;
}

PRP_API const CONT_Bitword *PRP_CALL CONT_BitmapRawUnchecked(
    const CONT_Bitmap *pBmp, PRP_Size *pWord_cap, PRP_Size *pBit_cap) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT(pWord_cap != NULL);
    PRP_DIAG_ASSERT(pBit_cap != NULL);

    *pWord_cap = pBmp->word_cap;
    *pBit_cap = pBmp->bit_cap;

    return pBmp->pWords;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapRawChecked(const CONT_Bitmap *pBmp,
                                                  PRP_Size *pWord_cap,
                                                  PRP_Size *pBit_cap,
                                                  void **pRaw) {
    if (!CONT_BitmapIsValid(pBmp) || !pWord_cap || !pBit_cap || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pWord_cap = pBmp->word_cap;
    *pBit_cap = pBmp->bit_cap;

    *pRaw = pBmp->pWords;

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL CONT_BitmapSetCount(const CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    return pBmp->set_c;
}

PRP_API PRP_Size PRP_CALL CONT_BitmapFFS(const CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    return pBmp->first_set;
}

PRP_API PRP_Size PRP_CALL CONT_BitmapBitCap(const CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    return pBmp->bit_cap;
}

PRP_API PRP_Size PRP_CALL CONT_BitmapBitRankUnchecked(const CONT_Bitmap *pBmp,
                                                      PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT_MSG(i < pBmp->bit_cap, "The bit index i is out of bounds.");

    PRP_Size idx = 0;
    for (PRP_Size j = 0; j < WORD_I(i); j++) {
        idx += CONT_BitwordPopCnt(pBmp->pWords[j]);
    }
    idx += CONT_BitwordPopCnt(pBmp->pWords[WORD_I(i)] & (BIT_MASK(i) - 1));

    return idx;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapBitRankChecked(const CONT_Bitmap *pBmp,
                                                      PRP_Size i,
                                                      PRP_Size *pRank) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    *pRank = CONT_BitmapBitRankUnchecked(pBmp, i);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapSetUnchecked(CONT_Bitmap *pBmp, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT_MSG(i < pBmp->bit_cap, "The bit index i is out of bounds.");

    PRP_Size word_i = WORD_I(i);
    CONT_Bitword mask = BIT_MASK(i);
    if (pBmp->pWords[word_i] & mask) {
        return;
    }
    pBmp->pWords[word_i] |= mask;
    pBmp->set_c++;
    if (i < pBmp->first_set) {
        // New first_set found.
        pBmp->first_set = i;
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapSetChecked(CONT_Bitmap *pBmp,
                                                  PRP_Size i) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    CONT_BitmapSetUnchecked(pBmp, i);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapClrUnchecked(CONT_Bitmap *pBmp, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT_MSG(i < pBmp->bit_cap, "The bit index i is out of bounds.");

    PRP_Size word_i = WORD_I(i);
    CONT_Bitword mask = BIT_MASK(i);
    if (pBmp->pWords[word_i] & mask) {
        pBmp->pWords[word_i] &= ~mask;
        pBmp->set_c--;
        if (i == pBmp->first_set) {
            // Recomputing fist set if we just cleared it.
            BitmapCalcFirstSet(pBmp, i + 1);
        }
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapClrChecked(CONT_Bitmap *pBmp,
                                                  PRP_Size i) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    CONT_BitmapClrUnchecked(pBmp, i);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapToggleUnchecked(CONT_Bitmap *pBmp,
                                                 PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT_MSG(i < pBmp->bit_cap, "The bit index i is out of bounds.");

    PRP_Size word_i = WORD_I(i);
    CONT_Bitword mask = BIT_MASK(i);
    pBmp->pWords[word_i] ^= mask;

    if (pBmp->pWords[word_i] & mask) {
        pBmp->set_c++;
        if (i < pBmp->first_set) {
            // New first_set found.
            pBmp->first_set = i;
        }
    } else {
        pBmp->set_c--;
        if (i == pBmp->first_set) {
            // Recomputing fist set if we just cleared it.
            BitmapCalcFirstSet(pBmp, i + 1);
        }
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapToggleChecked(CONT_Bitmap *pBmp,
                                                     PRP_Size i) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    CONT_BitmapToggleUnchecked(pBmp, i);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapIsSetUnchecked(const CONT_Bitmap *pBmp,
                                                    PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT_MSG(i < pBmp->bit_cap, "The bit index i is out of bounds.");

    return ((pBmp->pWords[WORD_I(i)] & BIT_MASK(i)) != 0);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapIsSetChecked(const CONT_Bitmap *pBmp,
                                                    PRP_Size i,
                                                    PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(pBmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBmp->bit_cap) {
        return PRP_ERR_OOB;
    }

    *pRslt = CONT_BitmapIsSetUnchecked(pBmp, i);

    return PRP_OK;
}

#define ASSERT_RANGE_OPS_VALIDITY(pBmp, i, j)                                  \
    do {                                                                       \
        ASSERT_INVARIANT_EXPR((pBmp));                                         \
        PRP_DIAG_ASSERT_MSG((i) < (j),                                         \
                            "Bit index i must be smaller than bit index j.");  \
        PRP_DIAG_ASSERT_MSG((i) < (pBmp)->bit_cap,                             \
                            "The bit index i is out of bounds.");              \
        PRP_DIAG_ASSERT_MSG((j) <= (pBmp)->bit_cap,                            \
                            "The bit index j is out of bounds.");              \
    } while (0)

#define CHECK_RANGE_OPS_VALIDITY(pBmp, i, j)                                   \
    do {                                                                       \
        if (!CONT_BitmapIsValid((pBmp)) || (i) >= (j)) {                       \
            return PRP_ERR_INV_ARG;                                            \
        }                                                                      \
        if ((i) >= (pBmp)->bit_cap || j > (pBmp)->bit_cap) {                   \
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

PRP_API void PRP_CALL CONT_BitmapSetRangeUnchecked(CONT_Bitmap *pBmp,
                                                   PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(pBmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        pBmp->set_c += (CONT_BitwordPopCnt(mask) -
                        CONT_BitwordPopCnt(pBmp->pWords[wi] & mask));
        pBmp->pWords[wi] |= mask;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        pBmp->set_c += CONT_BitwordPopCnt(mask) -
                       CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);
        pBmp->pWords[wi] |= mask;

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        pBmp->set_c += CONT_BitwordPopCnt(mask) -
                       CONT_BitwordPopCnt(pBmp->pWords[wj] & mask);
        pBmp->pWords[wj] |= mask;

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            pBmp->set_c += BITWORD_BITS - CONT_BitwordPopCnt(pBmp->pWords[wi]);
            pBmp->pWords[wi] = (CONT_Bitword)~0;
        }
    }

    if (pBmp->first_set == PRP_INVALID_INDEX || i < pBmp->first_set) {
        pBmp->first_set = i;
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapSetRangeChecked(CONT_Bitmap *pBmp,
                                                       PRP_Size i, PRP_Size j) {
    CHECK_RANGE_OPS_VALIDITY(pBmp, i, j);

    CONT_BitmapSetRangeUnchecked(pBmp, i, j);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapClrRangeUnchecked(CONT_Bitmap *pBmp,
                                                   PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(pBmp, i, j);

    if (!pBmp->set_c) {
        return;
    }

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);
        pBmp->pWords[wi] &= ~mask;
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);
        pBmp->pWords[wi] &= ~mask;

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wj] & mask);
        pBmp->pWords[wj] &= ~mask;

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wi]);
            pBmp->pWords[wi] = 0;
        }
    }

    if (pBmp->first_set >= i && pBmp->first_set < j) {
        BitmapCalcFirstSet(pBmp, j);
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapClrRangeChecked(CONT_Bitmap *pBmp,
                                                       PRP_Size i, PRP_Size j) {
    CHECK_RANGE_OPS_VALIDITY(pBmp, i, j);

    CONT_BitmapClrRangeUnchecked(pBmp, i, j);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapToggleRangeUnchecked(CONT_Bitmap *pBmp,
                                                      PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(pBmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);
        pBmp->pWords[wi] ^= mask;
        pBmp->set_c += CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);
        pBmp->pWords[wi] ^= mask;
        pBmp->set_c += CONT_BitwordPopCnt(pBmp->pWords[wi] & mask);

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wj] & mask);
        pBmp->pWords[wj] ^= mask;
        pBmp->set_c += CONT_BitwordPopCnt(pBmp->pWords[wj] & mask);

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            pBmp->set_c -= CONT_BitwordPopCnt(pBmp->pWords[wi]);
            pBmp->pWords[wi] = ~pBmp->pWords[wi];
            pBmp->set_c += CONT_BitwordPopCnt(pBmp->pWords[wi]);
        }
    }

    BitmapCalcFirstSet(pBmp, i);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapToggleRangeChecked(CONT_Bitmap *pBmp,
                                                          PRP_Size i,
                                                          PRP_Size j) {
    CHECK_RANGE_OPS_VALIDITY(pBmp, i, j);

    CONT_BitmapToggleRangeUnchecked(pBmp, i, j);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapIsSetRangeAnyUnchecked(
    const CONT_Bitmap *pBmp, PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(pBmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        return ((pBmp->pWords[wi] & mask) != 0);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((pBmp->pWords[wi] & mask) != 0) {
            return PRP_True;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((pBmp->pWords[wj] & mask) != 0) {
            return PRP_True;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (pBmp->pWords[wi]) {
                return PRP_True;
            }
        }
    }

    return PRP_False;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapIsSetRangeAnyChecked(
    const CONT_Bitmap *pBmp, PRP_Size i, PRP_Size j, PRP_Bool *pRslt) {
    CHECK_RANGE_OPS_VALIDITY(pBmp, i, j);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapIsSetRangeAnyUnchecked(pBmp, i, j);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapIsSetRangeAllUnchecked(
    const CONT_Bitmap *pBmp, PRP_Size i, PRP_Size j) {
    ASSERT_RANGE_OPS_VALIDITY(pBmp, i, j);

    PRP_Size last = j - 1;
    PRP_Size wi = WORD_I(i), wj = WORD_I(last);
    CONT_Bitword mask;
    if (wi == wj) {
        MAKE_SAME_WORD_MASK(mask, i, last);
        return ((pBmp->pWords[wi] & mask) == mask);
    } else {
        MAKE_PARTIAL_FIRST_WORD_MASK(mask, i);
        if ((pBmp->pWords[wi] & mask) != mask) {
            return PRP_False;
        }

        MAKE_PARTIAL_LAST_WORD_MASK(mask, last);
        if ((pBmp->pWords[wj] & mask) != mask) {
            return PRP_False;
        }

        // Full middle words.
        // This looks cooler than simple loop.
        for (++wi; wi < wj; wi++) {
            if (pBmp->pWords[wi] != (CONT_Bitword)~0) {
                return PRP_False;
            }
        }
    }

    return PRP_True;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapIsSetRangeAllChecked(
    const CONT_Bitmap *pBmp, PRP_Size i, PRP_Size j, PRP_Bool *pRslt) {
    CHECK_RANGE_OPS_VALIDITY(pBmp, i, j);
    if (!pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapIsSetRangeAllUnchecked(pBmp, i, j);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapIsEmptyUnchecked(const CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    return (pBmp->set_c == 0);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapIsEmptyChecked(const CONT_Bitmap *pBmp,
                                                      PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(pBmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = (pBmp->set_c == 0);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapIsFullUnchecked(const CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    return (pBmp->set_c == pBmp->bit_cap);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapIsFullChecked(const CONT_Bitmap *pBmp,
                                                     PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(pBmp) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = (pBmp->set_c == pBmp->bit_cap);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapNotUnchecked(CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    pBmp->first_set = PRP_INVALID_INDEX;
    for (PRP_Size i = 0; i < pBmp->word_cap; i++) {
        pBmp->pWords[i] = ~(pBmp->pWords[i]);
        // Resetting the fs_pos to new conditions.
        if (pBmp->first_set == PRP_INVALID_INDEX && pBmp->pWords[i]) {
            pBmp->first_set =
                CONT_BitwordFFS(pBmp->pWords[i]) + (i * BITWORD_BITS);
        }
    }

    pBmp->set_c = pBmp->bit_cap - pBmp->set_c;

    // Clearing the bits over bit_cap that were also set to 1 by not operation.
    PRP_Size r = pBmp->bit_cap & (BITWORD_BITS - 1);
    CONT_Bitword mask =
        ~((CONT_Bitword)0) >> ((BITWORD_BITS - r) & (BITWORD_BITS - 1));
    pBmp->pWords[pBmp->word_cap - 1] &= mask;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapNotChecked(CONT_Bitmap *pBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapNotUnchecked(pBmp);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapAndUnchecked(CONT_Bitmap *pBmp1,
                                              const CONT_Bitmap *pBmp2) {
    ASSERT_INVARIANT_EXPR(pBmp1);
    ASSERT_INVARIANT_EXPR(pBmp2);

    pBmp1->set_c = 0;
    pBmp1->first_set = PRP_INVALID_INDEX;
    PRP_Size min_cap = PRP_MIN(pBmp1->word_cap, pBmp2->word_cap);
    for (PRP_Size i = 0; i < min_cap; i++) {
        pBmp1->pWords[i] &= pBmp2->pWords[i];

        PRP_Size pc = CONT_BitwordPopCnt(pBmp1->pWords[i]);
        pBmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (pBmp1->first_set == PRP_INVALID_INDEX && pc) {
            pBmp1->first_set =
                CONT_BitwordFFS(pBmp1->pWords[i]) + (i * BITWORD_BITS);
        }
    }
    for (PRP_Size i = min_cap; i < pBmp1->word_cap; i++) {
        pBmp1->pWords[i] = 0;
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapAndChecked(CONT_Bitmap *pBmp1,
                                                  const CONT_Bitmap *pBmp2) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapAndUnchecked(pBmp1, pBmp2);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapOrUnchecked(CONT_Bitmap *pBmp1,
                                             const CONT_Bitmap *pBmp2) {
    ASSERT_INVARIANT_EXPR(pBmp1);
    ASSERT_INVARIANT_EXPR(pBmp2);

    pBmp1->set_c = 0;
    pBmp1->first_set = PRP_INVALID_INDEX;
    PRP_Size min_cap = PRP_MIN(pBmp1->word_cap, pBmp2->word_cap);
    for (PRP_Size i = 0; i < min_cap; i++) {
        pBmp1->pWords[i] |= pBmp2->pWords[i];

        PRP_Size pc = CONT_BitwordPopCnt(pBmp1->pWords[i]);
        pBmp1->set_c += pc;

        // Resetting the fs_pos to new conditions.
        if (pBmp1->first_set == PRP_INVALID_INDEX && pc) {
            pBmp1->first_set =
                CONT_BitwordFFS(pBmp1->pWords[i]) + (i * BITWORD_BITS);
        }
    }
    for (PRP_Size i = min_cap; i < pBmp1->word_cap; i++) {
        // Still updating set_c and fs_pos since in OR they can increase.
        PRP_Size pc = CONT_BitwordPopCnt(pBmp1->pWords[i]);
        pBmp1->set_c += pc;
        // Resetting the fs_pos to new conditions.
        if (pBmp1->first_set == PRP_INVALID_INDEX && pc) {
            pBmp1->first_set =
                CONT_BitwordFFS(pBmp1->pWords[i]) + (i * BITWORD_BITS);
        }
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapOrChecked(CONT_Bitmap *pBmp1,
                                                 const CONT_Bitmap *pBmp2) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapOrUnchecked(pBmp1, pBmp2);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapHasAllUnchecked(const CONT_Bitmap *pBmp1,
                                                     const CONT_Bitmap *pBmp2) {
    ASSERT_INVARIANT_EXPR(pBmp1);
    ASSERT_INVARIANT_EXPR(pBmp2);

    PRP_Size min_words = PRP_MIN(pBmp1->word_cap, pBmp2->word_cap);
    for (PRP_Size i = 0; i < min_words; i++) {
        if ((pBmp1->pWords[i] & pBmp2->pWords[i]) != pBmp2->pWords[i]) {
            return PRP_False;
        }
    }
    for (PRP_Size i = min_words; i < pBmp2->word_cap; i++) {
        if (pBmp2->pWords[i]) {
            return PRP_False;
        }
    }

    return PRP_True;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapHasAllChecked(const CONT_Bitmap *pBmp1,
                                                     const CONT_Bitmap *pBmp2,
                                                     PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapHasAllUnchecked(pBmp1, pBmp2);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapHasAnyUnchecked(const CONT_Bitmap *pBmp1,
                                                     const CONT_Bitmap *pBmp2) {
    ASSERT_INVARIANT_EXPR(pBmp1);
    ASSERT_INVARIANT_EXPR(pBmp2);

    PRP_Size min_words = PRP_MIN(pBmp1->word_cap, pBmp2->word_cap);
    for (PRP_Size i = 0; i < min_words; i++) {
        if ((pBmp1->pWords[i] & pBmp2->pWords[i])) {
            return PRP_True;
        }
    }

    // Not checking beyond min_words as in this case they are irrelevant.
    return PRP_False;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapHasAnyChecked(const CONT_Bitmap *pBmp1,
                                                     const CONT_Bitmap *pBmp2,
                                                     PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapHasAnyUnchecked(pBmp1, pBmp2);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BitmapCmpUnchecked(const CONT_Bitmap *pBmp1,
                                                  const CONT_Bitmap *pBmp2) {
    ASSERT_INVARIANT_EXPR(pBmp1);
    ASSERT_INVARIANT_EXPR(pBmp2);

    if (pBmp1->set_c != pBmp2->set_c || pBmp1->first_set != pBmp2->first_set) {
        return PRP_False;
    }

    PRP_Size min_words = PRP_MIN(pBmp1->word_cap, pBmp2->word_cap);
    PRP_Size max_words = PRP_MAX(pBmp1->word_cap, pBmp2->word_cap);
    if (memcmp(pBmp1->pWords, pBmp2->pWords,
               sizeof(CONT_Bitword) * min_words) != 0) {
        return PRP_False;
    }
    // This will not run for edge cases since loops.
    const CONT_Bitmap *pMx_pBmp =
        (pBmp1->word_cap == max_words) ? pBmp1 : pBmp2;
    for (PRP_Size i = min_words; i < max_words; i++) {
        if (pMx_pBmp->pWords[i]) {
            return PRP_False;
        }
    }

    return PRP_True;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapCmpChecked(const CONT_Bitmap *pBmp1,
                                                  const CONT_Bitmap *pBmp2,
                                                  PRP_Bool *pRslt) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BitmapCmpUnchecked(pBmp1, pBmp2);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BitmapResetUnchecked(CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    pBmp->set_c = 0;
    pBmp->first_set = PRP_INVALID_INDEX;
    memset(pBmp->pWords, 0, sizeof(CONT_Bitword) * pBmp->word_cap);
}

PRP_API PRP_Result PRP_CALL CONT_BitmapResetChecked(CONT_Bitmap *pBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BitmapResetUnchecked(pBmp);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapShrinkFitUnchecked(CONT_Bitmap *pBmp) {
    ASSERT_INVARIANT_EXPR(pBmp);

    if (pBmp->set_c) {
        PRP_Size i = pBmp->word_cap;
        // Finding last i that has a bit on. Till then shrink will happen.
        for (; i-- > 0 && !pBmp->pWords[i];)
            ;
        return CONT_BitmapChangeSizeUnchecked(pBmp, (i + 1) * BITWORD_BITS);
    } else {
        return CONT_BitmapChangeSizeUnchecked(pBmp,
                                              CONT_BITMAP_DEFAULT_BIT_CAP);
    }
}

PRP_API PRP_Result PRP_CALL CONT_BitmapShrinkFitChecked(CONT_Bitmap *pBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapShrinkFitUnchecked(pBmp);
}

PRP_API PRP_Result PRP_CALL
CONT_BitmapChangeSizeUnchecked(CONT_Bitmap *pBmp, PRP_Size new_bit_cap) {
    ASSERT_INVARIANT_EXPR(pBmp);
    PRP_DIAG_ASSERT_MSG(new_bit_cap > 0,
                        "The new_bit_cap of bitmap must be > 0.");
    PRP_DIAG_ASSERT_MSG(
        new_bit_cap <= CONT_BITMAP_MAX_BIT_CAP,
        "The new_bit_cap of bitmap must be <= CONT_BITMAP_MAX_BIT_CAP.");

    PRP_Size new_word_i = WORD_I(new_bit_cap);
    PRP_Size new_word_cap = new_word_i + 1;
    if (pBmp->word_cap == new_word_cap) {
        pBmp->bit_cap = new_bit_cap;
        return PRP_OK;
    }

    PRP_Size set_c_neg = 0;
    if (new_word_cap < pBmp->word_cap) {
        // Clearing the bits over bit_cap that were also set to 1 by not
        // operation.
        PRP_Size r = pBmp->bit_cap & (BITWORD_BITS - 1);
        CONT_Bitword mask =
            ~((CONT_Bitword)0) >> ((BITWORD_BITS - r) & (BITWORD_BITS - 1));
        // This calcs the partial word set_c reduction count on cao red.
        set_c_neg += CONT_BitwordPopCnt(pBmp->pWords[new_word_cap - 1] & mask);
        for (PRP_Size i = new_word_cap; i < pBmp->word_cap; i++) {
            set_c_neg += CONT_BitwordPopCnt(pBmp->pWords[i]);
        }
    }

    CONT_Bitword *pWords =
        realloc(pBmp->pWords, sizeof(CONT_Bitword) * new_word_cap);
    if (!pWords) {
        return PRP_ERR_OOM;
    }

    if (new_word_cap > pBmp->word_cap) {
        memset(&(pWords[pBmp->word_cap]), 0,
               sizeof(CONT_Bitword) * (new_word_cap - pBmp->word_cap));
    }
    pBmp->pWords = pWords;
    pBmp->word_cap = new_word_cap;
    pBmp->bit_cap = new_bit_cap;
    pBmp->set_c -= set_c_neg;
    if (pBmp->first_set >= pBmp->bit_cap) {
        /*
         * Bcuz if first_set index is weeded ppBmp in the size change the set_c
         * is 0 and we can just skip the function calls entirely.
         */
        pBmp->first_set = PRP_INVALID_INDEX;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BitmapChangeSizeChecked(CONT_Bitmap *pBmp,
                                                         PRP_Size new_bit_cap) {
    if (!CONT_BitmapIsValid(pBmp) || !new_bit_cap ||
        new_bit_cap > CONT_BITMAP_MAX_BIT_CAP) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BitmapChangeSizeUnchecked(pBmp, new_bit_cap);
}
