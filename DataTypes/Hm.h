#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Utils/Defs.h"

/**
 * DT_Hm
 *
 * A generic hashmap storing key-value pairs.
 *
 * - Uses user-provided hash and comparison functions.
 * - Supports custom destruction callbacks for keys and values.
 * - Designed as a lean alternative to high-level hashmaps (e.g. Python dict).
 */
typedef struct _Hm DT_Hm;

/**
 * Checks whether the given hashmap is structurally valid.
 *
 * @param hm Pointer to the hashmap.
 *
 * @return DT_true if valid, DT_false otherwise.
 */
PRP_FN_API DT_bool PRP_FN_CALL DT_HmIsValid(const DT_Hm *hm);

/**
 * Creates a hashmap with user-defined behavior.
 *
 * @param hash_fn     Function used to hash keys.
 * @param key_cmp_cb  Function used to compare keys.
 * @param key_del_cb  Callback to destroy keys (can be DT_null if not needed).
 * @param val_del_cb  Callback to destroy values (can be DT_null if not needed).
 * @param pHm         Output pointer receiving the created hashmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmCreateUnchecked(
    DT_u64 (*hash_fn)(const DT_void *key),
    DT_bool (*key_cmp_cb)(const DT_void *k1, const DT_void *k2),
    PRP_Result (*key_del_cb)(DT_void *key),
    PRP_Result (*val_del_cb)(DT_void *val), DT_Hm **pHm);

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
DT_HmCreateChecked(DT_u64 (*hash_fn)(const DT_void *key),
                   DT_bool (*key_cmp_cb)(const DT_void *k1, const DT_void *k2),
                   PRP_Result (*key_del_cb)(DT_void *key),
                   PRP_Result (*val_del_cb)(DT_void *val), DT_Hm **pHm);

/**
 * Deletes the hashmap and nullifies the pointer.
 *
 * @param pHm Pointer to hashmap pointer.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API DT_void PRP_FN_CALL DT_HmDeleteUnchecked(DT_Hm **pHm);

/**
 * Deletes the hashmap and nullifies the pointer.
 *
 * @param pHm Pointer to hashmap pointer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pHm or *pHm is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmDeleteChecked(DT_Hm **pHm);

/**
 * Inserts a key-value pair into the hashmap.
 *
 * @param hm  Hashmap instance.
 * @param key Key (must not be DT_null).
 * @param val Value associated with the key.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM or PRP_ERR_RES_EXHAUSTED if insertion fails.
 *
 * @note Unchecked variant:
 * - Asserts on invalid arguments in debug.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmAddUnchecked(DT_Hm *hm, DT_void *key,
                                                    DT_void *val,
                                                    DT_bool fail_on_duplicate);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmAddChecked(DT_Hm *hm, DT_void *key,
                                                  DT_void *val,
                                                  DT_bool fail_on_duplicate);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmGetUnchecked(const DT_Hm *hm,
                                                    DT_void *key,
                                                    DT_void **pVal);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmGetChecked(const DT_Hm *hm, DT_void *key,
                                                  DT_void **pVal);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmDelElemUnchecked(DT_Hm *hm,
                                                        DT_void *key);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmDelElemChecked(DT_Hm *hm, DT_void *key);

/**
 * Returns the number of elements currently stored.
 *
 * @param hm Hashmap instance.
 *
 * @return Number of elements.
 *
 * @note Assumes valid hashmap (asserts in debug).
 */
PRP_FN_API DT_size PRP_FN_CALL DT_HmLen(const DT_Hm *hm);

/**
 * Returns the maximum capacity supported by the hashmap.
 *
 * @return Maximum capacity.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_HmMaxCap(DT_void);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmForEachUnchecked(
    DT_Hm *hm,
    PRP_Result (*cb)(DT_void *key, DT_void *val, DT_void *pUser_data),
    DT_void *pUser_data);

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
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmForEachChecked(
    DT_Hm *hm,
    PRP_Result (*cb)(DT_void *key, DT_void *val, DT_void *pUser_data),
    DT_void *pUser_data);

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
PRP_FN_API DT_void PRP_FN_CALL DT_HmResetUnchecked(DT_Hm *hm);

/**
 * Resets the hashmap with validation.
 *
 * @param hm Hashmap instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if hashmap is invalid.
 */
PRP_FN_API PRP_Result PRP_FN_CALL DT_HmResetChecked(DT_Hm *hm);

#ifdef __cplusplus
}
#endif
