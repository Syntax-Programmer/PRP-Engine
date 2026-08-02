#include "Bffr.h"
#include "Diagnostics/Assert.h"
#include <string.h>

struct CONT_Bffr {
    PRP_Size cap;
    PRP_Size memb_size;
    PRP_U8 *mem;
};

#define ASSERT_INVARIANT_EXPR(pBffr)                                           \
    DIAG_ASSERT_MSG(CONT_BffrIsValid(pBffr),                                   \
                    "The given buffer is either NULL, or is corrupted.")

PRP_API PRP_Bool PRP_CALL CONT_BffrIsValid(const CONT_Bffr *pBffr) {
    return (pBffr != NULL && pBffr->mem != NULL && pBffr->memb_size > 0 &&
            pBffr->cap > 0 &&
            pBffr->cap <= CONT_BFFR_MAX_CAP(pBffr->memb_size));
}

PRP_API PRP_Result PRP_CALL CONT_BffrCreateUnchecked(PRP_Size memb_size,
                                                     PRP_Size cap,
                                                     CONT_Bffr **ppBffr) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(ppBffr != NULL);

    if (cap > CONT_BFFR_MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    CONT_Bffr *pBffr = malloc(sizeof(CONT_Bffr));
    if (!pBffr) {
        return PRP_ERR_OOM;
    }
    pBffr->mem = calloc(1, memb_size * cap);
    if (!pBffr->mem) {
        free(pBffr);
        return PRP_ERR_OOM;
    }
    pBffr->memb_size = memb_size;
    pBffr->cap = cap;

    *ppBffr = pBffr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BffrCreateChecked(PRP_Size memb_size,
                                                   PRP_Size cap,
                                                   CONT_Bffr **ppBffr) {
    if (!memb_size || !cap || !ppBffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrCreateUnchecked(memb_size, cap, ppBffr);
}

PRP_API PRP_Result PRP_CALL CONT_BffrCloneUnchecked(const CONT_Bffr *pBffr,
                                                    CONT_Bffr **ppBffr) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(ppBffr != NULL);

    // Unchecked since we checked for invariants above.
    PRP_Result code =
        CONT_BffrCreateUnchecked(pBffr->memb_size, pBffr->cap, ppBffr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Bffr *cpy = *ppBffr;
    memcpy(cpy->mem, pBffr->mem, pBffr->memb_size * pBffr->cap);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BffrCloneChecked(const CONT_Bffr *pBffr,
                                                  CONT_Bffr **ppBffr) {
    if (!CONT_BffrIsValid(pBffr) || !ppBffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrCloneUnchecked(pBffr, ppBffr);
}

PRP_API void PRP_CALL CONT_BffrDeleteUnchecked(CONT_Bffr **ppBffr) {
    DIAG_ASSERT(ppBffr != NULL);
    DIAG_ASSERT(*ppBffr != NULL && (*ppBffr)->mem != NULL);

    CONT_Bffr *pBffr = *ppBffr;

    free(pBffr->mem);

#ifdef PRP_DEBUG_MODE
    pBffr->mem = NULL;
    pBffr->memb_size = pBffr->cap = 0;
#endif

    free(pBffr);
    *ppBffr = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_BffrDeleteChecked(CONT_Bffr **ppBffr) {
    if (!ppBffr || !(*ppBffr) || !(*ppBffr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BffrDeleteUnchecked(ppBffr);

    return PRP_OK;
}

PRP_API const void *PRP_CALL CONT_BffrRawUnchecked(const CONT_Bffr *pBffr,
                                                   PRP_Size *pCap) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(pCap != NULL);

    *pCap = pBffr->cap;

    return pBffr->mem;
}

PRP_API PRP_Result PRP_CALL CONT_BffrRawChecked(const CONT_Bffr *pBffr,
                                                PRP_Size *pCap, void **pRaw) {
    if (!CONT_BffrIsValid(pBffr) || !pCap || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pCap = pBffr->cap;
    *pRaw = pBffr->mem;

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL CONT_BffrCap(const CONT_Bffr *pBffr) {
    ASSERT_INVARIANT_EXPR(pBffr);

    return pBffr->cap;
}

PRP_API PRP_Size PRP_CALL CONT_BffrMembSize(const CONT_Bffr *pBffr) {
    ASSERT_INVARIANT_EXPR(pBffr);

    return pBffr->memb_size;
}

PRP_API PRP_Size PRP_CALL CONT_BffrMaxCap(const CONT_Bffr *pBffr) {
    ASSERT_INVARIANT_EXPR(pBffr);

    return CONT_BFFR_MAX_CAP(pBffr->memb_size);
}

PRP_API void *PRP_CALL CONT_BffrGetUnchecked(const CONT_Bffr *pBffr,
                                             PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(i < pBffr->cap);

    return pBffr->mem + (i * pBffr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_BffrGetChecked(const CONT_Bffr *pBffr,
                                                PRP_Size i, void **ppDest) {
    if (!CONT_BffrIsValid(pBffr) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBffr->cap) {
        return PRP_ERR_OOB;
    }

    *ppDest = CONT_BffrGetUnchecked(pBffr, i);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BffrSetUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                            const void *pData) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i < pBffr->cap);

    memcpy(pBffr->mem + (i * pBffr->memb_size), pData, pBffr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_BffrSetChecked(CONT_Bffr *pBffr, PRP_Size i,
                                                const void *pData) {
    if (!CONT_BffrIsValid(pBffr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBffr->cap) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSetUnchecked(pBffr, i, pData);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BffrSetRangeUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                                 PRP_Size j,
                                                 const void *pData) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i < j);
    DIAG_ASSERT(i < pBffr->cap && j <= pBffr->cap);

    PRP_U8 *ptr = pBffr->mem + (i * pBffr->memb_size);
    for (; i < j; i++) {
        memcpy(ptr, pData, pBffr->memb_size);
        ptr += pBffr->memb_size;
    }
}

PRP_API PRP_Result PRP_CALL CONT_BffrSetRangeChecked(CONT_Bffr *pBffr,
                                                     PRP_Size i, PRP_Size j,
                                                     const void *pData) {
    if (!CONT_BffrIsValid(pBffr) || !pData || i >= j) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBffr->cap || j > pBffr->cap) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSetRangeUnchecked(pBffr, i, j, pData);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BffrSetManyUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                                const void *pData_arr,
                                                PRP_Size len) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(pData_arr != NULL);
    DIAG_ASSERT(i < pBffr->cap && pBffr->cap - i >= len);

    memcpy(pBffr->mem + (i * pBffr->memb_size), pData_arr,
           pBffr->memb_size * len);
}

PRP_API PRP_Result PRP_CALL CONT_BffrSetManyChecked(CONT_Bffr *pBffr,
                                                    PRP_Size i,
                                                    const void *pData_arr,
                                                    PRP_Size len) {
    if (!CONT_BffrIsValid(pBffr) || !pData_arr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBffr->cap || pBffr->cap - i < len) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSetManyUnchecked(pBffr, i, pData_arr, len);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_BffrCmpUnchecked(const CONT_Bffr *pBffr1,
                                                const CONT_Bffr *pBffr2) {
    ASSERT_INVARIANT_EXPR(pBffr1);
    ASSERT_INVARIANT_EXPR(pBffr2);

    if (pBffr1->cap != pBffr2->cap || pBffr1->memb_size != pBffr2->memb_size) {
        return PRP_False;
    }

    return (memcmp(pBffr1->mem, pBffr2->mem, pBffr1->cap * pBffr1->memb_size) ==
            0);
}

PRP_API PRP_Result PRP_CALL CONT_BffrCmpChecked(const CONT_Bffr *pBffr1,
                                                const CONT_Bffr *pBffr2,
                                                PRP_Bool *pRslt) {
    if (!CONT_BffrIsValid(pBffr1) || !CONT_BffrIsValid(pBffr2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BffrCmpUnchecked(pBffr1, pBffr2);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BffrExtendUnchecked(CONT_Bffr *pBffr1,
                                                     const CONT_Bffr *pBffr2) {
    ASSERT_INVARIANT_EXPR(pBffr1);
    ASSERT_INVARIANT_EXPR(pBffr2);
    DIAG_ASSERT(pBffr1->memb_size == pBffr2->memb_size);

    if (pBffr1->cap > CONT_BFFR_MAX_CAP(pBffr1->memb_size) - pBffr2->cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_cap = pBffr1->cap + pBffr2->cap, old_cap = pBffr1->cap;
    PRP_Result code = CONT_BffrChangeSizeUnchecked(pBffr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(pBffr1->mem + (old_cap * pBffr1->memb_size), pBffr2->mem,
           pBffr2->cap * pBffr2->memb_size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BffrExtendChecked(CONT_Bffr *pBffr1,
                                                   const CONT_Bffr *pBffr2) {
    if (!(CONT_BffrIsValid(pBffr1)) || !(CONT_BffrIsValid(pBffr2)) ||
        pBffr1->memb_size != pBffr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrExtendUnchecked(pBffr1, pBffr2);
}

PRP_API void PRP_CALL CONT_BffrSwapUnchecked(CONT_Bffr *pBffr, PRP_Size i,
                                             PRP_Size j, void *pSwap_bffr) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(pSwap_bffr != NULL);
    DIAG_ASSERT(i < pBffr->cap);
    DIAG_ASSERT(j < pBffr->cap);

    if (i == j) {
        return;
    }

    PRP_U8 *i_elem = pBffr->mem + (i * pBffr->memb_size);
    PRP_U8 *j_elem = pBffr->mem + (j * pBffr->memb_size);
    memcpy(pSwap_bffr, i_elem, pBffr->memb_size);
    memcpy(i_elem, j_elem, pBffr->memb_size);
    memcpy(j_elem, pSwap_bffr, pBffr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_BffrSwapChecked(CONT_Bffr *pBffr, PRP_Size i,
                                                 PRP_Size j, void *pSwap_bffr) {
    if (!CONT_BffrIsValid(pBffr) || !pSwap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pBffr->cap || j >= pBffr->cap) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSwapUnchecked(pBffr, i, j, pSwap_bffr);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_BffrClearUnchecked(CONT_Bffr *pBffr) {
    ASSERT_INVARIANT_EXPR(pBffr);

    memset(pBffr->mem, 0, pBffr->cap * pBffr->memb_size);
}

PRP_API PRP_Result PRP_CALL CONT_BffrClearChecked(CONT_Bffr *pBffr) {
    if (!CONT_BffrIsValid(pBffr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BffrClearUnchecked(pBffr);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BffrChangeSizeUnchecked(CONT_Bffr *pBffr,
                                                         PRP_Size new_cap) {
    ASSERT_INVARIANT_EXPR(pBffr);
    DIAG_ASSERT(new_cap > 0);

    if (pBffr->cap == new_cap) {
        return PRP_OK;
    }
    PRP_Size max_cap = CONT_BFFR_MAX_CAP(pBffr->memb_size);
    if (pBffr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_U8 *mem = realloc(pBffr->mem, new_cap * pBffr->memb_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    if (new_cap > pBffr->cap) {
        memset(mem + (pBffr->cap * pBffr->memb_size), 0,
               (new_cap - pBffr->cap) * pBffr->memb_size);
    }
    pBffr->mem = mem;
    pBffr->cap = new_cap;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_BffrChangeSizeChecked(CONT_Bffr *pBffr,
                                                       PRP_Size new_cap) {
    if (!CONT_BffrIsValid(pBffr) || !new_cap) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrChangeSizeUnchecked(pBffr, new_cap);
}
