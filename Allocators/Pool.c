#include "Pool.h"
#include "Diagnostics/Assert.h"
#include <string.h>

struct _Pool {
    PRP_Size cap;
    PRP_Size memb_size;
    PRP_U8 *free_list;
    PRP_U8 mem[];
};

#define MAX_ALLOCABLE_SIZE (PRP_SIZE_MAX - sizeof(MEM_Pool))
// Define it with max size so no extra checks for that.
#define MAX_CAP(memb_size) (MAX_ALLOCABLE_SIZE / memb_size)

#define ASSERT_INVARIANT_EXPR(pool)                                            \
    DIAG_ASSERT_MSG(MEM_PoolIsValid(pool),                                     \
                    "The given pool is either NULL, or is corrupted.")

PRP_API PRP_Bool PRP_CALL MEM_PoolIsValid(const MEM_Pool *pool) {
    return (pool != NULL && pool->memb_size > 0 &&
            pool->cap <= MAX_CAP(pool->memb_size));
}

PRP_API PRP_Result PRP_CALL MEM_PoolCreateUnchecked(PRP_Size memb_size,
                                                    PRP_Size cap,
                                                    MEM_Pool **pPool) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(pPool != NULL);

    memb_size = PRP_MAX(memb_size, sizeof(void *));
    if (cap > MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    // Padding the memb_size to ensure the freelist can work.
    MEM_Pool *pool = malloc(sizeof(MEM_Pool) + (memb_size * cap));
    if (!pool) {
        return PRP_ERR_OOM;
    }
    pool->memb_size = memb_size;
    pool->cap = cap;
    pool->free_list = NULL;
    PRP_U8 *curr = pool->mem;
    for (PRP_Size i = 0; i < pool->cap - 1; i++) {
        *((PRP_U8 **)curr) = curr + pool->memb_size;
        curr += memb_size;
    }
    *((PRP_U8 **)curr) = pool->free_list;
    pool->free_list = pool->mem;

    *pPool = pool;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolCreateChecked(PRP_Size memb_size,
                                                  PRP_Size cap,
                                                  MEM_Pool **pPool) {
    if (!memb_size || !cap || !pPool) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_PoolCreateUnchecked(memb_size, cap, pPool);
}

PRP_API void PRP_CALL MEM_PoolDeleteUnchecked(MEM_Pool **pPool) {
    DIAG_ASSERT(pPool != NULL);
    DIAG_ASSERT(*pPool != NULL);

    MEM_Pool *pool = *pPool;

    pool->memb_size = pool->cap = 0;
    pool->free_list = NULL;
    free(pool);
    *pPool = NULL;
}

PRP_API PRP_Result PRP_CALL MEM_PoolDeleteChecked(MEM_Pool **pPool) {
    if (!pPool || !(*pPool)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolDeleteUnchecked(pPool);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolAllocUnchecked(MEM_Pool *pool,
                                                   void **pDest) {
    ASSERT_INVARIANT_EXPR(pool);
    DIAG_ASSERT(pDest != NULL);

    if (!pool->free_list) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = pool->free_list;
    pool->free_list = *((PRP_U8 **)pool->free_list);
    *pDest = ptr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolAllocChecked(MEM_Pool *pool, void **pDest) {
    if (!MEM_PoolIsValid(pool) || !pDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_PoolAllocUnchecked(pool, pDest);
}

PRP_API PRP_Result PRP_CALL MEM_PoolCallocUnchecked(MEM_Pool *pool,
                                                    void **pDest) {
    ASSERT_INVARIANT_EXPR(pool);
    DIAG_ASSERT(pDest != NULL);

    if (!pool->free_list) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = pool->free_list;
    pool->free_list = *((PRP_U8 **)pool->free_list);
    memset(ptr, 0, pool->memb_size);
    *pDest = ptr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolCallocChecked(MEM_Pool *pool,
                                                  void **pDest) {
    if (!MEM_PoolIsValid(pool) || !pDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_PoolCallocUnchecked(pool, pDest);
}

PRP_API void PRP_CALL MEM_PoolFreeUnchecked(MEM_Pool *pool, void *ptr) {
    DIAG_ASSERT(ptr != NULL);
    ASSERT_INVARIANT_EXPR(pool);
    DIAG_ASSERT_MSG(
        ((PRP_U8 *)ptr > pool->mem &&
         (PRP_U8 *)ptr < pool->mem + (pool->cap * pool->memb_size) &&
         (((PRP_U8 *)ptr - pool->mem) % pool->memb_size) == 0),
        "The given pointer to free doesn't belong to the pool provided.");

    /*
     * This currently doesn't protect against double free of the memory. It will
     * curropt the memory eventually if a double free happens.
     *
     * TODO: Fix it.
     */
    *((PRP_U8 **)ptr) = pool->free_list;
    pool->free_list = ptr;
}

PRP_API PRP_Result PRP_CALL MEM_PoolFreeChecked(MEM_Pool *pool, void *ptr) {
    if (!ptr || !MEM_PoolIsValid(pool)) {
        return PRP_ERR_INV_ARG;
    }
    PRP_U8 *p = ptr;
    if (p < pool->mem || p >= pool->mem + pool->cap * pool->memb_size ||
        ((p - pool->mem) % pool->memb_size) != 0) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolFreeUnchecked(pool, ptr);

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL MEM_PoolCap(const MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    return pool->cap;
}

PRP_API PRP_Size PRP_CALL MEM_PoolMembSize(const MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    return pool->memb_size;
}

PRP_API PRP_Size PRP_CALL MEM_PoolMaxCap(const MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    return MAX_CAP(pool->memb_size);
}

PRP_API void PRP_CALL MEM_PoolResetUnchecked(MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    pool->free_list = NULL;
    PRP_U8 *curr = pool->mem;
    for (PRP_Size i = 0; i < pool->cap - 1; i++) {
        *((PRP_U8 **)curr) = curr + pool->memb_size;
        curr += pool->memb_size;
    }
    *((PRP_U8 **)curr) = pool->free_list;
    pool->free_list = pool->mem;
}

PRP_API PRP_Result PRP_CALL MEM_PoolResetChecked(MEM_Pool *pool) {
    if (!MEM_PoolIsValid(pool)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolResetUnchecked(pool);

    return PRP_OK;
}
