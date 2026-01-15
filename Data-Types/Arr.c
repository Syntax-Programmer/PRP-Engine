#include "Arr.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _Arr {
    DT_size cap;
    DT_size len;
    DT_size memb_size;
    DT_u8 *mem;
    // A sep bffr allocated so that any misc op can use it just in case safely.
    DT_u8 *elem_bffr;
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
 * @return PRP_FN_MALLOC_ERROR if the reallocation fails, otherwise
 * PRP_FN_SUCCESS;
 */
static PRP_FnCode ArrChangeSize(DT_Arr *arr, DT_size new_cap);

static PRP_FnCode ArrChangeSize(DT_Arr *arr, DT_size new_cap) {
    if (new_cap == MAX_CAP(arr->memb_size)) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Max capacity of the array reach, cannot grow further.");
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }
    if (new_cap > MAX_CAP(arr->memb_size)) {
        // We can saturate here since the array is len based not cap based.
        new_cap = MAX_CAP(arr->memb_size);
    }
    if (arr->cap == new_cap) {
        return PRP_FN_SUCCESS;
    }

    DT_u8 *mem = realloc(arr->mem, new_cap * arr->memb_size);
    if (!mem) {
        PRP_LOG_FN_MALLOC_ERROR(mem);
        return PRP_FN_MALLOC_ERROR;
    }
    arr->mem = mem;
    arr->cap = new_cap;

    return PRP_FN_SUCCESS;
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreate(DT_size memb_size, DT_size cap) {
    if (!memb_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "DT_Arr can't be made with memb_size=0.");
        return DT_null;
    }
    if (!cap) {
        cap = DEFAULT_ARR_CAP;
    }
    if (cap > MAX_CAP(memb_size)) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "DT_Arr create capcity too big to accomodate. Max "
                        "capacity of array with memb_size=%zu is %zu",
                        memb_size, MAX_CAP(memb_size));
        return DT_null;
    }

    DT_Arr *arr = malloc(sizeof(DT_Arr));
    if (!arr) {
        PRP_LOG_FN_MALLOC_ERROR(arr);
        return DT_null;
    }
    arr->mem = malloc(memb_size * cap);
    if (!arr->mem) {
        free(arr);
        PRP_LOG_FN_MALLOC_ERROR(arr->mem);
        return DT_null;
    }
    arr->elem_bffr = malloc(memb_size);
    if (!arr->elem_bffr) {
        free(arr->mem);
        free(arr);
        PRP_LOG_FN_MALLOC_ERROR(arr->elem_bffr);
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
    PRP_NULL_ARG_CHECK(arr, DT_null);

    DT_Arr *cpy = DT_ArrCreate(arr->memb_size, arr->cap);
    if (!cpy) {
        PRP_LOG_FN_MALLOC_ERROR(cpy);
        return DT_null;
    }
    cpy->len = arr->len;
    memcpy(cpy->mem, arr->mem, arr->memb_size * arr->len);

    return cpy;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrDelete(DT_Arr **pArr) {
    PRP_NULL_ARG_CHECK(pArr, PRP_FN_INV_ARG_ERROR);
    DT_Arr *arr = *pArr;
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);

    if (arr->mem) {
        free(arr->mem);
        arr->mem = DT_null;
    }
    if (arr->elem_bffr) {
        free(arr->elem_bffr);
        arr->elem_bffr = DT_null;
    }
    arr->cap = arr->len = arr->memb_size = 0;
    free(arr);
    *pArr = DT_null;

    return PRP_FN_SUCCESS;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRaw(const DT_Arr *arr,
                                                DT_size *pLen) {
    PRP_NULL_ARG_CHECK(arr, DT_null);
    PRP_NULL_ARG_CHECK(pLen, DT_null);

    *pLen = arr->len;

    return arr->mem;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrLen(const DT_Arr *arr) {
    PRP_NULL_ARG_CHECK(arr, PRP_INVALID_SIZE);

    return arr->len;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrCap(const DT_Arr *arr) {
    PRP_NULL_ARG_CHECK(arr, PRP_INVALID_SIZE);

    return arr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSize(const DT_Arr *arr) {
    PRP_NULL_ARG_CHECK(arr, PRP_INVALID_SIZE);

    return arr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCap(const DT_Arr *arr) {
    PRP_NULL_ARG_CHECK(arr, PRP_INVALID_SIZE);

    return MAX_CAP(arr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGet(const DT_Arr *arr, DT_size i) {
    PRP_NULL_ARG_CHECK(arr, DT_null);
    if (i >= arr->len) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the array index: %zu, of an array with len: %zu",
            i, arr->len);
        return DT_null;
    }

    return arr->mem + (i * arr->memb_size);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrSet(DT_Arr *arr, DT_size i,
                                            const DT_void *pData) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pData, PRP_FN_INV_ARG_ERROR);
    if (i >= arr->len) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the array index: %zu, of an array with len: %zu",
            i, arr->len);
        return PRP_FN_OOB_ERROR;
    }

    memcpy(arr->mem + (i * arr->memb_size), pData, arr->memb_size);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrPush(DT_Arr *arr,
                                             const DT_void *pData) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pData, PRP_FN_INV_ARG_ERROR);

    if (arr->len == arr->cap) {
        PRP_FnCode code = ArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap));
        if (code != PRP_FN_SUCCESS) {
            PRP_LOG_FN_CODE(
                code,
                "Cannot push more elements into the array. The array is full.");
            return code;
        }
    }
    memcpy(arr->mem + ((arr->len++) * arr->memb_size), pData, arr->memb_size);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrReserve(DT_Arr *arr, DT_size count) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    if (!count) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Cannot reserve 0 members in DT_Arr.");
        return PRP_FN_INV_ARG_ERROR;
    }

    if (arr->cap - arr->len >= count) {
        return PRP_FN_SUCCESS;
    }

    PRP_FnCode code = ArrChangeSize(arr, arr->len + count);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(
            code, "Cannot reserve %zu more elements into the array.", count);
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrInsert(DT_Arr *arr,
                                               const DT_void *pData,
                                               DT_size i) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pData, PRP_FN_INV_ARG_ERROR);
    if (i >= arr->len) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the array index: %zu, of an array with len: %zu",
            i, arr->len);
        return PRP_FN_OOB_ERROR;
    }

    if (arr->len == arr->cap) {
        PRP_FnCode code = ArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap));
        if (code != PRP_FN_SUCCESS) {
            PRP_LOG_FN_CODE(code, "Cannot insert more elements into the array. "
                                  "The array is full.");
            return code;
        }
    }
    memmove(arr->mem + ((i + 1) * arr->memb_size),
            arr->mem + (i * arr->memb_size), (arr->len - i) * arr->memb_size);
    memcpy(arr->mem + (i * arr->memb_size), pData, arr->memb_size);
    arr->len++;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrPop(DT_Arr *arr, DT_void *pDest) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);

    if (!arr->len) {
        PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                        "No more elements to pop from array.");
        return PRP_FN_RES_EXHAUSTED_ERROR;
    }
    arr->len--;
    if (pDest) {
        memcpy(pDest, arr->mem + (arr->len * arr->memb_size), arr->memb_size);
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrRemove(DT_Arr *arr, DT_void *pDest,
                                               DT_size i) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    if (i >= arr->len) {
        PRP_LOG_FN_CODE(
            PRP_FN_OOB_ERROR,
            "Tried accessing the array index: %zu, of an array with len: %zu",
            i, arr->len);
        return PRP_FN_OOB_ERROR;
    }

    if (pDest) {
        memcpy(pDest, arr->mem + (i * arr->memb_size), arr->memb_size);
    }
    memmove(arr->mem + (i * arr->memb_size),
            arr->mem + ((i + 1) * arr->memb_size),
            (arr->len - i - 1) * arr->memb_size);
    arr->len--;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrCmp(const DT_Arr *arr1,
                                            const DT_Arr *arr2,
                                            DT_bool *pRslt) {
    PRP_NULL_ARG_CHECK(arr1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(arr2, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(pRslt, PRP_FN_INV_ARG_ERROR);

    if (arr1->len != arr2->len || arr1->memb_size != arr2->memb_size) {
        *pRslt = DT_false;
        return PRP_FN_SUCCESS;
    }

    *pRslt = (memcmp(arr1->mem, arr2->mem, arr1->len * arr1->memb_size) == 0);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrExtend(DT_Arr *arr1,
                                               const DT_Arr *arr2) {
    PRP_NULL_ARG_CHECK(arr1, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(arr2, PRP_FN_INV_ARG_ERROR);
    if (arr1->memb_size != arr2->memb_size) {
        PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                        "Cannot join two arrays with different memb_sizes.");
        return PRP_FN_INV_ARG_ERROR;
    }

    if (arr1->len > MAX_CAP(arr1->memb_size) - arr2->len) {
        PRP_LOG_FN_CODE(
            PRP_FN_RES_EXHAUSTED_ERROR,
            "Combined capacity of arr1 exceeds max array capacity of %zu",
            MAX_CAP(arr1->memb_size));
    }
    DT_size new_cap = arr1->len + arr2->len;
    DT_u8 *mem = realloc(arr1->mem, new_cap * arr1->memb_size);
    if (mem) {
        PRP_LOG_FN_MALLOC_ERROR(mem);
        return PRP_FN_MALLOC_ERROR;
    }
    memcpy(mem + (arr1->len * arr1->memb_size), arr2->mem,
           arr2->len * arr2->memb_size);
    arr1->mem = mem;
    arr1->len = arr1->cap = new_cap;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrSwap(DT_Arr *arr, DT_size i,
                                             DT_size j) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    if (i >= arr->len || j >= arr->len) {
        PRP_LOG_FN_CODE(PRP_FN_OOB_ERROR,
                        "Tried accessing the array indices: %zu-%zu, of an "
                        "array with len: %zu",
                        i, j, arr->len);
        return PRP_FN_OOB_ERROR;
    }
    if (i == j) {
        return PRP_FN_SUCCESS;
    }

    DT_u8 *i_elem = arr->mem + (i * arr->memb_size);
    DT_u8 *j_elem = arr->mem + (j * arr->memb_size);
    memcpy(arr->elem_bffr, i_elem, arr->memb_size);
    memcpy(i_elem, j_elem, arr->memb_size);
    memcpy(j_elem, arr->elem_bffr, arr->memb_size);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrReset(DT_Arr *arr) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);

    arr->len = 0;

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_ArrShrinkFit(DT_Arr *arr) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);

    PRP_FnCode code =
        ArrChangeSize(arr, (arr->len) ? arr->len : DEFAULT_ARR_CAP);
    if (code != PRP_FN_SUCCESS) {
        PRP_LOG_FN_CODE(code, "Cannot shrink fit the given array.");
        return code;
    }

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
DT_ArrForEach(DT_Arr *arr, PRP_FnCode (*cb)(DT_void *pVal, DT_void *user_data),
              DT_void *user_data) {
    PRP_NULL_ARG_CHECK(arr, PRP_FN_INV_ARG_ERROR);
    PRP_NULL_ARG_CHECK(cb, PRP_FN_INV_ARG_ERROR);

    DT_u8 *mem = arr->mem;
    for (DT_size i = 0; i < arr->len; i++) {
        PRP_FnCode code = cb(mem, user_data);
        if (code != PRP_FN_SUCCESS) {
            return code;
        }
        mem += arr->memb_size;
    }

    return PRP_FN_SUCCESS;
}
