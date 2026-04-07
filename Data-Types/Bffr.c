#include "Bffr.h"
#include "../Diagnostics/Assert.h"
#include "Typedefs.h"
#include <string.h>

struct _Bffr {
    DT_size cap;
    DT_size memb_size;
    DT_u8 *mem;
};

#define ASSERT_INVARIANT_EXPR(bffr)                                            \
    DIAG_ASSERT_MSG(DT_BffrIsValid(bffr),                                      \
                    "The given buffer is either DT_null, or is corrupted.")

PRP_FN_API DT_bool PRP_FN_CALL DT_BffrIsValid(const DT_Bffr *bffr) {
    return (bffr != DT_null && bffr->mem != DT_null && bffr->memb_size > 0 &&
            bffr->cap > 0 && bffr->cap <= DT_BFFR_MAX_CAP(bffr->memb_size));
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCreateUnchecked(DT_size memb_size,
                                                         DT_size cap,
                                                         DT_Bffr **out) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(out != DT_null);

    if (cap > DT_BFFR_MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    DT_Bffr *bffr = malloc(sizeof(DT_Bffr));
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

    *out = bffr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCreateChecked(DT_size memb_size,
                                                       DT_size cap,
                                                       DT_Bffr **out) {
    if (!memb_size || !cap || !out) {
        return PRP_ERR_INV_ARG;
    }

    return DT_BffrCreateUnchecked(memb_size, cap, out);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCloneUnchecked(const DT_Bffr *bffr,
                                                        DT_Bffr **out) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(out != DT_null);

    // Unchecked since we checked for invariants above.
    PRP_Result code = DT_BffrCreateUnchecked(bffr->memb_size, bffr->cap, out);
    if (code != PRP_OK) {
        return code;
    }

    DT_Bffr *cpy = *out;
    memcpy(cpy->mem, bffr->mem, bffr->memb_size * bffr->cap);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCloneChecked(const DT_Bffr *bffr,
                                                      DT_Bffr **out) {
    if (!DT_BffrIsValid(bffr) || !out) {
        return PRP_ERR_INV_ARG;
    }

    return DT_BffrCloneUnchecked(bffr, out);
}

PRP_FN_API DT_void PRP_FN_CALL DT_BffrDeleteUnchecked(DT_Bffr **pBffr) {
    DIAG_ASSERT(pBffr != DT_null);
    DIAG_ASSERT(*pBffr != DT_null && (*pBffr)->mem != DT_null);

    DT_Bffr *bffr = *pBffr;

    free(bffr->mem);

#if !defined(PRP_NDEBUG)
    bffr->mem = DT_null;
    bffr->memb_size = bffr->cap = 0;
#endif

    free(bffr);
    *pBffr = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrDeleteChecked(DT_Bffr **pBffr) {
    if (!pBffr || !(*pBffr) || !(*pBffr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    DT_BffrDeleteUnchecked(pBffr);

    return PRP_OK;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_BffrRawUnchecked(const DT_Bffr *bffr,
                                                          DT_size *pCap) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pCap != DT_null);

    *pCap = bffr->cap;

    return bffr->mem;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrRawChecked(const DT_Bffr *bffr,
                                                    DT_size *pCap,
                                                    DT_void **pRaw) {
    if (!DT_BffrIsValid(bffr) || !pCap || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pCap = bffr->cap;
    *pRaw = bffr->mem;

    return PRP_OK;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrCap(const DT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    return bffr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrMembSize(const DT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    return bffr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrMaxCap(const DT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    return DT_BFFR_MAX_CAP(bffr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_BffrGetUnchecked(const DT_Bffr *bffr,
                                                    DT_size i) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(i < bffr->cap);

    return bffr->mem + (i * bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrGetChecked(const DT_Bffr *bffr,
                                                    DT_size i, DT_void **dest) {
    if (!DT_BffrIsValid(bffr) || !dest) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap) {
        return PRP_ERR_OOB;
    }

    *dest = DT_BffrGetUnchecked(bffr, i);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BffrSetUnchecked(DT_Bffr *bffr, DT_size i,
                                                   const DT_void *pData) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pData != DT_null);
    DIAG_ASSERT(i < bffr->cap);

    memcpy(bffr->mem + (i * bffr->memb_size), pData, bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetChecked(DT_Bffr *bffr, DT_size i,
                                                    const DT_void *pData) {
    if (!DT_BffrIsValid(bffr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap) {
        return PRP_ERR_OOB;
    }

    DT_BffrSetUnchecked(bffr, i, pData);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BffrSetRangeUnchecked(DT_Bffr *bffr,
                                                        DT_size i, DT_size j,
                                                        const DT_void *pData) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(pData != DT_null);
    DIAG_ASSERT(i < j);
    DIAG_ASSERT(i < bffr->cap && j <= bffr->cap);

    DT_u8 *ptr = bffr->mem + (i * bffr->memb_size);
    for (; i < j; i++) {
        memcpy(ptr, pData, bffr->memb_size);
        ptr += bffr->memb_size;
    }
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetRangeChecked(DT_Bffr *bffr,
                                                         DT_size i, DT_size j,
                                                         const DT_void *pData) {
    if (!DT_BffrIsValid(bffr) || !pData || i >= j) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap || j > bffr->cap) {
        return PRP_ERR_OOB;
    }

    DT_BffrSetRangeUnchecked(bffr, i, j, pData);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BffrSetManyUnchecked(DT_Bffr *bffr, DT_size i,
                                                       const DT_void *data_arr,
                                                       DT_size len) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(data_arr != DT_null);
    DIAG_ASSERT(i < bffr->cap && bffr->cap - i >= len);

    memcpy(bffr->mem + (i * bffr->memb_size), data_arr, bffr->memb_size * len);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetManyChecked(DT_Bffr *bffr,
                                                        DT_size i,
                                                        const DT_void *data_arr,
                                                        DT_size len) {
    if (!DT_BffrIsValid(bffr) || !data_arr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap || bffr->cap - i < len) {
        return PRP_ERR_OOB;
    }

    DT_BffrSetManyUnchecked(bffr, i, data_arr, len);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_BffrCmpUnchecked(const DT_Bffr *bffr1,
                                                   const DT_Bffr *bffr2) {
    ASSERT_INVARIANT_EXPR(bffr1);
    ASSERT_INVARIANT_EXPR(bffr2);

    if (bffr1->cap != bffr2->cap || bffr1->memb_size != bffr2->memb_size) {
        return DT_false;
    }

    return (memcmp(bffr1->mem, bffr2->mem, bffr1->cap * bffr1->memb_size) == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCmpChecked(const DT_Bffr *bffr1,
                                                    const DT_Bffr *bffr2,
                                                    DT_bool *pRslt) {
    if (!DT_BffrIsValid(bffr1) || !DT_BffrIsValid(bffr2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_BffrCmpUnchecked(bffr1, bffr2);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrExtendUnchecked(DT_Bffr *bffr1,
                                                         const DT_Bffr *bffr2) {
    ASSERT_INVARIANT_EXPR(bffr1);
    ASSERT_INVARIANT_EXPR(bffr2);
    DIAG_ASSERT(bffr1->memb_size == bffr2->memb_size);

    if (bffr1->cap > DT_BFFR_MAX_CAP(bffr1->memb_size) - bffr2->cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    DT_size new_cap = bffr1->cap + bffr2->cap, old_cap = bffr1->cap;
    PRP_Result code = DT_BffrChangeSizeUnchecked(bffr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(bffr1->mem + (old_cap * bffr1->memb_size), bffr2->mem,
           bffr2->cap * bffr2->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrExtendChecked(DT_Bffr *bffr1,
                                                       const DT_Bffr *bffr2) {
    if (!(DT_BffrIsValid(bffr1)) || !(DT_BffrIsValid(bffr2)) ||
        bffr1->memb_size != bffr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    return DT_BffrExtendUnchecked(bffr1, bffr2);
}

PRP_FN_API DT_void PRP_FN_CALL DT_BffrSwapUnchecked(DT_Bffr *bffr, DT_size i,
                                                    DT_size j,
                                                    DT_void *swap_bffr) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(swap_bffr != DT_null);
    DIAG_ASSERT(i < bffr->cap);
    DIAG_ASSERT(j < bffr->cap);

    if (i == j) {
        return;
    }

    DT_u8 *i_elem = bffr->mem + (i * bffr->memb_size);
    DT_u8 *j_elem = bffr->mem + (j * bffr->memb_size);
    memcpy(swap_bffr, i_elem, bffr->memb_size);
    memcpy(i_elem, j_elem, bffr->memb_size);
    memcpy(j_elem, swap_bffr, bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSwapChecked(DT_Bffr *bffr, DT_size i,
                                                     DT_size j,
                                                     DT_void *swap_bffr) {
    if (!DT_BffrIsValid(bffr) || !swap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= bffr->cap || j >= bffr->cap) {
        return PRP_ERR_OOB;
    }

    DT_BffrSwapUnchecked(bffr, i, j, swap_bffr);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_BffrClearUnchecked(DT_Bffr *bffr) {
    ASSERT_INVARIANT_EXPR(bffr);

    memset(bffr->mem, 0, bffr->cap * bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrClearChecked(DT_Bffr *bffr) {
    if (!DT_BffrIsValid(bffr)) {
        return PRP_ERR_INV_ARG;
    }

    DT_BffrClearUnchecked(bffr);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrChangeSizeUnchecked(DT_Bffr *bffr,
                                                             DT_size new_cap) {
    ASSERT_INVARIANT_EXPR(bffr);
    DIAG_ASSERT(new_cap > 0);

    DT_size max_cap = DT_BFFR_MAX_CAP(bffr->memb_size);
    if (bffr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (bffr->cap == new_cap) {
        return PRP_OK;
    }
    DT_u8 *mem = realloc(bffr->mem, new_cap * bffr->memb_size);
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrChangeSizeChecked(DT_Bffr *bffr,
                                                           DT_size new_cap) {
    if (!DT_BffrIsValid(bffr) || !new_cap) {
        return PRP_ERR_INV_ARG;
    }

    return DT_BffrChangeSizeUnchecked(bffr, new_cap);
}
