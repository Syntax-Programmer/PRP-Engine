#include "Arena.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct ALOC_Arena {
    PRP_Size size;
    PRP_Size ofs;
    PRP_U8 pMem[];
};

PRP_API PRP_Size PRP_CALL ALOC_ArenaMaxSize(void) {
    return PRP_SIZE_MAX - sizeof(ALOC_Arena);
}

PRP_API PRP_Bool PRP_CALL ALOC_ArenaIsValid(const ALOC_Arena *pArena) {
    return (pArena != NULL && pArena->size > 0 &&
            pArena->size <= ALOC_ArenaMaxSize() && pArena->ofs <= pArena->size);
}

#define ASSERT_VALIDITY(pArena)                                                \
    PRP_DIAG_ASSERT_MSG(ALOC_ArenaIsValid(pArena),                             \
                        "The given pArena is invalid.")

PRP_API PRP_Result PRP_CALL ALOC_ArenaCreateUnchecked(PRP_Size size,
                                                      ALOC_Arena **ppArena) {
    PRP_DIAG_ASSERT_MSG(size > 0,
                        "The size of the arena must be greater than 0.");
    PRP_DIAG_ASSERT_MSG(
        size > ALOC_ArenaMaxSize(),
        "The size of the arena must be less than the max allowed size.");
    PRP_DIAG_ASSERT(ppArena != nullptr);

    *ppArena = malloc(sizeof(ALOC_Arena) + size);
    if (!(*ppArena)) {
        *ppArena = nullptr;
        return PRP_ERR_OOM;
    }
    (*ppArena)->size = size;
    (*ppArena)->ofs = 0;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL ALOC_ArenaCreateChecked(PRP_Size size,
                                                    ALOC_Arena **ppArena) {
    if (size == 0 || size > ALOC_ArenaMaxSize() || !ppArena) {
        return PRP_ERR_INV_ARG;
    }

    return ALOC_ArenaCreateUnchecked(size, ppArena);
}

PRP_API void PRP_CALL ALOC_ArenaDestroyUnchecked(ALOC_Arena **ppArena) {
    PRP_DIAG_ASSERT(ppArena != nullptr);
    PRP_DIAG_ASSERT(*ppArena != nullptr);

    free(*ppArena);
    *ppArena = nullptr;
}

PRP_API PRP_Result PRP_CALL ALOC_ArenaDestroyChecked(ALOC_Arena **ppArena) {
    if (!ppArena || !(*ppArena)) {
        return PRP_ERR_INV_ARG;
    }

    ALOC_ArenaDestroyUnchecked(ppArena);

    return PRP_OK;
}

PRP_API void PRP_CALL ALOC_ArenaResetUnchecked(ALOC_Arena *pArena) {
    ASSERT_VALIDITY(pArena);

    pArena->ofs = 0;
#ifdef PRP_DEBUG_MODE
    memset(pArena->pMem, 0, pArena->size);
#endif
}

PRP_API PRP_Result PRP_CALL ALOC_ArenaResetChecked(ALOC_Arena *pArena) {
    if (!ALOC_ArenaIsValid(pArena)) {
        return PRP_ERR_INV_ARG;
    }

    ALOC_ArenaResetUnchecked(pArena);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL ALOC_ArenaAllocUnchecked(ALOC_Arena *pArena,
                                                     PRP_Size size,
                                                     PRP_Bool zero_initialize,
                                                     void **ppDest) {
    ASSERT_VALIDITY(pArena);
    PRP_DIAG_ASSERT_MSG(size > 0,
                        "The size of the allocation must be greater than 0.");
    PRP_DIAG_ASSERT(ppDest != nullptr);

    if (size > pArena->size - pArena->ofs) {
        *ppDest = nullptr;
        return PRP_ERR_RES_EXHAUSTED;
    }

    *ppDest = pArena->pMem + pArena->ofs;
    if (zero_initialize) {
        memset(*ppDest, 0, size);
    }
    pArena->ofs += size;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL ALOC_ArenaAllocChecked(ALOC_Arena *pArena,
                                                   PRP_Size size,
                                                   PRP_Bool zero_initialize,
                                                   void **ppDest) {
    if (!ALOC_ArenaIsValid(pArena) || size == 0 || !ppDest) {
        *ppDest = nullptr;
        return PRP_ERR_INV_ARG;
    }

    return ALOC_ArenaAllocUnchecked(pArena, size, zero_initialize, ppDest);
}

PRP_API PRP_Result PRP_CALL ALOC_ArenaAllocAlignedUnchecked(
    ALOC_Arena *pArena, PRP_Size size, PRP_Size align, PRP_Bool zero_initialize,
    void **ppDest) {
    ASSERT_VALIDITY(pArena);
    PRP_DIAG_ASSERT_MSG(size > 0,
                        "The size of the allocation must be greater than 0.");
    PRP_DIAG_ASSERT_MSG(align > 0 && (align & (align - 1)) == 0,
                        "The alignment must be a non zero power of 2.");
    PRP_DIAG_ASSERT(ppDest != nullptr);

    PRP_UIntPtr base = (PRP_UIntPtr)pArena->pMem;
    PRP_UIntPtr current = base + pArena->ofs;
    PRP_Size padding = (PRP_Size)((-(PRP_UIntPtr)current) & (align - 1));
    PRP_Size available = pArena->size - pArena->ofs;
    if (padding > available || size > available - padding) {
        *ppDest = nullptr;
        return PRP_ERR_RES_EXHAUSTED;
    }

    *ppDest = pArena->pMem + pArena->ofs + padding;
    if (zero_initialize) {
        memset(*ppDest, 0, size);
    }
    pArena->ofs += padding + size;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL
ALOC_ArenaAllocAlignedChecked(ALOC_Arena *pArena, PRP_Size size, PRP_Size align,
                              PRP_Bool zero_initialize, void **ppDest) {
    if (!ALOC_ArenaIsValid(pArena) || size == 0 || align == 0 ||
        (align & (align - 1)) != 0 || !ppDest) {
        *ppDest = nullptr;
        return PRP_ERR_INV_ARG;
    }

    return ALOC_ArenaAllocAlignedUnchecked(pArena, size, align, zero_initialize,
                                           ppDest);
}
