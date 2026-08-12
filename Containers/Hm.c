#include "Hm.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

/* ----  STD HASH FUNCS ---- */

PRP_U64 CONT_HmHashStr(const void *pStr_key) {
    const PRP_Char8 *pKey = pStr_key;
    const PRP_U64 FNV1A64_OFFSET_BASIS = 14695981039346656037ULL;
    const PRP_U64 FNV1A64_PRIME = 1099511628211ULL;

    PRP_U64 hash = FNV1A64_OFFSET_BASIS;

    while (*pKey) {
        hash ^= (PRP_U8)*pKey++;
        hash *= FNV1A64_PRIME;
    }

    return hash;
}

PRP_U64 CONT_HmHashSplitMix64(const void *pU64_key) {
    PRP_U64 x = *(const PRP_U64 *)pU64_key;

    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;

    return x;
}

/* ----  HASHMAP ---- */

// If 67% of the layout is filled, it is grown.
#define LOAD_FACTOR (0.67)
// Sentinel for a free index in the layout.
#define EMPTY_I ((PRP_Size)(-1))
// Sentinel for a currently free, previously occupied index in the layout.
#define DEAD_I ((PRP_Size)(-2))

// These helps in finding new indices when hash collision occurs.
#define PERTURB_CONST (5)
#define PERTURB_SHIFT (5)
#define PROBE(i, perturb, mask)                                                \
    do {                                                                       \
        i = (PERTURB_CONST * i + 1 + perturb) & mask;                          \
        perturb >>= PERTURB_SHIFT;                                             \
    } while (0)

#define INIT_LAYOUT_CAP (16)
#define LAYOUT_EMPTYING_MASK (0XFF)

#define INIT_ELEM_CAP (8)

typedef struct Elem {
    void *pKey;
    void *pVal;
    // To avoid recomputation each time we lookup.
    PRP_U64 hash;
} Elem;

struct CONT_Hm {
    /*
     * Stores the indices of entries mapped to physical hashed locations.
     * This step saves about 67% percent of mem and improves cache locality, by
     * only storing an index rather than the pKey-pVal-hash trio.
     */
    PRP_Size *pLayout;
    PRP_Size layout_cap;
    /*
     * Stores the pKey-pVal pairs in a tightly packed array separate from the
     * pHm layout. This also promotes iterability over every pKey/pVal, since
     * values are in an array.
     */
    Elem *pElems;
    PRP_Size elem_cap;
    PRP_Size elem_len;
    /*
     * User defined hash function so that the hashmap can be a general hashmap
     * that can store even things like structs and compound types.
     */
    PRP_U64 (*pHash_fn)(const void *pKey);
    /*
     * How will the pKey be compared to other keys during probing to determine
     * if it is a unique pKey or a duplicate one.
     *
     * This can also allow for some interesting pKey structure, where only one
     * field in the struct like pKey ot be uniques while other fields may be
     * duplicate.
     */
    PRP_Bool (*pKey_cmp_cb)(const void *pKey1, const void *pKey2);
    // Frees the memory of the pKey during the entry/hashmap deletion.
    PRP_Result (*pKey_del_cb)(void *pKey);
    // Frees the memory of the pVal during the entry/hashmap deletion.
    PRP_Result (*pVal_del_cb)(void *pVal);
};

#define MAX_LAYOUT_CAP (PRP_SIZE_MAX / sizeof(PRP_Size))
#define MAX_ELEM_CAP (PRP_SIZE_MAX / sizeof(Elem))

#define ASSERT_INVARIANT_EXPR(pHm)                                             \
    PRP_DIAG_ASSERT_MSG(CONT_HmIsValid(pHm), "The given pHm is invalid.")

/**
 * Grows the elem array of the hashmap safely.
 *
 * @param pHm The hashmap to grow the elems array of.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result GrowHmElems(CONT_Hm *pHm);
/**
 * Grows the layout array of the hashmap safely, purges all the dead slots and
 * reorders the array to match the new capacity.
 *
 * @param pHm the hashmap to grow the layout array of.
 *
 * @note Imp:
 * - If this fails its no issue there is still cap in the hashmap, just that we
 * don't return anything since whatever it returns is ignored based on the
 * assumption that there is still free space in the hashmap.
 */
