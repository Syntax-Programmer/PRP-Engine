#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * A fixed-size bump allocator.
 *
 * - Memory is allocated linearly.
 * - Individual frees are NOT supported.
 * - All allocations are invalidated on reset or delete.
 */
typedef struct ALOC_Arena ALOC_Arena;

/**
 * Queries the maximum logical size an arena can have.
 *
 * @return The theoretical max size of arena that is supported.
 */
PRP_API PRP_Size PRP_CALL ALOC_ArenaMaxSize(void);
/**
 * Checks whether the given pArena is structurally valid object.
 *
 * @param pArena The arena to check.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL ALOC_ArenaIsValid(const ALOC_Arena *pArena);

/**
 * Creates a new arena allocator.
 *
 * @param size Total size (in bytes) of the arena.
 * @param ppArena Output pointer that receives the created arena.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
[[nodiscard(
    "ALOC_ArenaCreateUnchecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaCreateUnchecked(PRP_Size size,
                                                      ALOC_Arena **ppArena);
/**
 * Creates a new arena allocator.
 *
 * @param size Total size (in bytes) of the arena.
 * @param ppArena Output pointer that receives the created arena.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "ALOC_ArenaCreateChecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaCreateChecked(PRP_Size size,
                                                    ALOC_Arena **ppArena);

/**
 * Destroys the arena and nullifies the object pointer.
 *
 * @param ppArena Pointer to the arena object.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_API void PRP_CALL ALOC_ArenaDestroyUnchecked(ALOC_Arena **ppArena);
/**
 * Destroys the arena and nullifies the object pointer.
 *
 * @param ppArena Pointer to the arena object.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if ppArena or *ppArena is nullptr.
 */
[[nodiscard(
    "ALOC_ArenaDestroyChecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaDestroyChecked(ALOC_Arena **ppArena);

/**
 * Resets and frees all allocations inside the arena all at once.
 *
 * @param pArena The arena object.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_API void PRP_CALL ALOC_ArenaResetUnchecked(ALOC_Arena *pArena);
/**
 * Resets and frees all allocations inside the arena all at once.
 *
 * @param pArena The arena object.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("ALOC_ArenaResetChecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaResetChecked(ALOC_Arena *pArena);

/**
 * Allocates a block of memory from the arena.
 *
 * @param pArena          The arena to allocate from.
 * @param size            The size (in bytes) to allocate.
 * @param zero_initialize If PRP_True, the allocated memory will be zero
 *                        initialized, uninitialized otherwise.
 * @param ppDest          Output pointer that receives the allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if available space in arena is insufficient.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 *
 * @warning
 * `ppDest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
[[nodiscard(
    "ALOC_ArenaAllocUnchecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaAllocUnchecked(ALOC_Arena *pArena,
                                                     PRP_Size size,
                                                     PRP_Bool zero_initialize,
                                                     void **ppDest);
/**
 * Allocates a block of memory from the arena.
 *
 * @param pArena          The arena to allocate from.
 * @param size            The size (in bytes) to allocate.
 * @param zero_initialize If PRP_True, the allocated memory will be zero
 *                        initialized, uninitialized otherwise.
 * @param ppDest          Output pointer that receives the allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if available space in arena is insufficient.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @warning
 * `ppDest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
[[nodiscard("ALOC_ArenaAllocChecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaAllocChecked(ALOC_Arena *pArena,
                                                   PRP_Size size,
                                                   PRP_Bool zero_initialize,
                                                   void **ppDest);

/**
 * Allocates a block of memory from the arena, aligned to the specified
 * alignment.
 *
 * @param pArena          The arena to allocate from.
 * @param size            The size (in bytes) to allocate.
 * @param align           The alignment for the allocation to follow.
 * @param zero_initialize If PRP_True, the allocated memory will be zero
 *                        initialized, uninitialized otherwise.
 * @param ppDest          Output pointer that receives the allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if available space in arena is insufficient.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 *
 * @warning
 * `ppDest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
[[nodiscard("ALOC_ArenaAllocAlignedUnchecked can fail and lead to undefined "
            "behavior.")]]
PRP_API PRP_Result PRP_CALL ALOC_ArenaAllocAlignedUnchecked(
    ALOC_Arena *pArena, PRP_Size size, PRP_Size align, PRP_Bool zero_initialize,
    void **ppDest);
/**
 * Allocates a block of memory from the arena, aligned to the specified
 * alignment.
 *
 * @param pArena          The arena to allocate from.
 * @param size            The size (in bytes) to allocate.
 * @param align           The alignment for the allocation to follow.
 * @param zero_initialize If PRP_True, the allocated memory will be zero
 *                        initialized, uninitialized otherwise.
 * @param ppDest          Output pointer that receives the allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if available space in arena is insufficient.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @warning
 * `ppDest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
[[nodiscard(
    "ALOC_ArenaAllocAlignedChecked can fail and lead to undefined behavior.")]]
PRP_API PRP_Result PRP_CALL
ALOC_ArenaAllocAlignedChecked(ALOC_Arena *pArena, PRP_Size size, PRP_Size align,
                              PRP_Bool zero_initialize, void **ppDest);

#ifdef __cplusplus
}
#endif
