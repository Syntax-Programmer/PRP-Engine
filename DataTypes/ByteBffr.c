#include "ByteBffr.h"
#include "Diagnostics/Assert.h"
#include <stdio.h>
#include <string.h>

struct _ByteBffr {
    DT_size size;
    DT_u8 *mem;
};

#define ASSERT_INVARIANT_EXPR(b_bffr)                                          \
    DIAG_ASSERT_MSG(                                                           \
        DT_ByteBffrIsValid(b_bffr),                                            \
        "The given byte buffer is either DT_null, or is corrupted.")

PRP_FN_API DT_bool PRP_FN_CALL DT_ByteBffrIsValid(const DT_ByteBffr *b_bffr) {
    return (b_bffr != DT_null && b_bffr->mem != DT_null && b_bffr->size > 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCreateUnchecked(DT_size size, DT_ByteBffr **pB_bffr) {
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(pB_bffr != DT_null);

    DT_ByteBffr *b_bffr = malloc(sizeof(DT_ByteBffr));
    if (!b_bffr) {
        return PRP_ERR_OOM;
    }
    b_bffr->mem = calloc(1, size);
    if (!b_bffr->mem) {
        free(b_bffr);
        return PRP_ERR_OOM;
    }
    b_bffr->size = size;

    *pB_bffr = b_bffr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCreateChecked(DT_size size, DT_ByteBffr **pB_bffr) {
    if (!size || !pB_bffr) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ByteBffrCreateUnchecked(size, pB_bffr);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCloneUnchecked(const DT_ByteBffr *b_bffr, DT_ByteBffr **pB_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(pB_bffr != DT_null);

    PRP_Result code = DT_ByteBffrCreateUnchecked(b_bffr->size, pB_bffr);
    if (code != PRP_OK) {
        return code;
    }

    DT_ByteBffr *cpy = *pB_bffr;
    memcpy(cpy->mem, b_bffr->mem, b_bffr->size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCloneChecked(const DT_ByteBffr *b_bffr, DT_ByteBffr **pB_bffr) {
    if (!DT_ByteBffrIsValid(b_bffr) || !pB_bffr) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ByteBffrCloneUnchecked(b_bffr, pB_bffr);
}

PRP_FN_API DT_void PRP_FN_CALL
DT_ByteBffrDeleteUnchecked(DT_ByteBffr **pB_bffr) {
    DIAG_ASSERT(pB_bffr != DT_null);
    DIAG_ASSERT(*pB_bffr != DT_null && (*pB_bffr)->mem != DT_null);

    DT_ByteBffr *b_bffr = *pB_bffr;

    free(b_bffr->mem);

#if !defined(PRP_NDEBUG)
    b_bffr->mem = DT_null;
    b_bffr->size = 0;
#endif

    free(b_bffr);
    *pB_bffr = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrDeleteChecked(DT_ByteBffr **pB_bffr) {
    if (!pB_bffr || !(*pB_bffr) || !(*pB_bffr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    DT_ByteBffrDeleteUnchecked(pB_bffr);

    return PRP_OK;
}

PRP_FN_API const DT_void *PRP_FN_CALL
DT_ByteBffrRawUnchecked(const DT_ByteBffr *b_bffr, DT_size *pSize) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(pSize != DT_null);

    *pSize = b_bffr->size;

    return b_bffr->mem;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrRawChecked(
    const DT_ByteBffr *b_bffr, DT_size *pSize, DT_void **pRaw) {
    if (!DT_ByteBffrIsValid(b_bffr) || !pSize || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pSize = b_bffr->size;
    *pRaw = b_bffr->mem;

    return PRP_OK;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ByteBffrSize(const DT_ByteBffr *b_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);

    return b_bffr->size;
}

PRP_FN_API DT_void *PRP_FN_CALL
DT_ByteBffrGetUnchecked(const DT_ByteBffr *b_bffr, DT_size ofs) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(ofs < b_bffr->size);

    return b_bffr->mem + ofs;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrGetChecked(
    const DT_ByteBffr *b_bffr, DT_size ofs, DT_void **ppDest) {
    if (!DT_ByteBffrIsValid(b_bffr) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= b_bffr->size) {
        return PRP_ERR_OOB;
    }

    *ppDest = DT_ByteBffrGetUnchecked(b_bffr, ofs);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_ByteBffrUploadUnchecked(DT_ByteBffr *b_bffr,
                                                          DT_size ofs,
                                                          DT_size size,
                                                          DT_void *pData) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(ofs < b_bffr->size);
    DIAG_ASSERT(b_bffr->size - ofs >= size);
    DIAG_ASSERT(pData != DT_null);

    memcpy(b_bffr->mem + ofs, pData, size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrUploadChecked(DT_ByteBffr *b_bffr,
                                                           DT_size ofs,
                                                           DT_size size,
                                                           DT_void *pData) {
    if (!DT_ByteBffrIsValid(b_bffr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= b_bffr->size || b_bffr->size - ofs < size) {
        return PRP_ERR_OOB;
    }

    DT_ByteBffrUploadUnchecked(b_bffr, ofs, size, pData);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL
DT_ByteBffrCopyUnchecked(const DT_ByteBffr *b_bffr1, DT_size ofs1,
                         DT_ByteBffr *b_bffr2, DT_size ofs2, DT_size size) {
    ASSERT_INVARIANT_EXPR(b_bffr1);
    ASSERT_INVARIANT_EXPR(b_bffr2);
    DIAG_ASSERT(ofs1 < b_bffr1->size);
    DIAG_ASSERT(ofs2 < b_bffr2->size);
    DIAG_ASSERT(b_bffr1->size - ofs1 >= size);
    DIAG_ASSERT(b_bffr2->size - ofs2 >= size);

    memcpy(b_bffr2->mem + ofs2, b_bffr1->mem + ofs1, size);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrCopyChecked(const DT_ByteBffr *b_bffr1, DT_size ofs1,
                       DT_ByteBffr *b_bffr2, DT_size ofs2, DT_size size) {
    if (!DT_ByteBffrIsValid(b_bffr1) || !DT_ByteBffrIsValid(b_bffr2)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs1 >= b_bffr1->size || b_bffr1->size - ofs1 < size ||
        ofs2 >= b_bffr2->size || b_bffr2->size - ofs2 < size) {
        return PRP_ERR_OOB;
    }

    DT_ByteBffrCopyUnchecked(b_bffr1, ofs1, b_bffr2, ofs2, size);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_ByteBffrFillUnchecked(DT_ByteBffr *b_bffr,
                                                        DT_size ofs,
                                                        DT_size size,
                                                        DT_u8 byte) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(ofs < b_bffr->size);
    DIAG_ASSERT(b_bffr->size - ofs >= size);

    memset(b_bffr->mem + ofs, byte, size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrFillChecked(DT_ByteBffr *b_bffr,
                                                         DT_size ofs,
                                                         DT_size size,
                                                         DT_u8 byte) {
    if (!DT_ByteBffrIsValid(b_bffr)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= b_bffr->size || b_bffr->size - ofs < size) {
        return PRP_ERR_OOB;
    }

    DT_ByteBffrFillUnchecked(b_bffr, ofs, size, byte);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_ByteBffrCmpUnchecked(
    const DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2) {
    ASSERT_INVARIANT_EXPR(b_bffr1);
    ASSERT_INVARIANT_EXPR(b_bffr2);

    if (b_bffr1->size != b_bffr2->size) {
        return DT_false;
    }

    return (memcmp(b_bffr1->mem, b_bffr2->mem, b_bffr1->size) == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrCmpChecked(
    const DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2, DT_bool *pRslt) {
    if (!DT_ByteBffrIsValid(b_bffr1) || !DT_ByteBffrIsValid(b_bffr2) ||
        !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_ByteBffrCmpUnchecked(b_bffr1, b_bffr2);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrExtendUnchecked(DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2) {
    ASSERT_INVARIANT_EXPR(b_bffr1);
    ASSERT_INVARIANT_EXPR(b_bffr2);

    if (DT_BYTE_BFFR_MAX_SIZE - b_bffr1->size < b_bffr2->size) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    DT_size new_size = b_bffr1->size + b_bffr2->size, old_size = b_bffr1->size;
    PRP_Result code = DT_ByteBffrChangeSizeUnchecked(b_bffr1, new_size);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(b_bffr1->mem + old_size, b_bffr2->mem, b_bffr2->size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrExtendChecked(DT_ByteBffr *b_bffr1, const DT_ByteBffr *b_bffr2) {
    if (!DT_ByteBffrIsValid(b_bffr1) || !DT_ByteBffrIsValid(b_bffr2)) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ByteBffrExtendUnchecked(b_bffr1, b_bffr2);
}

PRP_FN_API DT_void PRP_FN_CALL
DT_ByteBffrSwapRegionUnchecked(DT_ByteBffr *b_bffr, DT_size ofs1, DT_size ofs2,
                               DT_size size, DT_void *pSwap_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(pSwap_bffr != DT_null);
    DIAG_ASSERT(ofs1 < b_bffr->size);
    DIAG_ASSERT(b_bffr->size - ofs1 >= size);
    DIAG_ASSERT(ofs2 < b_bffr->size);
    DIAG_ASSERT(b_bffr->size - ofs2 >= size);
    DIAG_ASSERT(ofs1 > ofs2 + size || ofs2 > ofs1 + size);

    if (ofs1 == ofs2) {
        return;
    }

    DT_u8 *region1 = b_bffr->mem + ofs1;
    DT_u8 *region2 = b_bffr->mem + ofs2;

    memcpy(pSwap_bffr, region1, size);
    memcpy(region1, region2, size);
    memcpy(region2, pSwap_bffr, size);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrSwapRegionChecked(DT_ByteBffr *b_bffr, DT_size ofs1, DT_size ofs2,
                             DT_size size, DT_void *pSwap_bffr) {
    if (!DT_ByteBffrIsValid(b_bffr) || !pSwap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs1 >= b_bffr->size || b_bffr->size - ofs1 < size ||
        ofs2 >= b_bffr->size || b_bffr->size - ofs2 < size) {
        return PRP_ERR_OOB;
    }
    if (ofs1 < ofs2 + size && ofs2 < ofs1 + size) {
        return PRP_ERR_UNSUPPORTED;
    }

    DT_ByteBffrSwapRegionUnchecked(b_bffr, ofs1, ofs2, size, pSwap_bffr);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_ByteBffrClearUnchecked(DT_ByteBffr *b_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);

    memset(b_bffr->mem, 0, b_bffr->size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrClearChecked(DT_ByteBffr *b_bffr) {
    if (!DT_ByteBffrIsValid(b_bffr)) {
        return PRP_ERR_INV_ARG;
    }

    DT_ByteBffrClearUnchecked(b_bffr);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrReserveUnchecked(DT_ByteBffr *b_bffr, DT_size ofs, DT_size size) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(ofs <= b_bffr->size);
    DIAG_ASSERT(DT_BYTE_BFFR_MAX_SIZE - ofs >= size);

    if (b_bffr->size - ofs >= size) {
        return PRP_OK;
    }
    DT_size new_size = size + ofs;
    return DT_ByteBffrChangeSizeUnchecked(b_bffr, new_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ByteBffrReserveChecked(DT_ByteBffr *b_bffr,
                                                            DT_size ofs,
                                                            DT_size size) {
    if (!DT_ByteBffrIsValid(b_bffr)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs > b_bffr->size) {
        return PRP_ERR_OOB;
    }
    if (DT_BYTE_BFFR_MAX_SIZE - ofs < size) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    return DT_ByteBffrReserveUnchecked(b_bffr, ofs, size);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrChangeSizeUnchecked(DT_ByteBffr *b_bffr, DT_size new_size) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    DIAG_ASSERT(new_size > 0);

    if (b_bffr->size == DT_BYTE_BFFR_MAX_SIZE) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (b_bffr->size == new_size) {
        return PRP_OK;
    }
    DT_u8 *mem = realloc(b_bffr->mem, new_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    if (new_size > b_bffr->size) {
        memset(mem + b_bffr->size, 0, new_size - b_bffr->size);
    }
    b_bffr->mem = mem;
    b_bffr->size = new_size;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ByteBffrChangeSizeChecked(DT_ByteBffr *b_bffr, DT_size new_size) {
    if (!DT_ByteBffrIsValid(b_bffr) || !new_size) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ByteBffrChangeSizeUnchecked(b_bffr, new_size);
}