static void GrowHmLayout(CONT_Hm *pHm);
/**
 * Fetchs the layout and elem index of the given pKey.
 *
 * @param pHm       The pHm, the given pKey supposedly resides in.
 * @param pKey      The pKey to find the indices for.
 * @param pLayout_i The pointer to the layout index to store the result.
 * @param pElem_i   The pointer to the elem index to store the result.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if the pKey doesn't exist in hashmap.
 */
static PRP_Result FetchLayoutElemI(const CONT_Hm *pHm, const void *pKey,
                                   PRP_Size *pLayout_i, PRP_Size *pElem_i);

PRP_API PRP_Bool PRP_CALL CONT_HmIsValid(const CONT_Hm *pHm) {
    return (pHm != NULL && pHm->pLayout != NULL && pHm->pElems != NULL &&
            pHm->layout_cap > 0 && pHm->elem_cap > 0 &&
            pHm->layout_cap <= MAX_LAYOUT_CAP &&
            (pHm->layout_cap & (pHm->layout_cap - 1)) == 0 &&
            pHm->elem_cap <= MAX_ELEM_CAP && pHm->elem_len <= pHm->elem_cap &&
            pHm->elem_len <= pHm->layout_cap && pHm->pHash_fn != NULL &&
            pHm->pKey_cmp_cb != NULL);
}

