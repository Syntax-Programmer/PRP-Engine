#include "Hm.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

// If 67% of the layout is filled, it is grown.
#define LOAD_FACTOR (0.67)
// Sentinel for a free index in the layout.
#define EMPTY_I ((DT_size)(-1))
// Sentinel for a currently free, previously occupied index in the layout.
#define DEAD_I ((DT_size)(-2))

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

typedef struct {
    DT_void *key;
    DT_void *val;
    // To avoid recomputation each time we lookup.
    DT_u64 hash;
} Elem;

struct _Hm {
    /*
     * Stores the indices of entries mapped to physical hashed locations.
     * This step saves about 67% percent of mem and improves cache locality, by
     * only storing an index rather than the key-val-hash trio.
     */
    DT_size *layout;
    DT_size layout_cap;
    /*
     * Stores the key-val pairs in a tightly packed array separate from the hm
     * layout. This also promotes iterability over every key/val, since values
     * are in an array.
     */
    Elem *elems;
    DT_size elem_cap;
    DT_size elem_len;
    /*
     * User defined hash function so that the hashmap can be a general hashmap
     * that can store even things like structs and compound types.
     */
    DT_u64 (*hash_fn)(const DT_void *key);
    /*
     * How will the key be compared to other keys during probing to determine if
     * it is a unique key or a duplicate one.
     *
     * This can also allow for some interesting key structure, where only one
     * field in the struct like key ot be uniques while other fields may be
     * duplicate.
     */
    DT_bool (*key_cmp_cb)(const DT_void *k1, const DT_void *k2);
    // Frees the memory of the key during the entry/hashmap deletion.
    PRP_Result (*key_del_cb)(DT_void *key);
    // Frees the memory of the val during the entry/hashmap deletion.
    PRP_Result (*val_del_cb)(DT_void *val);
};

#define MAX_LAYOUT_CAP (DT_SIZE_MAX / sizeof(DT_size))
#define MAX_ELEM_CAP (DT_SIZE_MAX / sizeof(Elem))

#define INVARIANT_EXPR(hm)                                                     \
    ((hm) != DT_null && (hm)->layout != DT_null && (hm)->elems != DT_null &&   \
     (hm)->layout_cap > 0 && (hm)->elem_cap > 0 &&                             \
     (hm)->layout_cap <= MAX_LAYOUT_CAP &&                                     \
     ((hm)->layout_cap & ((hm)->layout_cap - 1)) == 0 &&                       \
     (hm)->elem_cap <= MAX_ELEM_CAP && (hm)->elem_len <= (hm)->elem_cap &&     \
     (hm)->elem_len <= (hm)->layout_cap && (hm)->hash_fn != DT_null &&         \
     (hm)->key_cmp_cb != DT_null && (hm)->key_del_cb != DT_null &&             \
     (hm)->val_del_cb != DT_null)
#define ASSERT_INVARIANT_EXPR(hm)                                              \
    DIAG_ASSERT_MSG(INVARIANT_EXPR(hm),                                        \
                    "The given hashmap is either DT_null, or is corrupted.")

/**
 * Grows the elem array of the hashmap safely.
 *
 * @param hm: The hashmap to grow the elems array of.
 *
 * @return PRP_ERR_RES_EXHAUSTED if the cap reaches the maximum, PRP_ERR_OOM if
 * the reallocation fails, otherwise PRP_OK;
 */
static PRP_Result GrowHmElems(DT_Hm *hm);
/**
 * Grows the layout array of the hashmap safely, purges all the dead slots and
 * reorders the array to match the new capacity.
 *
 * @param hm: the hashmap to grow the layout array of.
 *
 * @note: If this fails its no issue there is still cap in the hashmap, just
 * that we don't return anything since whatever it returns is ignored based on
 * the assumption that there is still free space in the hashmap.
 */
static DT_void GrowHmLayout(DT_Hm *hm);
/**
 * Fetchs the layout and elem index of the given key.
 *
 * @param hm: The hm, the given key supposedly resides in.
 * @param key: The key to find the indices for.
 * @param pLayout_i: The pointer to the layout index to store the result.
 * @param pElem_i: The pointer to the elem index to store the result.
 *
 * @return PRP_ERR_OOB if the key doesn't exist in the hashmap, otherwise
 * zzPRP_OK.
 */
