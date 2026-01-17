#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

/**
 * Arena, is an implementation of an fixed size arena allocator.
 * Its size is predetermined at the time of creation and can allocate memory
 * from that arena very quickly.
 * You cannot free individual allocations. All of the allocation will be freed
 * at once through the arena.
 */
typedef struct _Arena MEM_Arena;

/**
 * Returns the max size that an arena can have.
 *
 * @return The max size that an arena can have.
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_ArenaMaxSize(DT_void);
/**
 * Creates the arena with the given size.
 *
 * @param size: The max size(in bytes) of the arena.
 *
 * @retuns The pointer to the arena.
 */
PRP_FN_API MEM_Arena *PRP_FN_CALL MEM_ArenaCreate(DT_size size);
/**
 * Deletes the arena and sets the original MEM_Arena * to DT_null to prevent
 * use after free bugs.
 *
 * @param pArena: The pointer to the arena pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pArena is DT_null or the arena it points
 * to is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL MEM_ArenaDelete(MEM_Arena **pArena);
/**
 * Allocates a section of memory from the arena, the allocated chunk may contain
 * junk data, so caution is adviced.
 *
 * @param arena: The arena to allocate the chunk of mem from.
 * @prarm size: The size of the mem chunk to allocate.
 *
 * @return DT_null if the parameters are invalid in any way, DT_null if size
 * is too big for the current space left in the arena, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaAlloc(MEM_Arena *arena, DT_size size);
/**
 * Allocates a section of memory from the arena, the allocated region is all
 * preset to 0.
 *
 * @param arena: The arena to allocate the chunk of mem from.
 * @prarm size: The size of the mem chunk to allocate.
 *
 * @return DT_null if the parameters are invalid in any way, DT_null if size
 * is too big for the current space left in the arena, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_void *PRP_FN_CALL MEM_ArenaCalloc(MEM_Arena *arena, DT_size size);
/**
 * Resets the arena, which invalidates all existing alloations and empties the
 * entire arena.
 *
 * @param arena: The arena to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the arena is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL MEM_ArenaReset(MEM_Arena *arena);

#ifdef __cplusplus
}
#endif
