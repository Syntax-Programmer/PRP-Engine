#include "Arena.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _Arena {
    DT_size size;
    DT_size ofs;
    DT_u8 mem[];
};

#define MAX_ALLOCABLE_SIZE (DT_SIZE_MAX - sizeof(DT_Arena))

PRP_FN_API DT_Arena *PRP_FN_CALL DT_ArenaCreate(DT_size size) {
    if (!size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Arena can't be made with size=0.");
        return DT_null;
    }
    if (size > MAX_ALLOCABLE_SIZE) {
        PRP_LOG_FN_CODE(PRP_FN_INT_OVERFLOW_ERROR,
                        "DT_Arena can only with the max size of: %zu bytes",
                        MAX_ALLOCABLE_SIZE);
        return DT_null;
    }

    DT_Arena *arena = malloc(sizeof(DT_Arena) + size);
    if (!arena) {
        PRP_LOG_FN_MALLOC_ERROR(arena);
        return DT_null;
    }
    arena->size = size;
    arena->ofs = 0;

    return arena;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArenaDelete(DT_Arena **pArena) {
    PRP_NULL_ARG_CHECK(pArena, PRP_FN_INV_ARG_ERROR);
    DT_Arena *arena = *pArena;
    PRP_NULL_ARG_CHECK(arena, PRP_FN_INV_ARG_ERROR);

    arena->size = arena->ofs = 0;
    free(arena);
    *pArena = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_void *PRP_FN_CALL DT_ArenaAlloc(DT_Arena *arena, DT_size size) {
    PRP_NULL_ARG_CHECK(arena, DT_null);
    if (!size) {
        PRP_LOG_FN_INV_ARG_ERROR(size);
        return DT_null;
    }

    if (size > arena->size - arena->ofs) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Arena has %zu bytes memory left. Cannot allocate memory "
            "of size: %zu bytes.",
            arena->size - arena->ofs, size);
        return DT_null;
    }
    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL DT_ArenaCalloc(DT_Arena *arena, DT_size size) {
    PRP_NULL_ARG_CHECK(arena, DT_null);
    if (!size) {
        PRP_LOG_FN_INV_ARG_ERROR(size);
        return DT_null;
    }

    if (size > arena->size - arena->ofs) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Arena has %zu bytes memory left. Cannot allocate memory "
            "of size: %zu bytes.",
            arena->size - arena->ofs, size);
        return DT_null;
    }
    DT_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    memset(ptr, 0, size);

    return ptr;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArenaReset(DT_Arena *arena) {
    PRP_NULL_ARG_CHECK(arena, PRP_FN_INV_ARG_ERROR);

    arena->ofs = 0;
    memset(arena->mem, 0, arena->size);

    return PRP_FN_SUCCESS;
}
