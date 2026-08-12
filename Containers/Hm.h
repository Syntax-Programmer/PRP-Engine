#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/* ----  STD HASH FUNCS ---- */

/**
 * Hash func can be used to hash a string using FNV1a64 hashing.
 *
 * @param pStr_key The pKey to hash.
 *
 * @return The hash created of the pKey.
 *
 * @note Generic Use Case Tip:
 * - This can be used to hash any generic byte stream, just cast it to a
 * PRP_Char8 ptr and make sure the last byte is nul byte.
 */
PRP_U64 CONT_HmHashStr(const void *pStr_key);
/**
 * Hash func can be used to hash a u64 using SplitMix64 hashing.
 *
 * @param pU64_key The pKey to hash.
 *
 * @return The hash created of the pKey.
 *
 * @note Generic Use Case Tip:
 * - This can be used to hash ptrs, ints(any size), uints(any size) if the user
 * is okay of them being used as u64 ints.
 */
PRP_U64 CONT_HmHashSplitMix64(const void *u64_key);

/* ----  HASHMAP ---- */

/**
 * CONT_Hm
 *
 * A generic hashmap storing pKey-value pairs.
 *
 * - Uses user-provided hash and comparison functions.
 * - Supports custom destruction callbacks for keys and values.
 * - Designed as a lean alternative to high-level hashmaps (e.g. Python dict).
 */
typedef struct CONT_Hm CONT_Hm;

/**
 * Checks whether the given hashmap is structurally valid.
 *
 * @param pHm Pointer to the hashmap.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_API PRP_Bool PRP_CALL CONT_HmIsValid(const CONT_Hm *pHm);

/**
 * Creates a hashmap with user-defined behavior.
 *
 * @param pHash_fn     Function used to hash keys.
 * @param pKey_cmp_cb  Function used to compare keys.
 * @param pKey_del_cb  Callback to destroy keys (can be NULL if not needed).
 * @param pVal_del_cb  Callback to destroy values (can be NULL if not needed).
 * @param ppHm         Output pointer receiving the created hashmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_HmCreateUnchecked(
    PRP_U64 (*pHash_fn)(const void *pKey),
    PRP_Bool (*pKey_cmp_cb)(const void *pKey1, const void *pKey2),
    PRP_Result (*pKey_del_cb)(void *pKey),
    PRP_Result (*pVal_del_cb)(void *pVal), CONT_Hm **ppHm);

/**
 * Creates a hashmap with validation.
 *
 * @param pHash_fn     Function used to hash keys.
 * @param pKey_cmp_cb  Function used to compare keys.
 * @param pKey_del_cb  Callback to destroy keys (can be NULL if not needed).
 * @param pVal_del_cb  Callback to destroy values (can be NULL if not needed).
 * @param ppHm         Output pointer receiving the created hashmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_API PRP_Result PRP_CALL CONT_HmCreateChecked(
    PRP_U64 (*pHash_fn)(const void *pKey),
    PRP_Bool (*pKey_cmp_cb)(const void *pKey1, const void *pKey2),
    PRP_Result (*pKey_del_cb)(void *pKey),
    PRP_Result (*pVal_del_cb)(void *pVal), CONT_Hm **ppHm);

/**
 * Deletes the hashmap and nullifies the pointer.
 *
 * @param ppHm Pointer to hashmap pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_HmDeleteUnchecked(CONT_Hm **ppHm);

/**
 * Deletes the hashmap and nullifies the pointer.
 *
 * @param ppHm Pointer to hashmap pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if ppHm or *ppHm is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_HmDeleteChecked(CONT_Hm **ppHm);

/**
 * Inserts a pKey-value pair into the hashmap.
 *
 * @param pHm  Hashmap instance.
 * @param pKey Key (must not be NULL).
 * @param pVal Value associated with the pKey.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM or PRP_ERR_RES_EXHAUSTED if insertion fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_HmAddUnchecked(CONT_Hm *pHm, void *pKey,
                                                void *pVal,
                                                PRP_Bool fail_on_duplicate);

/**
 * Inserts a pKey-value pair with validation.
 *
 * @param pHm  Hashmap instance.
 * @param pKey Key (must not be NULL).
 * @param pVal Value associated with the pKey.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM or PRP_ERR_RES_EXHAUSTED if insertion fails.
 */
PRP_API PRP_Result PRP_CALL CONT_HmAddChecked(CONT_Hm *pHm, void *pKey,
                                              void *pVal,
                                              PRP_Bool fail_on_duplicate);

/**
 * Retrieves the value associated with a pKey.
 *
 * @param pHm  Hashmap instance.
 * @param pKey Key to search.
 * @param pVal Output pointer receiving the value if found.
 *
 * @return PRP_OK if pKey is found.
 * @return PRP_ERR_NOT_FOUND if pKey does not exist.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_HmGetUnchecked(const CONT_Hm *pHm, void *pKey,
                                                void **pVal);

/**
 * Retrieves the value associated with a pKey with validation.
 *
 * @param pHm  Hashmap instance.
 * @param pKey Key to search.
 * @param pVal Output pointer receiving the value if found.
 *
 * @return PRP_OK if pKey is found.
 * @return PRP_ERR_NOT_FOUND if pKey does not exist.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_HmGetChecked(const CONT_Hm *pHm, void *pKey,
                                              void **pVal);

/**
 * Removes a pKey-value pair from the hashmap.
 *
 * @param pHm  Hashmap instance.
 * @param pKey Key identifying the entry.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if pKey does not exist.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_HmDelElemUnchecked(CONT_Hm *pHm, void *pKey);

/**
 * Removes a pKey-value pair with validation.
 *
 * @param pHm  Hashmap instance.
 * @param pKey Key identifying the entry.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if pKey does not exist.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_HmDelElemChecked(CONT_Hm *pHm, void *pKey);

/**
 * Returns the number of elements currently stored.
 *
 * @param pHm Hashmap instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid hashmap (asserts in debug).
 */
PRP_API PRP_Size PRP_CALL CONT_HmLen(const CONT_Hm *pHm);

/**
 * Returns the maximum capacity supported by the hashmap.
 *
 * @return Maximum capacity.
 */
PRP_API PRP_Size PRP_CALL CONT_HmMaxCap(void);

/**
 * Iterates over all pKey-value pairs.
 *
 * @param pHm        Hashmap instance.
 * @param pCb        Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API PRP_Result PRP_CALL CONT_HmForEachUnchecked(
    CONT_Hm *pHm, PRP_Result (*pCb)(void *pKey, void *pVal, void *pUser_data),
    void *pUser_data);

/**
 * Iterates over all pKey-value pairs with validation.
 *
 * @param pHm        Hashmap instance.
 * @param pCb        Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_HmForEachChecked(
    CONT_Hm *pHm, PRP_Result (*pCb)(void *pKey, void *pVal, void *pUser_data),
    void *pUser_data);

/**
 * Resets the hashmap.
 *
 * - All entries are removed.
 * - Key/value destructors are invoked if provided.
 *
 * @param pHm Hashmap instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_API void PRP_CALL CONT_HmResetUnchecked(CONT_Hm *pHm);

/**
 * Resets the hashmap with validation.
 *
 * @param pHm Hashmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if hashmap is invalid.
 */
PRP_API PRP_Result PRP_CALL CONT_HmResetChecked(CONT_Hm *pHm);

#ifdef __cplusplus
}
#endif
