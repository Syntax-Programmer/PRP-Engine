#include "StringArr.h"
#include "Diagnostics/Assert.h"
#include <string.h>

typedef struct {
    PRP_Size ofs;
    PRP_Size len;
} StrInfo;

struct _StrArr {
    PRP_Size len;
    PRP_Size cap;
    StrInfo *pStr_info;

    PRP_Size bffr_size;
    PRP_Size occupied_size;
    PRP_Char8 *pBffr;
};

#define MAX_CAP (PRP_SIZE_MAX / sizeof(StrInfo))

#define ASSERT_INVARIANT_EXPR(str_arr)                                         \
    DIAG_ASSERT_MSG(                                                           \
        CONT_StrArrIsValid(str_arr),                                           \
        "The given string buffer is either NULL, or is corrupted.")

/**
 * Makes sure that the string of given len can be housed in strin-array.
 *
 * @param str_arr String-Array instance.
 * @param str_len Len of string to accomodate.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result AccomodateString(CONT_StrArr *str_arr, PRP_Size str_len);

PRP_FN_API PRP_Bool PRP_FN_CALL CONT_StrArrIsValid(const CONT_StrArr *str_arr) {
    return (str_arr != NULL && str_arr->pBffr != NULL &&
            str_arr->bffr_size > 0 &&
            str_arr->occupied_size <= str_arr->bffr_size && str_arr->cap > 0 &&
            str_arr->cap < MAX_CAP && str_arr->len <= str_arr->cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrCreateUnchecked(
    PRP_Size init_bffr_size, PRP_Size cap, CONT_StrArr **pStr_arr) {
    DIAG_ASSERT(init_bffr_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(pStr_arr != NULL);

    if (cap > MAX_CAP) {
        return PRP_ERR_OOM;
    }
    CONT_StrArr *str_arr = malloc(sizeof(CONT_StrArr));
    if (!str_arr) {
        return PRP_ERR_OOM;
    }
    str_arr->pBffr = malloc(init_bffr_size);
    if (!str_arr->pBffr) {
        free(str_arr);
        return PRP_ERR_OOM;
    }
    str_arr->pStr_info = malloc(sizeof(StrInfo) * cap);
    if (!str_arr->pStr_info) {
        free(str_arr->pBffr);
        free(str_arr);
        return PRP_ERR_OOM;
    }
    str_arr->bffr_size = init_bffr_size;
    str_arr->occupied_size = 0;
    str_arr->cap = cap;
    str_arr->len = 0;

    *pStr_arr = str_arr;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrCreateChecked(
    PRP_Size init_bffr_size, PRP_Size cap, CONT_StrArr **pStr_arr) {
    if (!init_bffr_size || !cap || !pStr_arr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrCreateUnchecked(init_bffr_size, cap, pStr_arr);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrCloneUnchecked(const CONT_StrArr *str_arr, CONT_StrArr **pStr_arr) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr_arr != NULL);

    PRP_Result code = CONT_StrArrCreateUnchecked(str_arr->occupied_size,
                                                 str_arr->len, pStr_arr);
    if (code != PRP_OK) {
        return code;
    }
    CONT_StrArr *cpy = *pStr_arr;
    cpy->occupied_size = str_arr->occupied_size;
    cpy->len = str_arr->len;
    memcpy(cpy->pBffr, str_arr->pBffr, str_arr->occupied_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrCloneChecked(const CONT_StrArr *str_arr, CONT_StrArr **pStr_arr) {
    if (!CONT_StrArrIsValid(str_arr) || !pStr_arr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrCloneUnchecked(str_arr, pStr_arr);
}

PRP_FN_API void PRP_FN_CALL CONT_StrArrDeleteUnchecked(CONT_StrArr **pStr_arr) {
    DIAG_ASSERT(pStr_arr != NULL);
    DIAG_ASSERT(*pStr_arr != NULL);
    DIAG_ASSERT((*pStr_arr)->pBffr != NULL && (*pStr_arr)->pStr_info != NULL);

    CONT_StrArr *str_arr = *pStr_arr;
    free(str_arr->pBffr);
    free(str_arr->pStr_info);

#ifdef PRP_DEBUG_MODE
    str_arr->pBffr = NULL;
    str_arr->pStr_info = NULL;
    str_arr->bffr_size = str_arr->occupied_size = 0;
    str_arr->cap = str_arr->len = 0;
#endif

    free(str_arr);
    *pStr_arr = NULL;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrDeleteChecked(CONT_StrArr **pStr_arr) {
    if (!pStr_arr || !(*pStr_arr) || !(*pStr_arr)->pBffr ||
        !(*pStr_arr)->pStr_info) {
        return PRP_ERR_INV_ARG;
    }

    CONT_StrArrDeleteUnchecked(pStr_arr);

    return PRP_OK;
}

PRP_FN_API PRP_Size PRP_FN_CALL CONT_StrArrLen(const CONT_StrArr *str_arr) {
    ASSERT_INVARIANT_EXPR(str_arr);

    return str_arr->len;
}

PRP_FN_API const PRP_Char8 *PRP_FN_CALL CONT_StrArrGetUnchecked(
    const CONT_StrArr *str_arr, PRP_Size i, PRP_Size *pStr_len) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(i < str_arr->len);
    DIAG_ASSERT(pStr_len != NULL);

    StrInfo info = str_arr->pStr_info[i];
    *pStr_len = info.len;

    return str_arr->pBffr + (info.ofs);
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrGetChecked(const CONT_StrArr *str_arr, PRP_Size i,
                      PRP_Size *pStr_len, const PRP_Char8 **pStr) {
    if (!CONT_StrArrIsValid(str_arr) || !pStr || !pStr_len) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= str_arr->len) {
        return PRP_ERR_OOB;
    }

    *pStr = CONT_StrArrGetUnchecked(str_arr, i, pStr_len);

    return PRP_OK;
}

static PRP_Result AccomodateString(CONT_StrArr *str_arr, PRP_Size str_len) {
    if (str_arr->bffr_size - str_arr->occupied_size < str_len) {
        if (str_arr->bffr_size == PRP_SIZE_MAX ||
            PRP_SIZE_MAX - str_arr->occupied_size < str_len) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Size new_size = str_len + str_arr->occupied_size;
        if (PRP_SIZE_MAX / 2 >= str_arr->bffr_size &&
            new_size / 2 < str_arr->bffr_size) {
            new_size = str_arr->bffr_size * 2;
        }

        PRP_Char8 *mem = realloc(str_arr->pBffr, new_size);
        if (!mem) {
            return PRP_ERR_OOM;
        }
        str_arr->pBffr = mem;
        str_arr->bffr_size = new_size;
    }
    if (str_arr->len == str_arr->cap) {
        if (str_arr->cap == MAX_CAP) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Size new_cap;
        if (MAX_CAP / 2 >= str_arr->cap) {
            new_cap = str_arr->cap * 2;
        } else {
            new_cap = MAX_CAP;
        }
        StrInfo *mem = realloc(str_arr->pStr_info, sizeof(StrInfo) * new_cap);
        if (!mem) {
            return PRP_ERR_OOM;
        }
        str_arr->pStr_info = mem;
        str_arr->cap = new_cap;
    }

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPushUnchecked(
    CONT_StrArr *str_arr, const PRP_Char8 *pStr, PRP_Size str_len) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr != NULL);
    DIAG_ASSERT(str_len > 0);

    PRP_Result code = AccomodateString(str_arr, str_len);
    if (code != PRP_OK) {
        return code;
    }
    str_arr->pStr_info[str_arr->len++] =
        (StrInfo){.len = str_len, .ofs = str_arr->occupied_size};
    memcpy(str_arr->pBffr + str_arr->occupied_size, pStr, str_len);
    str_arr->occupied_size += str_len;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPushChecked(CONT_StrArr *str_arr,
                                                         const PRP_Char8 *pStr,
                                                         PRP_Size str_len) {
    if (!CONT_StrArrIsValid(str_arr) || !pStr || !str_len) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrPushUnchecked(str_arr, pStr, str_len);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrInsertUnchecked(
    CONT_StrArr *str_arr, const PRP_Char8 *pStr, PRP_Size str_len, PRP_Size i) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr != NULL);
    DIAG_ASSERT(str_len > 0);
    DIAG_ASSERT(i <= str_arr->len);

    PRP_Result code = AccomodateString(str_arr, str_len);
    if (code != PRP_OK) {
        return code;
    }

    if (i == str_arr->len) {
        str_arr->pStr_info[str_arr->len++] =
            (StrInfo){.len = str_len, .ofs = str_arr->occupied_size};
        memcpy(str_arr->pBffr + str_arr->occupied_size, pStr, str_len);
        str_arr->occupied_size += str_len;

        return PRP_OK;
    }
    PRP_Size i_ofs = str_arr->pStr_info[i].ofs;
    for (PRP_Size j = str_arr->len; j > i; --j) {
        str_arr->pStr_info[j] = str_arr->pStr_info[j - 1];
        str_arr->pStr_info[j].ofs += str_len;
    }
    memmove(str_arr->pBffr + i_ofs + str_len, str_arr->pBffr + i_ofs,
            str_arr->occupied_size - i_ofs);
    memcpy(str_arr->pBffr + i_ofs, pStr, str_len);

    str_arr->pStr_info[i] = (StrInfo){.len = str_len, .ofs = i_ofs};
    str_arr->len++;
    str_arr->occupied_size += str_len;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrInsertChecked(
    CONT_StrArr *str_arr, const PRP_Char8 *pStr, PRP_Size str_len, PRP_Size i) {
    if (!CONT_StrArrIsValid(str_arr) || !pStr || !str_len) {
        return PRP_ERR_INV_ARG;
    }
    if (i > str_arr->len) {
        return PRP_ERR_OOB;
    }

    return CONT_StrArrInsertUnchecked(str_arr, pStr, str_len, i);
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPopUnchecked(CONT_StrArr *str_arr,
                                                          PRP_Char8 **ppStr,
                                                          PRP_Size *pStr_len) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT_MSG((ppStr == NULL) == (pStr_len == NULL),
                    "Both params shall either be provided or excluded.");
    if (ppStr) {
        DIAG_ASSERT(*ppStr != NULL);
    }

    if (!str_arr->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    StrInfo info = str_arr->pStr_info[--str_arr->len];
    if (ppStr && pStr_len) {
        memcpy(*ppStr, str_arr->pBffr + info.ofs, info.len);
        *pStr_len = info.len;
    }
    str_arr->occupied_size -= info.len;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPopChecked(CONT_StrArr *str_arr,
                                                        PRP_Char8 **ppStr,
                                                        PRP_Size *pStr_len) {
    if (!CONT_StrArrIsValid(str_arr) || (ppStr == NULL) != (pStr_len == NULL) ||
        (ppStr && !(*ppStr))) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrPopUnchecked(str_arr, ppStr, pStr_len);
}

PRP_FN_API void PRP_FN_CALL CONT_StrArrRemoveUnchecked(CONT_StrArr *str_arr,
                                                       PRP_Char8 **ppStr,
                                                       PRP_Size *pStr_len,
                                                       PRP_Size i) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(i < str_arr->len);
    DIAG_ASSERT_MSG((ppStr == NULL) == (pStr_len == NULL),
                    "Both params shall either be provided or excluded.");
    if (ppStr) {
        DIAG_ASSERT(*ppStr != NULL);
    }

    StrInfo to_rem = str_arr->pStr_info[i];
    for (PRP_Size j = i; j < str_arr->len - 1; j++) {
        str_arr->pStr_info[j] = str_arr->pStr_info[j + 1];
        str_arr->pStr_info[j].ofs -= to_rem.len;
    }
    if (ppStr && pStr_len) {
        *pStr_len = to_rem.len;
        memcpy(*ppStr, str_arr->pBffr + to_rem.ofs, to_rem.len);
    }
    memmove(str_arr->pBffr + to_rem.ofs,
            str_arr->pBffr + to_rem.ofs + to_rem.len,
            str_arr->occupied_size - to_rem.ofs - to_rem.len);
    str_arr->len--;
    str_arr->occupied_size -= to_rem.len;
}

PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrRemoveChecked(CONT_StrArr *str_arr,
                                                           PRP_Char8 **ppStr,
                                                           PRP_Size *pStr_len,
                                                           PRP_Size i) {
    if (!CONT_StrArrIsValid(str_arr) || (ppStr == NULL) != (pStr_len == NULL)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= str_arr->len) {
        return PRP_ERR_OOB;
    }

    CONT_StrArrRemoveUnchecked(str_arr, ppStr, pStr_len, i);

    return PRP_OK;
}

PRP_FN_API void PRP_FN_CALL CONT_StrArrResetUnchecked(CONT_StrArr *str_arr) {
    ASSERT_INVARIANT_EXPR(str_arr);

#ifdef PRP_DEBUG_MODE
    memset(str_arr->pBffr, 0, str_arr->bffr_size);
    memset(str_arr->pStr_info, 0, str_arr->cap * sizeof(StrInfo));
#endif

    str_arr->len = 0;
    str_arr->occupied_size = 0;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrResetChecked(CONT_StrArr *str_arr) {
    if (!CONT_StrArrIsValid(str_arr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_StrArrResetUnchecked(str_arr);

    return PRP_OK;
}

PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_StrArrSearchUnchecked(const CONT_StrArr *str_arr, const PRP_Char8 *pStr,
                           PRP_Size str_len, PRP_Size *pIdx) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr != NULL);
    DIAG_ASSERT(str_len > 0);

    for (PRP_Size i = 0; i < str_arr->len; i++) {
        StrInfo *pInfo = &str_arr->pStr_info[i];
        if (pInfo->len != str_len) {
            continue;
        }
        if (memcmp(str_arr->pBffr + pInfo->ofs, pStr, pInfo->len) == 0) {
            if (pIdx) {
                *pIdx = i;
            }
            return PRP_True;
        }
    }

    return PRP_False;
}

PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrSearchChecked(const CONT_StrArr *str_arr, const PRP_Char8 *pStr,
                         PRP_Size str_len, PRP_Bool *pRslt, PRP_Size *pIdx) {
    if (!CONT_StrArrIsValid(str_arr) || !pStr || !str_len || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_StrArrSearchUnchecked(str_arr, pStr, str_len, pIdx);

    return PRP_OK;
}
