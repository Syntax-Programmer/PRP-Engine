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
typedef struct _Pool MEM_Pool;

/*
 * @return The last error code set by the pool functions that don't return
 * PRP_Result explicitly.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolGetLastErrCode(DT_void);
/**
 * Creates the pool allocator with the given memb_size and cap.
 *
 * @param memb_size: The size of individual allocable blocks of the pool.
 * @param cap: The max number of members the pool should hold.
 *
 * @retuns The pointer to the pool.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API MEM_Pool *PRP_FN_CALL MEM_PoolCreateUnchecked(DT_size memb_size,
                                                         DT_size cap);
/**
 * Creates the pool allocator with the given memb_size and cap.
 *
 * @param memb_size: The size of individual allocable blocks of the pool.
 * @param cap: The max number of members the pool should hold.
 *
 * @retuns The pointer to the pool.
 */
PRP_FN_API MEM_Pool *PRP_FN_CALL MEM_PoolCreateChecked(DT_size memb_size,
                                                       DT_size cap);
/**
 * Deletes the pool and sets the original MEM_Pool * to DT_null to prevent use
 * after free bugs.
 *
 * @param pPool: The pointer to the pool pointer to delete.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_PoolDeleteUnchecked(MEM_Pool **pPool);
/**
 * Deletes the pool and sets the original MEM_Pool * to DT_null to prevent use
 * after free bugs.
 *
 * @param pPool: The pointer to the pool pointer to delete.
 *
 * @return PRP_ERR_INV_ARG if the pPool or *pPool is DT_null, otherwise it
 * returns PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolDeleteChecked(MEM_Pool **pPool);
/**
 * Allocates an element the pool, the allocated element may contain junk data,
 * so caution is adviced.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return DT_null if all the elements have been already allocated, otherwise
 * PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolAllocUnchecked(MEM_Pool *pool);
/**
 * Allocates an element the pool, the allocated element may contain junk data,
 * so caution is adviced.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return DT_null if the parameters are invalid in any way, DT_null if all
 * the elements have been already allocated, otherwise PRP_OK.
 */
PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolAllocChecked(MEM_Pool *pool);
/**
 * Allocates a section of memory from the arena, the allocated region is all
 * preset to 0.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return DT_null if all the elements have been already allocated, otherwise
 * PRP_OK.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolCallocUnchecked(MEM_Pool *pool);
/**
 * Allocates a section of memory from the arena, the allocated region is all
 * preset to 0.
 *
 * @param pool: The pool to allocate the element from.
 *
 * @return DT_null if the parameters are invalid in any way, DT_null if all
 * the elements have been already allocated, otherwise PRP_OK.
 */
PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolCallocChecked(MEM_Pool *pool);
/**
 * Frees an allocated ptr from the pool.
 *
 * @param pool: The pool to free mem to.
 * @param ptr: The ptr to free in the pool.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_PoolFreeUnchecked(MEM_Pool *pool,
                                                     DT_void *ptr);
/**
 * Frees an allocated ptr from the pool.
 *
 * @param pool: The pool to free mem to.
 * @param ptr: The ptr to free in the pool.
 *
 * @return PRP_ERR_INV_ARG if the pool or the ptr is invalid, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolFreeChecked(MEM_Pool *pool,
                                                      DT_void *ptr);
/**
 * Returns the current allocated cap of the pool that is passed to it.
 *
 * @param pool: The pool to get the cap of.
 *
 * @return The cap of the pool.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 *
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolCapUnchecked(const MEM_Pool *pool);
/**
 * Returns the current allocated cap of the pool that is passed to it.
 *
 * @param pool: The pool to get the cap of.
 *
 * @return PRP_INVALID_SIZE if the pool is invalid, otherwise the actual cap of
 * the pool.
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolCapChecked(const MEM_Pool *pool);
/**
 * Returns the memb size of the pool that is passed to it.
 *
 * @param pool: The pool to get the memb size of.
 *
 * @return The memb size of the pool.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 *
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMembSizeUnchecked(const MEM_Pool *pool);
/**
 * Returns the memb size of the pool that is passed to it.
 *
 * @param pool: The pool to get the memb size of.
 *
 * @return PRP_INVALID_SIZE if the pool is invalid, otherwise the actual memb
 * size of the pool.
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMembSizeChecked(const MEM_Pool *pool);
/**
 * Returns the max cap of the pool that is passed to it.
 *
 * @param pool: The pool to get the max cap of.
 *
 * @return The max cap of the pool.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 *
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMaxCapUnchecked(const MEM_Pool *pool);
/**
 * Returns the max cap of the pool that is passed to it.
 *
 * @param pool: The pool to get the max cap of.
 *
 * @return PRP_INVALID_SIZE if the pool is invalid, otherwise the actual max cap
 * the pool.
 */
PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMaxCapChecked(const MEM_Pool *pool);
/**
 * Resets the pool, which invalidates all existing alloations and empties the
 * entire pool.
 *
 * @param pool: The pool to reset.
 *
 * @note: This function doesn't check for argument validation in RELEASE mode.
 */
PRP_FN_API DT_void PRP_FN_CALL MEM_PoolResetUnchecked(MEM_Pool *pool);
/**
 * Resets the pool, which invalidates all existing alloations and empties the
 * entire pool.
 *
 * @param pool: The pool to reset.
 *
 * @return PRP_ERR_INV_ARG if the pool is invalid, otherwise PRP_OK.
 */
PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolResetChecked(MEM_Pool *pool);

#ifdef __cplusplus
}
#endif
