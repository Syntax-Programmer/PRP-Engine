#include "Pool.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

struct MEM_Pool {
    PRP_Size cap;
    PRP_Size memb_size;
    PRP_U8 *pFree_list;
    PRP_U8 pMem[];
};

#define MAX_ALLOCABLE_SIZE (PRP_SIZE_MAX - sizeof(MEM_Pool))
// Define it with max size so no extra checks for that.
#define MAX_CAP(memb_size) (MAX_ALLOCABLE_SIZE / memb_size)

#define ASSERT_INVARIANT_EXPR(pPool)                                           \
    PRP_DIAG_ASSERT_MSG(MEM_PoolIsValid(pPool), "The given pPool is invalid.")

PRP_API PRP_Bool PRP_CALL MEM_PoolIsValid(const MEM_Pool *pPool) {
    return (pPool != NULL && pPool->memb_size > 0 &&
            pPool->cap <= MAX_CAP(pPool->memb_size));
}

PRP_API PRP_Result PRP_CALL MEM_PoolCreateUnchecked(PRP_Size memb_size,
                                                    PRP_Size cap,
                                                    MEM_Pool **ppPool) {
    PRP_DIAG_ASSERT_MSG(memb_size > 0,
                        "The memb_size of the pool must be > 0.");
    PRP_DIAG_ASSERT_MSG(cap > 0, "The cap of the pool must be > 0.");
    PRP_DIAG_ASSERT(ppPool != NULL);

    *ppPool = NULL;
    memb_size = PRP_MAX(memb_size, sizeof(void *));
    if (cap > MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    // Padding the memb_size to ensure the freelist can work.
    MEM_Pool *pPool = malloc(sizeof(MEM_Pool) + (memb_size * cap));
    if (!pPool) {
        return PRP_ERR_OOM;
    }
    pPool->memb_size = memb_size;
    pPool->cap = cap;
    pPool->pFree_list = NULL;
    PRP_U8 *pCurr = pPool->pMem;
    for (PRP_Size i = 0; i < pPool->cap - 1; i++) {
        *((PRP_U8 **)pCurr) = pCurr + pPool->memb_size;
        pCurr += memb_size;
    }
    *((PRP_U8 **)pCurr) = pPool->pFree_list;
    pPool->pFree_list = pPool->pMem;

    *ppPool = pPool;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolCreateChecked(PRP_Size memb_size,
                                                  PRP_Size cap,
                                                  MEM_Pool **ppPool) {
    if (!memb_size || !cap || !ppPool) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_PoolCreateUnchecked(memb_size, cap, ppPool);
}

PRP_API void PRP_CALL MEM_PoolDeleteUnchecked(MEM_Pool **ppPool) {
    PRP_DIAG_ASSERT(ppPool != NULL);
    PRP_DIAG_ASSERT(*ppPool != NULL);

    MEM_Pool *pPool = *ppPool;

    pPool->memb_size = pPool->cap = 0;
    pPool->pFree_list = NULL;
    free(pPool);
    *ppPool = NULL;
}

PRP_API PRP_Result PRP_CALL MEM_PoolDeleteChecked(MEM_Pool **ppPool) {
    if (!ppPool || !(*ppPool)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolDeleteUnchecked(ppPool);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolAllocUnchecked(MEM_Pool *pPool,
                                                   void **ppDest) {
    ASSERT_INVARIANT_EXPR(pPool);
    PRP_DIAG_ASSERT(ppDest != NULL);

    if (!pPool->pFree_list) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = pPool->pFree_list;
    pPool->pFree_list = *((PRP_U8 **)pPool->pFree_list);
    *ppDest = ptr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolAllocChecked(MEM_Pool *pPool,
                                                 void **ppDest) {
    if (!MEM_PoolIsValid(pPool) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_PoolAllocUnchecked(pPool, ppDest);
}

PRP_API PRP_Result PRP_CALL MEM_PoolCallocUnchecked(MEM_Pool *pPool,
                                                    void **ppDest) {
    ASSERT_INVARIANT_EXPR(pPool);
    PRP_DIAG_ASSERT(ppDest != NULL);

    if (!pPool->pFree_list) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    void *ptr = pPool->pFree_list;
    pPool->pFree_list = *((PRP_U8 **)pPool->pFree_list);
    memset(ptr, 0, pPool->memb_size);
    *ppDest = ptr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL MEM_PoolCallocChecked(MEM_Pool *pPool,
                                                  void **ppDest) {
    if (!MEM_PoolIsValid(pPool) || !ppDest) {
        return PRP_ERR_INV_ARG;
    }

    return MEM_PoolCallocUnchecked(pPool, ppDest);
}

PRP_API void PRP_CALL MEM_PoolFreeUnchecked(MEM_Pool *pPool, void *ptr) {
    PRP_DIAG_ASSERT(ptr != NULL);
    ASSERT_INVARIANT_EXPR(pPool);
    PRP_DIAG_ASSERT_MSG(
        ((PRP_U8 *)ptr > pPool->mem &&
         (PRP_U8 *)ptr < pPool->mem + (pPool->cap * pPool->memb_size) &&
         (((PRP_U8 *)ptr - pPool->mem) % pPool->memb_size) == 0),
        "The given pointer to free doesn't belong to the pPool provided.");

    /*
     * This currently doesn't protect against double free of the memory. It will
     * curropt the memory eventually if a double free happens.
     *
     * TODO: Fix it.
     */
    *((PRP_U8 **)ptr) = pPool->pFree_list;
    pPool->pFree_list = ptr;
}

PRP_API PRP_Result PRP_CALL MEM_PoolFreeChecked(MEM_Pool *pPool, void *ptr) {
    if (!ptr || !MEM_PoolIsValid(pPool)) {
        return PRP_ERR_INV_ARG;
    }
    PRP_U8 *p = ptr;
    if (p < pPool->pMem || p >= pPool->pMem + pPool->cap * pPool->memb_size ||
        ((p - pPool->pMem) % pPool->memb_size) != 0) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolFreeUnchecked(pPool, ptr);

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL MEM_PoolCap(const MEM_Pool *pPool) {
    ASSERT_INVARIANT_EXPR(pPool);

    return pPool->cap;
}

PRP_API PRP_Size PRP_CALL MEM_PoolMembSize(const MEM_Pool *pPool) {
    ASSERT_INVARIANT_EXPR(pPool);

    return pPool->memb_size;
}

PRP_API PRP_Size PRP_CALL MEM_PoolMaxCap(const MEM_Pool *pPool) {
    ASSERT_INVARIANT_EXPR(pPool);

    return MAX_CAP(pPool->memb_size);
}

PRP_API void PRP_CALL MEM_PoolResetUnchecked(MEM_Pool *pPool) {
    ASSERT_INVARIANT_EXPR(pPool);

    pPool->pFree_list = NULL;
    PRP_U8 *pCurr = pPool->pMem;
    for (PRP_Size i = 0; i < pPool->cap - 1; i++) {
        *((PRP_U8 **)pCurr) = pCurr + pPool->memb_size;
        pCurr += pPool->memb_size;
    }
    *((PRP_U8 **)pCurr) = pPool->pFree_list;
    pPool->pFree_list = pPool->pMem;
}

PRP_API PRP_Result PRP_CALL MEM_PoolResetChecked(MEM_Pool *pPool) {
    if (!MEM_PoolIsValid(pPool)) {
        return PRP_ERR_INV_ARG;
    }

    MEM_PoolResetUnchecked(pPool);

    return PRP_OK;
}
