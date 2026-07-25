#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/* ----  STD HASH FUNCS ---- */

/**
 * Hash func can be used to hash a string using FNV1a64 hashing.
 *
 * @param key The key to hash.
 *
 * @return The hash created of the key.
 *
 * @note Generic Use Case Tip:
 * - This can be used to hash any generic byte stream, just cast it to a char
 * ptr and make sure the last byte is nul byte.
 */
PRP_U64 CONT_HmHashStr(const void *str_key);
/**
 * Hash func can be used to hash a u64 using SplitMix64 hashing.
 *
 * @param key The key to hash.
 *
 * @return The hash created of the key.
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
 * A generic hashmap storing key-value pairs.
 *
 * - Uses user-provided hash and comparison functions.
 * - Supports custom destruction callbacks for keys and values.
 * - Designed as a lean alternative to high-level hashmaps (e.g. Python dict).
 */
typedef struct _Hm CONT_Hm;

/**
 * Checks whether the given hashmap is structurally valid.
 *
 * @param hm Pointer to the hashmap.
 *
 * @return PRP_True if valid, PRP_False otherwise.
 */
PRP_FN_API PRP_Bool PRP_FN_CALL CONT_HmIsValid(const CONT_Hm *hm);

/**
 * Creates a hashmap with user-defined behavior.
 *
 * @param hash_fn     Function used to hash keys.
 * @param key_cmp_cb  Function used to compare keys.
 * @param key_del_cb  Callback to destroy keys (can be NULL if not needed).
 * @param val_del_cb  Callback to destroy values (can be NULL if not needed).
 * @param pHm         Output pointer receiving the created hashmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_HmCreateUnchecked(PRP_U64 (*hash_fn)(const void *key),
                     PRP_Bool (*key_cmp_cb)(const void *k1, const void *k2),
                     PRP_Result (*key_del_cb)(void *key),
                     PRP_Result (*val_del_cb)(void *val), CONT_Hm **pHm);

/**
 * Creates a hashmap with validation.
 *
 * @param hash_fn     Function used to hash keys.
 * @param key_cmp_cb  Function used to compare keys.
 * @param key_del_cb  Callback to destroy keys.
 * @param val_del_cb  Callback to destroy values.
 * @param pHm         Output pointer receiving the created hashmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
CONT_HmCreateChecked(PRP_U64 (*hash_fn)(const void *key),
                   PRP_Bool (*key_cmp_cb)(const void *k1, const void *k2),
                   PRP_Result (*key_del_cb)(void *key),
                   PRP_Result (*val_del_cb)(void *val), CONT_Hm **pHm);

/**
 * Deletes the hashmap and nullifies the pointer.
 *
 * @param pHm Pointer to hashmap pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_HmDeleteUnchecked(CONT_Hm **pHm);

/**
 * Deletes the hashmap and nullifies the pointer.
 *
 * @param pHm Pointer to hashmap pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pHm or *pHm is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmDeleteChecked(CONT_Hm **pHm);

/**
 * Inserts a key-value pair into the hashmap.
 *
 * @param hm  Hashmap instance.
 * @param key Key (must not be NULL).
 * @param val Value associated with the key.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM or PRP_ERR_RES_EXHAUSTED if insertion fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmAddUnchecked(CONT_Hm *hm, void *key,
                                                    void *val,
                                                    PRP_Bool fail_on_duplicate);

/**
 * Inserts a key-value pair with validation.
 *
 * @param hm  Hashmap instance.
 * @param key Key.
 * @param val Value.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_OOM or PRP_ERR_RES_EXHAUSTED if insertion fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmAddChecked(CONT_Hm *hm, void *key,
                                                  void *val,
                                                  PRP_Bool fail_on_duplicate);

/**
 * Retrieves the value associated with a key.
 *
 * @param hm   Hashmap instance.
 * @param key  Key to search.
 * @param pVal Output pointer receiving the value if found.
 *
 * @return PRP_OK if key is found.
 * @return PRP_ERR_NOT_FOUND if key does not exist.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmGetUnchecked(const CONT_Hm *hm, void *key,
                                                    void **pVal);

/**
 * Retrieves the value associated with a key with validation.
 *
 * @param hm   Hashmap instance.
 * @param key  Key to search.
 * @param pVal Output pointer receiving the value if found.
 *
 * @return PRP_OK if key is found.
 * @return PRP_ERR_NOT_FOUND if key does not exist.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmGetChecked(const CONT_Hm *hm, void *key,
                                                  void **pVal);

/**
 * Removes a key-value pair from the hashmap.
 *
 * @param hm  Hashmap instance.
 * @param key Key identifying the entry.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if key does not exist.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmDelElemUnchecked(CONT_Hm *hm, void *key);

/**
 * Removes a key-value pair with validation.
 *
 * @param hm  Hashmap instance.
 * @param key Key identifying the entry.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if key does not exist.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmDelElemChecked(CONT_Hm *hm, void *key);

/**
 * Returns the number of elements currently stored.
 *
 * @param hm Hashmap instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid hashmap (asserts in debug).
 */
PRP_FN_API PRP_Size PRP_FN_CALL CONT_HmLen(const CONT_Hm *hm);

/**
 * Returns the maximum capacity supported by the hashmap.
 *
 * @return Maximum capacity.
 */
PRP_FN_API PRP_Size PRP_FN_CALL CONT_HmMaxCap(void);

/**
 * Iterates over all key-value pairs.
 *
 * @param hm         Hashmap instance.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmForEachUnchecked(
    CONT_Hm *hm, PRP_Result (*cb)(void *key, void *val, void *pUser_data),
    void *pUser_data);

/**
 * Iterates over all key-value pairs with validation.
 *
 * @param hm         Hashmap instance.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmForEachChecked(
    CONT_Hm *hm, PRP_Result (*cb)(void *key, void *val, void *pUser_data),
    void *pUser_data);

/**
 * Resets the hashmap.
 *
 * - All entries are removed.
 * - Key/value destructors are invoked if provided.
 *
 * @param hm Hashmap instance.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API void PRP_FN_CALL CONT_HmResetUnchecked(CONT_Hm *hm);

/**
 * Resets the hashmap with validation.
 *
 * @param hm Hashmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if hashmap is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL CONT_HmResetChecked(CONT_Hm *hm);

#ifdef __cplusplus
}
#endif
