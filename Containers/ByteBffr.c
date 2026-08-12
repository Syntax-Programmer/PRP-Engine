#include "ByteBffr.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct CONT_ByteBffr {
    PRP_Size size;
    PRP_U8 *pMem;
};

#define ASSERT_INVARIANT_EXPR(pB_bffr)                                         \
    PRP_DIAG_ASSERT_MSG(                                                       \
        CONT_ByteBffrIsValid(pB_bffr),                                         \
        "The given byte buffer is either NULL, or is corrupted.")

PRP_API
PRP_Bool PRP_CALL CONT_ByteBffrIsValid(const CONT_ByteBffr *pB_bffr) {
    return (pB_bffr != NULL && pB_bffr->pMem != NULL && pB_bffr->size > 0);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCreateUnchecked(PRP_Size size, CONT_ByteBffr **ppB_bffr) {
    PRP_DIAG_ASSERT_MSG(size > 0, "The size of the byte buffer must be > 0.");
    PRP_DIAG_ASSERT(ppB_bffr != NULL);

    *ppB_bffr = NULL;
    CONT_ByteBffr *pB_bffr = malloc(sizeof(CONT_ByteBffr));
    if (!pB_bffr) {
        return PRP_ERR_OOM;
    }
    pB_bffr->pMem = calloc(1, size);
    if (!pB_bffr->pMem) {
        free(pB_bffr);
        return PRP_ERR_OOM;
    }
    pB_bffr->size = size;

    *ppB_bffr = pB_bffr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCreateChecked(PRP_Size size, CONT_ByteBffr **ppB_bffr) {
    if (!size || !ppB_bffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrCreateUnchecked(size, ppB_bffr);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrCloneUnchecked(
    const CONT_ByteBffr *pB_bffr, CONT_ByteBffr **ppB_bffr) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT(ppB_bffr != NULL);

    PRP_Result code = CONT_ByteBffrCreateUnchecked(pB_bffr->size, ppB_bffr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_ByteBffr *pCpy = *ppB_bffr;
    memcpy(pCpy->pMem, pB_bffr->pMem, pB_bffr->size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrCloneChecked(
    const CONT_ByteBffr *pB_bffr, CONT_ByteBffr **ppB_bffr) {
    if (!CONT_ByteBffrIsValid(pB_bffr) || !ppB_bffr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrCloneUnchecked(pB_bffr, ppB_bffr);
}

PRP_API void PRP_CALL CONT_ByteBffrDeleteUnchecked(CONT_ByteBffr **ppB_bffr) {
    PRP_DIAG_ASSERT(ppB_bffr != NULL);
    PRP_DIAG_ASSERT_MSG(*ppB_bffr != NULL && (*ppB_bffr)->pMem != NULL,
                        "The given *ppB_brr is invalid.");

    CONT_ByteBffr *pB_bffr = *ppB_bffr;

    free(pB_bffr->pMem);

#ifdef PRP_DEBUG_MODE
    pB_bffr->pMem = NULL;
    pB_bffr->size = 0;
#endif

    free(pB_bffr);
    *ppB_bffr = NULL;
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrDeleteChecked(CONT_ByteBffr **ppB_bffr) {
    if (!ppB_bffr || !(*ppB_bffr) || !(*ppB_bffr)->pMem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ByteBffrDeleteUnchecked(ppB_bffr);

    return PRP_OK;
}

PRP_API const void *PRP_CALL
CONT_ByteBffrRawUnchecked(const CONT_ByteBffr *pB_bffr, PRP_Size *pSize) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT(pSize != NULL);

    *pSize = pB_bffr->size;

    return pB_bffr->pMem;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrRawChecked(
    const CONT_ByteBffr *pB_bffr, PRP_Size *pSize, void **pRaw) {
    if (!CONT_ByteBffrIsValid(pB_bffr) || !pSize || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pSize = pB_bffr->size;
    *pRaw = pB_bffr->pMem;

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL CONT_ByteBffrSize(const CONT_ByteBffr *pB_bffr) {
    ASSERT_INVARIANT_EXPR(pB_bffr);

    return pB_bffr->size;
}

PRP_API void *PRP_CALL CONT_ByteBffrGetUnchecked(const CONT_ByteBffr *pB_bffr,
                                                 PRP_Size ofs) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT_MSG(ofs < pB_bffr->size, "The offset is out of bounds.");

    return pB_bffr->pMem + ofs;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrGetChecked(
    const CONT_ByteBffr *pB_bffr, PRP_Size ofs, void **ppDest) {
    if (!CONT_ByteBffrIsValid(pB_bffr) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= pB_bffr->size) {
        return PRP_ERR_OOB;
    }

    *ppDest = CONT_ByteBffrGetUnchecked(pB_bffr, ofs);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrUploadUnchecked(CONT_ByteBffr *pB_bffr,
                                                   PRP_Size ofs, PRP_Size size,
                                                   void *pData) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT_MSG(ofs < pB_bffr->size, "The offset is out of bounds.");
    PRP_DIAG_ASSERT_MSG(
        pB_bffr->size - ofs >= size,
        "The pData from the given ofs overflows the byte buffer.");
    PRP_DIAG_ASSERT(pData != NULL);

    memcpy(pB_bffr->pMem + ofs, pData, size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrUploadChecked(CONT_ByteBffr *pB_bffr,
                                                       PRP_Size ofs,
                                                       PRP_Size size,
                                                       void *pData) {
    if (!CONT_ByteBffrIsValid(pB_bffr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= pB_bffr->size || pB_bffr->size - ofs < size) {
        return PRP_ERR_OOB;
    }

    CONT_ByteBffrUploadUnchecked(pB_bffr, ofs, size, pData);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrCopyUnchecked(const CONT_ByteBffr *pB_bffr1,
                                                 PRP_Size ofs1,
                                                 CONT_ByteBffr *pB_bffr2,
                                                 PRP_Size ofs2, PRP_Size size) {
    ASSERT_INVARIANT_EXPR(pB_bffr1);
    ASSERT_INVARIANT_EXPR(pB_bffr2);
    PRP_DIAG_ASSERT_MSG(ofs1 < pB_bffr1->size,
                        "The offset is out of bounds for byte buffer 1.");
    PRP_DIAG_ASSERT_MSG(
        pB_bffr1->size - ofs1 >= size,
        "The pData from the given ofs overflows the byte buffer 1.");
    PRP_DIAG_ASSERT_MSG(ofs2 < pB_bffr2->size,
                        "The offset is out of bounds for byte buffer 2.");
    PRP_DIAG_ASSERT_MSG(
        pB_bffr2->size - ofs2 >= size,
        "The pData from the given ofs overflows the byte buffer 2.");

    memcpy(pB_bffr2->pMem + ofs2, pB_bffr1->pMem + ofs1, size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrCopyChecked(
    const CONT_ByteBffr *pB_bffr1, PRP_Size ofs1, CONT_ByteBffr *pB_bffr2,
    PRP_Size ofs2, PRP_Size size) {
    if (!CONT_ByteBffrIsValid(pB_bffr1) || !CONT_ByteBffrIsValid(pB_bffr2)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs1 >= pB_bffr1->size || pB_bffr1->size - ofs1 < size ||
        ofs2 >= pB_bffr2->size || pB_bffr2->size - ofs2 < size) {
        return PRP_ERR_OOB;
    }

    CONT_ByteBffrCopyUnchecked(pB_bffr1, ofs1, pB_bffr2, ofs2, size);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrFillUnchecked(CONT_ByteBffr *pB_bffr,
                                                 PRP_Size ofs, PRP_Size size,
                                                 PRP_U8 byte) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT_MSG(ofs < pB_bffr->size, "The offset is out of bounds.");
    PRP_DIAG_ASSERT_MSG(
        pB_bffr->size - ofs >= size,
        "The pData from the given ofs overflows the byte buffer.");

    memset(pB_bffr->pMem + ofs, byte, size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrFillChecked(CONT_ByteBffr *pB_bffr,
                                                     PRP_Size ofs,
                                                     PRP_Size size,
                                                     PRP_U8 byte) {
    if (!CONT_ByteBffrIsValid(pB_bffr)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs >= pB_bffr->size || pB_bffr->size - ofs < size) {
        return PRP_ERR_OOB;
    }

    CONT_ByteBffrFillUnchecked(pB_bffr, ofs, size, byte);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL CONT_ByteBffrCmpUnchecked(
    const CONT_ByteBffr *pB_bffr1, const CONT_ByteBffr *pB_bffr2) {
    ASSERT_INVARIANT_EXPR(pB_bffr1);
    ASSERT_INVARIANT_EXPR(pB_bffr2);

    if (pB_bffr1->size != pB_bffr2->size) {
        return PRP_False;
    }

    return (memcmp(pB_bffr1->pMem, pB_bffr2->pMem, pB_bffr1->size) == 0);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrCmpChecked(const CONT_ByteBffr *pB_bffr1,
                        const CONT_ByteBffr *pB_bffr2, PRP_Bool *pRslt) {
    if (!CONT_ByteBffrIsValid(pB_bffr1) || !CONT_ByteBffrIsValid(pB_bffr2) ||
        !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_ByteBffrCmpUnchecked(pB_bffr1, pB_bffr2);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrExtendUnchecked(
    CONT_ByteBffr *pB_bffr1, const CONT_ByteBffr *pB_bffr2) {
    ASSERT_INVARIANT_EXPR(pB_bffr1);
    ASSERT_INVARIANT_EXPR(pB_bffr2);

    if (CONT_BYTE_BFFR_MAX_SIZE - pB_bffr1->size < pB_bffr2->size) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_size = pB_bffr1->size + pB_bffr2->size,
             old_size = pB_bffr1->size;
    PRP_Result code = CONT_ByteBffrChangeSizeUnchecked(pB_bffr1, new_size);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(pB_bffr1->pMem + old_size, pB_bffr2->pMem, pB_bffr2->size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrExtendChecked(
    CONT_ByteBffr *pB_bffr1, const CONT_ByteBffr *pB_bffr2) {
    if (!CONT_ByteBffrIsValid(pB_bffr1) || !CONT_ByteBffrIsValid(pB_bffr2)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrExtendUnchecked(pB_bffr1, pB_bffr2);
}

PRP_API void PRP_CALL CONT_ByteBffrSwapRegionUnchecked(CONT_ByteBffr *pB_bffr,
                                                       PRP_Size ofs1,
                                                       PRP_Size ofs2,
                                                       PRP_Size size,
                                                       void *pSwap_bffr) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT(pSwap_bffr != NULL);
    PRP_DIAG_ASSERT_MSG(ofs1 < pB_bffr->size, "The offset 1 is out of bounds.");
    PRP_DIAG_ASSERT_MSG(
        pB_bffr->size - ofs1 >= size,
        "The pData from the given ofs 1 overflows the byte buffer.");
    PRP_DIAG_ASSERT_MSG(ofs2 < pB_bffr->size, "The offset 2 is out of bounds.");
    PRP_DIAG_ASSERT_MSG(
        pB_bffr->size - ofs2 >= size,
        "The pData from the given ofs 2 overflows the byte buffer.");
    PRP_DIAG_ASSERT_MSG(ofs1 + size <= ofs2 || ofs2 + size <= ofs1,
                        "The swap regions must not overlap.");

    if (ofs1 == ofs2) {
        return;
    }

    PRP_U8 *pRegion1 = pB_bffr->pMem + ofs1;
    PRP_U8 *pRegion2 = pB_bffr->pMem + ofs2;

    memcpy(pSwap_bffr, pRegion1, size);
    memcpy(pRegion1, pRegion2, size);
    memcpy(pRegion2, pSwap_bffr, size);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrSwapRegionChecked(CONT_ByteBffr *pB_bffr, PRP_Size ofs1,
                               PRP_Size ofs2, PRP_Size size, void *pSwap_bffr) {
    if (!CONT_ByteBffrIsValid(pB_bffr) || !pSwap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs1 >= pB_bffr->size || pB_bffr->size - ofs1 < size ||
        ofs2 >= pB_bffr->size || pB_bffr->size - ofs2 < size) {
        return PRP_ERR_OOB;
    }
    if (ofs1 < ofs2 + size && ofs2 < ofs1 + size) {
        return PRP_ERR_UNSUPPORTED;
    }

    CONT_ByteBffrSwapRegionUnchecked(pB_bffr, ofs1, ofs2, size, pSwap_bffr);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_ByteBffrClearUnchecked(CONT_ByteBffr *pB_bffr) {
    ASSERT_INVARIANT_EXPR(pB_bffr);

    memset(pB_bffr->pMem, 0, pB_bffr->size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrClearChecked(CONT_ByteBffr *pB_bffr) {
    if (!CONT_ByteBffrIsValid(pB_bffr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ByteBffrClearUnchecked(pB_bffr);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrReserveUnchecked(
    CONT_ByteBffr *pB_bffr, PRP_Size ofs, PRP_Size size) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT_MSG(ofs <= pB_bffr->size, "The offset is out of bounds.");
    PRP_DIAG_ASSERT_MSG(
        CONT_BYTE_BFFR_MAX_SIZE - ofs >= size,
        "The pData from the given ofs exceedsw the max size of a byte buffer.");

    if (pB_bffr->size - ofs >= size) {
        return PRP_OK;
    }
    PRP_Size new_size = size + ofs;
    return CONT_ByteBffrChangeSizeUnchecked(pB_bffr, new_size);
}

PRP_API PRP_Result PRP_CALL CONT_ByteBffrReserveChecked(CONT_ByteBffr *pB_bffr,
                                                        PRP_Size ofs,
                                                        PRP_Size size) {
    if (!CONT_ByteBffrIsValid(pB_bffr)) {
        return PRP_ERR_INV_ARG;
    }
    if (ofs > pB_bffr->size) {
        return PRP_ERR_OOB;
    }
    if (CONT_BYTE_BFFR_MAX_SIZE - ofs < size) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    return CONT_ByteBffrReserveUnchecked(pB_bffr, ofs, size);
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrChangeSizeUnchecked(CONT_ByteBffr *pB_bffr, PRP_Size new_size) {
    ASSERT_INVARIANT_EXPR(pB_bffr);
    PRP_DIAG_ASSERT_MSG(new_size > 0,
                        "The new size of byte buffer must be > 0.");

    if (pB_bffr->size == new_size) {
        return PRP_OK;
    }
    if (pB_bffr->size == CONT_BYTE_BFFR_MAX_SIZE) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    PRP_U8 *pMem = realloc(pB_bffr->pMem, new_size);
    if (!pMem) {
        return PRP_ERR_OOM;
    }
    if (new_size > pB_bffr->size) {
        memset(pMem + pB_bffr->size, 0, new_size - pB_bffr->size);
    }
    pB_bffr->pMem = pMem;
    pB_bffr->size = new_size;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL
CONT_ByteBffrChangeSizeChecked(CONT_ByteBffr *pB_bffr, PRP_Size new_size) {
    if (!CONT_ByteBffrIsValid(pB_bffr) || !new_size) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ByteBffrChangeSizeUnchecked(pB_bffr, new_size);
}
