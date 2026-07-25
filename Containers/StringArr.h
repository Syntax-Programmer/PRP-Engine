#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
 * A dynamically auto-resizing array to store strings.
 *
 * - Elements are stored contiguously in memory.
 * - Size grows automatically to meet demands.
 * - Not type-safe; user must ensure correct usage.
 * - We ditch '\0' terminated strings for size based ops.
 *
 * Lifetime Rules:
 * - Memory returned by getters becomes invalid after any mutating operation.
 * - Buffer must be deleted using CONT_StrAr
 rDelete* APIs.
 */
typedef struct _StrArr CONT_StrArr;

/**
 * Checks whether the given string-array is structurally valid.
 *
 * @param str_arr Pointer to the string-array.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_FN_API PRP_Bool PRP_FN_CALL CONT_StrArrIsValid(const CONT_StrArr *str_arr);

/**
 * Creates a dynamic string-array.
 *
 * @param init_bffr_size Initial size (in bytes) expected to be used.
 * @param cap            The number of strings to accomodate for.
 * @param pStr_arr       Output pointer receiving the string-array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrCreateUnchecked(
    PRP_Size init_bffr_size, PRP_Size cap, CONT_StrArr **pStr_arr);
/**
 * Creates a dynamic string-array.
 *
 * @param init_bffr_size Initial size (in bytes) expected to be used.
 * @param cap            The number of strings to accomodate for.
 * @param pStr_arr       Output pointer receiving the string-array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrCreateChecked(
    PRP_Size init_bffr_size, PRP_Size cap, CONT_StrArr **pStr_arr);
/**
 * Deep clones the given string-array.
 *
 * @param str_arr  The string-array to clone.
 * @param pStr_Arr Output pointer receiving the string-array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrCloneUnchecked(const CONT_StrArr *str_arr, CONT_StrArr **pStr_arr);
/**
 * Deep clones the given string-array.
 *
 * @param str_arr  The string-array to clone.
 * @param pStr_Arr Output pointer receiving the string-array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrCloneChecked(const CONT_StrArr *str_arr, CONT_StrArr **pStr_arr);

/**
 * Deletes the string-array and nullifies the pointer.
 *
 * @param pStr_arr Pointer to string-array pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_StrArrDeleteUnchecked(CONT_StrArr **pStr_arr);
/**
 * Deletes the string-array and nullifies the pointer.
 *
 * @param pStr_arr Pointer to string-array pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pArr or *pArr is invalid
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrDeleteChecked(CONT_StrArr **pStr_arr);

/**
 * Returns the number of strings currently stored.
 *
 * @param str_arr String-Array instance.
 *
 * @return Number of strings.
 *
 * @note Assumes valid string-array (asserts in debug).
 */
PRP_FN_API PRP_Size PRP_FN_CALL CONT_StrArrLen(const CONT_StrArr *str_arr);

