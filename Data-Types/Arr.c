#include "Arr.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Arr {
    DT_size cap;
    DT_size len;
    DT_size memb_size;
    DT_u8 *mem;
};

#define DEFAULT_ARR_CAP (16)

#define DEFAULT_NEW_CAP(cap) ((cap) * 2)
#define MAX_CAP(memb_size) (DT_SIZE_MAX / memb_size)

/**
 * Changes the capacity of the given array to the provided new cap safely.
 *
 * @param arr: The array to change the cap of.
 * @param new_cap: The new cap of the array to change to.
 *
 * @return PRP_ERR_OOM if the reallocation fails, PRP_ERR_RES_EXHAUSTED if arr
 * cap reaches a max, otherwise PRP_OK.
 */
static PRP_Result ArrChangeSize(DT_Arr *arr, DT_size new_cap);

static PRP_Result ArrChangeSize(DT_Arr *arr, DT_size new_cap) {
    DIAG_ASSERT(arr != DT_null);

    if (new_cap == MAX_CAP(arr->memb_size)) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    if (new_cap > MAX_CAP(arr->memb_size)) {
        // We can saturate here since the array is len based not cap based.
        new_cap = MAX_CAP(arr->memb_size);
    }
    if (arr->cap == new_cap) {
        return PRP_OK;
    }

    DT_u8 *mem = realloc(arr->mem, new_cap * arr->memb_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    arr->mem = mem;
    arr->cap = new_cap;

    return PRP_OK;
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreate(DT_size memb_size, DT_size cap) {
    DIAG_GUARD(memb_size > 0, DT_null);
    DIAG_GUARD(cap > 0, DT_null);

    if (cap > MAX_CAP(memb_size)) {
        return DT_null;
    }

    DT_Arr *arr = malloc(sizeof(DT_Arr));
    if (!arr) {
        return DT_null;
    }
    arr->mem = malloc(memb_size * cap);
    if (!arr->mem) {
        free(arr);
        return DT_null;
    }
    arr->memb_size = memb_size;
    arr->cap = cap;
    arr->len = 0;

    return arr;
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreateDefault(DT_size memb_size) {
    return DT_ArrCreate(memb_size, DEFAULT_ARR_CAP);
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrClone(DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, DT_null);

    DT_Arr *cpy = DT_ArrCreate(arr->memb_size, arr->cap);
    if (!cpy) {
        return DT_null;
    }
    cpy->len = arr->len;
    memcpy(cpy->mem, arr->mem, arr->memb_size * arr->len);

    return cpy;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrDelete(DT_Arr **pArr) {
    DIAG_GUARD(pArr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(*pArr != DT_null, PRP_ERR_INV_ARG);

    DT_Arr *arr = *pArr;

    if (arr->mem) {
        free(arr->mem);
        arr->mem = DT_null;
    }
    arr->cap = arr->len = arr->memb_size = 0;
    free(arr);
    *pArr = DT_null;

    return PRP_OK;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRaw(const DT_Arr *arr,
                                                DT_size *pLen) {
    DIAG_GUARD(arr != DT_null, DT_null);
    DIAG_GUARD(pLen != DT_null, DT_null);

    *pLen = arr->len;

    return arr->mem;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrLen(const DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, PRP_INVALID_SIZE);

    return arr->len;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrCap(const DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, PRP_INVALID_SIZE);

    return arr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSize(const DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, PRP_INVALID_SIZE);

    return arr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCap(const DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, PRP_INVALID_SIZE);

    return MAX_CAP(arr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGet(const DT_Arr *arr, DT_size i) {
    DIAG_GUARD(arr != DT_null, DT_null);
    DIAG_GUARD(i < arr->len, DT_null);

    return arr->mem + (i * arr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSet(DT_Arr *arr, DT_size i,
                                            const DT_void *pData) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pData != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < arr->len, PRP_ERR_OOB);

    memcpy(arr->mem + (i * arr->memb_size), pData, arr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPush(DT_Arr *arr,
                                             const DT_void *pData) {

    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pData != DT_null, PRP_ERR_INV_ARG);

    if (arr->len == arr->cap) {
        PRP_Result code = ArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap));
        if (code != PRP_OK) {
            return code;
        }
    }
    memcpy(arr->mem + ((arr->len++) * arr->memb_size), pData, arr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserve(DT_Arr *arr, DT_size count) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(count > 0, PRP_ERR_INV_ARG);

    if (arr->cap - arr->len >= count) {
        return PRP_OK;
    }

    return ArrChangeSize(arr, arr->len + count);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsert(DT_Arr *arr,
                                               const DT_void *pData,
                                               DT_size i) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pData != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i <= arr->len, PRP_ERR_OOB);

    if (arr->len == arr->cap) {
        PRP_Result code = ArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap));
        if (code != PRP_OK) {
            return code;
        }
    }
    memmove(arr->mem + ((i + 1) * arr->memb_size),
            arr->mem + (i * arr->memb_size), (arr->len - i) * arr->memb_size);
    memcpy(arr->mem + (i * arr->memb_size), pData, arr->memb_size);
    arr->len++;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPop(DT_Arr *arr, DT_void *pDest) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);

    if (!arr->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    arr->len--;
    if (pDest) {
        memcpy(pDest, arr->mem + (arr->len * arr->memb_size), arr->memb_size);
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrRemove(DT_Arr *arr, DT_void *pDest,
                                               DT_size i) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < arr->len, PRP_ERR_OOB);

    if (pDest) {
        memcpy(pDest, arr->mem + (i * arr->memb_size), arr->memb_size);
    }
    memmove(arr->mem + (i * arr->memb_size),
            arr->mem + ((i + 1) * arr->memb_size),
            (arr->len - i - 1) * arr->memb_size);
    arr->len--;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCmp(const DT_Arr *arr1,
                                            const DT_Arr *arr2,
                                            DT_bool *pRslt) {
    DIAG_GUARD(arr1 != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(arr2 != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pRslt != DT_null, PRP_ERR_INV_ARG);

    if (arr1->len != arr2->len || arr1->memb_size != arr2->memb_size) {
        *pRslt = DT_false;
        return PRP_OK;
    }

    *pRslt = (memcmp(arr1->mem, arr2->mem, arr1->len * arr1->memb_size) == 0);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtend(DT_Arr *arr1,
                                               const DT_Arr *arr2) {
    DIAG_GUARD(arr1 != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(arr2 != DT_null, PRP_ERR_INV_ARG);

    if (arr1->memb_size != arr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    if (arr1->len > MAX_CAP(arr1->memb_size) - arr2->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    DT_size new_cap = arr1->len + arr2->len;
    PRP_Result code = ArrChangeSize(arr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(arr1->mem + (arr1->len * arr1->memb_size), arr2->mem,
           arr2->len * arr2->memb_size);
    arr1->len = new_cap;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSwap(DT_Arr *arr, DT_size i, DT_size j,
                                             DT_void *swap_bffr) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(swap_bffr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(i < arr->len, PRP_ERR_OOB);
    DIAG_GUARD(j < arr->len, PRP_ERR_OOB);

    if (i == j) {
        return PRP_OK;
    }

    DT_u8 *i_elem = arr->mem + (i * arr->memb_size);
    DT_u8 *j_elem = arr->mem + (j * arr->memb_size);
    memcpy(swap_bffr, i_elem, arr->memb_size);
    memcpy(i_elem, j_elem, arr->memb_size);
    memcpy(j_elem, swap_bffr, arr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReset(DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);

#if !defined(PRP_NDEBUG)
    memset(arr->mem, 0, arr->len * arr->memb_size);
#endif
    arr->len = 0;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFit(DT_Arr *arr) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);

    return ArrChangeSize(arr, (arr->len) ? arr->len : DEFAULT_ARR_CAP);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_ArrForEach(DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
              DT_void *user_data) {
    DIAG_GUARD(arr != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(cb != DT_null, PRP_ERR_INV_ARG);

    DT_u8 *mem = arr->mem;
    for (DT_size i = 0; i < arr->len; i++) {
        PRP_Result code = cb(mem, user_data);
        if (code != PRP_OK) {
            return code;
        }
        mem += arr->memb_size;
    }

    return PRP_OK;
}
