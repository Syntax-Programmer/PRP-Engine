#include "ByteBffr.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct CONT_ByteBffr {
    PRP_Size size;
    PRP_U8 *mem;
};

#define ASSERT_INVARIANT_EXPR(b_bffr)                                          \
    PRP_DIAG_ASSERT_MSG(                                                       \
        CONT_ByteBffrIsValid(b_bffr),                                          \
        "The given byte buffer is either NULL, or is corrupted.")

PRP_API
PRP_Bool PRP_CALL CONT_ByteBffrIsValid(const CONT_ByteBffr *b_bffr) {
    return (b_bffr != NULL && b_bffr->mem != NULL && b_bffr->size > 0);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCreateUnchecked(PRP_Size size, CONT_ByteBffr **pB_bffr) {
    PRP_DIAG_ASSERT(size > 0);
    PRP_DIAG_ASSERT(pB_bffr != NULL);

    CONT_ByteBffr *b_bffr = malloc(sizeof(CONT_ByteBffr));
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

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCreateChecked(PRP_Size size, CONT_ByteBffr **pB_bffr) {
    if (!size || !pB_bffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrCreateUnchecked(size, pB_bffr);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrCloneUnchecked(
    const CONT_ByteBffr *b_bffr, CONT_ByteBffr **pB_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(pB_bffr != NULL);

    PRP_Result code = CONT_ByteBffrCreateUnchecked(b_bffr->size, pB_bffr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_ByteBffr *cpy = *pB_bffr;
    memcpy(cpy->mem, b_bffr->mem, b_bffr->size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrCloneChecked(
    const CONT_ByteBffr *b_bffr, CONT_ByteBffr **pB_bffr) {
    if (!CONT_ByteBffrIsValid(b_bffr) || !pB_bffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrCloneUnchecked(b_bffr, pB_bffr);
}

PRP_API void PRP_CALL CONT_ByteBffrDeleteUnchecked(CONT_ByteBffr **pB_bffr) {
    PRP_DIAG_ASSERT(pB_bffr != NULL);
    PRP_DIAG_ASSERT(*pB_bffr != NULL && (*pB_bffr)->mem != NULL);

    CONT_ByteBffr *b_bffr = *pB_bffr;

    free(b_bffr->mem);

#ifdef PRP_DEBUG_MODE
    b_bffr->mem = NULL;
    b_bffr->size = 0;
#endif

    free(b_bffr);
    *pB_bffr = NULL;
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrDeleteChecked(CONT_ByteBffr **pB_bffr) {
    if (!pB_bffr || !(*pB_bffr) || !(*pB_bffr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ByteBffrDeleteUnchecked(pB_bffr);

    return PRP_OK;
}

PRP_API const void *PRP_CALL
CONT_ByteBffrRawUnchecked(const CONT_ByteBffr *b_bffr, PRP_Size *pSize) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(pSize != NULL);

    *pSize = b_bffr->size;

    return b_bffr->mem;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrRawChecked(const CONT_ByteBffr *b_bffr,
                                                    PRP_Size *pSize,
                                                    void **pRaw) {
    if (!CONT_ByteBffrIsValid(b_bffr) || !pSize || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pSize = b_bffr->size;
    *pRaw = b_bffr->mem;

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL CONT_ByteBffrSize(const CONT_ByteBffr *b_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);

    return b_bffr->size;
}

PRP_API void *PRP_CALL CONT_ByteBffrGetUnchecked(const CONT_ByteBffr *b_bffr,
                                                 PRP_Size ofs) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(ofs < b_bffr->size);

    return b_bffr->mem + ofs;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrGetChecked(const CONT_ByteBffr *b_bffr,
                                                    PRP_Size ofs,
                                                    void **ppDest) {
    if (!CONT_ByteBffrIsValid(b_bffr) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= b_bffr->size) {
        return PRP_ERR_OOB;
    }

    *ppDest = CONT_ByteBffrGetUnchecked(b_bffr, ofs);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrUploadUnchecked(CONT_ByteBffr *b_bffr,
                                                   PRP_Size ofs, PRP_Size size,
                                                   void *pData) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(ofs < b_bffr->size);
    PRP_DIAG_ASSERT(b_bffr->size - ofs >= size);
    PRP_DIAG_ASSERT(pData != NULL);

    memcpy(b_bffr->mem + ofs, pData, size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrUploadChecked(CONT_ByteBffr *b_bffr,
                                                       PRP_Size ofs,
                                                       PRP_Size size,
                                                       void *pData) {
    if (!CONT_ByteBffrIsValid(b_bffr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= b_bffr->size || b_bffr->size - ofs < size) {
        return PRP_ERR_OOB;
    }

    CONT_ByteBffrUploadUnchecked(b_bffr, ofs, size, pData);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrCopyUnchecked(const CONT_ByteBffr *b_bffr1,
                                                 PRP_Size ofs1,
                                                 CONT_ByteBffr *b_bffr2,
                                                 PRP_Size ofs2, PRP_Size size) {
    ASSERT_INVARIANT_EXPR(b_bffr1);
    ASSERT_INVARIANT_EXPR(b_bffr2);
    PRP_DIAG_ASSERT(ofs1 < b_bffr1->size);
    PRP_DIAG_ASSERT(ofs2 < b_bffr2->size);
    PRP_DIAG_ASSERT(b_bffr1->size - ofs1 >= size);
    PRP_DIAG_ASSERT(b_bffr2->size - ofs2 >= size);

    memcpy(b_bffr2->mem + ofs2, b_bffr1->mem + ofs1, size);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCopyChecked(const CONT_ByteBffr *b_bffr1, PRP_Size ofs1,
                         CONT_ByteBffr *b_bffr2, PRP_Size ofs2, PRP_Size size) {
    if (!CONT_ByteBffrIsValid(b_bffr1) || !CONT_ByteBffrIsValid(b_bffr2)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs1 >= b_bffr1->size || b_bffr1->size - ofs1 < size ||
        ofs2 >= b_bffr2->size || b_bffr2->size - ofs2 < size) {
        return PRP_ERR_OOB;
    }

    CONT_ByteBffrCopyUnchecked(b_bffr1, ofs1, b_bffr2, ofs2, size);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrFillUnchecked(CONT_ByteBffr *b_bffr,
                                                 PRP_Size ofs, PRP_Size size,
                                                 PRP_U8 byte) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(ofs < b_bffr->size);
    PRP_DIAG_ASSERT(b_bffr->size - ofs >= size);

    memset(b_bffr->mem + ofs, byte, size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrFillChecked(CONT_ByteBffr *b_bffr,
                                                     PRP_Size ofs,
                                                     PRP_Size size,
                                                     PRP_U8 byte) {
    if (!CONT_ByteBffrIsValid(b_bffr)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= b_bffr->size || b_bffr->size - ofs < size) {
        return PRP_ERR_OOB;
    }

    CONT_ByteBffrFillUnchecked(b_bffr, ofs, size, byte);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_ByteBffrCmpUnchecked(
    const CONT_ByteBffr *b_bffr1, const CONT_ByteBffr *b_bffr2) {
    ASSERT_INVARIANT_EXPR(b_bffr1);
    ASSERT_INVARIANT_EXPR(b_bffr2);

    if (b_bffr1->size != b_bffr2->size) {
        return PRP_False;
    }

    return (memcmp(b_bffr1->mem, b_bffr2->mem, b_bffr1->size) == 0);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCmpChecked(const CONT_ByteBffr *b_bffr1,
                        const CONT_ByteBffr *b_bffr2, PRP_Bool *pRslt) {
    if (!CONT_ByteBffrIsValid(b_bffr1) || !CONT_ByteBffrIsValid(b_bffr2) ||
        !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_ByteBffrCmpUnchecked(b_bffr1, b_bffr2);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrExtendUnchecked(
    CONT_ByteBffr *b_bffr1, const CONT_ByteBffr *b_bffr2) {
    ASSERT_INVARIANT_EXPR(b_bffr1);
    ASSERT_INVARIANT_EXPR(b_bffr2);

    if (CONT_BYTE_BFFR_MAX_SIZE - b_bffr1->size < b_bffr2->size) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_size = b_bffr1->size + b_bffr2->size, old_size = b_bffr1->size;
    PRP_Result code = CONT_ByteBffrChangeSizeUnchecked(b_bffr1, new_size);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(b_bffr1->mem + old_size, b_bffr2->mem, b_bffr2->size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrExtendChecked(
    CONT_ByteBffr *b_bffr1, const CONT_ByteBffr *b_bffr2) {
    if (!CONT_ByteBffrIsValid(b_bffr1) || !CONT_ByteBffrIsValid(b_bffr2)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrExtendUnchecked(b_bffr1, b_bffr2);
}

PRP_API void PRP_CALL CONT_ByteBffrSwapRegionUnchecked(CONT_ByteBffr *b_bffr,
                                                       PRP_Size ofs1,
                                                       PRP_Size ofs2,
                                                       PRP_Size size,
                                                       void *pSwap_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(pSwap_bffr != NULL);
    PRP_DIAG_ASSERT(ofs1 < b_bffr->size);
    PRP_DIAG_ASSERT(b_bffr->size - ofs1 >= size);
    PRP_DIAG_ASSERT(ofs2 < b_bffr->size);
    PRP_DIAG_ASSERT(b_bffr->size - ofs2 >= size);
    PRP_DIAG_ASSERT(ofs1 > ofs2 + size || ofs2 > ofs1 + size);

    if (ofs1 == ofs2) {
        return;
    }

    PRP_U8 *region1 = b_bffr->mem + ofs1;
    PRP_U8 *region2 = b_bffr->mem + ofs2;

    memcpy(pSwap_bffr, region1, size);
    memcpy(region1, region2, size);
    memcpy(region2, pSwap_bffr, size);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrSwapRegionChecked(CONT_ByteBffr *b_bffr, PRP_Size ofs1,
                               PRP_Size ofs2, PRP_Size size, void *pSwap_bffr) {
    if (!CONT_ByteBffrIsValid(b_bffr) || !pSwap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs1 >= b_bffr->size || b_bffr->size - ofs1 < size ||
        ofs2 >= b_bffr->size || b_bffr->size - ofs2 < size) {
        return PRP_ERR_OOB;
    }
    if (ofs1 < ofs2 + size && ofs2 < ofs1 + size) {
        return PRP_ERR_UNSUPPORTED;
    }

    CONT_ByteBffrSwapRegionUnchecked(b_bffr, ofs1, ofs2, size, pSwap_bffr);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrClearUnchecked(CONT_ByteBffr *b_bffr) {
    ASSERT_INVARIANT_EXPR(b_bffr);

    memset(b_bffr->mem, 0, b_bffr->size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrClearChecked(CONT_ByteBffr *b_bffr) {
    if (!CONT_ByteBffrIsValid(b_bffr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ByteBffrClearUnchecked(b_bffr);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrReserveUnchecked(CONT_ByteBffr *b_bffr,
                                                          PRP_Size ofs,
                                                          PRP_Size size) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(ofs <= b_bffr->size);
    PRP_DIAG_ASSERT(CONT_BYTE_BFFR_MAX_SIZE - ofs >= size);

    if (b_bffr->size - ofs >= size) {
        return PRP_OK;
    }
    PRP_Size new_size = size + ofs;
    return CONT_ByteBffrChangeSizeUnchecked(b_bffr, new_size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrReserveChecked(CONT_ByteBffr *b_bffr,
                                                        PRP_Size ofs,
                                                        PRP_Size size) {
    if (!CONT_ByteBffrIsValid(b_bffr)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs > b_bffr->size) {
        return PRP_ERR_OOB;
    }
    if (CONT_BYTE_BFFR_MAX_SIZE - ofs < size) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    return CONT_ByteBffrReserveUnchecked(b_bffr, ofs, size);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrChangeSizeUnchecked(CONT_ByteBffr *b_bffr, PRP_Size new_size) {
    ASSERT_INVARIANT_EXPR(b_bffr);
    PRP_DIAG_ASSERT(new_size > 0);

    if (b_bffr->size == new_size) {
        return PRP_OK;
    }
    if (b_bffr->size == CONT_BYTE_BFFR_MAX_SIZE) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    PRP_U8 *mem = realloc(b_bffr->mem, new_size);
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

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrChangeSizeChecked(CONT_ByteBffr *b_bffr, PRP_Size new_size) {
    if (!CONT_ByteBffrIsValid(b_bffr) || !new_size) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrChangeSizeUnchecked(b_bffr, new_size);
}