static PRP_Result FetchLayoutElemI(const DT_Hm *hm, const DT_void *key,
                                   DT_size *pLayout_i, DT_size *pElem_i);

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmGetLastErrCode(DT_void) {
    return last_err_code;
}

PRP_FN_API DT_Hm *PRP_FN_CALL DT_HmCreateUnchecked(
    DT_u64 (*hash_fn)(const DT_void *key),
    DT_bool (*key_cmp_cb)(const DT_void *k1, const DT_void *k2),
    PRP_Result (*key_del_cb)(DT_void *key),
    PRP_Result (*val_del_cb)(DT_void *val)) {
    DIAG_ASSERT(hash_fn != DT_null);
    DIAG_ASSERT(key_cmp_cb != DT_null);
    DIAG_ASSERT(key_del_cb != DT_null);
    DIAG_ASSERT(val_del_cb != DT_null);

    DT_Hm *hm = calloc(1, sizeof(DT_Hm));
    if (!hm) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    hm->layout = malloc(sizeof(DT_size) * INIT_LAYOUT_CAP);
    if (!hm->layout) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        free(hm);
        return DT_null;
    }
    hm->elems = malloc(sizeof(Elem) * INIT_ELEM_CAP);
    if (!hm->elems) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        free(hm->layout);
        free(hm);
        return DT_null;
    }
    hm->hash_fn = hash_fn;
    hm->key_cmp_cb = key_cmp_cb;
    hm->key_del_cb = key_del_cb;
    hm->val_del_cb = val_del_cb;
    hm->elem_cap = INIT_ELEM_CAP;
    hm->elem_len = 0;
    hm->layout_cap = INIT_LAYOUT_CAP;
    // using 0XFF since memset works per byte and it performs correctly.
    memset(hm->layout, LAYOUT_EMPTYING_MASK, sizeof(DT_size) * INIT_LAYOUT_CAP);

    return hm;
}

PRP_FN_API DT_Hm *PRP_FN_CALL
DT_HmCreateChecked(DT_u64 (*hash_fn)(const DT_void *key),
                   DT_bool (*key_cmp_cb)(const DT_void *k1, const DT_void *k2),
                   PRP_Result (*key_del_cb)(DT_void *key),
                   PRP_Result (*val_del_cb)(DT_void *val)) {
    if (!hash_fn || !key_cmp_cb || !key_del_cb || !val_del_cb) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_HmCreateUnchecked(hash_fn, key_cmp_cb, key_del_cb, val_del_cb);
}

