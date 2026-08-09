#include "Arena.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct MEM_Arena {
    PRP_Size size;
    PRP_Size ofs;
    PRP_U8 mem[];
};

#define MAX_ALLOCABLE_SIZE (PRP_SIZE_MAX - sizeof(MEM_Arena))

#define ASSERT_INVARIANT_EXPR(pArena)                                          \
    PRP_DIAG_ASSERT_MSG(MEM_ArenaIsValid(pArena),                              \
                        "The given pArena is invalid.")

PRP_API PRP_Bool PRP_CALL MEM_ArenaIsValid(const MEM_Arena *pArena) {
    return (pArena != NULL && pArena->size > 0 &&
            pArena->size <= MAX_ALLOCABLE_SIZE && pArena->ofs <= pArena->size);
}

PRP_API PRP_Size PRP_CALL MEM_ArenaMaxSize(void) { return MAX_ALLOCABLE_SIZE; }

PRP_API PRP_Result PRP_CALL MEM_ArenaCreateUnchecked(PRP_Size size,
                                                     MEM_Arena **ppArena) {
    PRP_DIAG_ASSERT_MSG(size > 0, "The size of the arena must be > 0.");
    PRP_DIAG_ASSERT_MSG(size <= MAX_ALLOCABLE_SIZE,
                        "The size of the arena must be lower than the allowed "
                        "maximum");
    PRP_DIAG_ASSERT(ppArena != NULL);

    *ppArena = NULL;
    MEM_Arena *pArena = malloc(sizeof(MEM_Arena) + size);
    if (!pArena) {
        return PRP_ERR_OOM;
    }
    pArena->size = size;
    pArena->ofs = 0;
    *ppArena = pArena;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_ArenaCreateChecked(PRP_Size size,
                                                   MEM_Arena **ppArena) {
    if (!size || size > MAX_ALLOCABLE_SIZE || !ppArena) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaCreateUnchecked(size, ppArena);
}

PRP_API void PRP_CALL MEM_ArenaDeleteUnchecked(MEM_Arena **ppArena) {
    PRP_DIAG_ASSERT(ppArena != NULL);
    PRP_DIAG_ASSERT(*ppArena != NULL);

    MEM_Arena *pArena = *ppArena;
    free(pArena);
    *ppArena = NULL;
}

PRP_API PRP_Result PRP_CALL MEM_ArenaDeleteChecked(MEM_Arena **ppArena) {
    if (!ppArena || !(*ppArena)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_ArenaDeleteUnchecked(ppArena);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_ArenaAllocUnchecked(MEM_Arena *pArena,
                                                    PRP_Size size,
                                                    void **ppDest) {
    ASSERT_INVARIANT_EXPR(pArena);
    PRP_DIAG_ASSERT_MSG(size > 0, "The size of the allocation must be > 0.");
    PRP_DIAG_ASSERT(ppDest != NULL);

    if (size > pArena->size - pArena->ofs) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = pArena->mem + pArena->ofs;
    pArena->ofs += size;
    *ppDest = ptr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_ArenaAllocChecked(MEM_Arena *pArena,
                                                  PRP_Size size,
                                                  void **ppDest) {
    if (!MEM_ArenaIsValid(pArena) || !size || !ppDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaAllocUnchecked(pArena, size, ppDest);
}

PRP_API PRP_Result PRP_CALL MEM_ArenaCallocUnchecked(MEM_Arena *pArena,
                                                     PRP_Size size,
                                                     void **ppDest) {
    ASSERT_INVARIANT_EXPR(pArena);
    PRP_DIAG_ASSERT_MSG(size > 0, "The size of the allocation must be > 0.");
    PRP_DIAG_ASSERT(ppDest != NULL);

    if (size > pArena->size - pArena->ofs) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = pArena->mem + pArena->ofs;
    pArena->ofs += size;
    memset(ptr, 0, size);
    *ppDest = ptr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_ArenaCallocChecked(MEM_Arena *pArena,
                                                   PRP_Size size,
                                                   void **ppDest) {
    if (!MEM_ArenaIsValid(pArena) || !size || !ppDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaCallocUnchecked(pArena, size, ppDest);
}

PRP_API void PRP_CALL MEM_ArenaResetUnchecked(MEM_Arena *pArena) {
    ASSERT_INVARIANT_EXPR(pArena);

    pArena->ofs = 0;
#ifdef PRP_DEBUG_MODE
    memset(pArena->mem, 0, pArena->size);
#endif
}

PRP_API PRP_Result PRP_CALL MEM_ArenaResetChecked(MEM_Arena *pArena) {
    if (!MEM_ArenaIsValid(pArena)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_ArenaResetUnchecked(pArena);

    return PRP_OK;
}