PRP_API PRP_Result PRP_CALL CONT_HmCreateUnchecked(
    PRP_U64 (*pHash_fn)(const void *pKey),
    PRP_Bool (*pKey_cmp_cb)(const void *pKey1, const void *pKey2),
    PRP_Result (*pKey_del_cb)(void *pKey),
    PRP_Result (*pVal_del_cb)(void *pVal), CONT_Hm **ppHm) {
    PRP_DIAG_ASSERT(pHash_fn != NULL);
    PRP_DIAG_ASSERT(pKey_cmp_cb != NULL);
    PRP_DIAG_ASSERT(ppHm != NULL);

    *ppHm = NULL;
    CONT_Hm *pHm = calloc(1, sizeof(CONT_Hm));
    if (!pHm) {
        return PRP_ERR_OOM;
    }
    pHm->pLayout = malloc(sizeof(PRP_Size) * INIT_LAYOUT_CAP);
    if (!pHm->pLayout) {
        free(pHm);
        return PRP_ERR_OOM;
    }
    pHm->pElems = malloc(sizeof(Elem) * INIT_ELEM_CAP);
    if (!pHm->pElems) {
        free(pHm->pLayout);
        free(pHm);
        return PRP_ERR_OOM;
    }
    pHm->pHash_fn = pHash_fn;
    pHm->pKey_cmp_cb = pKey_cmp_cb;
    pHm->pKey_del_cb = pKey_del_cb;
    pHm->pVal_del_cb = pVal_del_cb;
    pHm->elem_cap = INIT_ELEM_CAP;
    pHm->elem_len = 0;
    pHm->layout_cap = INIT_LAYOUT_CAP;
    // using 0XFF since memset works per byte and it performs correctly.
    memset(pHm->pLayout, LAYOUT_EMPTYING_MASK,
           sizeof(PRP_Size) * INIT_LAYOUT_CAP);

    *ppHm = pHm;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_HmCreateChecked(
    PRP_U64 (*pHash_fn)(const void *pKey),
    PRP_Bool (*pKey_cmp_cb)(const void *pKey1, const void *pKey2),
    PRP_Result (*pKey_del_cb)(void *pKey),
    PRP_Result (*pVal_del_cb)(void *pVal), CONT_Hm **ppHm) {
    if (!pHash_fn || !pKey_cmp_cb || !pKey_del_cb || !pVal_del_cb || !ppHm) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_HmCreateUnchecked(pHash_fn, pKey_cmp_cb, pKey_del_cb,
                                  pVal_del_cb, ppHm);
}

PRP_API void PRP_CALL CONT_HmDeleteUnchecked(CONT_Hm **ppHm) {
    PRP_DIAG_ASSERT(ppHm != NULL);
    PRP_DIAG_ASSERT_MSG(*ppHm != NULL && (*ppHm)->pLayout != NULL &&
                            (*ppHm)->pElems != NULL,
                        "The given *ppHm is invalid.");

    CONT_Hm *pHm = *ppHm;

    free(pHm->pLayout);
    for (PRP_Size i = 0; i < pHm->elem_len; i++) {
        Elem elem = pHm->pElems[i];
        if (pHm->pKey_del_cb) {
            pHm->pKey_del_cb(elem.pKey);
        }
        if (elem.pVal && pHm->pVal_del_cb) {
            pHm->pVal_del_cb(elem.pVal);
        }
    }
    free(pHm->pElems);

#ifdef PRP_DEBUG_MODE
    pHm->pLayout = NULL;
    pHm->pElems = NULL;
    pHm->elem_cap = pHm->layout_cap = pHm->elem_len = 0;
    pHm->pHash_fn = NULL;
    pHm->pKey_cmp_cb = NULL;
    pHm->pKey_del_cb = pHm->pVal_del_cb = NULL;
#endif

    free(pHm);
    *ppHm = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_HmDeleteChecked(CONT_Hm **ppHm) {
    if (!ppHm || !(*ppHm) || !(*ppHm)->pLayout || !(*ppHm)->pElems) {
        return PRP_ERR_INV_ARG;
    }

    CONT_HmDeleteUnchecked(ppHm);

    return PRP_OK;
}

static PRP_Result GrowHmElems(CONT_Hm *pHm) {
    if (pHm->elem_cap == MAX_ELEM_CAP) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_cap = pHm->elem_cap * 2;
    if (new_cap > MAX_ELEM_CAP) {
        new_cap = MAX_ELEM_CAP;
    }
    Elem *pElems = realloc(pHm->pElems, sizeof(Elem) * new_cap);
    if (!pElems) {
        return PRP_ERR_OOM;
    }
    pHm->pElems = pElems;
    pHm->elem_cap = new_cap;

    return PRP_OK;
}

static void GrowHmLayout(CONT_Hm *pHm) {
    if (pHm->layout_cap == MAX_LAYOUT_CAP) {
        return;
    }
    PRP_Size new_cap = pHm->layout_cap * 2;
    if (new_cap > MAX_LAYOUT_CAP) {
        new_cap = MAX_LAYOUT_CAP;
    }
    PRP_Size *pLayout = realloc(pHm->pLayout, sizeof(PRP_Size) * new_cap);
    if (!pLayout) {
        return;
    }
    pHm->pLayout = pLayout;
    pHm->layout_cap = new_cap;

    memset(pHm->pLayout, LAYOUT_EMPTYING_MASK,
           sizeof(PRP_Size) * pHm->layout_cap);
    // Rehashing and deleting all the dead slots.
    PRP_U64 mask = pHm->layout_cap - 1;
    for (PRP_Size i = 0; i < pHm->elem_len; i++) {
        Elem elem = pHm->pElems[i];
        PRP_U64 perturb = elem.hash, j = perturb & mask;
        while (pHm->pLayout[j] != EMPTY_I) {
            PROBE(j, perturb, mask);
        }
        pHm->pLayout[j] = i;
    }

    return;
}

PRP_API PRP_Result PRP_CALL CONT_HmAddUnchecked(CONT_Hm *pHm, void *pKey,
                                                void *pVal,
                                                PRP_Bool fail_on_duplicate) {
    ASSERT_INVARIANT_EXPR(pHm);
    PRP_DIAG_ASSERT(pKey != NULL);

    if (pHm->elem_len == pHm->elem_cap) {
        PRP_Result code = GrowHmElems(pHm);
        if (code != PRP_OK) {
            return code;
        }
    }
    if (pHm->elem_len >= (PRP_Size)((PRP_F64)pHm->layout_cap * LOAD_FACTOR)) {
        // This doesn't fail, as there is still space in layout.
        GrowHmLayout(pHm);
    }
    if (pHm->elem_len == pHm->layout_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    PRP_U64 mask = pHm->layout_cap - 1;
    PRP_U64 hash = pHm->pHash_fn(pKey);
    PRP_U64 perturb = hash;
    PRP_U64 i = perturb & mask, j = EMPTY_I;
    while (pHm->pLayout[i] != EMPTY_I) {
        if (pHm->pLayout[i] == DEAD_I) {
            // Marking dead index for reuse. But searching fwd for pKey match
            j = i;
        } else {
            PRP_Size elem_i = pHm->pLayout[i];
            if (!pHm->pKey_cmp_cb(pKey, pHm->pElems[elem_i].pKey)) {
                PROBE(i, perturb, mask);
                continue;
            }
            if (fail_on_duplicate) {
                return PRP_ERR_ALREADY_EXISTS;
            }
            if (pHm->pElems[elem_i].pVal && pHm->pVal_del_cb) {
                pHm->pVal_del_cb(pHm->pElems[elem_i].pVal);
            }
            pHm->pElems[elem_i].pVal = pVal;
            return PRP_OK;
        }
        PROBE(i, perturb, mask);
    }
    // At this point, memory is guaranteed.
    pHm->pLayout[(j != EMPTY_I) ? j : i] = pHm->elem_len;
    pHm->pElems[pHm->elem_len++] =
        (Elem){.pKey = pKey, .pVal = pVal, .hash = hash};

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_HmAddChecked(CONT_Hm *pHm, void *pKey,
                                              void *pVal,
                                              PRP_Bool fail_on_duplicate) {
    if (!CONT_HmIsValid(pHm) || !pKey) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_HmAddUnchecked(pHm, pKey, pVal, fail_on_duplicate);
}

PRP_API PRP_Result PRP_CALL CONT_HmGetUnchecked(const CONT_Hm *pHm, void *pKey,
                                                void **pVal) {
    ASSERT_INVARIANT_EXPR(pHm);
    PRP_DIAG_ASSERT(pKey != NULL);
    PRP_DIAG_ASSERT(pVal != NULL);

    PRP_U64 mask = pHm->layout_cap - 1;
    PRP_U64 hash = pHm->pHash_fn(pKey);
    PRP_U64 perturb = hash;
    PRP_U64 i = perturb & mask;
    while (pHm->pLayout[i] != EMPTY_I) {
        if (pHm->pLayout[i] != DEAD_I) {
            // Checking all non dead i for pKey match.
            PRP_Size elem_i = pHm->pLayout[i];
            if (pHm->pKey_cmp_cb(pKey, pHm->pElems[elem_i].pKey)) {
                *pVal = pHm->pElems[elem_i].pVal;
                return PRP_OK;
            }
        }
        PROBE(i, perturb, mask);
    }

    return PRP_ERR_NOT_FOUND;
}

PRP_API PRP_Result PRP_CALL CONT_HmGetChecked(const CONT_Hm *pHm, void *pKey,
                                              void **pVal) {
    if (!CONT_HmIsValid(pHm) || !pKey || !pVal) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_HmGetUnchecked(pHm, pKey, pVal);
}

static PRP_Result FetchLayoutElemI(const CONT_Hm *pHm, const void *pKey,
                                   PRP_Size *pLayout_i, PRP_Size *pElem_i) {
    *pLayout_i = *pElem_i = PRP_INVALID_INDEX;

    PRP_U64 mask = pHm->layout_cap - 1;
    PRP_U64 hash = pHm->pHash_fn(pKey);
    PRP_U64 perturb = hash;
    PRP_U64 i = perturb & mask;
    while (pHm->pLayout[i] != EMPTY_I) {
        if (pHm->pLayout[i] != DEAD_I) {
            PRP_U64 elem_i = pHm->pLayout[i];
            if (pHm->pKey_cmp_cb(pKey, pHm->pElems[elem_i].pKey)) {
                *pLayout_i = i;
                *pElem_i = elem_i;
                return PRP_OK;
            }
        }
        PROBE(i, perturb, mask);
    }

    return PRP_ERR_NOT_FOUND;
}

PRP_API PRP_Result PRP_CALL CONT_HmDelElemUnchecked(CONT_Hm *pHm, void *pKey) {
    ASSERT_INVARIANT_EXPR(pHm);
    PRP_DIAG_ASSERT(pKey != NULL);

    PRP_Size key_layout_i, key_elem_i;
    PRP_Result code = FetchLayoutElemI(pHm, pKey, &key_layout_i, &key_elem_i);
    if (code != PRP_OK) {
        return code;
    }
    PRP_Size last_layout_i, last_elem_i;
    // This shouldn't really fail ever.
    code = FetchLayoutElemI(pHm, pHm->pElems[pHm->elem_len - 1].pKey,
                            &last_layout_i, &last_elem_i);
    PRP_DIAG_VERIFY(code == PRP_OK);

    Elem to_del = pHm->pElems[key_elem_i];
    if (pHm->pKey_del_cb) {
        pHm->pKey_del_cb(to_del.pKey);
    }
    if (to_del.pVal && pHm->pVal_del_cb) {
        pHm->pVal_del_cb(to_del.pVal);
    }

    pHm->pElems[key_elem_i] = pHm->pElems[last_elem_i];
    pHm->elem_len--;
    pHm->pLayout[last_layout_i] = key_elem_i;
    pHm->pLayout[key_layout_i] = DEAD_I;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_HmDelElemChecked(CONT_Hm *pHm, void *pKey) {
    if (!CONT_HmIsValid(pHm) || !pKey) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_HmDelElemUnchecked(pHm, pKey);
}

PRP_API PRP_Size PRP_CALL CONT_HmLen(const CONT_Hm *pHm) {
    ASSERT_INVARIANT_EXPR(pHm);

    return pHm->elem_len;
}

PRP_API PRP_Size PRP_CALL CONT_HmMaxCap(void) { return MAX_ELEM_CAP; }

PRP_API PRP_Result PRP_CALL CONT_HmForEachUnchecked(
    CONT_Hm *pHm, PRP_Result (*pCb)(void *pKey, void *pVal, void *pUser_data),
    void *pUser_data) {
    ASSERT_INVARIANT_EXPR(pHm);
    PRP_DIAG_ASSERT(pCb != NULL);

    for (PRP_Size i = 0; i < pHm->elem_len; i++) {
        Elem elem = pHm->pElems[i];
        PRP_Result code = pCb(elem.pKey, elem.pVal, pUser_data);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_HmForEachChecked(
    CONT_Hm *pHm, PRP_Result (*pCb)(void *pKey, void *pVal, void *pUser_data),
    void *pUser_data) {
    if (!CONT_HmIsValid(pHm) || !pCb) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_HmForEachUnchecked(pHm, pCb, pUser_data);
}

PRP_API void PRP_CALL CONT_HmResetUnchecked(CONT_Hm *pHm) {
    ASSERT_INVARIANT_EXPR(pHm);

    // Setting all to empty indices as memset works per byte.
    memset(pHm->pLayout, LAYOUT_EMPTYING_MASK,
           sizeof(PRP_Size) * pHm->layout_cap);
    for (PRP_Size i = 0; i < pHm->elem_len; i++) {
        Elem elem = pHm->pElems[i];
        if (pHm->pKey_del_cb) {
            pHm->pKey_del_cb(elem.pKey);
        }
        if (elem.pVal && pHm->pVal_del_cb) {
            pHm->pVal_del_cb(elem.pVal);
        }
    }
#ifdef PRP_DEBUG_MODE
    memset(pHm->pElems, 0, pHm->elem_len * sizeof(Elem));
#endif
    pHm->elem_len = 0;
}

PRP_API PRP_Result PRP_CALL CONT_HmResetChecked(CONT_Hm *pHm) {
    if (!CONT_HmIsValid(pHm)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_HmResetUnchecked(pHm);

    return PRP_OK;
}
