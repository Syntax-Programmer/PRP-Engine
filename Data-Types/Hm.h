#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Utils/Defs.h"

/**
 * A generic hashmap that can store key-value pairs.
 * This supports user defined hash functions for flexibility. This hashmap
 * implementation is counterpart to cPython's dicts, but a little more
 * leaner(since python is very generic).
 */
typedef struct _Hm DT_Hm;

/**
 * Creates the hashmap with the user specified parameters.
 *
 * @param hash_fn: The function that is used to produce the hash of the key the
 * user wants to use.
 * @param key_cmp_cb: This function compares different keys to check if they are
 * the same. This allows for user defined structs to check for match with the
 * specifics they want.
 * @param key_del_cb: The callback that will free the memory of the key
 * correctly and safely when we delete the hashmap, or remove an entry from the
 * hashmap.
 * @param val_del_cb: The callback that will free the memory of the val
 * correctly and safely when we delete the hashmap, or remove an entry from the
 * hashmap.
 *
 * @return The pointer to the hashmap.
 */
PRP_FN_API DT_Hm *PRP_FN_CALL
    DT_HmCreate(DT_u64 (*hash_fn)(const DT_void *key),
                DT_bool (*key_cmp_cb)(const DT_void *k1, const DT_void *k2),
                PRP_FnCode (*key_del_cb)(DT_void *key),
                PRP_FnCode (*val_del_cb)(DT_void *val));
/**
 * Deletes the hashmap and sets the original DT_Hm * to DT_null to prevent use
 * after free bugs.
 *
 * @param pHm: The pointer to the hashmap pointer to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the pHm is DT_null or the hashmap it points
 * to is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmDelete(DT_Hm **pHm);
/**
 * Adds a new key-value pair to the given hashmap. Given the key cannot be
 * DT_null.
 *
 * @param hm: The hashmap to add the key-val pair to.
 * @param key: The key of the entry used to access the value.
 * @param val: The value the key maps to.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if there is no way to add more key-val pairs to
 * the hashmap, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmAdd(DT_Hm *hm, DT_void *key,
                                           DT_void *val);
/**
 * Fetches the value for the given key in the hashmap.
 *
 * @param hm: The hashmap to fetch the value from.
 * @param key: The key to fetch the value it maps to.
 *
 * @return DT_null if the parameters are invalid in any way, or there is not
 * matching key-val pair in the hashmap with the provided key, otherwise it
 * returns the value.
 */
PRP_FN_API DT_void *PRP_FN_CALL DT_HmGet(DT_Hm *hm, DT_void *key);
/**
 * Deletes the key-val pair from the given hashmap.
 *
 * @param hm: The hashmap to delete the key-val pair from.
 * @param key: The key identifying which key-val pair to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_OOB_ERROR if there is no matching key-val pair in the hashmap with the
 * provided key, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmDelElem(DT_Hm *hm, DT_void *key);
/**
 * Fetches the number of key-val pairs the hashmap is currently holding.
 *
 * @param hm: The hashmap to get the len of.
 *
 * @return PRP_INVALID_SIZE if the hashmap is invalid, otherwise the len of the
 * provided hashmap.
 */
PRP_FN_API DT_size PRP_FN_CALL DT_HmLen(DT_Hm *hm);
/**
 * Performs a foreach operation of each of the key-val pair of the hashmap.
 * Calling cb per element.
 *
 * @param hm: The hashmap on which the foreach will happen.
 * @param cb: The callback tho be called per key-val pair. If this doesn't
 * return PRP_FN_SUCCESS, further execution will be halted.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmForEach(DT_Hm *hm,
                                               PRP_FnCode (*cb)(DT_void *key,
                                                                DT_void *val));
/**
 * Resets the hashmap to make it like a brand new hashmap with no entries.
 *
 * @param hm: The hashmap to reset.
 *
 * @return PRP_FN_INV_ARG_ERROR if the hashmap is invalid in some way, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_HmReset(DT_Hm *hm);

#ifdef __cplusplus
}
#endif
