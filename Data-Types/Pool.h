#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Utils/Defs.h"

/**
 * A modification of the arena allocator, pool allocator allocates elements of a
 * fixed size.
 * This also provides O(1) alloc and free ops in the pool. Which is perfect for
 * rapidly created and deleted objects.
 */
typedef struct _Pool PRP_Pool;

/**
 * Creates the pool allocator with the given memb_size and cap.
 *
 * @param memb_size: The size of individual allocable blocks of the pool.
 * @param cap: The max number of members the pool should hold.
 *
 * @retuns The pointer to the pool.
 */
PRP_FN_API PRP_Pool *PRP_FN_CALL PRP_PoolCreate(PRP_size memb_size,
                                                PRP_size cap);
/**
 * Deletes the pool and sets the original PRP_Pool * to PRP_null to prevent use
 * after free bugs.
 *
 * @param pPool: The pointer to the pool pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pPool is PRP_null or the pool it points
 * to is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_PoolDelete(PRP_Pool **pPool);
/**
 * Allocates an element the pool, the allocated element may contain junk data,
 * so caution is adviced.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return PRP_null if the parameters are invalid in any way, PRP_null if all
 * the elements have been already allocated, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_void *PRP_FN_CALL PRP_PoolAlloc(PRP_Pool *pool);
/**
 * Allocates a section of memory from the arena, the allocated region is all
 * preset to 0.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return PRP_null if the parameters are invalid in any way, PRP_null if all
 * the elements have been already allocated, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_void *PRP_FN_CALL PRP_PoolCalloc(PRP_Pool *pool);
/**
 * Frees an allocated ptr from the pool.
 *
 * @param pool: The pool to free mem to.
 * @param ptr: The ptr to free in the pool.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pool is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_PoolFree(PRP_Pool *pool, PRP_void *ptr);
/**
 * Resets the pool, which invalidates all existing alloations and empties the
 * entire pool.
 *
 * @param pool: The pool to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pool is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_PoolReset(PRP_Pool *pool);

#ifdef __cplusplus
}
#endif
