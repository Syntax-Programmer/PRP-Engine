#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Utils/Defs.h"

/**
 * Arena, is an implementation of an fixed size arena allocator.
 * Its size is predetermined at the time of creation and can allocate memory
 * from that arena very quickly.
 * You cannot free individual allocations. All of the allocation will be freed
 * at once through the arena.
 */
typedef struct _Arena PRP_Arena;

/**
 * Creates the arena with the given size.
 *
 * @param size: The max size(in bytes) of the arena.
 *
 * @retuns The pointer to the arena.
 */
PRP_FN_API PRP_Arena *PRP_FN_CALL PRP_ArenaCreate(PRP_size size);
/**
 * Deletes the arena and sets the original PRP_Arena * to PRP_null to prevent
 * use after free bugs.
 *
 * @param pArena: The pointer to the arena pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pArena is PRP_null or the arena it points
 * to is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_ArenaDelete(PRP_Arena **pArena);
/**
 * Allocates a section of memory from the arena, the allocated chunk may contain
 * junk data, so caution is adviced.
 *
 * @param arena: The arena to allocate the chunk of mem from.
 * @prarm size: The size of the mem chunk to allocate.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if size is too big for the current space left in
 * the arena, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_void *PRP_FN_CALL PRP_ArenaAlloc(PRP_Arena *arena,
                                                PRP_size size);
/**
 * Allocates a section of memory from the arena, the allocated region is all
 * preset to 0.
 *
 * @param arena: The arena to allocate the chunk of mem from.
 * @prarm size: The size of the mem chunk to allocate.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if size is too big for the current space left in
 * the arena, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_void *PRP_FN_CALL PRP_ArenaCalloc(PRP_Arena *arena,
                                                 PRP_size size);
/**
 * Resets the arena, which invalidates all existing alloations and empties the
 * entire arena.
 *
 * @param arena: The arena to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the arena is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_ArenaReset(PRP_Arena *arena);

#ifdef __cplusplus
}
#endif
