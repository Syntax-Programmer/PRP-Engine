#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * MEM_Pool
 *
 * A fixed-size pPool allocator.
 *
 * - Allocates memory in fixed-size blocks.
 * - Supports O(1) allocation and deallocation.
 * - Uses an internal free list for reuse.
 *
 * Suitable for frequently created and destroyed objects of uniform size.
 */
typedef struct MEM_Pool MEM_Pool;

/**
 * Checks whether the given pPool is structurally valid.
 *
 * @param pPool Pointer to the pPool.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL MEM_PoolIsValid(const MEM_Pool *pPool);

/**
 * Creates a pPool allocator.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Maximum number of elements.
 * @param ppPool    Output pointer receiving the created pPool.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails or size exceeds limits.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolCreateUnchecked(PRP_Size memb_size,
                                                    PRP_Size cap,
                                                    MEM_Pool **ppPool);

/**
 * Creates a pPool allocator with validation.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Maximum number of elements.
 * @param ppPool    Output pointer receiving the created pPool.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails or size exceeds limits.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolCreateChecked(PRP_Size memb_size,
                                                  PRP_Size cap,
                                                  MEM_Pool **ppPool);

/**
 * Deletes the pPool and nullifies the pointer.
 *
 * @param ppPool Pointer to pPool pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL MEM_PoolDeleteUnchecked(MEM_Pool **ppPool);

/**
 * Deletes the pPool and nullifies the pointer.
 *
 * @param ppPool Pointer to pPool pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if ppPool or *ppPool is invalid.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolDeleteChecked(MEM_Pool **ppPool);

/**
 * Allocates a block from the pPool.
 *
 * The returned memory is uninitialized.
 *
 * @param pPool  Pool to allocate from.
 * @param ppDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 *
 * @warning
 * ppDest MUST be a void**. Passing T** is undefined behavior.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolAllocUnchecked(MEM_Pool *pPool,
                                                   void **ppDest);

/**
 * Allocates a block from the pPool with validation.
 *
 * @param pPool  Pool to allocate from.
 * @param ppDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolAllocChecked(MEM_Pool *pPool,
                                                 void **ppDest);

/**
 * Allocates a zero-initialized block from the pPool.
 *
 * @param pPool  Pool to allocate from.
 * @param ppDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolCallocUnchecked(MEM_Pool *pPool,
                                                    void **ppDest);

/**
 * Allocates a zero-initialized block from the pPool with validation.
 *
 * @param pPool  Pool to allocate from.
 * @param ppDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolCallocChecked(MEM_Pool *pPool,
                                                  void **ppDest);

/**
 * Frees a previously allocated block back to the pPool.
 *
 * @param pPool Pool to return memory to.
 * @param ptr   Pointer previously allocated from this pPool.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure ptr belongs to this pPool.
 *
 * @warning
 * Double-free is NOT detected and leads to undefined behavior.
 */
PRP_API void PRP_CALL MEM_PoolFreeUnchecked(MEM_Pool *pPool, void *ptr);

/**
 * Frees a previously allocated block with validation.
 *
 * @param pPool Pool to return memory to.
 * @param ptr   Pointer previously allocated from this pPool.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pPool or ptr is invalid.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolFreeChecked(MEM_Pool *pPool, void *ptr);

/**
 * Returns the capacity (number of elements) of the pPool.
 *
 * @param pPool Pool instance.
 *
 * @return Pool capacity.
 *
 * @note Assumes valid pPool (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL MEM_PoolCap(const MEM_Pool *pPool);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param pPool Pool instance.
 *
 * @return Element size.
 *
 * @note Assumes valid pPool (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL MEM_PoolMembSize(const MEM_Pool *pPool);
/**
 * Returns the maximum possible capacity for this pPool configuration.
 *
 * @param pPool Pool instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid pPool (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL MEM_PoolMaxCap(const MEM_Pool *pPool);

/**
 * Resets the pPool.
 *
 * - All allocated blocks become invalid.
 * - Free list is rebuilt.
 *
 * @param pPool Pool to reset.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL MEM_PoolResetUnchecked(MEM_Pool *pPool);

/**
 * Resets the pPool with validation.
 *
 * @param pPool Pool to reset.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pPool is invalid.
 */
PRP_API PRP_Result PRP_CALL MEM_PoolResetChecked(MEM_Pool *pPool);

#ifdef __cplusplus
}
#endif
