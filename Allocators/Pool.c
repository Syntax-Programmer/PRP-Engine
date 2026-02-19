#include "Pool.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Pool {
    DT_size cap;
    DT_size memb_size;
    DT_u8 *free_list;
    DT_u8 mem[];
};

#define MAX_ALLOCABLE_SIZE (DT_SIZE_MAX - sizeof(MEM_Pool))
// Define it with max size so no extra checks for that.
#define MAX_CAP(memb_size) (MAX_ALLOCABLE_SIZE / memb_size)

#define INVARIANT_EXPR(pool)                                                   \
    ((pool) != DT_null && (pool)->memb_size > 0 &&                             \
     (pool)->cap <= MAX_CAP((pool)->memb_size))
#define ASSERT_INVARIANT_EXPR(pool)                                            \
    DIAG_ASSERT_MSG(INVARIANT_EXPR(pool),                                      \
                    "The given pool is either DT_null, or is corrupted.")

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolGetLastErrCode(DT_void) {
    return last_err_code;
}

PRP_FN_API MEM_Pool *PRP_FN_CALL MEM_PoolCreateUnchecked(DT_size memb_size,
                                                         DT_size cap) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);

    memb_size = PRP_MAX(memb_size, sizeof(DT_void *));
    if (cap > MAX_CAP(memb_size)) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }

    // Padding the memb_size to ensure the freelist can work.
    MEM_Pool *pool = malloc(sizeof(MEM_Pool) + (memb_size * cap));
    if (!pool) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    pool->memb_size = memb_size;
    pool->cap = cap;
    pool->free_list = DT_null;
    DT_u8 *curr = pool->mem;
    for (DT_size i = 0; i < pool->cap - 1; i++) {
        *((DT_u8 **)curr) = curr + pool->memb_size;
        curr += memb_size;
    }
    *((DT_u8 **)curr) = pool->free_list;
    pool->free_list = pool->mem;

    return pool;
}

PRP_FN_API MEM_Pool *PRP_FN_CALL MEM_PoolCreateChecked(DT_size memb_size,
                                                       DT_size cap) {
    if (!memb_size || !cap) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return MEM_PoolCreateUnchecked(memb_size, cap);
}

PRP_FN_API DT_void PRP_FN_CALL MEM_PoolDeleteUnchecked(MEM_Pool **pPool) {
    DIAG_ASSERT(pPool != DT_null);
    DIAG_ASSERT(*pPool != DT_null);

    MEM_Pool *pool = *pPool;

    pool->memb_size = pool->cap = 0;
    pool->free_list = DT_null;
    free(pool);
    *pPool = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolDeleteChecked(MEM_Pool **pPool) {
    if (!pPool || !(*pPool)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolDeleteUnchecked(pPool);

    return PRP_OK;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolAllocUnchecked(MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    if (!pool->free_list) {
        SET_LAST_ERR_CODE(PRP_ERR_RES_EXHAUSTED);
        return DT_null;
    }

    DT_void *ptr = pool->free_list;
    pool->free_list = *((DT_u8 **)pool->free_list);

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolAllocChecked(MEM_Pool *pool) {
    if (!INVARIANT_EXPR(pool)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return MEM_PoolAllocUnchecked(pool);
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolCallocUnchecked(MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    if (!pool->free_list) {
        SET_LAST_ERR_CODE(PRP_ERR_RES_EXHAUSTED);
        return DT_null;
    }

    DT_void *ptr = pool->free_list;
    pool->free_list = *((DT_u8 **)pool->free_list);
    memset(ptr, 0, pool->memb_size);

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolCallocChecked(MEM_Pool *pool) {
    if (!INVARIANT_EXPR(pool)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return MEM_PoolCallocUnchecked(pool);
}

PRP_FN_API DT_void PRP_FN_CALL MEM_PoolFreeUnchecked(MEM_Pool *pool,
                                                     DT_void *ptr) {
    DIAG_ASSERT(ptr != DT_null);
    ASSERT_INVARIANT_EXPR(pool);
    DIAG_ASSERT_MSG(
        ((DT_u8 *)ptr > pool->mem &&
         (DT_u8 *)ptr < pool->mem + (pool->cap * pool->memb_size) &&
         (((DT_u8 *)ptr - pool->mem) % pool->memb_size) == 0),
        "The given pointer to free doesn't belong to the pool provided.");

    /*
     * This currently doesn't protect against double free of the memory. It will
     * curropt the memory eventually if a double free happens.
     *
     * TODO: Fix it.
     */
    *((DT_u8 **)ptr) = pool->free_list;
    pool->free_list = ptr;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolFreeChecked(MEM_Pool *pool,
                                                      DT_void *ptr) {
    if (!ptr || !INVARIANT_EXPR(pool)) {
        return PRP_ERR_INV_ARG;
    }
    DT_u8 *p = ptr;
    if (p < pool->mem || p >= pool->mem + pool->cap * pool->memb_size ||
        ((p - pool->mem) % pool->memb_size) != 0) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolFreeUnchecked(pool, ptr);

    return PRP_OK;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolCapUnchecked(const MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    return pool->cap;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolCapChecked(const MEM_Pool *pool) {
    if (!INVARIANT_EXPR(pool)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return pool->cap;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMembSizeUnchecked(const MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    return pool->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMembSizeChecked(const MEM_Pool *pool) {
    if (!INVARIANT_EXPR(pool)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return pool->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMaxCapUnchecked(const MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    return MAX_CAP(pool->memb_size);
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMaxCapChecked(const MEM_Pool *pool) {
    if (!INVARIANT_EXPR(pool)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return MAX_CAP(pool->memb_size);
}

PRP_FN_API DT_void PRP_FN_CALL MEM_PoolResetUnchecked(MEM_Pool *pool) {
    ASSERT_INVARIANT_EXPR(pool);

    pool->free_list = DT_null;
    DT_u8 *curr = pool->mem;
    for (DT_size i = 0; i < pool->cap - 1; i++) {
        *((DT_u8 **)curr) = curr + pool->memb_size;
        curr += pool->memb_size;
    }
    *((DT_u8 **)curr) = pool->free_list;
    pool->free_list = pool->mem;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolResetChecked(MEM_Pool *pool) {
    if (!INVARIANT_EXPR(pool)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolResetUnchecked(pool);

    return PRP_OK;
}
