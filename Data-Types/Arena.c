#include "Arena.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _Arena {
    PRP_size size;
    PRP_size ofs;
    PRP_u8 mem[];
};

#define ARENA_VALIDITY_CHECK(arena, ret)                                       \
    do {                                                                       \
        if (!arena) {                                                          \
            PRP_LOG_FN_INV_ARG_ERROR(arena);                                   \
            return ret;                                                        \
        }                                                                      \
    } while (0)

PRP_FN_API DT_Arena *PRP_FN_CALL DT_ArenaCreate(PRP_size size) {
    if (!size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Arena can't be made with size=0.");
        return PRP_null;
    }

    DT_Arena *arena = malloc(sizeof(DT_Arena) + size);
    if (!arena) {
        PRP_LOG_FN_MALLOC_ERROR(arena);
        return PRP_null;
    }
    arena->size = size;
    arena->ofs = 0;

    return arena;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArenaDelete(DT_Arena **pArena) {
    if (!pArena) {
        PRP_LOG_FN_INV_ARG_ERROR(pArena);
        return PRP_FN_INV_ARG_ERROR;
    }
    DT_Arena *arena = *pArena;
    ARENA_VALIDITY_CHECK(arena, PRP_FN_INV_ARG_ERROR);

    arena->size = arena->ofs = 0;
    free(arena);
    *pArena = PRP_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_void *PRP_FN_CALL DT_ArenaAlloc(DT_Arena *arena, PRP_size size) {
    ARENA_VALIDITY_CHECK(arena, PRP_null);
    if (!size) {
        PRP_LOG_FN_INV_ARG_ERROR(size);
        return PRP_null;
    }

    if (arena->ofs + size > arena->size) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Arena has %zu bytes memory left. Cannot allocate memory "
            "of size: %zu bytes.");
        return PRP_null;
    }
    PRP_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;

    return ptr;
}

PRP_FN_API PRP_void *PRP_FN_CALL DT_ArenaCalloc(DT_Arena *arena,
                                                PRP_size size) {
    ARENA_VALIDITY_CHECK(arena, PRP_null);
    if (!size) {
        PRP_LOG_FN_INV_ARG_ERROR(size);
        return PRP_null;
    }

    if (arena->ofs + size > arena->size) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Arena has %zu bytes memory left. Cannot allocate memory "
            "of size: %zu bytes.");
        return PRP_null;
    }
    PRP_void *ptr = arena->mem + arena->ofs;
    arena->ofs += size;
    memset(ptr, 0, size);

    return ptr;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArenaReset(DT_Arena *arena) {
    ARENA_VALIDITY_CHECK(arena, PRP_FN_INV_ARG_ERROR);

    arena->ofs = 0;
    memset(arena->mem, 0, arena->size);

    return PRP_FN_SUCCESS;
}
