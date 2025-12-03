#include "Pool.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _Pool {
    DT_size cap;
    DT_size memb_size;
    DT_u8 *free_list;
    DT_u8 mem[];
};

#define POOL_VALIDITY_CHECK(pool, ret)                                         \
    do {                                                                       \
        if (!pool) {                                                           \
            PRP_LOG_FN_INV_ARG_ERROR(pool);                                    \
            return ret;                                                        \
        }                                                                      \
    } while (0)

PRP_FN_API DT_Pool *PRP_FN_CALL DT_PoolCreate(DT_size memb_size, DT_size cap) {
    if (!memb_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Pool can't be made with memb_size=0.");
        return DT_null;
    }
    if (!cap) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Pool can't be made with cap=0.");
        return DT_null;
    }

    DT_Pool *pool = malloc(sizeof(DT_Pool) + (memb_size * cap));
    if (!pool) {
        PRP_LOG_FN_MALLOC_ERROR(pool);
        return DT_null;
    }
    // Padding the memb_size to ensure the freelist can work.
    pool->memb_size =
        (memb_size >= sizeof(DT_void *)) ? memb_size : sizeof(DT_void *);
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

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_PoolDelete(DT_Pool **pPool) {
    if (!pPool) {
        PRP_LOG_FN_INV_ARG_ERROR(pPool);
        return PRP_FN_INV_ARG_ERROR;
    }
    DT_Pool *pool = *pPool;
    POOL_VALIDITY_CHECK(pool, PRP_FN_INV_ARG_ERROR);

    pool->memb_size = pool->cap = 0;
    pool->free_list = DT_null;
    free(pool);
    *pPool = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_void *PRP_FN_CALL DT_PoolAlloc(DT_Pool *pool) {
    POOL_VALIDITY_CHECK(pool, DT_null);

    if (!pool->free_list) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot allocate more objects, pool is full.");
        return DT_null;
    }

    DT_void *ptr = pool->free_list;
    pool->free_list = *((DT_u8 **)pool->free_list);

    return ptr;
}

PRP_FN_API DT_void *PRP_FN_CALL DT_PoolCalloc(DT_Pool *pool) {
    POOL_VALIDITY_CHECK(pool, DT_null);

    if (!pool->free_list) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "Cannot allocate more objects, pool is full.");
        return DT_null;
    }

    DT_void *ptr = pool->free_list;
    pool->free_list = *((DT_u8 **)pool->free_list);
    memset(ptr, 0, pool->memb_size);

    return ptr;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_PoolFree(DT_Pool *pool, DT_void *ptr) {
    POOL_VALIDITY_CHECK(pool, PRP_FN_INV_ARG_ERROR);
    if (!ptr) {
        PRP_LOG_FN_INV_ARG_ERROR(ptr);
        return PRP_FN_INV_ARG_ERROR;
    }

    DT_u8 *p = ptr;
    if (p < pool->mem || p >= pool->mem + pool->cap * pool->memb_size ||
        ((p - pool->mem) % pool->memb_size) != 0) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "The given ptr to free is not part of the pool's "
                        "allocated memory.");
        return PRP_FN_INV_ARG_ERROR;
    }

    /*
     * This currently doesn't protect against double free of the memory. It will
     * curropt the memory eventually if a double free happens.
     *
     * TODO: Fix it.
     */
    *((DT_u8 **)ptr) = pool->free_list;
    pool->free_list = ptr;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_PoolReset(DT_Pool *pool) {
    POOL_VALIDITY_CHECK(pool, PRP_FN_INV_ARG_ERROR);

    pool->free_list = DT_null;
    DT_u8 *curr = pool->mem;
    for (DT_size i = 0; i < pool->cap - 1; i++) {
        *((DT_u8 **)curr) = curr + pool->memb_size;
        curr += pool->memb_size;
    }
    *((DT_u8 **)curr) = pool->free_list;
    pool->free_list = pool->mem;

    return PRP_FN_SUCCESS;
}
