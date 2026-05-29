#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Utils/Defs.h"

/**
 * MEM_Arena
 *
 * A fixed-size arena allocator.
 *
 * - Memory is allocated linearly.
 * - Individual frees are NOT supported.
 * - All allocations are invalidated on reset or delete.
 *
 * This allocator is extremely fast but requires careful lifetime management.
 */
typedef struct _Arena MEM_Arena;

/**
 * Checks whether the given arena is structurally valid.
 *
 * @param arena Pointer to the arena.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL MEM_ArenaIsValid(const MEM_Arena *arena);

/**
 * Returns the maximum allocatable arena size in bytes.
 *
 * @return Maximum supported arena size.
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_ArenaMaxSize(DT_void);

/**
 * Creates a new arena.
 *
 * @param size   Total size (in bytes) of the arena.
 * @param pArena Output pointer that receives the created arena.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCreateUnchecked(DT_size size,
                                                           MEM_Arena **pArena);

/**
 * Creates a new arena with full argument validation.
 *
 * @param size   Total size (in bytes) of the arena.
 * @param pArena Output pointer that receives the created arena.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCreateChecked(DT_size size,
                                                         MEM_Arena **pArena);

/**
 * Deletes the arena and nullifies the pointer.
 *
 * @param pArena Pointer to arena pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_ArenaDeleteUnchecked(MEM_Arena **pArena);

/**
 * Deletes the arena and nullifies the pointer.
 *
 * @param pArena Pointer to arena pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArena or *pArena is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaDeleteChecked(MEM_Arena **pArena);

/**
 * Allocates a block of memory from the arena.
 *
 * The returned memory is uninitialized.
 *
 * @param arena Arena to allocate from.
 * @param size  Number of bytes to allocate.
 * @param dest  Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if insufficient space remains.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 *
 * @warning
 * `dest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaAllocUnchecked(MEM_Arena *arena,
                                                          DT_size size,
                                                          DT_void **dest);

/**
 * Allocates a block of memory from the arena with validation.
 *
 * The returned memory is uninitialized.
 *
 * @param arena Arena to allocate from.
 * @param size  Number of bytes to allocate.
 * @param dest  Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if insufficient space remains.
 *
 * @warning
 * `dest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaAllocChecked(MEM_Arena *arena,
                                                        DT_size size,
                                                        DT_void **dest);

/**
 * Allocates a zero-initialized block of memory from the arena.
 *
 * @param arena Arena to allocate from.
 * @param size  Number of bytes to allocate.
 * @param dest  Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if insufficient space remains.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 *
 * @warning
 * `dest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCallocUnchecked(MEM_Arena *arena,
                                                           DT_size size,
                                                           DT_void **dest);

/**
 * Allocates a zero-initialized block of memory with validation.
 *
 * @param arena Arena to allocate from.
 * @param size  Number of bytes to allocate.
 * @param dest  Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if insufficient space remains.
 *
 * @warning
 * `dest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaCallocChecked(MEM_Arena *arena,
                                                         DT_size size,
                                                         DT_void **dest);

/**
 * Resets the arena.
 *
 * - All previous allocations become invalid.
 * - Offset is reset to zero.
 *
 * @param arena Arena to reset.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_ArenaResetUnchecked(MEM_Arena *arena);

/**
 * Resets the arena with validation.
 *
 * @param arena Arena to reset.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arena is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_ArenaResetChecked(MEM_Arena *arena);

#ifdef __cplusplus
}
#endif
