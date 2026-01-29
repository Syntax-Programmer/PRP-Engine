#include "Bffr.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Bffr {
    DT_size cap;
    DT_size memb_size;
    DT_u8 *mem;
};

#define DEFAULT_BFFR_CAP (16)
#define MAX_CAP(memb_size) (DT_SIZE_MAX / memb_size)

PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreate(DT_size memb_size, DT_size cap) {
    DIAG_GUARD(memb_size > 0, DT_null);
    DIAG_GUARD(cap > 0, DT_null);

    if (cap > MAX_CAP(memb_size)) {
        return DT_null;
    }
    DT_Bffr *bffr = malloc(sizeof(DT_Bffr));
    if (!bffr) {
        return DT_null;
    }
    bffr->mem = calloc(1, memb_size * cap);
    if (!bffr->mem) {
        free(bffr);
        return DT_null;
    }
    bffr->memb_size = memb_size;
    bffr->cap = cap;

    return bffr;
}

PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreateDefault(DT_size memb_size) {
    return DT_BffrCreate(memb_size, DEFAULT_BFFR_CAP);
}

PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrClone(DT_Bffr *bffr) {
    DIAG_GUARD(bffr != DT_null, DT_null);

    DT_Bffr *cpy = DT_BffrCreate(bffr->memb_size, bffr->cap);
    if (!cpy) {
        return DT_null;
    }
    memcpy(cpy->mem, bffr->mem, bffr->memb_size * bffr->cap);

    return cpy;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrDelete(DT_Bffr **pBffr) {
    DIAG_GUARD(pBffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(*pBffr != DT_null, PRP_ERR_INV_ARG);

    DT_Bffr *bffr = *pBffr;

    if (bffr->mem) {
        free(bffr->mem);
        bffr->mem = DT_null;
    }
    bffr->memb_size = bffr->cap = 0;
    free(bffr);
    *pBffr = DT_null;

    return PRP_OK;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_BffrRaw(const DT_Bffr *bffr,
                                                 DT_size *pCap) {
    DIAG_GUARD(bffr != DT_null, DT_null);
    DIAG_GUARD(pCap != DT_null, DT_null);

    *pCap = bffr->cap;

    return bffr->mem;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrCap(const DT_Bffr *bffr) {
    DIAG_GUARD(bffr != DT_null, PRP_INVALID_SIZE);

    return bffr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrMembSize(const DT_Bffr *bffr) {
    DIAG_GUARD(bffr != DT_null, PRP_INVALID_SIZE);

    return bffr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrMaxCap(const DT_Bffr *bffr) {
    DIAG_GUARD(bffr != DT_null, PRP_INVALID_SIZE);

    return MAX_CAP(bffr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_BffrGet(const DT_Bffr *bffr, DT_size i) {
    DIAG_GUARD(bffr != DT_null, DT_null);
    DIAG_GUARD(i < bffr->cap, DT_null);

    return bffr->mem + (i * bffr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSet(DT_Bffr *bffr, DT_size i,
                                             const DT_void *pData) {
    DIAG_GUARD(bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pData != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < bffr->cap, PRP_ERR_OOB);

    memcpy(bffr->mem + (i * bffr->memb_size), pData, bffr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetRange(DT_Bffr *bffr, DT_size i,
                                                  DT_size j,
                                                  const DT_void *pData) {
    DIAG_GUARD(bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pData != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < bffr->cap || j < bffr->cap, PRP_ERR_OOB);
    DIAG_GUARD(i < j, PRP_ERR_INV_ARG);

    DT_u8 *ptr = bffr->mem + (i * bffr->memb_size);
    for (; i < j; i++) {
        memcpy(ptr, pData, bffr->memb_size);
        ptr += bffr->memb_size;
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSetMany(DT_Bffr *bffr, DT_size i,
                                                 const DT_void *data_arr,
                                                 DT_size len) {
    DIAG_GUARD(bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(data_arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < bffr->cap && bffr->cap - i > len, PRP_ERR_OOB);

    memcpy(bffr->mem + (i * bffr->memb_size), data_arr, bffr->memb_size * len);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrCmp(const DT_Bffr *bffr1,
                                             const DT_Bffr *bffr2,
                                             DT_bool *pRslt) {
    DIAG_GUARD(bffr1 != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(bffr2 != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pRslt != DT_null, PRP_ERR_INV_ARG);

    if (bffr1->cap != bffr2->cap || bffr1->memb_size != bffr2->memb_size) {
        *pRslt = DT_false;
        return PRP_OK;
    }

    *pRslt =
        (memcmp(bffr1->mem, bffr2->mem, bffr1->cap * bffr1->memb_size) == 0);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrExtend(DT_Bffr *bffr1,
                                                const DT_Bffr *bffr2) {
    DIAG_GUARD(bffr1 != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(bffr2 != DT_null, PRP_ERR_INV_ARG);

    if (bffr1->memb_size != bffr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    if (bffr1->cap > MAX_CAP(bffr1->memb_size) - bffr2->cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    DT_size new_cap = bffr1->cap + bffr2->cap;
    DT_u8 *mem = realloc(bffr1->mem, new_cap * bffr1->memb_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    memcpy(mem + (bffr1->cap * bffr1->memb_size), bffr2->mem,
           bffr2->cap * bffr2->memb_size);
    bffr1->mem = mem;
    bffr1->cap += bffr2->cap;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrSwap(DT_Bffr *bffr, DT_size i,
                                              DT_size j, DT_void *swap_bffr) {
    DIAG_GUARD(bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(swap_bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < bffr->cap, PRP_ERR_OOB);
    DIAG_GUARD(j < bffr->cap, PRP_ERR_OOB);

    if (i == j) {
        return PRP_OK;
    }

    DT_u8 *i_elem = bffr->mem + (i * bffr->memb_size);
    DT_u8 *j_elem = bffr->mem + (j * bffr->memb_size);
    memcpy(swap_bffr, i_elem, bffr->memb_size);
    memcpy(i_elem, j_elem, bffr->memb_size);
    memcpy(j_elem, swap_bffr, bffr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrClear(DT_Bffr *bffr) {
    DIAG_GUARD(bffr != DT_null, PRP_ERR_INV_ARG);

    memset(bffr->mem, 0, bffr->cap * bffr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_BffrChangeSize(DT_Bffr *bffr,
                                                    DT_size new_cap) {
    DIAG_GUARD(bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(new_cap > 0, PRP_ERR_INV_ARG);

    if (new_cap > MAX_CAP(bffr->memb_size)) {
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
