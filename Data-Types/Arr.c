#include "Arr.h"
#include "../Diagnostics/Assert.h"
#include <string.h>

struct _Arr {
    DT_size cap;
    DT_size len;
    DT_size memb_size;
    DT_u8 *mem;
};

#define DEFAULT_NEW_CAP(cap) ((cap) * 2)

#define INVARIANT_EXPR(arr)                                                    \
    ((arr) != DT_null && (arr)->mem != DT_null && (arr)->memb_size > 0 &&      \
     (arr)->cap > 0 && (arr)->cap <= DT_ARR_MAX_CAP((arr)->memb_size) &&       \
     (arr)->len <= (arr)->cap)
#define ASSERT_INVARIANT_EXPR(arr)                                             \
    DIAG_ASSERT_MSG(INVARIANT_EXPR(arr),                                       \
                    "The given array is either DT_null, or is corrupted.")
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
    ASSERT_INVARIANT_EXPR(arr);

    DT_size max_cap = DT_ARR_MAX_CAP(arr->memb_size);
    if (arr->cap == max_cap || new_cap > max_cap) {
        return PRP_ERR_RES_EXHAUSTED;
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrGetLastErrCode(DT_void) {
    return last_err_code;
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreateUnchecked(DT_size memb_size,
                                                     DT_size cap) {
    DIAG_ASSERT(memb_size > 0);
    DIAG_ASSERT(cap > 0);

    if (cap > DT_ARR_MAX_CAP(memb_size)) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }

    DT_Arr *arr = malloc(sizeof(DT_Arr));
    if (!arr) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    arr->mem = malloc(memb_size * cap);
    if (!arr->mem) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        free(arr);
        return DT_null;
    }
    arr->memb_size = memb_size;
    arr->cap = cap;
    arr->len = 0;

    return arr;
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCreateChecked(DT_size memb_size,
                                                   DT_size cap) {
    if (!memb_size || !cap) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_ArrCreateUnchecked(memb_size, cap);
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCloneUnchecked(const DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    // Unchecked since we checked for invariants above.
    DT_Arr *cpy = DT_ArrCreateUnchecked(arr->memb_size, arr->cap);
    if (!cpy) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        return DT_null;
    }
    cpy->len = arr->len;
    memcpy(cpy->mem, arr->mem, arr->memb_size * arr->len);

    return cpy;
}

PRP_FN_API DT_Arr *PRP_FN_CALL DT_ArrCloneChecked(const DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    return DT_ArrCloneUnchecked(arr);
}

PRP_FN_API DT_void PRP_FN_CALL DT_ArrDeleteUnchecked(DT_Arr **pArr) {
    DIAG_ASSERT(pArr != DT_null);
    DIAG_ASSERT(*pArr != DT_null && (*pArr)->mem != DT_null);

    DT_Arr *arr = *pArr;

    free(arr->mem);

#if !defined(PRP_NDEBUG)
    arr->mem = DT_null;
    arr->cap = arr->len = arr->memb_size = 0;
#endif

    free(arr);
    *pArr = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrDeleteChecked(DT_Arr **pArr) {
    if (!pArr || !(*pArr) || !(*pArr)->mem) {
        return PRP_ERR_INV_ARG;
    }

    DT_ArrDeleteUnchecked(pArr);

    return PRP_OK;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRawUnchecked(const DT_Arr *arr,
                                                         DT_size *pLen) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pLen != DT_null);

    *pLen = arr->len;

    return arr->mem;
}

PRP_FN_API const DT_void *PRP_FN_CALL DT_ArrRawChecked(const DT_Arr *arr,
                                                       DT_size *pLen) {
    if (!INVARIANT_EXPR(arr) || !pLen) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }

    *pLen = arr->len;

    return arr->mem;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrLenUnchecked(const DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return arr->len;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrLenChecked(const DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return arr->len;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrCapUnchecked(const DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return arr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrCapChecked(const DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return arr->cap;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSizeUnchecked(const DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return arr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMembSizeChecked(const DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return arr->memb_size;
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCapUnchecked(const DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return DT_ARR_MAX_CAP(arr->memb_size);
}

PRP_FN_API DT_size PRP_FN_CALL DT_ArrMaxCapChecked(const DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return PRP_INVALID_SIZE;
    }

    return DT_ARR_MAX_CAP(arr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGetUnchecked(const DT_Arr *arr,
                                                   DT_size i) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(i < arr->len);

    return arr->mem + (i * arr->memb_size);
}

PRP_FN_API DT_void *PRP_FN_CALL DT_ArrGetChecked(const DT_Arr *arr, DT_size i) {
    if (!INVARIANT_EXPR(arr)) {
        SET_LAST_ERR_CODE(PRP_ERR_INV_ARG);
        return DT_null;
    }
    if (i >= arr->len) {
        SET_LAST_ERR_CODE(PRP_ERR_OOB);
        return DT_null;
    }

    return DT_ArrGetUnchecked(arr, i);
}

PRP_FN_API DT_void PRP_FN_CALL DT_ArrSetUnchecked(DT_Arr *arr, DT_size i,
                                                  const DT_void *pData) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pData != DT_null);
    DIAG_ASSERT(i < arr->len);

    memcpy(arr->mem + (i * arr->memb_size), pData, arr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSetChecked(DT_Arr *arr, DT_size i,
                                                   const DT_void *pData) {
    if (!INVARIANT_EXPR(arr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len) {
        return PRP_ERR_OOB;
    }

    DT_ArrSetUnchecked(arr, i, pData);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPushUnchecked(DT_Arr *arr,
                                                      const DT_void *pData) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pData != DT_null);

    if (arr->len == arr->cap) {
        PRP_Result code = ArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap));
        if (code != PRP_OK) {
            return code;
        }
    }
    memcpy(arr->mem + ((arr->len++) * arr->memb_size), pData, arr->memb_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPushChecked(DT_Arr *arr,
                                                    const DT_void *pData) {
    if (!INVARIANT_EXPR(arr) || !pData) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ArrPushUnchecked(arr, pData);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserveUnchecked(DT_Arr *arr,
                                                         DT_size count) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(count > 0);

    if (arr->cap - arr->len >= count) {
        return PRP_OK;
    }

    return ArrChangeSize(arr, arr->len + count);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrReserveChecked(DT_Arr *arr,
                                                       DT_size count) {
    if (!INVARIANT_EXPR(arr) || !count) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ArrReserveUnchecked(arr, count);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsertUnchecked(DT_Arr *arr,
                                                        const DT_void *pData,
                                                        DT_size i) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(pData != DT_null);
    DIAG_ASSERT(i <= arr->len);

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

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrInsertChecked(DT_Arr *arr,
                                                      const DT_void *pData,
                                                      DT_size i) {
    if (!INVARIANT_EXPR(arr) || !pData) {
        return PRP_ERR_INV_ARG;
    }
    if (i > arr->len) {
        return PRP_ERR_OOB;
    }

    return DT_ArrInsertUnchecked(arr, pData, i);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPopUnchecked(DT_Arr *arr,
                                                     DT_void *pDest) {
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrPopChecked(DT_Arr *arr,
                                                   DT_void *pDest) {
    if (!INVARIANT_EXPR(arr)) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ArrPopUnchecked(arr, pDest);
}

PRP_FN_API DT_void PRP_FN_CALL DT_ArrRemoveUnchecked(DT_Arr *arr,
                                                     DT_void *pDest,
                                                     DT_size i) {
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrRemoveChecked(DT_Arr *arr,
                                                      DT_void *pDest,
                                                      DT_size i) {
    if (!INVARIANT_EXPR(arr)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len) {
        return PRP_ERR_OOB;
    }

    DT_ArrRemoveUnchecked(arr, pDest, i);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL DT_ArrCmpUnchecked(const DT_Arr *arr1,
                                                  const DT_Arr *arr2) {
    ASSERT_INVARIANT_EXPR(arr1);
    ASSERT_INVARIANT_EXPR(arr2);

    if (arr1->len != arr2->len || arr1->memb_size != arr2->memb_size) {
        return DT_false;
    }

    return (memcmp(arr1->mem, arr2->mem, arr1->len * arr1->memb_size) == 0);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrCmpChecked(const DT_Arr *arr1,
                                                   const DT_Arr *arr2,
                                                   DT_bool *pRslt) {
    if (!INVARIANT_EXPR(arr1) || !INVARIANT_EXPR(arr2) || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_ArrCmpUnchecked(arr1, arr2);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtendUnchecked(DT_Arr *arr1,
                                                        const DT_Arr *arr2) {
    ASSERT_INVARIANT_EXPR(arr1);
    ASSERT_INVARIANT_EXPR(arr2);
    DIAG_ASSERT(arr1->memb_size == arr2->memb_size);

    if (arr1->len > DT_ARR_MAX_CAP(arr1->memb_size) - arr2->len) {
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrExtendChecked(DT_Arr *arr1,
                                                      const DT_Arr *arr2) {
    if (!INVARIANT_EXPR(arr1) || !INVARIANT_EXPR(arr2) ||
        arr1->memb_size != arr2->memb_size) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ArrExtendUnchecked(arr1, arr2);
}

PRP_FN_API DT_void PRP_FN_CALL DT_ArrSwapUnchecked(DT_Arr *arr, DT_size i,
                                                   DT_size j,
                                                   DT_void *swap_bffr) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(swap_bffr != DT_null);
    DIAG_ASSERT(i < arr->len);
    DIAG_ASSERT(j < arr->len);

    if (i == j) {
        return;
    }

    DT_u8 *i_elem = arr->mem + (i * arr->memb_size);
    DT_u8 *j_elem = arr->mem + (j * arr->memb_size);
    memcpy(swap_bffr, i_elem, arr->memb_size);
    memcpy(i_elem, j_elem, arr->memb_size);
    memcpy(j_elem, swap_bffr, arr->memb_size);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrSwapChecked(DT_Arr *arr, DT_size i,
                                                    DT_size j,
                                                    DT_void *swap_bffr) {
    if (!INVARIANT_EXPR(arr) || !swap_bffr) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= arr->len || j >= arr->len) {
        return PRP_ERR_OOB;
    }

    DT_ArrSwapUnchecked(arr, i, j, swap_bffr);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_ArrResetUnchecked(DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

#if !defined(PRP_NDEBUG)
    memset(arr->mem, 0, arr->len * arr->memb_size);
#endif
    arr->len = 0;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrResetChecked(DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        return PRP_ERR_INV_ARG;
    }

    DT_ArrResetUnchecked(arr);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFitUnchecked(DT_Arr *arr) {
    ASSERT_INVARIANT_EXPR(arr);

    return ArrChangeSize(arr, (arr->len) ? arr->len : DT_ARR_DEFAULT_CAP);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrShrinkFitChecked(DT_Arr *arr) {
    if (!INVARIANT_EXPR(arr)) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ArrShrinkFitUnchecked(arr);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrForEachUnchecked(
    DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data) {
    ASSERT_INVARIANT_EXPR(arr);
    DIAG_ASSERT(cb != DT_null);

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

PRP_FN_API PRP_Result PRP_FN_CALL DT_ArrForEachChecked(
    DT_Arr *arr, PRP_Result (*cb)(DT_void *pVal, DT_void *user_data),
    DT_void *user_data) {
    if (!INVARIANT_EXPR(arr) || !cb) {
        return PRP_ERR_INV_ARG;
    }

    return DT_ArrForEachUnchecked(arr, cb, user_data);
}
