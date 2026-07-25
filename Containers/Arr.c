#include "Arr.h"
#include "Diagnostics/Assert.h"
#include <string.h>

struct _Arr {
    PRP_Size cap;
    PRP_Size len;
    PRP_Size memb_size;
    PRP_U8 *mem;
};

#define ASSERT_INVARIANT_EXPR(arr)                                             \
    DIAG_ASSERT_MSG(CONT_ArrIsValid(arr),                                        \
                    "The given array is either NULL, or is corrupted.")

/**
 * Centralized std policy for array cap increases.
 *
 * @param curr_cap Current cap of the array.
 * @param max_cap  The max cap of the array.
 *
 * @return New cap of the array.
 *
 * @note:
 * - If returned cap == curr_cap, we have hit the max cap and must not do any
 * allocation.
 */
static PRP_Size CapIncPolicy(PRP_Size curr_cap, PRP_Size max_cap);
/**
 * Safely change size of the array.
 *
 * @param arr    Array instance.
 * @param new_cap The new cap of the array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ArrChangeSize(CONT_Arr *arr, PRP_Size new_cap);

static PRP_Size CapIncPolicy(PRP_Size curr_cap, PRP_Size max_cap) {
    if (max_cap / 2 >= curr_cap) {
        return curr_cap * 2;
    } else {
        return max_cap;
    }
}

static PRP_Result ArrChangeSize(CONT_Arr *arr, PRP_Size new_cap) {
    if (arr->cap == new_cap) {
        return PRP_OK;
    }
    PRP_Size max_cap = CONT_ARR_MAX_CAP(arr->memb_size);
    if (arr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
    }

    PRP_U8 *mem = realloc(arr->mem, new_cap * arr->memb_size);
    if (!mem) {
        return PRP_ERR_OOM;
    }
    arr->mem = mem;
    arr->cap = new_cap;

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_ArrIsValid(const CONT_Arr *arr) {
    return (arr != NULL && arr->mem != NULL && arr->memb_size > 0 &&
            arr->cap > 0 && arr->cap <= CONT_ARR_MAX_CAP(arr->memb_size) &&
            arr->len <= arr->cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCreateUnchecked(PRP_Size memb_size,
                                                        PRP_Size cap,
                                                        CONT_Arr **pArr) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(pArr != NULL);

    if (cap > CONT_ARR_MAX_CAP(memb_size)) {
        return PRP_ERR_OOM;
    }

    CONT_Arr *arr = malloc(sizeof(CONT_Arr));
    if (!arr) {
        return PRP_ERR_OOM;
    }
    arr->mem = malloc(memb_size * cap);
    if (!arr->mem) {
        free(arr);
        return PRP_ERR_OOM;
    }
    arr->memb_size = memb_size;
    arr->cap = cap;
    arr->len = 0;

    *pArr = arr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCreateChecked(PRP_Size memb_size,
                                                      PRP_Size cap,
                                                      CONT_Arr **pArr) {
    if (!memb_size || !cap || !pArr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrCreateUnchecked(memb_size, cap, pArr);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCloneUnchecked(const CONT_Arr *arr,
                                                       CONT_Arr **pArr) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pArr != NULL);

    // Unchecked since we checked for invariants above.
    PRP_Result code = CONT_ArrCreateUnchecked(arr->memb_size, arr->cap, pArr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Arr *cpy = *pArr;
    cpy->len = arr->len;
    memcpy(cpy->mem, arr->mem, arr->memb_size * arr->len);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCloneChecked(const CONT_Arr *arr,
                                                     CONT_Arr **pArr) {
    if (!CONT_ArrIsValid(arr) || !pArr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrCloneUnchecked(arr, pArr);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCreateWithDataUnchecked(
    PRP_Size memb_size, const void *membs, PRP_Size len, CONT_Arr **pArr) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(len > 0);
    DIAG_ASSERT(pArr != NULL);

    PRP_Result code = CONT_ArrCreateUnchecked(memb_size, len, pArr);
    if (code != PRP_OK) {
        return code;
    }

    CONT_Arr *arr = *pArr;
    memcpy(arr->mem, membs, memb_size * len);
    arr->len = len;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCreateWithDataChecked(
    PRP_Size memb_size, const void *membs, PRP_Size len, CONT_Arr **pArr) {
    if (!memb_size || !len || !pArr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrCreateWithDataUnchecked(memb_size, membs, len, pArr);
}

PRP_FN_API void PRP_FN_CALL CONT_ArrDeleteUnchecked(CONT_Arr **pArr) {
    DIAG_ASSERT(pArr != NULL);
    DIAG_ASSERT(*pArr != NULL && (*pArr)->mem != NULL);

    CONT_Arr *arr = *pArr;

    free(arr->mem);

#if !defined(PRP_NDEBUG)
    arr->mem = NULL;
    arr->cap = arr->len = arr->memb_size = 0;
#endif

    free(arr);
    *pArr = NULL;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrDeleteChecked(CONT_Arr **pArr) {
    if (!pArr || !(*pArr) || !(*pArr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ArrDeleteUnchecked(pArr);

    return PRP_OK;
}

PRP_FN_API const void *PRP_FN_CALL CONT_ArrRawUnchecked(const CONT_Arr *arr,
                                                      PRP_Size *pLen) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pLen != NULL);

    *pLen = arr->len;

    return arr->mem;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrRawChecked(const CONT_Arr *arr,
                                                   PRP_Size *pLen,
                                                   const void **pRaw) {
    if (!CONT_ArrIsValid(arr) || !pLen || !pRaw) {
        return PRP_ERR_INV_ARG;
    }

    *pLen = arr->len;
    *pRaw = arr->mem;

    return PRP_OK;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_ArrLen(const CONT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return arr->len;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_ArrCap(const CONT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return arr->cap;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_ArrMembSize(const CONT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return arr->memb_size;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_ArrMaxCap(const CONT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return CONT_ARR_MAX_CAP(arr->memb_size);
}

PRP_FN_API void *PRP_FN_CALL CONT_ArrGetUnchecked(const CONT_Arr *arr, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(i < arr->len);

    return arr->mem + (i * arr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrGetChecked(const CONT_Arr *arr,
                                                   PRP_Size i, void **dest) {
    if (!CONT_ArrIsValid(arr) || !dest) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len) {
        return PRP_ERR_OOB;
    }

    *dest = CONT_ArrGetUnchecked(arr, i);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_ArrSetUnchecked(CONT_Arr *arr, PRP_Size i,
                                               const void *pData) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i < arr->len);

    memcpy(arr->mem + (i * arr->memb_size), pData, arr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrSetChecked(CONT_Arr *arr, PRP_Size i,
                                                   const void *pData) {
    if (!CONT_ArrIsValid(arr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len) {
        return PRP_ERR_OOB;
    }

    CONT_ArrSetUnchecked(arr, i, pData);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrPushUnchecked(CONT_Arr *arr,
                                                      const void *pData) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pData != NULL);

    if (arr->len == arr->cap) {
        PRP_Size new_cap =
            CapIncPolicy(arr->cap, CONT_ARR_MAX_CAP(arr->memb_size));
        if (new_cap == arr->cap) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Result code = ArrChangeSize(arr, new_cap);
        if (code != PRP_OK) {
            return code;
        }
    }
    memcpy(arr->mem + ((arr->len++) * arr->memb_size), pData, arr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrPushChecked(CONT_Arr *arr,
                                                    const void *pData) {
    if (!CONT_ArrIsValid(arr) || !pData) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrPushUnchecked(arr, pData);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrReserveUnchecked(CONT_Arr *arr,
                                                         PRP_Size count) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(count > 0);

    if (arr->cap - arr->len >= count) {
        return PRP_OK;
    }

    return ArrChangeSize(arr, arr->len + count);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrReserveChecked(CONT_Arr *arr,
                                                       PRP_Size count) {
    if (!CONT_ArrIsValid(arr) || !count) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrReserveUnchecked(arr, count);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrInsertUnchecked(CONT_Arr *arr,
                                                        const void *pData,
                                                        PRP_Size i) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pData != NULL);
    DIAG_ASSERT(i <= arr->len);

    if (arr->len == arr->cap) {
        PRP_Size new_cap =
            CapIncPolicy(arr->cap, CONT_ARR_MAX_CAP(arr->memb_size));
        if (new_cap == arr->cap) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Result code = ArrChangeSize(arr, new_cap);
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

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrInsertChecked(CONT_Arr *arr,
                                                      const void *pData,
                                                      PRP_Size i) {
    if (!CONT_ArrIsValid(arr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i > arr->len) {
        return PRP_ERR_OOB;
    }

    return CONT_ArrInsertUnchecked(arr, pData, i);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrPopUnchecked(CONT_Arr *arr, void *pDest) {
    ASSERT_INVARIANT_EXPR(arr);

    if (!arr->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    arr->len--;
    if (pDest) {
        memcpy(pDest, arr->mem + (arr->len * arr->memb_size), arr->memb_size);
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrPopChecked(CONT_Arr *arr, void *pDest) {
    if (!CONT_ArrIsValid(arr)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrPopUnchecked(arr, pDest);
}

PRP_FN_API void PRP_FN_CALL CONT_ArrRemoveUnchecked(CONT_Arr *arr, void *pDest,
                                                  PRP_Size i) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(i < arr->len);

    if (pDest) {
        memcpy(pDest, arr->mem + (i * arr->memb_size), arr->memb_size);
    }
    memmove(arr->mem + (i * arr->memb_size),
            arr->mem + ((i + 1) * arr->memb_size),
            (arr->len - i - 1) * arr->memb_size);
    arr->len--;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrRemoveChecked(CONT_Arr *arr, void *pDest,
                                                      PRP_Size i) {
    if (!CONT_ArrIsValid(arr)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len) {
        return PRP_ERR_OOB;
    }

    CONT_ArrRemoveUnchecked(arr, pDest, i);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_ArrCmpUnchecked(const CONT_Arr *arr1,
                                                   const CONT_Arr *arr2) {
    ASSERT_INVARIANT_EXPR(arr1);
    ASSERT_INVARIANT_EXPR(arr2);

    if (arr1->len != arr2->len || arr1->memb_size != arr2->memb_size) {
        return PRP_False;
    }

    return (memcmp(arr1->mem, arr2->mem, arr1->len * arr1->memb_size) == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrCmpChecked(const CONT_Arr *arr1,
                                                   const CONT_Arr *arr2,
                                                   PRP_Bool *pRslt) {
    if (!CONT_ArrIsValid(arr1) || !CONT_ArrIsValid(arr2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_ArrCmpUnchecked(arr1, arr2);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrExtendUnchecked(CONT_Arr *arr1,
                                                        const CONT_Arr *arr2) {
    ASSERT_INVARIANT_EXPR(arr1);
    ASSERT_INVARIANT_EXPR(arr2);
    DIAG_ASSERT(arr1->memb_size == arr2->memb_size);

    if (arr1->len > CONT_ARR_MAX_CAP(arr1->memb_size) - arr2->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    PRP_Size new_cap = arr1->len + arr2->len;
    PRP_Result code = ArrChangeSize(arr1, new_cap);
    if (code != PRP_OK) {
        return code;
    }
    memcpy(arr1->mem + (arr1->len * arr1->memb_size), arr2->mem,
           arr2->len * arr2->memb_size);
    arr1->len = new_cap;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrExtendChecked(CONT_Arr *arr1,
                                                      const CONT_Arr *arr2) {
    if (!CONT_ArrIsValid(arr1) || !CONT_ArrIsValid(arr2) ||
        arr1->memb_size != arr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrExtendUnchecked(arr1, arr2);
}

PRP_FN_API void PRP_FN_CALL CONT_ArrSwapUnchecked(CONT_Arr *arr, PRP_Size i,
                                                PRP_Size j, void *swap_bffr) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(swap_bffr != NULL);
    DIAG_ASSERT(i < arr->len);
    DIAG_ASSERT(j < arr->len);

    if (i == j) {
        return;
    }

    PRP_U8 *i_elem = arr->mem + (i * arr->memb_size);
    PRP_U8 *j_elem = arr->mem + (j * arr->memb_size);
    memcpy(swap_bffr, i_elem, arr->memb_size);
    memcpy(i_elem, j_elem, arr->memb_size);
    memcpy(j_elem, swap_bffr, arr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrSwapChecked(CONT_Arr *arr, PRP_Size i,
                                                    PRP_Size j,
                                                    void *swap_bffr) {
    if (!CONT_ArrIsValid(arr) || !swap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len || j >= arr->len) {
        return PRP_ERR_OOB;
    }

    CONT_ArrSwapUnchecked(arr, i, j, swap_bffr);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_ArrResetUnchecked(CONT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

#if !defined(PRP_NDEBUG)
    memset(arr->mem, 0, arr->len * arr->memb_size);
#endif
    arr->len = 0;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrResetChecked(CONT_Arr *arr) {
    if (!CONT_ArrIsValid(arr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_ArrResetUnchecked(arr);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrShrinkFitUnchecked(CONT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return ArrChangeSize(arr, (arr->len) ? arr->len : CONT_ARR_DEFAULT_CAP);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrShrinkFitChecked(CONT_Arr *arr) {
    if (!CONT_ArrIsValid(arr)) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrShrinkFitUnchecked(arr);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrForEachUnchecked(
    CONT_Arr *arr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(cb != NULL);

    PRP_U8 *mem = arr->mem;
    for (PRP_Size i = 0; i < arr->len; i++) {
        PRP_Result code = cb(mem, pUser_data);
        if (code != PRP_OK) {
            return code;
        }
        mem += arr->memb_size;
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_ArrForEachChecked(
    CONT_Arr *arr, PRP_Result (*cb)(void *pVal, void *pUser_data),
    void *pUser_data) {
    if (!CONT_ArrIsValid(arr) || !cb) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_ArrForEachUnchecked(arr, cb, pUser_data);
}
