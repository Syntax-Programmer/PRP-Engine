#include "Hm.h"
#include "../Utils/Logger.h"
#include <string.h>

// If 67% of the layout is filled, it is grown.
#define LOAD_FACTOR (0.67)
// Sentinel for a free index in the layout.
#define EMPTY_I ((PRP_size) - 1)
// Sentinel for a currently free, previously occupied index in the layout.
#define DEAD_I ((PRP_size) - 2)

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
    PRP_void *key;
    PRP_void *val;
    // To avoid recomputation each time we lookup.
    PRP_u64 hash;
} Elem;

struct _Hm {
    /*
     * Stores the indices of entries mapped to physical hashed locations.
     * This step saves about 67% percent of mem and improves cache locality, by
     * only storing an index rather than the key-val-hash trio.
     */
    PRP_size *layout;
    PRP_size layout_cap;
    /*
     * Stores the key-val pairs in a tightly packed array separate from the hm
     * layout. This also promotes iterability over every key/val, since values
     * are in an array.
     */
    Elem *elems;
    PRP_size elem_cap;
    PRP_size elem_len;
    /*
     * User defined hash function so that the hashmap can be a general hashmap
     * that can store even things like structs and compound types.
     */
    PRP_u64 (*hash_fn)(const PRP_void *key);
    /*
     * How will the key be compared to other keys during probing to determine if
     * it is a unique key or a duplicate one.
     *
     * This can also allow for some interesting key structure, where only one
     * field in the struct like key ot be uniques while other fields may be
     * duplicate.
     */
    PRP_bool (*key_cmp_cb)(const PRP_void *k1, const PRP_void *k2);
    // Frees the memory of the key during the entry/hashmap deletion.
    PRP_FnCode (*key_del_cb)(PRP_void *key);
    // Frees the memory of the val during the entry/hashmap deletion.
    PRP_FnCode (*val_del_cb)(PRP_void *val);
};

#define HM_VALIDITY_CHECK(hm, ret)                                             \
    do {                                                                       \
        if (!hm) {                                                             \
            PRP_LOG_FN_INV_ARG_ERROR(hm);                                      \
            return ret;                                                        \
        }                                                                      \
    } while (0)

static PRP_FnCode GrowHmElems(DT_Hm *hm);
static PRP_FnCode GrowHmLayout(DT_Hm *hm);

