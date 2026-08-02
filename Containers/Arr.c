#include "Arr.h"
#include "Diagnostics/Assert.h"
#include <string.h>

struct CONT_Arr {
    PRP_Size cap;
    PRP_Size len;
    PRP_Size memb_size;
    PRP_U8 *mem;
};

#define ASSERT_INVARIANT_EXPR(pArr)                                            \
    DIAG_ASSERT_MSG(CONT_ArrIsValid(pArr),                                     \
                    "The given array is either NULL, or is corrupted.")

/**
 * Centralized std policy for array cap increases.
 *
 * @param curr_cap Current cap of the array.
 * @param max_cap  The max cap of the array.
 *
 * @return New cap of the array.
 *
 * @note:
 * - If returned cap == curr_cap, we have hit the max cap and must not do any
 * allocation.
 */
static PRP_Size CapIncPolicy(PRP_Size curr_cap, PRP_Size max_cap);
/**
 * Safely change size of the array.
 *
 * @param pArr    Array instance.
 * @param new_cap The new cap of the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ArrChangeSize(CONT_Arr *pArr, PRP_Size new_cap);

static PRP_Size CapIncPolicy(PRP_Size curr_cap, PRP_Size max_cap) {
    if (max_cap / 2 >= curr_cap) {
        return curr_cap * 2;
    } else {
        return max_cap;
    }
}

static PRP_Result ArrChangeSize(CONT_Arr *pArr, PRP_Size new_cap) {
    if (pArr->cap == new_cap) {
        return PRP_OK;
    }
    PRP_Size max_cap = CONT_ARR_MAX_CAP(pArr->memb_size);
    if (pArr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    PRP_U8 *mem = realloc(pArr->mem, new_cap * pArr->memb_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    pArr->mem = mem;
    pArr->cap = new_cap;

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_ArrIsValid(const CONT_Arr *pArr) {
    return (pArr != NULL && pArr->mem != NULL && pArr->memb_size > 0 &&
            pArr->cap > 0 && pArr->cap <= CONT_ARR_MAX_CAP(pArr->memb_size) &&
            pArr->len <= pArr->cap);
}

PRP_API PRP_Result PRP_CALL CONT_ArrCreateUnchecked(PRP_Size memb_size,
                                                    PRP_Size cap,
                                                    CONT_Arr **ppArr) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(ppArr != NULL);

    if (cap > CONT_ARR_MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    CONT_Arr *pArr = malloc(sizeof(CONT_Arr));
    if (!pArr) {
        return PRP_ERR_OOM;
    }
    pArr->mem = malloc(memb_size * cap);
    if (!pArr->mem) {
        free(pArr);
        return PRP_ERR_OOM;
    }
    pArr->memb_size = memb_size;
    pArr->cap = cap;
    pArr->len = 0;

    *ppArr = pArr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrCreateChecked(PRP_Size memb_size,
                                                  PRP_Size cap,
                                                  CONT_Arr **ppArr) {
    if (!memb_size || !cap || !ppArr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrCreateUnchecked(memb_size, cap, ppArr);
}

PRP_API PRP_Result PRP_CALL CONT_ArrCloneUnchecked(const CONT_Arr *pArr,
                                                   CONT_Arr **ppArr) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(ppArr != NULL);

    // Unchecked since we checked for invariants above.
    PRP_Result code =
        CONT_ArrCreateUnchecked(pArr->memb_size, pArr->cap, ppArr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Arr *cpy = *ppArr;
    cpy->len = pArr->len;
    memcpy(cpy->mem, pArr->mem, pArr->memb_size * pArr->len);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrCloneChecked(const CONT_Arr *pArr,
                                                 CONT_Arr **ppArr) {
    if (!CONT_ArrIsValid(pArr) || !ppArr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrCloneUnchecked(pArr, ppArr);
}

PRP_API PRP_Result PRP_CALL CONT_ArrCreateWithDataUnchecked(PRP_Size memb_size,
                                                            const void *membs,
                                                            PRP_Size len,
                                                            CONT_Arr **ppArr) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(len > 0);
    DIAG_ASSERT(ppArr != NULL);

    PRP_Result code = CONT_ArrCreateUnchecked(memb_size, len, ppArr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Arr *pArr = *ppArr;
    memcpy(pArr->mem, membs, memb_size * len);
    pArr->len = len;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrCreateWithDataChecked(PRP_Size memb_size,
                                                          const void *membs,
                                                          PRP_Size len,
                                                          CONT_Arr **ppArr) {
    if (!memb_size || !len || !ppArr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrCreateWithDataUnchecked(memb_size, membs, len, ppArr);
}

PRP_API void PRP_CALL CONT_ArrDeleteUnchecked(CONT_Arr **ppArr) {
    DIAG_ASSERT(ppArr != NULL);
    DIAG_ASSERT(*ppArr != NULL && (*ppArr)->mem != NULL);

    CONT_Arr *pArr = *ppArr;

    free(pArr->mem);

#ifdef PRP_DEBUG_MODE
    pArr->mem = NULL;
    pArr->cap = pArr->len = pArr->memb_size = 0;
#endif

    free(pArr);
    *ppArr = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_ArrDeleteChecked(CONT_Arr **ppArr) {
    if (!ppArr || !(*ppArr) || !(*ppArr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ArrDeleteUnchecked(ppArr);

    return PRP_OK;
}

PRP_API const void *PRP_CALL CONT_ArrRawUnchecked(const CONT_Arr *pArr,
                                                  PRP_Size *pLen) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(pLen != NULL);

    *pLen = pArr->len;

    return pArr->mem;
}

PRP_API PRP_Result PRP_CALL CONT_ArrRawChecked(const CONT_Arr *pArr,
                                               PRP_Size *pLen,
                                               const void **pRaw) {
    if (!CONT_ArrIsValid(pArr) || !pLen || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pLen = pArr->len;
    *pRaw = pArr->mem;

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL CONT_ArrLen(const CONT_Arr *pArr) {
    ASSERT_INVARIANT_EXPR(pArr);

    return pArr->len;
}

PRP_API PRP_Size PRP_CALL CONT_ArrCap(const CONT_Arr *pArr) {
    ASSERT_INVARIANT_EXPR(pArr);

    return pArr->cap;
}

PRP_API PRP_Size PRP_CALL CONT_ArrMembSize(const CONT_Arr *pArr) {
    ASSERT_INVARIANT_EXPR(pArr);

    return pArr->memb_size;
}

PRP_API PRP_Size PRP_CALL CONT_ArrMaxCap(const CONT_Arr *pArr) {
    ASSERT_INVARIANT_EXPR(pArr);

    return CONT_ARR_MAX_CAP(pArr->memb_size);
}

PRP_API void *PRP_CALL CONT_ArrGetUnchecked(const CONT_Arr *pArr, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(i < pArr->len);

    return pArr->mem + (i * pArr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_ArrGetChecked(const CONT_Arr *pArr, PRP_Size i,
                                               void **dest) {
    if (!CONT_ArrIsValid(pArr) || !dest) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pArr->len) {
        return PRP_ERR_OOB;
    }

    *dest = CONT_ArrGetUnchecked(pArr, i);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ArrSetUnchecked(CONT_Arr *pArr, PRP_Size i,
                                           const void *pData) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i < pArr->len);

    memcpy(pArr->mem + (i * pArr->memb_size), pData, pArr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_ArrSetChecked(CONT_Arr *pArr, PRP_Size i,
                                               const void *pData) {
    if (!CONT_ArrIsValid(pArr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pArr->len) {
        return PRP_ERR_OOB;
    }

    CONT_ArrSetUnchecked(pArr, i, pData);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrPushUnchecked(CONT_Arr *pArr,
                                                  const void *pData) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(pData != NULL);

    if (pArr->len == pArr->cap) {
        PRP_Size new_cap =
            CapIncPolicy(pArr->cap, CONT_ARR_MAX_CAP(pArr->memb_size));
        if (new_cap == pArr->cap) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Result code = ArrChangeSize(pArr, new_cap);
        if (code != PRP_OK) {
            return code;
        }
    }
    memcpy(pArr->mem + ((pArr->len++) * pArr->memb_size), pData,
           pArr->memb_size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrPushChecked(CONT_Arr *pArr,
                                                const void *pData) {
    if (!CONT_ArrIsValid(pArr) || !pData) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrPushUnchecked(pArr, pData);
}

PRP_API PRP_Result PRP_CALL CONT_ArrReserveUnchecked(CONT_Arr *pArr,
                                                     PRP_Size count) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(count > 0);

    if (pArr->cap - pArr->len >= count) {
        return PRP_OK;
    }

    return ArrChangeSize(pArr, pArr->len + count);
}

PRP_API PRP_Result PRP_CALL CONT_ArrReserveChecked(CONT_Arr *pArr,
                                                   PRP_Size count) {
    if (!CONT_ArrIsValid(pArr) || !count) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrReserveUnchecked(pArr, count);
}

PRP_API PRP_Result PRP_CALL CONT_ArrInsertUnchecked(CONT_Arr *pArr,
                                                    const void *pData,
                                                    PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i <= pArr->len);

    if (pArr->len == pArr->cap) {
        PRP_Size new_cap =
            CapIncPolicy(pArr->cap, CONT_ARR_MAX_CAP(pArr->memb_size));
        if (new_cap == pArr->cap) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Result code = ArrChangeSize(pArr, new_cap);
        if (code != PRP_OK) {
            return code;
        }
    }
    memmove(pArr->mem + ((i + 1) * pArr->memb_size),
            pArr->mem + (i * pArr->memb_size),
            (pArr->len - i) * pArr->memb_size);
    memcpy(pArr->mem + (i * pArr->memb_size), pData, pArr->memb_size);
    pArr->len++;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrInsertChecked(CONT_Arr *pArr,
                                                  const void *pData,
                                                  PRP_Size i) {
    if (!CONT_ArrIsValid(pArr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i > pArr->len) {
        return PRP_ERR_OOB;
    }

    return CONT_ArrInsertUnchecked(pArr, pData, i);
}

PRP_API PRP_Result PRP_CALL CONT_ArrPopUnchecked(CONT_Arr *pArr, void *pDest) {
    ASSERT_INVARIANT_EXPR(pArr);

    if (!pArr->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    pArr->len--;
    if (pDest) {
        memcpy(pDest, pArr->mem + (pArr->len * pArr->memb_size),
               pArr->memb_size);
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrPopChecked(CONT_Arr *pArr, void *pDest) {
    if (!CONT_ArrIsValid(pArr)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrPopUnchecked(pArr, pDest);
}

PRP_API void PRP_CALL CONT_ArrRemoveUnchecked(CONT_Arr *pArr, void *pDest,
                                              PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(i < pArr->len);

    if (pDest) {
        memcpy(pDest, pArr->mem + (i * pArr->memb_size), pArr->memb_size);
    }
    memmove(pArr->mem + (i * pArr->memb_size),
            pArr->mem + ((i + 1) * pArr->memb_size),
            (pArr->len - i - 1) * pArr->memb_size);
    pArr->len--;
}

PRP_API PRP_Result PRP_CALL CONT_ArrRemoveChecked(CONT_Arr *pArr, void *pDest,
                                                  PRP_Size i) {
    if (!CONT_ArrIsValid(pArr)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pArr->len) {
        return PRP_ERR_OOB;
    }

    CONT_ArrRemoveUnchecked(pArr, pDest, i);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_ArrCmpUnchecked(const CONT_Arr *pArr1,
                                               const CONT_Arr *pArr2) {
    ASSERT_INVARIANT_EXPR(pArr1);
    ASSERT_INVARIANT_EXPR(pArr2);

    if (pArr1->len != pArr2->len || pArr1->memb_size != pArr2->memb_size) {
        return PRP_False;
    }

    return (memcmp(pArr1->mem, pArr2->mem, pArr1->len * pArr1->memb_size) == 0);
}

PRP_API PRP_Result PRP_CALL CONT_ArrCmpChecked(const CONT_Arr *pArr1,
                                               const CONT_Arr *pArr2,
                                               PRP_Bool *pRslt) {
    if (!CONT_ArrIsValid(pArr1) || !CONT_ArrIsValid(pArr2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_ArrCmpUnchecked(pArr1, pArr2);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrExtendUnchecked(CONT_Arr *pArr1,
                                                    const CONT_Arr *pArr2) {
    ASSERT_INVARIANT_EXPR(pArr1);
    ASSERT_INVARIANT_EXPR(pArr2);
    DIAG_ASSERT(pArr1->memb_size == pArr2->memb_size);

    if (pArr1->len > CONT_ARR_MAX_CAP(pArr1->memb_size) - pArr2->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_cap = pArr1->len + pArr2->len;
    PRP_Result code = ArrChangeSize(pArr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(pArr1->mem + (pArr1->len * pArr1->memb_size), pArr2->mem,
           pArr2->len * pArr2->memb_size);
    pArr1->len = new_cap;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrExtendChecked(CONT_Arr *pArr1,
                                                  const CONT_Arr *pArr2) {
    if (!CONT_ArrIsValid(pArr1) || !CONT_ArrIsValid(pArr2) ||
        pArr1->memb_size != pArr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrExtendUnchecked(pArr1, pArr2);
}

PRP_API void PRP_CALL CONT_ArrSwapUnchecked(CONT_Arr *pArr, PRP_Size i,
                                            PRP_Size j, void *swap_bffr) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(swap_bffr != NULL);
    DIAG_ASSERT(i < pArr->len);
    DIAG_ASSERT(j < pArr->len);

    if (i == j) {
        return;
    }

    PRP_U8 *i_elem = pArr->mem + (i * pArr->memb_size);
    PRP_U8 *j_elem = pArr->mem + (j * pArr->memb_size);
    memcpy(swap_bffr, i_elem, pArr->memb_size);
    memcpy(i_elem, j_elem, pArr->memb_size);
    memcpy(j_elem, swap_bffr, pArr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_ArrSwapChecked(CONT_Arr *pArr, PRP_Size i,
                                                PRP_Size j, void *swap_bffr) {
    if (!CONT_ArrIsValid(pArr) || !swap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pArr->len || j >= pArr->len) {
        return PRP_ERR_OOB;
    }

    CONT_ArrSwapUnchecked(pArr, i, j, swap_bffr);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ArrResetUnchecked(CONT_Arr *pArr) {
    ASSERT_INVARIANT_EXPR(pArr);

#ifdef PRP_DEBUG_MODE
    memset(pArr->mem, 0, pArr->len * pArr->memb_size);
#endif
    pArr->len = 0;
}

PRP_API PRP_Result PRP_CALL CONT_ArrResetChecked(CONT_Arr *pArr) {
    if (!CONT_ArrIsValid(pArr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ArrResetUnchecked(pArr);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrShrinkFitUnchecked(CONT_Arr *pArr) {
    ASSERT_INVARIANT_EXPR(pArr);

    return ArrChangeSize(pArr, (pArr->len) ? pArr->len : CONT_ARR_DEFAULT_CAP);
}

PRP_API PRP_Result PRP_CALL CONT_ArrShrinkFitChecked(CONT_Arr *pArr) {
    if (!CONT_ArrIsValid(pArr)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrShrinkFitUnchecked(pArr);
}

PRP_API PRP_Result PRP_CALL CONT_ArrForEachUnchecked(
    CONT_Arr *pArr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    ASSERT_INVARIANT_EXPR(pArr);
    DIAG_ASSERT(cb != NULL);

    PRP_U8 *mem = pArr->mem;
    for (PRP_Size i = 0; i < pArr->len; i++) {
        PRP_Result code = cb(mem, pUser_data);
        if (code != PRP_OK) {
            return code;
        }
        mem += pArr->memb_size;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ArrForEachChecked(
    CONT_Arr *pArr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    if (!CONT_ArrIsValid(pArr) || !cb) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrForEachUnchecked(pArr, cb, pUser_data);
}
