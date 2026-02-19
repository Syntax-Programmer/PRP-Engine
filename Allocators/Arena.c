#include "Arena.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Arena {
    DT_size size;
    DT_size ofs;
    DT_u8 mem[];
};

#define MAX_ALLOCABLE_SIZE (DT_SIZE_MAX - sizeof(MEM_Arena))

#define INVARIANT_EXPR(arena)                                                  \
    ((arena) != DT_null && (arena)->size > 0 &&                                \
     (arena)->size <= MAX_ALLOCABLE_SIZE && (arena)->ofs <= (arena)->size)
#define ASSERT_INVARIANT_EXPR(arena)                                           \
    DIAG_ASSERT_MSG(INVARIANT_EXPR(arena),                                     \
                    "The given arena is either DT_null, or is corrupted.")

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaGetLastErrCode(DT_void) {
    return last_err_code;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_ArenaMaxSize(DT_void) {
    return MAX_ALLOCABLE_SIZE;
}

PRP_FN_API MEM_Arena *PRP_FN_CALL MEM_ArenaCreateUnchecked(DT_size size) {
    DIAG_ASSERT(size > 0);
    DIAG_ASSERT(size <= MAX_ALLOCABLE_SIZE);

    MEM_Arena *arena = malloc(sizeof(MEM_Arena) + size);
    if (!arena) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    arena->size = size;
    arena->ofs = 0;

    return arena;
}

PRP_FN_API MEM_Arena *PRP_FN_CALL MEM_ArenaCreateChecked(DT_size size) {
    if (!(size > 0 && size <= MAX_ALLOCABLE_SIZE)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return MEM_ArenaCreateUnchecked(size);
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

PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaAllocUnchecked(MEM_Arena *arena,
                                                        DT_size size) {
    ASSERT_INVARIANT_EXPR(arena);
    DIAG_ASSERT(size > 0);

    if (size > arena->size - arena->ofs) {
        SET_LAST_ERR_CODE(PRP_ERR_RES_EXHAUSTED);
        return DT_null;
    }

    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaAllocChecked(MEM_Arena *arena,
                                                      DT_size size) {
    if (!INVARIANT_EXPR(arena) || !size) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return MEM_ArenaAllocUnchecked(arena, size);
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaCallocUnchecked(MEM_Arena *arena,
                                                         DT_size size) {
    ASSERT_INVARIANT_EXPR(arena);
    DIAG_ASSERT(size > 0);

    if (size > arena->size - arena->ofs) {
        SET_LAST_ERR_CODE(PRP_ERR_RES_EXHAUSTED);
        return DT_null;
    }

    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    memset(ptr, 0, size);

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaCallocChecked(MEM_Arena *arena,
                                                       DT_size size) {
    if (!INVARIANT_EXPR(arena) || !size) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return MEM_ArenaCallocUnchecked(arena, size);
}

PRP_FN_API DT_void PRP_FN_CALL MEM_ArenaResetUnchecked(MEM_Arena *arena) {
    ASSERT_INVARIANT_EXPR(arena);

    arena->ofs = 0;
#if !defined(PRP_NDEBUG)
    memset(arena->mem, 0, arena->size);
#endif
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaResetChecked(MEM_Arena *arena) {
    if (!INVARIANT_EXPR(arena)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_ArenaResetUnchecked(arena);

    return PRP_OK;
}
