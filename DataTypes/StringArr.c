#include "StringArr.h"
#include "Diagnostics/Assert.h"
#include <string.h>

typedef struct {
    DT_size ofs;
    DT_size len;
} StrInfo;

struct _StrArr {
    DT_size len;
    DT_size cap;
    StrInfo *pStr_info;

    DT_size bffr_size;
    DT_size occupied_size;
    DT_char *pBffr;
};

#define MAX_CAP (DT_SIZE_MAX / sizeof(StrInfo))

#define ASSERT_INVARIANT_EXPR(str_arr)                                         \
    DIAG_ASSERT_MSG(                                                           \
        DT_StrArrIsValid(str_arr),                                             \
        "The given string buffer is either DT_null, or is corrupted.")

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
static PRP_Result AccomodateString(DT_StrArr *str_arr, DT_size str_len);

PRP_FN_API DT_bool PRP_FN_CALL DT_StrArrIsValid(const DT_StrArr *str_arr) {
    return (str_arr != DT_null && str_arr->pBffr != DT_null &&
            str_arr->bffr_size > 0 &&
            str_arr->occupied_size <= str_arr->bffr_size && str_arr->cap > 0 &&
            str_arr->cap < MAX_CAP && str_arr->len <= str_arr->cap);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrCreateUnchecked(
    DT_size init_bffr_size, DT_size cap, DT_StrArr **pStr_arr) {
    DIAG_ASSERT(init_bffr_size > 0);
    DIAG_ASSERT(cap > 0);
    DIAG_ASSERT(pStr_arr != DT_null);

    if (cap > MAX_CAP) {
        return PRP_ERR_OOM;
    }
    DT_StrArr *str_arr = malloc(sizeof(DT_StrArr));
    if (!str_arr) {
        return PRP_ERR_OOM;
    }
    str_arr->pBffr = malloc(sizeof(init_bffr_size));
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrCreateChecked(DT_size init_bffr_size,
                                                         DT_size cap,
                                                         DT_StrArr **pStr_arr) {
    if (!init_bffr_size || !cap || !pStr_arr) {
        return PRP_ERR_INV_ARG;
    }

    return DT_StrArrCreateUnchecked(init_bffr_size, cap, pStr_arr);
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_StrArrCloneUnchecked(const DT_StrArr *str_arr, DT_StrArr **pStr_arr) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr_arr != DT_null);

    PRP_Result code = DT_StrArrCreateUnchecked(str_arr->occupied_size,
                                               str_arr->len, pStr_arr);
    if (code != PRP_OK) {
        return code;
    }
    DT_StrArr *cpy = *pStr_arr;
    cpy->occupied_size = str_arr->occupied_size;
    cpy->len = str_arr->len;
    memcpy(cpy->pBffr, str_arr->pBffr, str_arr->occupied_size);

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_StrArrCloneChecked(const DT_StrArr *str_arr, DT_StrArr **pStr_arr) {
    if (!DT_StrArrIsValid(str_arr) || !pStr_arr) {
        return PRP_ERR_INV_ARG;
    }

    return DT_StrArrCloneUnchecked(str_arr, pStr_arr);
}

PRP_FN_API DT_void PRP_FN_CALL DT_StrArrDeleteUnchecked(DT_StrArr **pStr_arr) {
    DIAG_ASSERT(pStr_arr != DT_null);
    DIAG_ASSERT(*pStr_arr != DT_null);
    DIAG_ASSERT((*pStr_arr)->pBffr != DT_null &&
                (*pStr_arr)->pStr_info != DT_null);

    DT_StrArr *str_arr = *pStr_arr;
    free(str_arr->pBffr);
    free(str_arr->pStr_info);

#if !defined(PRP_NDEBUG)
    str_arr->pBffr = DT_null;
    str_arr->pStr_info = DT_null;
    str_arr->bffr_size = str_arr->occupied_size = 0;
    str_arr->cap = str_arr->len = 0;
#endif

    free(str_arr);
    *pStr_arr = DT_null;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrDeleteChecked(DT_StrArr **pStr_arr) {
    if (!pStr_arr || !(*pStr_arr) || !(*pStr_arr)->pBffr ||
        !(*pStr_arr)->pStr_info) {
        return PRP_ERR_INV_ARG;
    }

    DT_StrArrDeleteUnchecked(pStr_arr);

    return PRP_OK;
}

PRP_FN_API DT_size PRP_FN_CALL DT_StrArrLen(const DT_StrArr *str_arr) {
    ASSERT_INVARIANT_EXPR(str_arr);

    return str_arr->len;
}

PRP_FN_API const DT_char *PRP_FN_CALL
DT_StrArrGetUnchecked(const DT_StrArr *str_arr, DT_size i, DT_size *pStr_len) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(i < str_arr->len);
    DIAG_ASSERT(pStr_len != DT_null);

    StrInfo info = str_arr->pStr_info[i];
    *pStr_len = info.len;

    return str_arr->pBffr + (info.ofs);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrGetChecked(const DT_StrArr *str_arr,
                                                      DT_size i,
                                                      DT_size *pStr_len,
                                                      const DT_char **pStr) {
    if (!DT_StrArrIsValid(str_arr) || !pStr || !pStr_len) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= str_arr->len) {
        return PRP_ERR_OOB;
    }

    *pStr = DT_StrArrGetUnchecked(str_arr, i, pStr_len);

    return PRP_OK;
}

static PRP_Result AccomodateString(DT_StrArr *str_arr, DT_size str_len) {
    if (str_arr->bffr_size - str_arr->occupied_size < str_len) {
        if (str_arr->bffr_size == DT_SIZE_MAX ||
            DT_SIZE_MAX - str_arr->occupied_size < str_len) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        DT_size new_size = str_len + str_arr->occupied_size;
        if (DT_SIZE_MAX / 2 >= str_arr->bffr_size &&
            new_size / 2 < str_arr->bffr_size) {
            new_size = str_arr->bffr_size * 2;
        }

        DT_char *mem = realloc(str_arr->pBffr, new_size);
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
        DT_size new_cap;
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrPushUnchecked(DT_StrArr *str_arr,
                                                         const DT_char *pStr,
                                                         DT_size str_len) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr != DT_null);
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrPushChecked(DT_StrArr *str_arr,
                                                       const DT_char *pStr,
                                                       DT_size str_len) {
    if (!DT_StrArrIsValid(str_arr) || !pStr || !str_len) {
        return PRP_ERR_INV_ARG;
    }

    return DT_StrArrPushUnchecked(str_arr, pStr, str_len);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrInsertUnchecked(DT_StrArr *str_arr,
                                                           const DT_char *pStr,
                                                           DT_size str_len,
                                                           DT_size i) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr != DT_null);
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
    DT_size i_ofs = str_arr->pStr_info[i].ofs;
    for (DT_size j = str_arr->len; j > i; --j) {
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrInsertChecked(DT_StrArr *str_arr,
                                                         const DT_char *pStr,
                                                         DT_size str_len,
                                                         DT_size i) {
    if (!DT_StrArrIsValid(str_arr) || !pStr || !str_len) {
        return PRP_ERR_INV_ARG;
    }
    if (i > str_arr->len) {
        return PRP_ERR_OOB;
    }

    return DT_StrArrInsertUnchecked(str_arr, pStr, str_len, i);
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrPopUnchecked(DT_StrArr *str_arr,
                                                        DT_char **ppStr,
                                                        DT_size *pStr_len) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT_MSG((ppStr == DT_null) == (pStr_len == DT_null),
                    "Both params shall either be provided or excluded.");
    if (ppStr) {
        DIAG_ASSERT(*ppStr != DT_null);
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrPopChecked(DT_StrArr *str_arr,
                                                      DT_char **ppStr,
                                                      DT_size *pStr_len) {
    if (!DT_StrArrIsValid(str_arr) ||
        (ppStr == DT_null) != (pStr_len == DT_null) || (ppStr && !(*ppStr))) {
        return PRP_ERR_INV_ARG;
    }

    return DT_StrArrPopUnchecked(str_arr, ppStr, pStr_len);
}

PRP_FN_API DT_void PRP_FN_CALL DT_StrArrRemoveUnchecked(DT_StrArr *str_arr,
                                                        DT_char **ppStr,
                                                        DT_size *pStr_len,
                                                        DT_size i) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(i < str_arr->len);
    DIAG_ASSERT_MSG((ppStr == DT_null) == (pStr_len == DT_null),
                    "Both params shall either be provided or excluded.");
    if (ppStr) {
        DIAG_ASSERT(*ppStr != DT_null);
    }

    StrInfo to_rem = str_arr->pStr_info[i];
    for (DT_size j = i; j < str_arr->len - 1; j++) {
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

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrRemoveChecked(DT_StrArr *str_arr,
                                                         DT_char **ppStr,
                                                         DT_size *pStr_len,
                                                         DT_size i) {
    if (!DT_StrArrIsValid(str_arr) ||
        (ppStr == DT_null) != (pStr_len == DT_null)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= str_arr->len) {
        return PRP_ERR_OOB;
    }

    DT_StrArrRemoveUnchecked(str_arr, ppStr, pStr_len, i);

    return PRP_OK;
}

PRP_FN_API DT_void PRP_FN_CALL DT_StrArrResetUnchecked(DT_StrArr *str_arr) {
    ASSERT_INVARIANT_EXPR(str_arr);

#if !defined(PRP_NDEBUG)
    memset(str_arr->pBffr, 0, str_arr->bffr_size);
    memset(str_arr->pStr_info, 0, str_arr->cap * sizeof(StrInfo));
#endif

    str_arr->len = 0;
    str_arr->occupied_size = 0;
}

PRP_FN_API PRP_Result PRP_FN_CALL DT_StrArrResetChecked(DT_StrArr *str_arr) {
    if (!DT_StrArrIsValid(str_arr)) {
        return PRP_ERR_INV_ARG;
    }

    DT_StrArrResetUnchecked(str_arr);

    return PRP_OK;
}

PRP_FN_API DT_bool PRP_FN_CALL
DT_StrArrSearchUnchecked(const DT_StrArr *str_arr, const DT_char *pStr,
                         DT_size str_len, DT_size *pIdx) {
    ASSERT_INVARIANT_EXPR(str_arr);
    DIAG_ASSERT(pStr != DT_null);
    DIAG_ASSERT(str_len > 0);

    for (DT_size i = 0; i < str_arr->len; i++) {
        StrInfo *pInfo = &str_arr->pStr_info[i];
        if (pInfo->len != str_len) {
            continue;
        }
        if (memcmp(str_arr->pBffr + pInfo->ofs, pStr, pInfo->len) == 0) {
            if (pIdx) {
                *pIdx = i;
            }
            return DT_true;
        }
    }

    return DT_false;
}

PRP_FN_API PRP_Result PRP_FN_CALL
DT_StrArrSearchChecked(const DT_StrArr *str_arr, const DT_char *pStr,
                       DT_size str_len, DT_bool *pRslt, DT_size *pIdx) {
    if (!DT_StrArrIsValid(str_arr) || !pStr || !str_len || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = DT_StrArrSearchUnchecked(str_arr, pStr, str_len, pIdx);

    return PRP_OK;
}
