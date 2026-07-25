#include "Arena.h"
#include "Diagnostics/Assert.h"
#include <string.h>

struct _Arena {
    PRP_Size size;
    PRP_Size ofs;
    PRP_U8 mem[];
};

#define MAX_ALLOCABLE_SIZE (PRP_SIZE_MAX - sizeof(MEM_Arena))

#define ASSERT_INVARIANT_EXPR(arena)                                           \
    DIAG_ASSERT_MSG(MEM_ArenaIsValid(arena),                                   \
                    "The given arena is either NULL, or is corrupted.")

PRP_FN_API PRP_Bool PRP_FN_CALL MEM_ArenaIsValid(const MEM_Arena *arena) {
    return (arena != NULL && arena->size > 0 &&
            arena->size <= MAX_ALLOCABLE_SIZE && arena->ofs <= arena->size);
}

PRP_FN_API PRP_Size PRP_FN_CALL MEM_ArenaMaxSize(void) {
    return MAX_ALLOCABLE_SIZE;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCreateUnchecked(PRP_Size size,
                                                           MEM_Arena **pArena) {
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(size <= MAX_ALLOCABLE_SIZE);
    DIAG_ASSERT(pArena != NULL);

    MEM_Arena *arena = malloc(sizeof(MEM_Arena) + size);
    if (!arena) {
        return PRP_ERR_OOM;
    }
    arena->size = size;
    arena->ofs = 0;
    *pArena = arena;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCreateChecked(PRP_Size size,
                                                         MEM_Arena **pArena) {
    if (!size || size > MAX_ALLOCABLE_SIZE || !pArena) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaCreateUnchecked(size, pArena);
}

PRP_FN_API void PRP_FN_CALL MEM_ArenaDeleteUnchecked(MEM_Arena **pArena) {
    DIAG_ASSERT(pArena != NULL);
    DIAG_ASSERT(*pArena != NULL);

    MEM_Arena *arena = *pArena;
    free(arena);
    *pArena = NULL;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaDeleteChecked(MEM_Arena **pArena) {
    if (!pArena || !(*pArena)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_ArenaDeleteUnchecked(pArena);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaAllocUnchecked(MEM_Arena *arena,
                                                          PRP_Size size,
                                                          void **pDest) {
    ASSERT_INVARIANT_EXPR(arena);
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(pDest != NULL);

    if (size > arena->size - arena->ofs) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    *pDest = ptr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaAllocChecked(MEM_Arena *arena,
                                                        PRP_Size size,
                                                        void **pDest) {
    if (!MEM_ArenaIsValid(arena) || !size || !pDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaAllocUnchecked(arena, size, pDest);
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCallocUnchecked(MEM_Arena *arena,
                                                           PRP_Size size,
                                                           void **pDest) {
    ASSERT_INVARIANT_EXPR(arena);
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(pDest != NULL);

    if (size > arena->size - arena->ofs) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    memset(ptr, 0, size);
    *pDest = ptr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCallocChecked(MEM_Arena *arena,
                                                         PRP_Size size,
                                                         void **pDest) {
    if (!MEM_ArenaIsValid(arena) || !size || !pDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaCallocUnchecked(arena, size, pDest);
}

PRP_FN_API void PRP_FN_CALL MEM_ArenaResetUnchecked(MEM_Arena *arena) {
    ASSERT_INVARIANT_EXPR(arena);

    arena->ofs = 0;
#ifdef PRP_DEBUG_MODE
    memset(arena->mem, 0, arena->size);
#endif
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaResetChecked(MEM_Arena *arena) {
    if (!MEM_ArenaIsValid(arena)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_ArenaResetUnchecked(arena);

    return PRP_OK;
}
