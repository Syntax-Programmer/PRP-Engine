#include "Arena.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Arena {
    DT_size size;
    DT_size ofs;
    DT_u8 mem[];
};

#define MAX_ALLOCABLE_SIZE (DT_SIZE_MAX - sizeof(MEM_Arena))

#define ASSERT_INVARIANT_EXPR(arena)                                           \
    DIAG_ASSERT_MSG(MEM_ArenaIsValid(arena),                                   \
                    "The given arena is either DT_null, or is corrupted.")

PRP_FN_API DT_bool PRP_FN_CALL MEM_ArenaIsValid(const MEM_Arena *arena) {
    return (arena != DT_null && arena->size > 0 &&
            arena->size <= MAX_ALLOCABLE_SIZE && arena->ofs <= arena->size);
}

PRP_FN_API DT_size PRP_FN_CALL MEM_ArenaMaxSize(DT_void) {
    return MAX_ALLOCABLE_SIZE;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCreateUnchecked(DT_size size,
                                                           MEM_Arena **out) {
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(size <= MAX_ALLOCABLE_SIZE);
    DIAG_ASSERT(out != DT_null);

    MEM_Arena *arena = malloc(sizeof(MEM_Arena) + size);
    if (!arena) {
        return PRP_ERR_OOM;
    }
    arena->size = size;
    arena->ofs = 0;
    *out = arena;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCreateChecked(DT_size size,
                                                         MEM_Arena **out) {
    if (!size || size > MAX_ALLOCABLE_SIZE || !out) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaCreateUnchecked(size, out);
}

PRP_FN_API DT_void PRP_FN_CALL MEM_ArenaDeleteUnchecked(MEM_Arena **pArena) {
    DIAG_ASSERT(pArena != DT_null);
    DIAG_ASSERT(*pArena != DT_null);

    MEM_Arena *arena = *pArena;
    free(arena);
    *pArena = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaDeleteChecked(MEM_Arena **pArena) {
    if (!pArena || !(*pArena)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_ArenaDeleteUnchecked(pArena);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaAllocUnchecked(MEM_Arena *arena,
                                                          DT_size size,
                                                          DT_void **dest) {
    ASSERT_INVARIANT_EXPR(arena);
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(dest != DT_null);

    if (size > arena->size - arena->ofs) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    *dest = ptr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaAllocChecked(MEM_Arena *arena,
                                                        DT_size size,
                                                        DT_void **dest) {
    if (!MEM_ArenaIsValid(arena) || !size || !dest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaAllocUnchecked(arena, size, dest);
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCallocUnchecked(MEM_Arena *arena,
                                                           DT_size size,
                                                           DT_void **dest) {
    ASSERT_INVARIANT_EXPR(arena);
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(dest != DT_null);

    if (size > arena->size - arena->ofs) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    memset(ptr, 0, size);
    *dest = ptr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCallocChecked(MEM_Arena *arena,
                                                         DT_size size,
                                                         DT_void **dest) {
    if (!MEM_ArenaIsValid(arena) || !size || !dest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_ArenaCallocUnchecked(arena, size, dest);
}

PRP_FN_API DT_void PRP_FN_CALL MEM_ArenaResetUnchecked(MEM_Arena *arena) {
    ASSERT_INVARIANT_EXPR(arena);

    arena->ofs = 0;
#if !defined(PRP_NDEBUG)
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
