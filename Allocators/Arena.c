#include "Arena.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Arena {
    DT_size size;
    DT_size ofs;
    DT_u8 mem[];
};

#define MAX_ALLOCABLE_SIZE (DT_SIZE_MAX - sizeof(MEM_Arena))

PRP_FN_API DT_size PRP_FN_CALL MEM_ArenaMaxSize(DT_void) {
    return MAX_ALLOCABLE_SIZE;
}

PRP_FN_API MEM_Arena *PRP_FN_CALL MEM_ArenaCreate(DT_size size) {
    DIAG_GUARD(size > 0 && size <= MAX_ALLOCABLE_SIZE, DT_null);

    MEM_Arena *arena = malloc(sizeof(MEM_Arena) + size);
    if (!arena) {
        return DT_null;
    }
    arena->size = size;
    arena->ofs = 0;

    return arena;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaDelete(MEM_Arena **pArena) {
    DIAG_GUARD(pArena != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(*pArena != DT_null, PRP_ERR_INV_ARG);

    MEM_Arena *arena = *pArena;
    free(arena);
    *pArena = DT_null;

    return PRP_OK;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaAlloc(MEM_Arena *arena, DT_size size) {
    DIAG_GUARD(arena != DT_null, DT_null);
    DIAG_GUARD(size > 0, DT_null);

    if (size > arena->size - arena->ofs) {
        return DT_null;
    }

    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaCalloc(MEM_Arena *arena,
                                                DT_size size) {
    DIAG_GUARD(arena != DT_null, DT_null);
    DIAG_GUARD(size > 0, DT_null);

    if (size > arena->size - arena->ofs) {
        return DT_null;
    }

    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    memset(ptr, 0, size);

    return ptr;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaReset(MEM_Arena *arena) {
    DIAG_GUARD(arena != DT_null, PRP_ERR_INV_ARG);

    arena->ofs = 0;
#if !defined(PRP_NDEBUG)
    memset(arena->mem, 0, arena->size);
#endif

    return PRP_OK;
}
