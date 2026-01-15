#include "Bffr.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _Bffr {
    DT_size cap;
    DT_size memb_size;
    DT_u8 *mem;
    // A sep bffr allocated so that any misc op can use it just in case safely.
    DT_u8 *elem_bffr;
};

#define DEFAULT_BFFR_CAP (16)
#define MAX_CAP(memb_size) (DT_SIZE_MAX / memb_size)

PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreate(DT_size memb_size, DT_size cap) {
    if (!memb_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Bffr can't be made with memb_size=0.");
        return DT_null;
    }
    if (!cap) {
        cap = DEFAULT_BFFR_CAP;
    }
    if (cap > MAX_CAP(memb_size)) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "DT_Bffr create capcity too big to accomodate. Max "
                        "capacity of buffer with memb_size=%zu is %zu",
                        memb_size, MAX_CAP(memb_size));
        return DT_null;
    }

    if (!memb_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Bffr can't be made with memb_size=0.");
        return DT_null;
    }
    if (!cap) {
        cap = DEFAULT_BFFR_CAP;
    }

    DT_Bffr *bffr = malloc(sizeof(DT_Bffr));
    if (!bffr) {
        PRP_LOG_FN_MALLOC_ERROR(bffr);
        return DT_null;
    }
    bffr->mem = calloc(1, memb_size * cap);
    if (!bffr->mem) {
        free(bffr);
        PRP_LOG_FN_MALLOC_ERROR(bffr->mem);
        return DT_null;
    }
    bffr->elem_bffr = malloc(memb_size);
    if (!bffr->elem_bffr) {
        free(bffr->mem);
        free(bffr);
        PRP_LOG_FN_MALLOC_ERROR(bffr->elem_bffr);
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
    PRP_NULL_ARG_CHECK(bffr, DT_null);

    DT_Bffr *cpy = DT_BffrCreate(bffr->memb_size, bffr->cap);
    if (!cpy) {
        PRP_LOG_FN_MALLOC_ERROR(cpy);
        return DT_null;
    }
    memcpy(cpy->mem, bffr->mem, bffr->memb_size * bffr->cap);

    return cpy;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrDelete(DT_Bffr **pBffr) {
    PRP_NULL_ARG_CHECK(pBffr, PRP_FN_INV_ARG_ERROR);
    DT_Bffr *bffr = *pBffr;
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);

    if (bffr->mem) {
        free(bffr->mem);
        bffr->mem = DT_null;
    }
    if (bffr->elem_bffr) {
        free(bffr->elem_bffr);
        bffr->elem_bffr = DT_null;
    }
    bffr->memb_size = bffr->cap = 0;
    free(bffr);
    *pBffr = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_BffrRaw(const DT_Bffr *bffr,
                                                 DT_size *pCap) {
    PRP_NULL_ARG_CHECK(bffr, DT_null);
    PRP_NULL_ARG_CHECK(pCap, DT_null);

    *pCap = bffr->cap;

    return bffr->mem;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrCap(const DT_Bffr *bffr) {
    PRP_NULL_ARG_CHECK(bffr, PRP_INVALID_SIZE);

    return bffr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrMembSize(const DT_Bffr *bffr) {
    PRP_NULL_ARG_CHECK(bffr, PRP_INVALID_SIZE);

    return bffr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_BffrMaxCap(const DT_Bffr *bffr) {
    PRP_NULL_ARG_CHECK(bffr, PRP_INVALID_SIZE);

    return MAX_CAP(bffr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_BffrGet(const DT_Bffr *bffr, DT_size i) {
    PRP_NULL_ARG_CHECK(bffr, DT_null);
    if (i >= bffr->cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the buffer index: %zu, of a buffer with cap: %zu",
            i, bffr->cap);
        return DT_null;
    }

    return bffr->mem + (i * bffr->memb_size);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSet(DT_Bffr *bffr, DT_size i,
                                             const DT_void *pData) {
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pData, PRP_FN_INV_ARG_ERROR);
    if (i >= bffr->cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the buffer index: %zu, of a buffer with cap: %zu",
            i, bffr->cap);
        return PRP_FN_OOB_ERROR;
    }

    memcpy(bffr->mem + (i * bffr->memb_size), pData, bffr->memb_size);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSetRange(DT_Bffr *bffr, DT_size i,
                                                  DT_size j,
                                                  const DT_void *pData) {
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pData, PRP_FN_INV_ARG_ERROR);
    if (i > j) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "i can't be greater than j for this operation.");
        return PRP_FN_INV_ARG_ERROR;
    }
    if (i >= bffr->cap || j > bffr->cap) {
        PRP_LOG_FN_CODE(PRP_FN_OOB_ERROR,
                        "Tried accessing the buffer index: %zu-%zu, of a "
                        "buffer with cap: %zu",
                        i, j, bffr->cap);
        return PRP_FN_OOB_ERROR;
    }

    DT_u8 *ptr = bffr->mem + (i * bffr->memb_size);
    for (; i < j; i++) {
        memcpy(ptr, pData, bffr->memb_size);
        ptr += bffr->memb_size;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSetMany(DT_Bffr *bffr, DT_size i,
                                                 const DT_void *data_arr,
                                                 DT_size len) {
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(data_arr, PRP_FN_INV_ARG_ERROR);
    if (i >= bffr->cap || len > bffr->cap - i) {
        PRP_LOG_FN_CODE(PRP_FN_OOB_ERROR,
                        "Tried accessing the buffer index: %zu to fill %zu "
                        "elements, of a buffer with cap: %zu",
                        i, len, bffr->cap);
        return PRP_FN_OOB_ERROR;
    }

    memcpy(bffr->mem + (i * bffr->memb_size), data_arr, bffr->memb_size * len);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrCmp(const DT_Bffr *bffr1,
                                             const DT_Bffr *bffr2,
                                             DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(bffr1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bffr2, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    if (bffr1->cap != bffr2->cap || bffr1->memb_size != bffr2->memb_size) {
        *pRslt = DT_false;
        return PRP_FN_SUCCESS;
    }

    *pRslt =
        (memcmp(bffr1->mem, bffr2->mem, bffr1->cap * bffr1->memb_size) == 0);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrExtend(DT_Bffr *bffr1,
                                                const DT_Bffr *bffr2) {
    PRP_NULL_ARG_CHECK(bffr1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(bffr2, PRP_FN_INV_ARG_ERROR);
    if (bffr1->memb_size != bffr2->memb_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Cannot join two buffers with different memb_sizes.");
        return PRP_FN_INV_ARG_ERROR;
    }

    if (bffr1->cap > MAX_CAP(bffr1->memb_size) - bffr2->cap) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Combined capacity of bffr1 exceeds max buffer capacity of %zu",
            MAX_CAP(bffr1->memb_size));
    }
    DT_size new_cap = bffr1->cap + bffr2->cap;
    DT_u8 *mem = realloc(bffr1->mem, new_cap * bffr1->memb_size);
    if (!mem) {
        PRP_LOG_FN_INV_ARG_ERROR(mem);
        return PRP_FN_MALLOC_ERROR;
    }
    memcpy(mem + (bffr1->cap * bffr1->memb_size), bffr2->mem,
           bffr2->cap * bffr2->memb_size);
    bffr1->mem = mem;
    bffr1->cap += bffr2->cap;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSwap(DT_Bffr *bffr, DT_size i,
                                              DT_size j) {
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);
    if (i >= bffr->cap || j >= bffr->cap) {
        PRP_LOG_FN_CODE(PRP_FN_OOB_ERROR,
                        "Tried accessing the buffer indices: %zu-%zu, of a "
                        "buffer with cap: %zu",
                        i, j, bffr->cap);
        return PRP_FN_OOB_ERROR;
    }
    if (i == j) {
        return PRP_FN_SUCCESS;
    }

    DT_u8 *i_elem = bffr->mem + (i * bffr->memb_size);
    DT_u8 *j_elem = bffr->mem + (j * bffr->memb_size);
    memcpy(bffr->elem_bffr, i_elem, bffr->memb_size);
    memcpy(i_elem, j_elem, bffr->memb_size);
    memcpy(j_elem, bffr->elem_bffr, bffr->memb_size);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrClear(DT_Bffr *bffr) {
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);

    memset(bffr->mem, 0, bffr->cap * bffr->memb_size);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrChangeSize(DT_Bffr *bffr,
                                                    DT_size new_cap) {
    PRP_NULL_ARG_CHECK(bffr, PRP_FN_INV_ARG_ERROR);
    if (!new_cap) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Cannot change size of the buffer to 0 elements.");
        return PRP_FN_INV_ARG_ERROR;
    }
    if (new_cap > MAX_CAP(bffr->memb_size)) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Cannot change cap of buffer to %zu, max cap of the buffer is %zu",
            new_cap, MAX_CAP(bffr->memb_size));
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }

    if (bffr->cap == new_cap) {
        return PRP_FN_SUCCESS;
    }
    DT_u8 *mem = realloc(bffr->mem, new_cap * bffr->memb_size);
    if (!mem) {
        PRP_LOG_FN_INV_ARG_ERROR(mem);
        return PRP_FN_MALLOC_ERROR;
    }
    if (new_cap > bffr->cap) {
        memset(mem + (bffr->cap * bffr->memb_size), 0,
               (new_cap - bffr->cap) * bffr->memb_size);
    }
    bffr->mem = mem;
    bffr->cap = new_cap;

    return PRP_FN_SUCCESS;
}
