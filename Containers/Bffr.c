#include "Bffr.h"
#include "Diagnostics/Assert.h"
#include <string.h>

struct _Bffr {
    PRP_Size cap;
    PRP_Size memb_size;
    PRP_U8 *mem;
};

#define ASSERT_INVARIANT_EXPR(bffr)                                            \
    DIAG_ASSERT_MSG(CONT_BffrIsValid(bffr),                                    \
                    "The given buffer is either NULL, or is corrupted.")

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BffrIsValid(const CONT_Bffr *bffr) {
    return (bffr != NULL && bffr->mem != NULL && bffr->memb_size > 0 &&
            bffr->cap > 0 && bffr->cap <= CONT_BFFR_MAX_CAP(bffr->memb_size));
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCreateUnchecked(PRP_Size memb_size,
                                                           PRP_Size cap,
                                                           CONT_Bffr **pBffr) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(pBffr != NULL);

    if (cap > CONT_BFFR_MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    CONT_Bffr *bffr = malloc(sizeof(CONT_Bffr));
    if (!bffr) {
        return PRP_ERR_OOM;
    }
    bffr->mem = calloc(1, memb_size * cap);
    if (!bffr->mem) {
        free(bffr);
        return PRP_ERR_OOM;
    }
    bffr->memb_size = memb_size;
    bffr->cap = cap;

    *pBffr = bffr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCreateChecked(PRP_Size memb_size,
                                                         PRP_Size cap,
                                                         CONT_Bffr **pBffr) {
    if (!memb_size || !cap || !pBffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrCreateUnchecked(memb_size, cap, pBffr);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCloneUnchecked(const CONT_Bffr *bffr,
                                                          CONT_Bffr **pBffr) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pBffr != NULL);

    // Unchecked since we checked for invariants above.
    PRP_Result code =
        CONT_BffrCreateUnchecked(bffr->memb_size, bffr->cap, pBffr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Bffr *cpy = *pBffr;
    memcpy(cpy->mem, bffr->mem, bffr->memb_size * bffr->cap);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCloneChecked(const CONT_Bffr *bffr,
                                                        CONT_Bffr **pBffr) {
    if (!CONT_BffrIsValid(bffr) || !pBffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrCloneUnchecked(bffr, pBffr);
}

PRP_FN_API void PRP_FN_CALL CONT_BffrDeleteUnchecked(CONT_Bffr **pBffr) {
    DIAG_ASSERT(pBffr != NULL);
    DIAG_ASSERT(*pBffr != NULL && (*pBffr)->mem != NULL);

    CONT_Bffr *bffr = *pBffr;

    free(bffr->mem);

#ifdef PRP_DEBUG_MODE
    bffr->mem = NULL;
    bffr->memb_size = bffr->cap = 0;
#endif

    free(bffr);
    *pBffr = NULL;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrDeleteChecked(CONT_Bffr **pBffr) {
    if (!pBffr || !(*pBffr) || !(*pBffr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BffrDeleteUnchecked(pBffr);

    return PRP_OK;
}

PRP_FN_API const void *PRP_FN_CALL CONT_BffrRawUnchecked(const CONT_Bffr *bffr,
                                                         PRP_Size *pCap) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pCap != NULL);

    *pCap = bffr->cap;

    return bffr->mem;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrRawChecked(const CONT_Bffr *bffr,
                                                      PRP_Size *pCap,
                                                      void **pRaw) {
    if (!CONT_BffrIsValid(bffr) || !pCap || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pCap = bffr->cap;
    *pRaw = bffr->mem;

    return PRP_OK;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BffrCap(const CONT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    return bffr->cap;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BffrMembSize(const CONT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    return bffr->memb_size;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_BffrMaxCap(const CONT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    return CONT_BFFR_MAX_CAP(bffr->memb_size);
}

PRP_FN_API void *PRP_FN_CALL CONT_BffrGetUnchecked(const CONT_Bffr *bffr,
                                                   PRP_Size i) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(i < bffr->cap);

    return bffr->mem + (i * bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrGetChecked(const CONT_Bffr *bffr,
                                                      PRP_Size i, void **dest) {
    if (!CONT_BffrIsValid(bffr) || !dest) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap) {
        return PRP_ERR_OOB;
    }

    *dest = CONT_BffrGetUnchecked(bffr, i);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BffrSetUnchecked(CONT_Bffr *bffr, PRP_Size i,
                                                  const void *pData) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i < bffr->cap);

    memcpy(bffr->mem + (i * bffr->memb_size), pData, bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSetChecked(CONT_Bffr *bffr,
                                                      PRP_Size i,
                                                      const void *pData) {
    if (!CONT_BffrIsValid(bffr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSetUnchecked(bffr, i, pData);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BffrSetRangeUnchecked(CONT_Bffr *bffr,
                                                       PRP_Size i, PRP_Size j,
                                                       const void *pData) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i < j);
    DIAG_ASSERT(i < bffr->cap && j <= bffr->cap);

    PRP_U8 *ptr = bffr->mem + (i * bffr->memb_size);
    for (; i < j; i++) {
        memcpy(ptr, pData, bffr->memb_size);
        ptr += bffr->memb_size;
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSetRangeChecked(CONT_Bffr *bffr,
                                                           PRP_Size i,
                                                           PRP_Size j,
                                                           const void *pData) {
    if (!CONT_BffrIsValid(bffr) || !pData || i >= j) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap || j > bffr->cap) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSetRangeUnchecked(bffr, i, j, pData);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BffrSetManyUnchecked(CONT_Bffr *bffr,
                                                      PRP_Size i,
                                                      const void *data_arr,
                                                      PRP_Size len) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(data_arr != NULL);
    DIAG_ASSERT(i < bffr->cap && bffr->cap - i >= len);

    memcpy(bffr->mem + (i * bffr->memb_size), data_arr, bffr->memb_size * len);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSetManyChecked(CONT_Bffr *bffr,
                                                          PRP_Size i,
                                                          const void *data_arr,
                                                          PRP_Size len) {
    if (!CONT_BffrIsValid(bffr) || !data_arr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap || bffr->cap - i < len) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSetManyUnchecked(bffr, i, data_arr, len);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_BffrCmpUnchecked(const CONT_Bffr *bffr1,
                                                      const CONT_Bffr *bffr2) {
    ASSERT_INVARIANT_EXPR(bffr1);
    ASSERT_INVARIANT_EXPR(bffr2);

    if (bffr1->cap != bffr2->cap || bffr1->memb_size != bffr2->memb_size) {
        return PRP_False;
    }

    return (memcmp(bffr1->mem, bffr2->mem, bffr1->cap * bffr1->memb_size) == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrCmpChecked(const CONT_Bffr *bffr1,
                                                      const CONT_Bffr *bffr2,
                                                      PRP_Bool *pRslt) {
    if (!CONT_BffrIsValid(bffr1) || !CONT_BffrIsValid(bffr2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_BffrCmpUnchecked(bffr1, bffr2);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BffrExtendUnchecked(CONT_Bffr *bffr1, const CONT_Bffr *bffr2) {
    ASSERT_INVARIANT_EXPR(bffr1);
    ASSERT_INVARIANT_EXPR(bffr2);
    DIAG_ASSERT(bffr1->memb_size == bffr2->memb_size);

    if (bffr1->cap > CONT_BFFR_MAX_CAP(bffr1->memb_size) - bffr2->cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_cap = bffr1->cap + bffr2->cap, old_cap = bffr1->cap;
    PRP_Result code = CONT_BffrChangeSizeUnchecked(bffr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(bffr1->mem + (old_cap * bffr1->memb_size), bffr2->mem,
           bffr2->cap * bffr2->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BffrExtendChecked(CONT_Bffr *bffr1, const CONT_Bffr *bffr2) {
    if (!(CONT_BffrIsValid(bffr1)) || !(CONT_BffrIsValid(bffr2)) ||
        bffr1->memb_size != bffr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrExtendUnchecked(bffr1, bffr2);
}

PRP_FN_API void PRP_FN_CALL CONT_BffrSwapUnchecked(CONT_Bffr *bffr, PRP_Size i,
                                                   PRP_Size j,
                                                   void *swap_bffr) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(swap_bffr != NULL);
    DIAG_ASSERT(i < bffr->cap);
    DIAG_ASSERT(j < bffr->cap);

    if (i == j) {
        return;
    }

    PRP_U8 *i_elem = bffr->mem + (i * bffr->memb_size);
    PRP_U8 *j_elem = bffr->mem + (j * bffr->memb_size);
    memcpy(swap_bffr, i_elem, bffr->memb_size);
    memcpy(i_elem, j_elem, bffr->memb_size);
    memcpy(j_elem, swap_bffr, bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrSwapChecked(CONT_Bffr *bffr,
                                                       PRP_Size i, PRP_Size j,
                                                       void *swap_bffr) {
    if (!CONT_BffrIsValid(bffr) || !swap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap || j >= bffr->cap) {
        return PRP_ERR_OOB;
    }

    CONT_BffrSwapUnchecked(bffr, i, j, swap_bffr);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_BffrClearUnchecked(CONT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    memset(bffr->mem, 0, bffr->cap * bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrClearChecked(CONT_Bffr *bffr) {
    if (!CONT_BffrIsValid(bffr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_BffrClearUnchecked(bffr);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_BffrChangeSizeUnchecked(CONT_Bffr *bffr, PRP_Size new_cap) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(new_cap > 0);

    if (bffr->cap == new_cap) {
        return PRP_OK;
    }
    PRP_Size max_cap = CONT_BFFR_MAX_CAP(bffr->memb_size);
    if (bffr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_U8 *mem = realloc(bffr->mem, new_cap * bffr->memb_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    if (new_cap > bffr->cap) {
        memset(mem + (bffr->cap * bffr->memb_size), 0,
               (new_cap - bffr->cap) * bffr->memb_size);
    }
    bffr->mem = mem;
    bffr->cap = new_cap;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_BffrChangeSizeChecked(CONT_Bffr *bffr,
                                                             PRP_Size new_cap) {
    if (!CONT_BffrIsValid(bffr) || !new_cap) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_BffrChangeSizeUnchecked(bffr, new_cap);
}