PRP_FN_API DT_Hm *PRP_FN_CALL
DT_HmCreate(PRP_u64 (*hash_fn)(const PRP_void *key),
            PRP_bool (*key_cmp_cb)(const PRP_void *k1, const PRP_void *k2),
            PRP_FnCode (*key_del_cb)(PRP_void *key),
            PRP_FnCode (*val_del_cb)(PRP_void *val)) {
    if (!hash_fn) {
        PRP_LOG_FN_INV_ARG_ERROR(hash_fn);
        return PRP_null;
    }
    if (!key_cmp_cb) {
        PRP_LOG_FN_INV_ARG_ERROR(key_cmp_cb);
        return PRP_null;
    }
    if (!key_del_cb) {
        PRP_LOG_FN_INV_ARG_ERROR(key_del_cb);
        return PRP_null;
    }
    if (!val_del_cb) {
        PRP_LOG_FN_INV_ARG_ERROR(val_del_cb);
        return PRP_null;
    }

    DT_Hm *hm = calloc(1, sizeof(DT_Hm));
    if (!hm) {
        PRP_LOG_FN_MALLOC_ERROR(hm);
        return PRP_null;
    }
    hm->layout = malloc(sizeof(PRP_size) * INIT_LAYOUT_CAP);
    if (!hm->layout) {
        free(hm);
        PRP_LOG_FN_MALLOC_ERROR(hm->layout);
        return PRP_null;
    }
    hm->elems = malloc(sizeof(Elem) * INIT_ELEM_CAP);
    if (!hm->elems) {
        free(hm->layout);
        free(hm);
        PRP_LOG_FN_MALLOC_ERROR(hm->elems);
        return PRP_null;
    }
    hm->hash_fn = hash_fn;
    hm->key_cmp_cb = key_cmp_cb;
    hm->key_del_cb = key_del_cb;
    hm->val_del_cb = val_del_cb;
    hm->elem_cap = INIT_ELEM_CAP;
    hm->elem_len = 0;
    hm->layout_cap = INIT_LAYOUT_CAP;
    // using 0XFF since memset works per byte and it performs correctly.
    memset(hm->layout, LAYOUT_EMPTYING_MASK,
           sizeof(PRP_size) * INIT_LAYOUT_CAP);

    return hm;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmDelete(DT_Hm **pHm) {
    if (!pHm) {
        PRP_LOG_FN_INV_ARG_ERROR(pHm);
        return PRP_FN_INV_ARG_ERROR;
    }
    DT_Hm *hm = *pHm;
    HM_VALIDITY_CHECK(hm, PRP_FN_INV_ARG_ERROR);

    if (hm->layout) {
        free(hm->layout);
        hm->layout = PRP_null;
    }
    if (hm->elems) {
        for (PRP_size i = 0; i < hm->elem_len; i++) {
            Elem elem = hm->elems[i];
            hm->key_del_cb(elem.key);
            if (elem.val) {
                hm->val_del_cb(elem.val);
            }
        }
        free(hm->elems);
        hm->elems = PRP_null;
    }
    hm->elem_cap = hm->layout_cap = hm->elem_len = 0;
    hm->hash_fn = PRP_null;
    hm->key_cmp_cb = PRP_null;
    hm->key_del_cb = hm->val_del_cb = PRP_null;
    free(hm);
    *pHm = PRP_null;

    return PRP_FN_SUCCESS;
}

static PRP_FnCode GrowHmElems(DT_Hm *hm) {
    Elem *elems = realloc(hm->elems, sizeof(Elem) * hm->elem_cap * 2);
    if (!elems) {
        PRP_LOG_FN_MALLOC_ERROR(elems);
        return PRP_FN_MALLOC_ERROR;
    }
    hm->elems = elems;
    hm->elem_cap *= 2;

    return PRP_FN_SUCCESS;
}

static PRP_FnCode GrowHmLayout(DT_Hm *hm) {
    PRP_size *layout =
        realloc(hm->layout, sizeof(PRP_size) * hm->layout_cap * 2);
    if (!layout) {
        PRP_LOG_FN_MALLOC_ERROR(layout);
        return PRP_FN_MALLOC_ERROR;
    }
    hm->layout = layout;
    hm->layout_cap *= 2;

    memset(hm->layout, LAYOUT_EMPTYING_MASK, sizeof(PRP_size) * hm->layout_cap);
    // Rehashing and deleting all the dead slots.
    PRP_u64 mask = hm->layout_cap - 1;
    for (PRP_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        PRP_u64 perturb = elem.hash, j = perturb & mask;
        while (layout[j] != EMPTY_I) {
            PROBE(j, perturb, mask);
        }
        hm->layout[j] = i;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmAdd(DT_Hm *hm, PRP_void *key,
                                           PRP_void *val) {
    HM_VALIDITY_CHECK(hm, PRP_FN_INV_ARG_ERROR);
    if (!key) {
        PRP_LOG_FN_INV_ARG_ERROR(key);
        return PRP_FN_INV_ARG_ERROR;
    }

    if (hm->elem_len == hm->elem_cap && GrowHmElems(hm) != PRP_FN_SUCCESS) {
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }
    if (hm->elem_len >= hm->layout_cap * LOAD_FACTOR) {
        // This doesn't fail, as there is still space in layout.
        GrowHmLayout(hm);
    }
    // Don't check for hm->elem_len == hm->elem_cap as it is handled above.
    if (hm->elem_len == hm->layout_cap) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "DT_Hm is completely filled, prev resize attempt must "
                        "have failed.");
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }

    PRP_u64 mask = hm->layout_cap - 1;
    PRP_u64 hash = hm->hash_fn(key);
    PRP_u64 perturb = hash;
    PRP_u64 i = perturb & mask, j = EMPTY_I;
    while (hm->layout[i] != EMPTY_I) {
        if (hm->layout[i] == DEAD_I) {
            // Marking dead index for reuse. But searching fwd for key match
            // check.
            j = i;
        } else {
            PRP_size elem_i = hm->layout[i];
            if (!hm->key_cmp_cb(key, hm->elems[elem_i].key)) {
                PROBE(i, perturb, mask);
                continue;
            }
            if (hm->elems[elem_i].val) {
                hm->val_del_cb(hm->elems[elem_i].val);
            }
            hm->elems[elem_i].val = val;
            return PRP_FN_SUCCESS;
        }
        PROBE(i, perturb, mask);
    }
    // At this point, memory is guaranteed.
    hm->layout[(j != EMPTY_I) ? j : i] = hm->elem_len;
    hm->elems[hm->elem_len++] = (Elem){.key = key, .val = val, .hash = hash};

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_void *PRP_FN_CALL DT_HmGet(DT_Hm *hm, PRP_void *key) {
    HM_VALIDITY_CHECK(hm, PRP_null);
    if (!key) {
        PRP_LOG_FN_INV_ARG_ERROR(key);
        return PRP_null;
    }

    PRP_u64 mask = hm->layout_cap - 1;
    PRP_u64 hash = hm->hash_fn(key);
    PRP_u64 perturb = hash;
    PRP_u64 i = perturb & mask;
    while (hm->layout[i] != EMPTY_I) {
        if (hm->layout[i] != DEAD_I) {
            // Checking all non dead i for key match.
            PRP_size elem_i = hm->layout[i];
            if (hm->key_cmp_cb(key, hm->elems[elem_i].key)) {
                return hm->elems[elem_i].val;
            }
        }
        PROBE(i, perturb, mask);
    }

    return PRP_null;
}

static PRP_FnCode FetchLayoutElemI(DT_Hm *hm, PRP_void *key,
                                   PRP_size *pLayout_i, PRP_size *pElem_i) {
    *pLayout_i = *pElem_i = PRP_INVALID_INDEX;

    PRP_u64 mask = hm->layout_cap - 1;
    PRP_u64 hash = hm->hash_fn(key);
    PRP_u64 perturb = hash;
    PRP_u64 i = perturb & mask;
    while (hm->layout[i] != EMPTY_I) {
        if (hm->layout[i] != DEAD_I) {
            PRP_u64 elem_i = hm->layout[i];
            if (hm->key_cmp_cb(key, hm->elems[i].key)) {
                *pLayout_i = i;
                *pElem_i = elem_i;
                return PRP_FN_SUCCESS;
            }
        }
        PROBE(i, perturb, mask);
    }

    return PRP_FN_OOB_ERROR;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmDelElem(DT_Hm *hm, PRP_void *key) {
    HM_VALIDITY_CHECK(hm, PRP_FN_INV_ARG_ERROR);
    if (!key) {
        PRP_LOG_FN_INV_ARG_ERROR(key);
        return PRP_FN_INV_ARG_ERROR;
    }

    PRP_size key_layout_i, key_elem_i;
    if (FetchLayoutElemI(hm, key, &key_layout_i, &key_elem_i) !=
        PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(PRP_FN_OOB_ERROR,
                        "The given key to delete doesn't exist in the hashmap");
        return PRP_FN_OOB_ERROR;
    }
    PRP_size last_layout_i, last_elem_i;
    // This shouldn't really fail ever.
    FetchLayoutElemI(hm, hm->elems[hm->elem_len - 1].key, &last_layout_i,
                     &last_elem_i);

    Elem to_del = hm->elems[key_elem_i];
    hm->key_del_cb(to_del.key);
    if (to_del.val) {
        hm->val_del_cb(to_del.val);
    }

    hm->elems[key_elem_i] = hm->elems[last_elem_i];
    hm->elem_len--;
    hm->layout[last_layout_i] = key_elem_i;
    hm->layout[key_layout_i] = DEAD_I;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_size PRP_FN_CALL DT_HmLen(DT_Hm *hm) {
    HM_VALIDITY_CHECK(hm, PRP_INVALID_SIZE);

    return hm->elem_len;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
DT_HmForEach(DT_Hm *hm, PRP_FnCode (*cb)(PRP_void *key, PRP_void *val)) {
    HM_VALIDITY_CHECK(hm, PRP_FN_INV_ARG_ERROR);
    if (!cb) {
        PRP_LOG_FN_INV_ARG_ERROR(cb);
        return PRP_FN_INV_ARG_ERROR;
    }

    for (PRP_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        if (cb(elem.key, elem.val) != PRP_FN_SUCCESS) {
            /*
             * We don't care why the foreach was called to be terminated. There
             * was no error from our side so even after termination it is still
             * a success.
             */
            return PRP_FN_SUCCESS;
        }
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmReset(DT_Hm *hm) {
    HM_VALIDITY_CHECK(hm, PRP_FN_INV_ARG_ERROR);

    // Setting all to empty indices as memset works per byte.
    memset(hm->layout, LAYOUT_EMPTYING_MASK,
           sizeof(PRP_size) * INIT_LAYOUT_CAP);
    for (PRP_size i = 0; i < hm->elem_len; i++) {
        Elem elem = hm->elems[i];
        hm->key_del_cb(elem.key);
        if (elem.val) {
            hm->val_del_cb(elem.val);
        }
    }
    hm->elem_len = 0;

    return PRP_FN_SUCCESS;
}