PRP_FN_API DT_void PRP_FN_CALL DT_HmDeleteUnchecked(DT_Hm **pHm) {
    DIAG_ASSERT(pHm != DT_null);
    DIAG_ASSERT(*pHm != DT_null);
    DIAG_ASSERT((*pHm)->layout != DT_null && (*pHm)->elems != DT_null);

    DT_Hm *hm = *pHm;

    free(hm->layout);
    for (DT_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        hm->key_del_cb(elem.key);
        if (elem.val) {
            hm->val_del_cb(elem.val);
        }
    }
    free(hm->elems);

#if !defined(PRP_NDEBUG)
    hm->layout = DT_null;
    hm->elems = DT_null;
    hm->elem_cap = hm->layout_cap = hm->elem_len = 0;
    hm->hash_fn = DT_null;
    hm->key_cmp_cb = DT_null;
    hm->key_del_cb = hm->val_del_cb = DT_null;
#endif

    free(hm);
    *pHm = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmDeleteChecked(DT_Hm **pHm) {
    if (!pHm || !(*pHm) || !(*pHm)->layout || !(*pHm)->elems) {
        return PRP_ERR_INV_ARG;
    }

    DT_HmDeleteUnchecked(pHm);

    return PRP_OK;
}

static PRP_Result GrowHmElems(DT_Hm *hm) {
    ASSERT_INVARIANT_EXPR(hm);

    if (hm->elem_cap == MAX_ELEM_CAP) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    DT_size new_cap = hm->elem_cap * 2;
    if (new_cap > MAX_ELEM_CAP) {
        new_cap = MAX_ELEM_CAP;
    }
    Elem *elems = realloc(hm->elems, sizeof(Elem) * new_cap);
    if (!elems) {
        return PRP_ERR_OOM;
    }
    hm->elems = elems;
    hm->elem_cap = new_cap;

    return PRP_OK;
}

static DT_void GrowHmLayout(DT_Hm *hm) {
    ASSERT_INVARIANT_EXPR(hm);

    if (hm->layout_cap == MAX_LAYOUT_CAP) {
        return;
    }
    DT_size new_cap = hm->layout_cap * 2;
    if (new_cap > MAX_LAYOUT_CAP) {
        new_cap = MAX_LAYOUT_CAP;
    }
    DT_size *layout = realloc(hm->layout, sizeof(DT_size) * new_cap);
    if (!layout) {
        return;
    }
    hm->layout = layout;
    hm->layout_cap = new_cap;

    memset(hm->layout, LAYOUT_EMPTYING_MASK, sizeof(DT_size) * hm->layout_cap);
    // Rehashing and deleting all the dead slots.
    DT_u64 mask = hm->layout_cap - 1;
    for (DT_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        DT_u64 perturb = elem.hash, j = perturb & mask;
        while (hm->layout[j] != EMPTY_I) {
            PROBE(j, perturb, mask);
        }
        hm->layout[j] = i;
    }

    return;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmAddUnchecked(DT_Hm *hm, DT_void *key,
                                                    DT_void *val) {
    ASSERT_INVARIANT_EXPR(hm);
    DIAG_ASSERT(key != DT_null);

    if (hm->elem_len == hm->elem_cap) {
        PRP_Result code = GrowHmElems(hm);
        if (code != PRP_OK) {
            return code;
        }
    }
    if (hm->elem_len >= hm->layout_cap * LOAD_FACTOR) {
        // This doesn't fail, as there is still space in layout.
        GrowHmLayout(hm);
    }
    if (hm->elem_len == hm->layout_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    DT_u64 mask = hm->layout_cap - 1;
    DT_u64 hash = hm->hash_fn(key);
    DT_u64 perturb = hash;
    DT_u64 i = perturb & mask, j = EMPTY_I;
    while (hm->layout[i] != EMPTY_I) {
        if (hm->layout[i] == DEAD_I) {
            // Marking dead index for reuse. But searching fwd for key match
            // check.
            j = i;
        } else {
            DT_size elem_i = hm->layout[i];
            if (!hm->key_cmp_cb(key, hm->elems[elem_i].key)) {
                PROBE(i, perturb, mask);
                continue;
            }
            if (hm->elems[elem_i].val) {
                hm->val_del_cb(hm->elems[elem_i].val);
            }
            hm->elems[elem_i].val = val;
            return PRP_OK;
        }
        PROBE(i, perturb, mask);
    }
    // At this point, memory is guaranteed.
    hm->layout[(j != EMPTY_I) ? j : i] = hm->elem_len;
    hm->elems[hm->elem_len++] = (Elem){.key = key, .val = val, .hash = hash};

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmAddChecked(DT_Hm *hm, DT_void *key,
                                                  DT_void *val) {
    if (!INVARIANT_EXPR(hm) || !key) {
        return PRP_ERR_INV_ARG;
    }

    return DT_HmAddUnchecked(hm, key, val);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_HmGetUnchecked(const DT_Hm *hm,
                                                  DT_void *key) {
    ASSERT_INVARIANT_EXPR(hm);
    DIAG_ASSERT(key != DT_null);

    DT_u64 mask = hm->layout_cap - 1;
    DT_u64 hash = hm->hash_fn(key);
    DT_u64 perturb = hash;
    DT_u64 i = perturb & mask;
    while (hm->layout[i] != EMPTY_I) {
        if (hm->layout[i] != DEAD_I) {
            // Checking all non dead i for key match.
            DT_size elem_i = hm->layout[i];
            if (hm->key_cmp_cb(key, hm->elems[elem_i].key)) {
                return hm->elems[elem_i].val;
            }
        }
        PROBE(i, perturb, mask);
    }

    SET_LAST_ERR_CODE(PRP_ERR_NOT_FOUND);
    return DT_null;
}

PRP_FN_API DT_void *PRP_FN_CALL DT_HmGetChecked(const DT_Hm *hm, DT_void *key) {
    if (!INVARIANT_EXPR(hm) || !key) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_HmGetUnchecked(hm, key);
}

static PRP_Result FetchLayoutElemI(const DT_Hm *hm, const DT_void *key,
                                   DT_size *pLayout_i, DT_size *pElem_i) {
    ASSERT_INVARIANT_EXPR(hm);
    DIAG_ASSERT(key != DT_null);
    DIAG_ASSERT(pLayout_i != DT_null);
    DIAG_ASSERT(pElem_i != DT_null);

    *pLayout_i = *pElem_i = PRP_INVALID_INDEX;

    DT_u64 mask = hm->layout_cap - 1;
    DT_u64 hash = hm->hash_fn(key);
    DT_u64 perturb = hash;
    DT_u64 i = perturb & mask;
    while (hm->layout[i] != EMPTY_I) {
        if (hm->layout[i] != DEAD_I) {
            DT_u64 elem_i = hm->layout[i];
            if (hm->key_cmp_cb(key, hm->elems[elem_i].key)) {
                *pLayout_i = i;
                *pElem_i = elem_i;
                return PRP_OK;
            }
        }
        PROBE(i, perturb, mask);
    }

    return PRP_ERR_NOT_FOUND;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmDelElemUnchecked(DT_Hm *hm,
                                                        DT_void *key) {
    ASSERT_INVARIANT_EXPR(hm);
    DIAG_ASSERT(key != DT_null);

    DT_size key_layout_i, key_elem_i;
    PRP_Result code = FetchLayoutElemI(hm, key, &key_layout_i, &key_elem_i);
    if (code != PRP_OK) {
        return code;
    }
    DT_size last_layout_i, last_elem_i;
    // This shouldn't really fail ever.
    code = FetchLayoutElemI(hm, hm->elems[hm->elem_len - 1].key, &last_layout_i,
                            &last_elem_i);
    DIAG_VERIFY(code == PRP_OK);

    Elem to_del = hm->elems[key_elem_i];
    hm->key_del_cb(to_del.key);
    if (to_del.val) {
        hm->val_del_cb(to_del.val);
    }

    hm->elems[key_elem_i] = hm->elems[last_elem_i];
    hm->elem_len--;
    hm->layout[last_layout_i] = key_elem_i;
    hm->layout[key_layout_i] = DEAD_I;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmDelElemChecked(DT_Hm *hm, DT_void *key) {
    if (!INVARIANT_EXPR(hm) || !key) {
        return PRP_ERR_INV_ARG;
    }

    return DT_HmDelElemUnchecked(hm, key);
}

PRP_FN_API DT_size PRP_FN_CALL DT_HmLenUnchecked(const DT_Hm *hm) {
    ASSERT_INVARIANT_EXPR(hm);

    return hm->elem_len;
}

PRP_FN_API DT_size PRP_FN_CALL DT_HmLenChecked(const DT_Hm *hm) {
    if (!INVARIANT_EXPR(hm)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return hm->elem_len;
}

PRP_FN_API DT_size PRP_FN_CALL DT_HmMaxCap(DT_void) { return MAX_ELEM_CAP; }

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmForEachUnchecked(
    DT_Hm *hm, PRP_Result (*cb)(DT_void *key, DT_void *val, DT_void *user_data),
    DT_void *user_data) {
    ASSERT_INVARIANT_EXPR(hm);
    DIAG_ASSERT(cb != DT_null);

    for (DT_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        PRP_Result code = cb(elem.key, elem.val, user_data);
        if (code != PRP_OK) {
            return code;
        }
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmForEachChecked(
    DT_Hm *hm, PRP_Result (*cb)(DT_void *key, DT_void *val, DT_void *user_data),
    DT_void *user_data) {
    if (!INVARIANT_EXPR(hm) || !cb) {
        return PRP_ERR_INV_ARG;
    }

    return DT_HmForEachUnchecked(hm, cb, user_data);
}

PRP_FN_API DT_void PRP_FN_CALL DT_HmResetUnchecked(DT_Hm *hm) {
    ASSERT_INVARIANT_EXPR(hm);

    // Setting all to empty indices as memset works per byte.
    memset(hm->layout, LAYOUT_EMPTYING_MASK, sizeof(DT_size) * hm->layout_cap);
    for (DT_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        hm->key_del_cb(elem.key);
        if (elem.val) {
            hm->val_del_cb(elem.val);
        }
    }
#if !defined(PRP_NDEBUG)
    memset(hm->elems, 0, hm->elem_len * sizeof(Elem));
#endif
    hm->elem_len = 0;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_HmResetChecked(DT_Hm *hm) {
    if (!INVARIANT_EXPR(hm)) {
        return PRP_ERR_INV_ARG;
    }

    DT_HmResetUnchecked(hm);

    return PRP_OK;
}
