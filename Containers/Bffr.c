#include "Bffr.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct CONT_Bffr {
    PRP_Size cap;
    PRP_Size memb_size;
    PRP_U8 *pMem;
};

#define ASSERT_INVARIANT_EXPR(pBffr)                                           \
    PRP_DIAG_ASSERT_MSG(CONT_BffrIsValid(pBffr), "The given pBffr is "         \
                                                 "invalid.")

PRP_API PRP_Bool PRP_CALL CONT_BffrIsValid(const CONT_Bffr *pBffr) {
    return (pBffr != NULL && pBffr->pMem != NULL && pBffr->memb_size > 0 &&
            pBffr->cap > 0 &&
            pBffr->cap <= CONT_BFFR_MAX_CAP(pBffr->memb_size));
}

PRP_API PRP_Result PRP_CALL CONT_BffrCreateUnchecked(PRP_Size memb_size,
                                                     PRP_Size cap,
                                                     CONT_Bffr **ppBffr) {
    PRP_DIAG_ASSERT_MSG(memb_size > 0,
                        "The memb_size of the buffer must be > 0.");
    PRP_DIAG_ASSERT_MSG(cap > 0, "The cap of the buffer must be > 0.");
    PRP_DIAG_ASSERT(ppBffr != NULL);

    *ppBffr = NULL;
    if (cap > CONT_BFFR_MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    CONT_Bffr *pBffr = malloc(sizeof(CONT_Bffr));
    if (!pBffr) {
        return PRP_ERR_OOM;
    }
    pBffr->pMem = calloc(1, memb_size * cap);
    if (!pBffr->pMem) {
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
    PRP_DIAG_ASSERT(ppBffr != NULL);

    // Unchecked since we checked for invariants above.
    PRP_Result code =
        CONT_BffrCreateUnchecked(pBffr->memb_size, pBffr->cap, ppBffr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Bffr *pCpy = *ppBffr;
    memcpy(pCpy->pMem, pBffr->pMem, pBffr->memb_size * pBffr->cap);

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
    PRP_DIAG_ASSERT(ppBffr != NULL);
    PRP_DIAG_ASSERT_MSG(*ppBffr != NULL && (*ppBffr)->pMem != NULL,
                        "The given *ppBffr is invalid.");

    CONT_Bffr *pBffr = *ppBffr;

    free(pBffr->pMem);

#ifdef PRP_DEBUG_MODE
    pBffr->pMem = NULL;
    pBffr->memb_size = pBffr->cap = 0;
#endif

    free(pBffr);
    *ppBffr = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_BffrDeleteChecked(CONT_Bffr **ppBffr) {
    if (!ppBffr || !(*ppBffr) || !(*ppBffr)->pMem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BffrDeleteUnchecked(ppBffr);

    return PRP_OK;
}

PRP_API const void *PRP_CALL CONT_BffrRawUnchecked(const CONT_Bffr *pBffr,
                                                   PRP_Size *pCap) {
    ASSERT_INVARIANT_EXPR(pBffr);
    PRP_DIAG_ASSERT(pCap != NULL);

    *pCap = pBffr->cap;

    return pBffr->pMem;
}

PRP_API PRP_Result PRP_CALL CONT_BffrRawChecked(const CONT_Bffr *pBffr,
                                                PRP_Size *pCap, void **pRaw) {
    if (!CONT_BffrIsValid(pBffr) || !pCap || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pCap = pBffr->cap;
    *pRaw = pBffr->pMem;

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
    PRP_DIAG_ASSERT_MSG(i < pBffr->cap, "The index i is out of bounds.");

    return pBffr->pMem + (i * pBffr->memb_size);
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
    PRP_DIAG_ASSERT(pData != NULL);
    PRP_DIAG_ASSERT_MSG(i < pBffr->cap, "The index i is out of bounds.");

    memcpy(pBffr->pMem + (i * pBffr->memb_size), pData, pBffr->memb_size);
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
    PRP_DIAG_ASSERT(pData != NULL);
    PRP_DIAG_ASSERT_MSG(i < j, "Index i must be smaller than index j.");
    PRP_DIAG_ASSERT_MSG(i < pBffr->cap, "The index i is out of bounds.");
    PRP_DIAG_ASSERT_MSG(j <= pBffr->cap, "The index j is out of bounds.");

    PRP_U8 *ptr = pBffr->pMem + (i * pBffr->memb_size);
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
    PRP_DIAG_ASSERT(pData_arr != NULL);
    PRP_DIAG_ASSERT_MSG(i < pBffr->cap, "The index i is out of bounds.");
    PRP_DIAG_ASSERT_MSG(pBffr->cap - i >= len,
                        "The requested range exceeds the buffer bounds.");

    memcpy(pBffr->pMem + (i * pBffr->memb_size), pData_arr,
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

    return (memcmp(pBffr1->pMem, pBffr2->pMem,
                   pBffr1->cap * pBffr1->memb_size) == 0);
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
    PRP_DIAG_ASSERT_MSG(
        pBffr1->memb_size == pBffr2->memb_size,
        "To extend, the memb_size of both buffers shall match.");

    if (pBffr1->cap > CONT_BFFR_MAX_CAP(pBffr1->memb_size) - pBffr2->cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_cap = pBffr1->cap + pBffr2->cap, old_cap = pBffr1->cap;
    PRP_Result code = CONT_BffrChangeSizeUnchecked(pBffr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(pBffr1->pMem + (old_cap * pBffr1->memb_size), pBffr2->pMem,
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
    PRP_DIAG_ASSERT(pSwap_bffr != NULL);
    PRP_DIAG_ASSERT_MSG(i < pBffr->cap, "The index i is out of bounds.");
    PRP_DIAG_ASSERT_MSG(j < pBffr->cap, "The index j is out of bounds.");

    if (i == j) {
        return;
    }

    PRP_U8 *pI_elem = pBffr->pMem + (i * pBffr->memb_size);
    PRP_U8 *pJ_elem = pBffr->pMem + (j * pBffr->memb_size);
    memcpy(pSwap_bffr, pI_elem, pBffr->memb_size);
    memcpy(pI_elem, pJ_elem, pBffr->memb_size);
    memcpy(pJ_elem, pSwap_bffr, pBffr->memb_size);
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

    memset(pBffr->pMem, 0, pBffr->cap * pBffr->memb_size);
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
    PRP_DIAG_ASSERT_MSG(new_cap > 0, "The new cap of buffer must be > 0.");

    if (pBffr->cap == new_cap) {
        return PRP_OK;
    }
    PRP_Size max_cap = CONT_BFFR_MAX_CAP(pBffr->memb_size);
    if (pBffr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_U8 *pMem = realloc(pBffr->pMem, new_cap * pBffr->memb_size);
    if (!pMem) {
        return PRP_ERR_OOM;
    }
    if (new_cap > pBffr->cap) {
        memset(pMem + (pBffr->cap * pBffr->memb_size), 0,
               (new_cap - pBffr->cap) * pBffr->memb_size);
    }
    pBffr->pMem = pMem;
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
