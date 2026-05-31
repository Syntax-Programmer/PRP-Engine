#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Utils/Defs.h"

/**
 * MEM_Pool
 *
 * A fixed-size pool allocator.
 *
 * - Allocates memory in fixed-size blocks.
 * - Supports O(1) allocation and deallocation.
 * - Uses an internal free list for reuse.
 *
 * Suitable for frequently created and destroyed objects of uniform size.
 */
typedef struct _Pool MEM_Pool;

/**
 * Checks whether the given pool is structurally valid.
 *
 * @param pool Pointer to the pool.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL MEM_PoolIsValid(const MEM_Pool *pool);

/**
 * Creates a pool allocator.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Maximum number of elements.
 * @param pPool     Output pointer receiving the created pool.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails or size exceeds limits.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure validity.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolCreateUnchecked(DT_size memb_size,
                                                          DT_size cap,
                                                          MEM_Pool **pPool);

/**
 * Creates a pool allocator with validation.
 *
 * @param memb_size Size (in bytes) of each element.
 * @param cap       Maximum number of elements.
 * @param pPool     Output pointer receiving the created pool.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails or size exceeds limits.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolCreateChecked(DT_size memb_size,
                                                        DT_size cap,
                                                        MEM_Pool **pPool);

/**
 * Deletes the pool and nullifies the pointer.
 *
 * @param pPool Pointer to pool pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_PoolDeleteUnchecked(MEM_Pool **pPool);

/**
 * Deletes the pool and nullifies the pointer.
 *
 * @param pPool Pointer to pool pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pPool or *pPool is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolDeleteChecked(MEM_Pool **pPool);

/**
 * Allocates a block from the pool.
 *
 * The returned memory is uninitialized.
 *
 * @param pool  Pool to allocate from.
 * @param pDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 *
 * @warning
 * pDest MUST be a void**. Passing T** is undefined behavior.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolAllocUnchecked(MEM_Pool *pool,
                                                         DT_void **pDest);

/**
 * Allocates a block from the pool with validation.
 *
 * @param pool  Pool to allocate from.
 * @param pDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolAllocChecked(MEM_Pool *pool,
                                                       DT_void **pDest);

/**
 * Allocates a zero-initialized block from the pool.
 *
 * @param pool  Pool to allocate from.
 * @param pDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolCallocUnchecked(MEM_Pool *pool,
                                                          DT_void **pDest);

/**
 * Allocates a zero-initialized block from the pool with validation.
 *
 * @param pool  Pool to allocate from.
 * @param pDest Output pointer receiving allocated memory.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if no free blocks remain.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolCallocChecked(MEM_Pool *pool,
                                                        DT_void **pDest);

/**
 * Frees a previously allocated block back to the pool.
 *
 * @param pool Pool to return memory to.
 * @param ptr  Pointer previously allocated from this pool.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 * - Caller must ensure ptr belongs to this pool.
 *
 * @warning
 * Double-free is NOT detected and leads to undefined behavior.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_PoolFreeUnchecked(MEM_Pool *pool,
                                                     DT_void *ptr);

/**
 * Frees a previously allocated block with validation.
 *
 * @param pool Pool to return memory to.
 * @param ptr  Pointer previously allocated from this pool.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pool or ptr is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolFreeChecked(MEM_Pool *pool,
                                                      DT_void *ptr);

/**
 * Returns the capacity (number of elements) of the pool.
 *
 * @param pool Pool instance.
 *
 * @return Pool capacity.
 *
 * @note Assumes valid pool (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolCap(const MEM_Pool *pool);
/**
 * Returns the size (in bytes) of each element.
 *
 * @param pool Pool instance.
 *
 * @return Element size.
 *
 * @note Assumes valid pool (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMembSize(const MEM_Pool *pool);
/**
 * Returns the maximum possible capacity for this pool configuration.
 *
 * @param pool Pool instance.
 *
 * @return Maximum capacity.
 *
 * @note Assumes valid pool (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMaxCap(const MEM_Pool *pool);

/**
 * Resets the pool.
 *
 * - All allocated blocks become invalid.
 * - Free list is rebuilt.
 *
 * @param pool Pool to reset.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_PoolResetUnchecked(MEM_Pool *pool);

/**
 * Resets the pool with validation.
 *
 * @param pool Pool to reset.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pool is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolResetChecked(MEM_Pool *pool);

#ifdef __cplusplus
}
#endif
