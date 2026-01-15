#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

/**
 * A modification of the arena allocator, pool allocator allocates elements of a
 * fixed size.
 * This also provides O(1) alloc and free ops in the pool. Which is perfect for
 * rapidly created and deleted objects.
 */
typedef struct _Pool DT_Pool;

/**
 * Creates the pool allocator with the given memb_size and cap.
 *
 * @param memb_size: The size of individual allocable blocks of the pool.
 * @param cap: The max number of members the pool should hold.
 *
 * @retuns The pointer to the pool.
 */
PRP_FN_API DT_Pool *PRP_FN_CALL DT_PoolCreate(DT_size memb_size, DT_size cap);
/**
 * Deletes the pool and sets the original DT_Pool * to DT_null to prevent use
 * after free bugs.
 *
 * @param pPool: The pointer to the pool pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pPool is DT_null or the pool it points
 * to is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_PoolDelete(DT_Pool **pPool);
/**
 * Allocates an element the pool, the allocated element may contain junk data,
 * so caution is adviced.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return DT_null if the parameters are invalid in any way, DT_null if all
 * the elements have been already allocated, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_PoolAlloc(DT_Pool *pool);
/**
 * Allocates a section of memory from the arena, the allocated region is all
 * preset to 0.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return DT_null if the parameters are invalid in any way, DT_null if all
 * the elements have been already allocated, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_PoolCalloc(DT_Pool *pool);
/**
 * Frees an allocated ptr from the pool.
 *
 * @param pool: The pool to free mem to.
 * @param ptr: The ptr to free in the pool.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pool is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_PoolFree(DT_Pool *pool, DT_void *ptr);
/**
 * Returns the current allocated cap of the pool that is passed to it.
 *
 * @param pool: The pool to get the cap of.
 *
 * @return PRP_INVALID_SIZE if the pool is invalid, otherwise the actual cap of
 * the pool.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_PoolCap(const DT_Pool *pool);
/**
 * Returns the member size of the pool that is passed to it.
 *
 * @param pool: The pool to get the memb_size of.
 *
 * @return PRP_INVALID_SIZE if the pool is invalid, otherwise the actual
 * memb_size of the pool.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_PoolMembSize(const DT_Pool *pool);
/**
 * Returns the max capacity of the pool that is passed to it based on its
 * memb_size.
 *
 * @param pool: The pool to get the max capacity of.
 *
 * @return PRP_INVALID_SIZE if the pool is invalid, otherwise the actual
 * max capacity of the pool.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_PoolMaxCap(const DT_Pool *pool);
/**
 * Resets the pool, which invalidates all existing alloations and empties the
 * entire pool.
 *
 * @param pool: The pool to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pool is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_PoolReset(DT_Pool *pool);

#ifdef __cplusplus
}
#endif
