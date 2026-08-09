#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * MEM_Arena
 *
 * A fixed-size pArena allocator.
 *
 * - Memory is allocated linearly.
 * - Individual frees are NOT supported.
 * - All allocations are invalidated on reset or delete.
 *
 * This allocator is extremely fast but requires careful lifetime management.
 */
typedef struct MEM_Arena MEM_Arena;

/**
 * Checks whether the given pArena is structurally valid.
 *
 * @param pArena Pointer to the pArena.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL MEM_ArenaIsValid(const MEM_Arena *pArena);

/**
 * Returns the maximum allocatable pArena size in bytes.
 *
 * @return Maximum supported pArena size.
 */
PRP_API PRP_Size PRP_CALL MEM_ArenaMaxSize(void);

/**
 * Creates a new pArena.
 *
 * @param size    Total size (in bytes) of the pArena.
 * @param ppArena Output pointer that receives the created pArena.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_API PRP_Result PRP_CALL MEM_ArenaCreateUnchecked(PRP_Size size,
                                                     MEM_Arena **ppArena);

/**
 * Creates a new pArena with full argument validation.
 *
 * @param size    Total size (in bytes) of the pArena.
 * @param ppArena Output pointer that receives the created pArena.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_API PRP_Result PRP_CALL MEM_ArenaCreateChecked(PRP_Size size,
                                                   MEM_Arena **ppArena);

/**
 * Deletes the pArena and nullifies the pointer.
 *
 * @param ppArena Pointer to pArena pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_API void PRP_CALL MEM_ArenaDeleteUnchecked(MEM_Arena **ppArena);

/**
 * Deletes the pArena and nullifies the pointer.
 *
 * @param ppArena Pointer to pArena pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if ppArena or *ppArena is invalid.
 */
PRP_API PRP_Result PRP_CALL MEM_ArenaDeleteChecked(MEM_Arena **ppArena);

/**
 * Allocates a block of memory from the pArena.
 *
 * The returned memory is uninitialized.
 *
 * @param pArena Arena to allocate from.
 * @param size   Number of bytes to allocate.
 * @param ppDest Output pointer receiving allocated memory.
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
PRP_API PRP_Result PRP_CALL MEM_ArenaAllocUnchecked(MEM_Arena *pArena,
                                                    PRP_Size size,
                                                    void **ppDest);

/**
 * Allocates a block of memory from the pArena with validation.
 *
 * The returned memory is uninitialized.
 *
 * @param pArena Arena to allocate from.
 * @param size   Number of bytes to allocate.
 * @param ppDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if insufficient space remains.
 *
 * @warning
 * `dest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
PRP_API PRP_Result PRP_CALL MEM_ArenaAllocChecked(MEM_Arena *pArena,
                                                  PRP_Size size, void **ppDest);

/**
 * Allocates a zero-initialized block of memory from the pArena.
 *
 * @param pArena Arena to allocate from.
 * @param size   Number of bytes to allocate.
 * @param ppDest Output pointer receiving allocated memory.
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
PRP_API PRP_Result PRP_CALL MEM_ArenaCallocUnchecked(MEM_Arena *pArena,
                                                     PRP_Size size,
                                                     void **ppDest);

/**
 * Allocates a zero-initialized block of memory with validation.
 *
 * @param pArena Arena to allocate from.
 * @param size   Number of bytes to allocate.
 * @param ppDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if insufficient space remains.
 *
 * @warning
 * `dest` MUST be a `void **`.
 * Passing `T **` (e.g., `int **`) is undefined behavior.
 */
PRP_API PRP_Result PRP_CALL MEM_ArenaCallocChecked(MEM_Arena *pArena,
                                                   PRP_Size size,
                                                   void **ppDest);

/**
 * Resets the pArena.
 *
 * - All previous allocations become invalid.
 * - Offset is reset to zero.
 *
 * @param pArena Arena to reset.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_API void PRP_CALL MEM_ArenaResetUnchecked(MEM_Arena *pArena);

/**
 * Resets the pArena with validation.
 *
 * @param pArena Arena to reset.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArena is invalid.
 */
PRP_API PRP_Result PRP_CALL MEM_ArenaResetChecked(MEM_Arena *pArena);

#ifdef __cplusplus
}
#endif