/**
 * Retrieves the string at the given index.
 *
 * @param str_arr  String-Array instance.
 * @param i        The index into the string-array.
 * @param pStr_len Output pointer to store len of the string.
 *
 * @return The string pointer at the index.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API const PRP_Char8 *PRP_FN_CALL CONT_StrArrGetUnchecked(
    const CONT_StrArr *str_arr, PRP_Size i, PRP_Size *pStr_len);
/**
 * Retrieves the string at the given index.
 *
 * @param str_arr  String-Array instance.
 * @param i        The index into the string-array.
 * @param pStr_len Output pointer to store len of the string.
 * @param pStr     Output pointer to store the string ptr.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrGetChecked(const CONT_StrArr *str_arr, PRP_Size i,
                      PRP_Size *pStr_len, const PRP_Char8 **pStr);
/**
 * Pushes a new string at the end of the string-array.
 *
 * @param str_arr String-Array instance.
 * @param pStr    Pointer to string to push.
 * @param str_len Len of the string to push.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPushUnchecked(
    CONT_StrArr *str_arr, const PRP_Char8 *pStr, PRP_Size str_len);
/**
 * Pushes a new string at the end of the string-array.
 *
 * @param str_arr String-Array instance.
 * @param pStr    Pointer to string to push.
 * @param str_len Len of the string to push.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPushChecked(CONT_StrArr *str_arr,
                                                         const PRP_Char8 *pStr,
                                                         PRP_Size str_len);
/**
 * Inserts a new string at the give index of the string-array.
 *
 * @param str_arr String-Array instance.
 * @param pStr    Pointer to string to push.
 * @param str_len Len of the string to push.
 * @param i       The index into the string-array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrInsertUnchecked(
    CONT_StrArr *str_arr, const PRP_Char8 *pStr, PRP_Size str_len, PRP_Size i);
/**
 * Inserts a new string at the give index of the string-array.
 *
 * @param str_arr String-Array instance.
 * @param pStr    Pointer to string to push.
 * @param str_len Len of the string to push.
 * @param i       The index into the string-array.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrInsertChecked(
    CONT_StrArr *str_arr, const PRP_Char8 *pStr, PRP_Size str_len, PRP_Size i);
/**
 * Pops the last string from the string-array
 *
 * @param str_arr  String-Array instance.
 * @param ppStr    Optional output pointer to store the string data at.
 * @param pStr_len Optional output pointer to store the len of the string.
 *
 * @note Invariant:
 * - Either both ppStr and pStr_len must be valid pointer or both must be NULL.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no elements to pop.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPopUnchecked(CONT_StrArr *str_arr,
                                                          PRP_Char8 **ppStr,
                                                          PRP_Size *pStr_len);
/**
 * Pops the last string from the string-array
 *
 * @param str_arr  String-Array instance.
 * @param ppStr    Optional output pointer to store the string data at.
 * @param pStr_len Optional output pointer to store the len of the string.
 *
 * @note Invariant:
 * - Either both ppStr and pStr_len must be valid pointer or both must be NULL.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if no elements to pop.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrPopChecked(CONT_StrArr *str_arr,
                                                        PRP_Char8 **ppStr,
                                                        PRP_Size *pStr_len);
/**
 * Removes string at the give index and repack the string-array.
 *
 * @param str_arr  String-Array instance.
 * @param ppStr    Optional output pointer to store the string data at.
 * @param pStr_len Optional output pointer to store the len of the string.
 * @param i        The index into the string-array.
 *
 * @note Invariant:
 * - Either both ppStr and pStr_len must be valid pointer or both must be NULL.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_StrArrRemoveUnchecked(CONT_StrArr *str_arr,
                                                       PRP_Char8 **ppStr,
                                                       PRP_Size *pStr_len,
                                                       PRP_Size i);
/**
 * Removes string at the give index and repack the string-array.
 *
 * @param str_arr  String-Array instance.
 * @param ppStr    Optional output pointer to store the string data at.
 * @param pStr_len Optional output pointer to store the len of the string.
 * @param i        The index into the string-array.
 *
 * @note Invariant:
 * - Either both ppStr and pStr_len must be valid pointer or both must be NULL.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOB if index out of bounds.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrRemoveChecked(CONT_StrArr *str_arr,
                                                           PRP_Char8 **ppStr,
                                                           PRP_Size *pStr_len,
                                                           PRP_Size i);

/**
 * Resets the string-array.
 *
 * Only sets len to 0 and 0s out everything.
 *
 * @param str_arr String-Array instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_StrArrResetUnchecked(CONT_StrArr *str_arr);
/**
 * Resets the string-array.
 *
 * Only sets len to 0 and 0s out everything.
 *
 * @param str_arr String-Array instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_StrArrResetChecked(CONT_StrArr *str_arr);

/**
 * Searches if the given string exists inside the string-array.
 *
 * @param str_arr String-Array instance.
 * @param pStr    Pointer to the string to search.
 * @param str_len Length of the string to search.
 * @param pIdx    Optional output pointer to the index the found string.
 *
 * @return PRP_True if found, PRP_False otherwise.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Bool PRP_FN_CALL
CONT_StrArrSearchUnchecked(const CONT_StrArr *str_arr, const PRP_Char8 *pStr,
                           PRP_Size str_len, PRP_Size *pIdx);
/**
 * Searches if the given string exists inside the string-array.
 *
 * @param str_arr String-Array instance.
 * @param pStr    Pointer to the string to search.
 * @param str_len Length of the string to search.
 * @param pRslt   The pointer to where the result is stored.
 * @param pIdx    Optional output pointer to the index the found string.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_StrArrSearchChecked(const CONT_StrArr *str_arr, const PRP_Char8 *pStr,
                         PRP_Size str_len, PRP_Bool *pRslt, PRP_Size *pIdx);

#ifdef __cplusplus
}
#endif
