#include "StringArr.h"
#include "Core/Diagnostics/Assert/Assert.h"
#include <string.h>

typedef struct StrInfo {
    PRP_Size ofs;
    PRP_Size len;
} StrInfo;

struct CONT_StrArr {
    PRP_Size len;
    PRP_Size cap;
    StrInfo *pStr_info;

    PRP_Size bffr_size;
    PRP_Size occupied_size;
    PRP_Char8 *pBffr;
};

#define MAX_CAP (PRP_SIZE_MAX / sizeof(StrInfo))

#define ASSERT_INVARIANT_EXPR(pStr_arr)                                        \
    PRP_DIAG_ASSERT_MSG(CONT_StrArrIsValid(pStr_arr),                          \
                        "The given string array is invalid.")

/**
 * Makes sure that the string of given len can be housed in strin-array.
 *
 * @param pStr_arr String-Array instance.
 * @param str_len  Len of string to accomodate.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result AccomodateString(CONT_StrArr *pStr_arr, PRP_Size str_len);

PRP_API PRP_Bool PRP_CALL CONT_StrArrIsValid(const CONT_StrArr *pStr_arr) {
    return (pStr_arr != NULL && pStr_arr->pBffr != NULL &&
            pStr_arr->bffr_size > 0 &&
            pStr_arr->occupied_size <= pStr_arr->bffr_size &&
            pStr_arr->cap > 0 && pStr_arr->cap < MAX_CAP &&
            pStr_arr->len <= pStr_arr->cap);
}

PRP_API PRP_Result PRP_CALL CONT_StrArrCreateUnchecked(
    PRP_Size init_bffr_size, PRP_Size cap, CONT_StrArr **ppStr_arr) {
    PRP_DIAG_ASSERT_MSG(init_bffr_size > 0,
                        "The initial string buffer size of the must be > 0.");
    PRP_DIAG_ASSERT_MSG(cap > 0, "The cap of the string array must be > 0.");
    PRP_DIAG_ASSERT(ppStr_arr != NULL);

    *ppStr_arr = NULL;
    if (cap > MAX_CAP) {
        return PRP_ERR_OOM;
    }
    CONT_StrArr *pStr_arr = malloc(sizeof(CONT_StrArr));
    if (!pStr_arr) {
        return PRP_ERR_OOM;
    }
    pStr_arr->pBffr = malloc(init_bffr_size);
    if (!pStr_arr->pBffr) {
        free(pStr_arr);
        return PRP_ERR_OOM;
    }
    pStr_arr->pStr_info = malloc(sizeof(StrInfo) * cap);
    if (!pStr_arr->pStr_info) {
        free(pStr_arr->pBffr);
        free(pStr_arr);
        return PRP_ERR_OOM;
    }
    pStr_arr->bffr_size = init_bffr_size;
    pStr_arr->occupied_size = 0;
    pStr_arr->cap = cap;
    pStr_arr->len = 0;

    *ppStr_arr = pStr_arr;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrCreateChecked(PRP_Size init_bffr_size,
                                                     PRP_Size cap,
                                                     CONT_StrArr **ppStr_arr) {
    if (!init_bffr_size || !cap || !ppStr_arr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrCreateUnchecked(init_bffr_size, cap, ppStr_arr);
}

PRP_API PRP_Result PRP_CALL CONT_StrArrCloneUnchecked(
    const CONT_StrArr *pStr_arr, CONT_StrArr **ppStr_arr) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT(ppStr_arr != NULL);

    PRP_Result code = CONT_StrArrCreateUnchecked(pStr_arr->occupied_size,
                                                 pStr_arr->len, ppStr_arr);
    if (code != PRP_OK) {
        return code;
    }
    CONT_StrArr *cpy = *ppStr_arr;
    cpy->occupied_size = pStr_arr->occupied_size;
    cpy->len = pStr_arr->len;
    memcpy(cpy->pBffr, pStr_arr->pBffr, pStr_arr->occupied_size);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrCloneChecked(const CONT_StrArr *pStr_arr,
                                                    CONT_StrArr **ppStr_arr) {
    if (!CONT_StrArrIsValid(pStr_arr) || !ppStr_arr) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrCloneUnchecked(pStr_arr, ppStr_arr);
}

PRP_API void PRP_CALL CONT_StrArrDeleteUnchecked(CONT_StrArr **ppStr_arr) {
    PRP_DIAG_ASSERT(ppStr_arr != NULL);
    PRP_DIAG_ASSERT_MSG(*ppStr_arr != NULL && (*ppStr_arr)->pBffr != NULL &&
                            (*ppStr_arr)->pStr_info != NULL,
                        "The given *ppStr_arr is invalid.");

    CONT_StrArr *pStr_arr = *ppStr_arr;
    free(pStr_arr->pBffr);
    free(pStr_arr->pStr_info);

#ifdef PRP_DEBUG_MODE
    pStr_arr->pBffr = NULL;
    pStr_arr->pStr_info = NULL;
    pStr_arr->bffr_size = pStr_arr->occupied_size = 0;
    pStr_arr->cap = pStr_arr->len = 0;
#endif

    free(pStr_arr);
    *ppStr_arr = NULL;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrDeleteChecked(CONT_StrArr **ppStr_arr) {
    if (!ppStr_arr || !(*ppStr_arr) || !(*ppStr_arr)->pBffr ||
        !(*ppStr_arr)->pStr_info) {
        return PRP_ERR_INV_ARG;
    }

    CONT_StrArrDeleteUnchecked(ppStr_arr);

    return PRP_OK;
}

PRP_API PRP_Size PRP_CALL CONT_StrArrLen(const CONT_StrArr *pStr_arr) {
    ASSERT_INVARIANT_EXPR(pStr_arr);

    return pStr_arr->len;
}

PRP_API const PRP_Char8 *PRP_CALL CONT_StrArrGetUnchecked(
    const CONT_StrArr *pStr_arr, PRP_Size i, PRP_Size *pStr_len) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT_MSG(i < pStr_arr->len, "The index i is out of bounds.");
    PRP_DIAG_ASSERT(pStr_len != NULL);

    StrInfo info = pStr_arr->pStr_info[i];
    *pStr_len = info.len;

    return pStr_arr->pBffr + (info.ofs);
}

PRP_API PRP_Result PRP_CALL CONT_StrArrGetChecked(const CONT_StrArr *pStr_arr,
                                                  PRP_Size i,
                                                  PRP_Size *pStr_len,
                                                  const PRP_Char8 **ppStr) {
    if (!CONT_StrArrIsValid(pStr_arr) || !ppStr || !pStr_len) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pStr_arr->len) {
        return PRP_ERR_OOB;
    }

    *ppStr = CONT_StrArrGetUnchecked(pStr_arr, i, pStr_len);

    return PRP_OK;
}

static PRP_Result AccomodateString(CONT_StrArr *pStr_arr, PRP_Size str_len) {
    if (pStr_arr->bffr_size - pStr_arr->occupied_size < str_len) {
        if (pStr_arr->bffr_size == PRP_SIZE_MAX ||
            PRP_SIZE_MAX - pStr_arr->occupied_size < str_len) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Size new_size = str_len + pStr_arr->occupied_size;
        if (PRP_SIZE_MAX / 2 >= pStr_arr->bffr_size &&
            new_size / 2 < pStr_arr->bffr_size) {
            new_size = pStr_arr->bffr_size * 2;
        }

        PRP_Char8 *mem = realloc(pStr_arr->pBffr, new_size);
        if (!mem) {
            return PRP_ERR_OOM;
        }
        pStr_arr->pBffr = mem;
        pStr_arr->bffr_size = new_size;
    }
    if (pStr_arr->len == pStr_arr->cap) {
        if (pStr_arr->cap == MAX_CAP) {
            return PRP_ERR_RES_EXHAUSTED;
        }
        PRP_Size new_cap;
        if (MAX_CAP / 2 >= pStr_arr->cap) {
            new_cap = pStr_arr->cap * 2;
        } else {
            new_cap = MAX_CAP;
        }
        StrInfo *mem = realloc(pStr_arr->pStr_info, sizeof(StrInfo) * new_cap);
        if (!mem) {
            return PRP_ERR_OOM;
        }
        pStr_arr->pStr_info = mem;
        pStr_arr->cap = new_cap;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrPushUnchecked(CONT_StrArr *pStr_arr,
                                                     const PRP_Char8 *pStr,
                                                     PRP_Size str_len) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT(pStr != NULL);
    PRP_DIAG_ASSERT_MSG(str_len > 0, "The len of the string must be > 0.");

    PRP_Result code = AccomodateString(pStr_arr, str_len);
    if (code != PRP_OK) {
        return code;
    }
    pStr_arr->pStr_info[pStr_arr->len++] =
        (StrInfo){.len = str_len, .ofs = pStr_arr->occupied_size};
    memcpy(pStr_arr->pBffr + pStr_arr->occupied_size, pStr, str_len);
    pStr_arr->occupied_size += str_len;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrPushChecked(CONT_StrArr *pStr_arr,
                                                   const PRP_Char8 *pStr,
                                                   PRP_Size str_len) {
    if (!CONT_StrArrIsValid(pStr_arr) || !pStr || !str_len) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrPushUnchecked(pStr_arr, pStr, str_len);
}

PRP_API PRP_Result PRP_CALL CONT_StrArrInsertUnchecked(CONT_StrArr *pStr_arr,
                                                       const PRP_Char8 *pStr,
                                                       PRP_Size str_len,
                                                       PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT(pStr != NULL);
    PRP_DIAG_ASSERT_MSG(str_len > 0, "The len of the string must be > 0.");
    PRP_DIAG_ASSERT_MSG(i <= pStr_arr->len, "The index i is out of bounds.");

    PRP_Result code = AccomodateString(pStr_arr, str_len);
    if (code != PRP_OK) {
        return code;
    }

    if (i == pStr_arr->len) {
        pStr_arr->pStr_info[pStr_arr->len++] =
            (StrInfo){.len = str_len, .ofs = pStr_arr->occupied_size};
        memcpy(pStr_arr->pBffr + pStr_arr->occupied_size, pStr, str_len);
        pStr_arr->occupied_size += str_len;

        return PRP_OK;
    }
    PRP_Size i_ofs = pStr_arr->pStr_info[i].ofs;
    for (PRP_Size j = pStr_arr->len; j > i; --j) {
        pStr_arr->pStr_info[j] = pStr_arr->pStr_info[j - 1];
        pStr_arr->pStr_info[j].ofs += str_len;
    }
    memmove(pStr_arr->pBffr + i_ofs + str_len, pStr_arr->pBffr + i_ofs,
            pStr_arr->occupied_size - i_ofs);
    memcpy(pStr_arr->pBffr + i_ofs, pStr, str_len);

    pStr_arr->pStr_info[i] = (StrInfo){.len = str_len, .ofs = i_ofs};
    pStr_arr->len++;
    pStr_arr->occupied_size += str_len;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrInsertChecked(CONT_StrArr *pStr_arr,
                                                     const PRP_Char8 *pStr,
                                                     PRP_Size str_len,
                                                     PRP_Size i) {
    if (!CONT_StrArrIsValid(pStr_arr) || !pStr || !str_len) {
        return PRP_ERR_INV_ARG;
    }
    if (i > pStr_arr->len) {
        return PRP_ERR_OOB;
    }

    return CONT_StrArrInsertUnchecked(pStr_arr, pStr, str_len, i);
}

PRP_API PRP_Result PRP_CALL CONT_StrArrPopUnchecked(CONT_StrArr *pStr_arr,
                                                    PRP_Char8 **ppStr,
                                                    PRP_Size *pStr_len) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT_MSG((ppStr == NULL) == (pStr_len == NULL),
                        "Both params shall either be provided or excluded.");
    if (ppStr) {
        PRP_DIAG_ASSERT(*ppStr != NULL);
    }

    if (!pStr_arr->len) {
        return PRP_ERR_RES_EXHAUSTED;
    }
    StrInfo info = pStr_arr->pStr_info[--pStr_arr->len];
    if (ppStr && pStr_len) {
        memcpy(*ppStr, pStr_arr->pBffr + info.ofs, info.len);
        *pStr_len = info.len;
    }
    pStr_arr->occupied_size -= info.len;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrPopChecked(CONT_StrArr *pStr_arr,
                                                  PRP_Char8 **ppStr,
                                                  PRP_Size *pStr_len) {
    if (!CONT_StrArrIsValid(pStr_arr) ||
        (ppStr == NULL) != (pStr_len == NULL) || (ppStr && !(*ppStr))) {
        return PRP_ERR_INV_ARG;
    }

    return CONT_StrArrPopUnchecked(pStr_arr, ppStr, pStr_len);
}

PRP_API void PRP_CALL CONT_StrArrRemoveUnchecked(CONT_StrArr *pStr_arr,
                                                 PRP_Char8 **ppStr,
                                                 PRP_Size *pStr_len,
                                                 PRP_Size i) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT_MSG(i < pStr_arr->len, "The index i is out of bounds.");
    PRP_DIAG_ASSERT_MSG((ppStr == NULL) == (pStr_len == NULL),
                        "Both params shall either be provided or excluded.");
    if (ppStr) {
        PRP_DIAG_ASSERT(*ppStr != NULL);
    }

    StrInfo to_rem = pStr_arr->pStr_info[i];
    for (PRP_Size j = i; j < pStr_arr->len - 1; j++) {
        pStr_arr->pStr_info[j] = pStr_arr->pStr_info[j + 1];
        pStr_arr->pStr_info[j].ofs -= to_rem.len;
    }
    if (ppStr && pStr_len) {
        *pStr_len = to_rem.len;
        memcpy(*ppStr, pStr_arr->pBffr + to_rem.ofs, to_rem.len);
    }
    memmove(pStr_arr->pBffr + to_rem.ofs,
            pStr_arr->pBffr + to_rem.ofs + to_rem.len,
            pStr_arr->occupied_size - to_rem.ofs - to_rem.len);
    pStr_arr->len--;
    pStr_arr->occupied_size -= to_rem.len;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrRemoveChecked(CONT_StrArr *pStr_arr,
                                                     PRP_Char8 **ppStr,
                                                     PRP_Size *pStr_len,
                                                     PRP_Size i) {
    if (!CONT_StrArrIsValid(pStr_arr) ||
        (ppStr == NULL) != (pStr_len == NULL)) {
        return PRP_ERR_INV_ARG;
    }
    if (i >= pStr_arr->len) {
        return PRP_ERR_OOB;
    }

    CONT_StrArrRemoveUnchecked(pStr_arr, ppStr, pStr_len, i);

    return PRP_OK;
}

PRP_API void PRP_CALL CONT_StrArrResetUnchecked(CONT_StrArr *pStr_arr) {
    ASSERT_INVARIANT_EXPR(pStr_arr);

#ifdef PRP_DEBUG_MODE
    memset(pStr_arr->pBffr, 0, pStr_arr->bffr_size);
    memset(pStr_arr->pStr_info, 0, pStr_arr->cap * sizeof(StrInfo));
#endif

    pStr_arr->len = 0;
    pStr_arr->occupied_size = 0;
}

PRP_API PRP_Result PRP_CALL CONT_StrArrResetChecked(CONT_StrArr *pStr_arr) {
    if (!CONT_StrArrIsValid(pStr_arr)) {
        return PRP_ERR_INV_ARG;
    }

    CONT_StrArrResetUnchecked(pStr_arr);

    return PRP_OK;
}

PRP_API PRP_Bool PRP_CALL
CONT_StrArrSearchUnchecked(const CONT_StrArr *pStr_arr, const PRP_Char8 *pStr,
                           PRP_Size str_len, PRP_Size *pIdx) {
    ASSERT_INVARIANT_EXPR(pStr_arr);
    PRP_DIAG_ASSERT(pStr != NULL);
    PRP_DIAG_ASSERT_MSG(str_len > 0, "The len of the string must be > 0.");

    for (PRP_Size i = 0; i < pStr_arr->len; i++) {
        StrInfo *pInfo = &pStr_arr->pStr_info[i];
        if (pInfo->len != str_len) {
            continue;
        }
        if (memcmp(pStr_arr->pBffr + pInfo->ofs, pStr, pInfo->len) == 0) {
            if (pIdx) {
                *pIdx = i;
            }
            return PRP_True;
        }
    }

    return PRP_False;
}

PRP_API PRP_Result PRP_CALL
CONT_StrArrSearchChecked(const CONT_StrArr *pStr_arr, const PRP_Char8 *pStr,
                         PRP_Size str_len, PRP_Bool *pRslt, PRP_Size *pIdx) {
    if (!CONT_StrArrIsValid(pStr_arr) || !pStr || !str_len || !pRslt) {
        return PRP_ERR_INV_ARG;
    }

    *pRslt = CONT_StrArrSearchUnchecked(pStr_arr, pStr, str_len, pIdx);

    return PRP_OK;
}
