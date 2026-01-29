#include "Pool.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Pool {
    DT_size cap;
    DT_size memb_size;
    DT_u8 *free_list;
    DT_u8 mem[];
};

#define DEFAULT_POOL_CAP (16)
#define MAX_ALLOCABLE_SIZE (DT_SIZE_MAX - sizeof(MEM_Pool))
// Define it with max size so no extra checks for that.
#define MAX_CAP(memb_size) (MAX_ALLOCABLE_SIZE / memb_size)

PRP_FN_API MEM_Pool *PRP_FN_CALL MEM_PoolCreate(DT_size memb_size,
                                                DT_size cap) {
    DIAG_GUARD(memb_size > 0, DT_null);
    DIAG_GUARD(cap > 0, DT_null);

    memb_size = PRP_MAX(memb_size, sizeof(DT_void *));
    if (cap > MAX_CAP(memb_size)) {
        return DT_null;
    }

    // Padding the memb_size to ensure the freelist can work.
    MEM_Pool *pool = malloc(sizeof(MEM_Pool) + (memb_size * cap));
    if (!pool) {
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

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolDelete(MEM_Pool **pPool) {
    DIAG_GUARD(pPool != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(*pPool != DT_null, PRP_ERR_INV_ARG);

    MEM_Pool *pool = *pPool;

    pool->memb_size = pool->cap = 0;
    pool->free_list = DT_null;
    free(pool);
    *pPool = DT_null;

    return PRP_OK;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolAlloc(MEM_Pool *pool) {
    DIAG_GUARD(pool != DT_null, DT_null);

    if (!pool->free_list) {
        return DT_null;
    }

    DT_void *ptr = pool->free_list;
    pool->free_list = *((DT_u8 **)pool->free_list);

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL MEM_PoolCalloc(MEM_Pool *pool) {
    DIAG_GUARD(pool != DT_null, DT_null);

    if (!pool->free_list) {
        return DT_null;
    }

    DT_void *ptr = pool->free_list;
    pool->free_list = *((DT_u8 **)pool->free_list);
    memset(ptr, 0, pool->memb_size);

    return ptr;
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolFree(MEM_Pool *pool, DT_void *ptr) {
    DIAG_GUARD(pool != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(ptr != DT_null, PRP_ERR_INV_ARG);

    DT_u8 *p = ptr;
    if (p < pool->mem || p >= pool->mem + pool->cap * pool->memb_size ||
        ((p - pool->mem) % pool->memb_size) != 0) {
        return PRP_ERR_INV_ARG;
    }

    /*
     * This currently doesn't protect against double free of the memory. It will
     * curropt the memory eventually if a double free happens.
     *
     * TODO: Fix it.
     */
    *((DT_u8 **)ptr) = pool->free_list;
    pool->free_list = ptr;

    return PRP_OK;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolCap(const MEM_Pool *pool) {
    DIAG_GUARD(pool != DT_null, PRP_INVALID_SIZE);

    return pool->cap;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMembSize(const MEM_Pool *pool) {
    DIAG_GUARD(pool != DT_null, PRP_INVALID_SIZE);

    return pool->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL MEM_PoolMaxCap(const MEM_Pool *pool) {
    DIAG_GUARD(pool != DT_null, PRP_INVALID_SIZE);

    return MAX_CAP(pool->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL MEM_PoolReset(MEM_Pool *pool) {
    DIAG_GUARD(pool != DT_null, PRP_ERR_INV_ARG);

    pool->free_list = DT_null;
    DT_u8 *curr = pool->mem;
    for (DT_size i = 0; i < pool->cap - 1; i++) {
        *((DT_u8 **)curr) = curr + pool->memb_size;
        curr += pool->memb_size;
    }
    *((DT_u8 **)curr) = pool->free_list;
    pool->free_list = pool->mem;

    return PRP_OK;
}
