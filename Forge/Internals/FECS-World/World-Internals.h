#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Bitmap.h"
#include "DataTypes/StringArr.h"
#include "Diagnostics/Assert.h"
#include "ForgeECS/Internals/Typedefs.h"

/**
 * All function declared in this header expect all the parameter to be valid and
 * in perfect condition.
 */

/* ----  CREATE INFO ---- */

typedef struct {
    FECS_SystemId system_id;
    DT_size layout_id_match_count;
    // These will be taken ownership of by the world.
    // Caller must not destroy/access after successful SystemInstanceCreate().
    FECS_LayoutId *pLayout_id_matches;
    /*
     * Number of strides to compute, this is equal to the comp_ids_needed_count
     * in the FECS_SystemInfo.
     */
    DT_size stride_dispatch_count;
} FECS_SystemInstanceCreateInfo;

typedef struct {
    DT_size layout_count;
    // These will be freed.
    DT_Bitmap **ppLayout_create_infos;
    // These will be taken ownership of by the world.
    // Caller must not destroy or access after successful WorldCreate().
    DT_StrArr *pLayout_names;

    DT_size system_instance_count;
    // These will be freed.
    FECS_SystemInstanceCreateInfo *pSystem_instance_create_infos;
    // These will be taken ownership of by the world.
    // Caller must not destroy or access after successful WorldCreate().
    DT_StrArr *pSystem_instance_names;
} FECS_WorldCreateInfo;

/* ----  LAYOUTS ---- */

#define CHUNK_CAP (64)
typedef DT_u64 FECS_ChunkFreeSlotType;

typedef struct {
    DT_u32 gens[CHUNK_CAP];
    FECS_ChunkFreeSlotType free_slot_bitset;
    DT_u8 pChunk_mem[];
} FECS_Chunk;

DIAG_STATIC_ASSERT(CHUNK_CAP == sizeof(DT_u64) * 8,
                   "free_slot bit width must match CHUNK_CAP");

typedef struct {
    DT_Bitmap *pComp_set;
    /*
     * This stores the stride of each component's arrays the layout chunk
     * stores. This is used to identify memory location of specific component
     * arrays and component for entities.
     * Cap of this array is equal to:
     * DT_BitmapSetCount(FECS_Layout::pComp_set);
     *
     * And each stride entry corresponds to the corresponding bit set at that
     * RANK in the pComp_set bitmap.
     */
    DT_size *pComp_arr_strides;
    /**
     * Prefix population counts of FECS_Layout::pComp_set.
     *
     * Entry i stores the total number of set bits in bitmap words
     * [0, i - 1]. In other words:
     *
     *     prefix_popcnts[i] =
     *         popcnt(words[0]) + ... + popcnt(words[i - 1])
     *
     * The prefix count for word 0 is implicitly 0. These prefix counts are used
     * to compute the bit-rank of a component in O(1) time.
     *
     * Array capacity:
     *     WORD_I(DT_BitmapBitCap(FECS_Layout::pComp_set)) + 1
     *
     * @note:
     * - Usage of DT_16 to store popcnts (that ultimately represent the number
     * of comps set in each bitmap word) sets a hard cap of UINT16_MAX as the
     * max number of components that can be registered.
     */
    DT_u16 *pWord_prefix_popcnts;
    DT_Arr *pChunk_ptrs;
    DT_Bitmap *pFree_chunk_bitset;
    DT_size chunk_total_size;
} FECS_Layout;

/**
 * Until entities are created, layouts have a relatively small memory footprint.
 * The primary factor affecting an empty layout's size is the number of
 * registered component arrays it manages.
 *
 * Approximate memory footprint of an empty layout:
 *     sizeof(DT_size) * component_count + ~(300-400) bytes
 *
 * where:
 *   - sizeof(DT_size) * component_count accounts for the component stride
 * array.
 *   - ~300 bytes is an implementation-dependent approximation of the remaining
 *     metadata and supporting data structures (bitmaps, chunk pointers, etc.).
 *
 * This approximation is intended for estimation only and is not part of the
 * API contract.
 */

/**
 * Creates a fully defined layout from the given create info.
 *
 * @param pCreate_info The schema to what the layout contains.
 * @param pLayout      Output pointer to the newly created layout.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LayoutCreate(DT_Bitmap *pCreate_info, FECS_Layout *pLayout);
/**
 * Deletes internals-only of the existing layout.
 *
 * @param pLayout The layout to delete internals of.
 */
DT_void LayoutDelete(FECS_Layout *pLayout);

/* ----  SYSTEM INSTANCES ---- */

typedef struct {
    FECS_SystemId system_id;
    DT_size layout_id_match_count;
    FECS_LayoutId *pLayout_id_matches;
    /**
     * A preallocated buffer for all the strides of components to be loaded into
     * at the start of each layout.
     * This will be used during system execution to support how the user access
     * the component.
     *
     * The len of this is exactly tied to system_id, and is exactly equal to the
     * FECS_SystemInfo::comp_ids_needed_count.
     */
    DT_size *pStride_dispatches;
} FECS_SystemInstance;

/**
 * Creates a fully defined system instance from the given create info.
 *
 * @param pCreate_info     The schema to what the system instance contains.
 * @param pSystem_instance Output pointer to the newly created system instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if qllocation fails.
 */
PRP_Result SystemInstanceCreate(FECS_SystemInstanceCreateInfo *pCreate_info,
                                FECS_SystemInstance *pSystem_instance);
/**
 * Deletes internals-only of the existing system instance.
 *
 * @param pSystem_instance The system instance to delete internals of.
 */
DT_void SystemInstanceDelete(FECS_SystemInstance *pSystem_instance);

/* ----  WORLD ---- */

typedef struct {
    DT_size layout_count;
    FECS_Layout *pLayouts;
    DT_StrArr *pLayout_names;

    DT_size system_instance_count;
    FECS_SystemInstance *pSystem_instances;
    DT_StrArr *pSystem_instance_names;
} FECS_World;

/**
 * Deletes a given world.
 * Used inside the DT_DSArr's elem_del_cb.
 *
 * @param pWorld World to delete.
 *
 * @return PRP_OK on success.
 */
PRP_Result WorldDeleteCb(DT_void *pWorld);
/**
 * Creates a fully defined world from the given create info.
 * Consumes the entire create info regardless of success or fail. So no need to
 * free after passing create info here.
 *
 * @param pCreate_info The schema to what the world contains.
 * @param pWorld       Output pointer to the newly created world.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result WorldCreate(FECS_WorldCreateInfo *pCreate_info, FECS_World *pWorld);
/**
 * Searches for a specified layout name inside the given world.
 *
 * @param pWorld   The world to search the layout in.
 * @param pname    The name of the layout to search.
 * @param name_len The len of the name to search.
 *
 * @return FECS_LayoutId if the name is found, otherwise FECS_INVALID_ID.
 */
FECS_LayoutId WorldFindLayout(const FECS_World *pWorld, const DT_char *pName,
                              DT_size name_len);
/**
 * Searches for a specified system instance name inside the given world.
 *
 * @param pWorld   The world to search the system instance in.
 * @param pname    The name of the system instance to search.
 * @param name_len The len of the name to search.
 *
 * @return FECS_SystemInstanceId if the name is found, otherwise
 *         FECS_INVALID_ID.
 */
FECS_SystemInstanceId WorldFindSystemInstance(const FECS_World *pWorld,
                                              const DT_char *pName,
                                              DT_size name_len);

/* ----  ENTITIES ---- */

/**
 * Spawns a new entity into the given layout.
 *
 * @param pWorld    World, the layout belongs to.
 * @param layout_id The layout to spawn entity from.
 * @param pEntity   The pointer to where the entity will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result EntitySpawn(FECS_World *pWorld, FECS_LayoutId layout_id,
                       FECS_EntityId *pEntity);
/**
 * Spawns a group of entities at once into the given layout.
 *
 * Will return a group with less entities created if it fails mid allocation.
 *
 * @param pWorld       World, the layout belongs to.
 * @param layout_id    The layout to spawn entities from.
 * @param entity_count The number of entities to spawn.
 * @param ppGroup      The pointer to where the entities will be stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result EntityGroupSpawn(FECS_World *pWorld, FECS_LayoutId layout_id,
                            DT_size entity_count, FECS_EntityGroupId **ppGroup);
/**
 * Checks if the given entity is valid.
 *
 * @param pWorld  World, the entity belongs to.
 * @param entity  The entitiy to check.
 *
 * @return DT_true if valid.
 * @return DT_false if invalid.
 */
DT_bool EntityIsValid(FECS_World *pWorld, const FECS_EntityId entity);
/**
 * Checks if the given entity group is valid.
 *
 * @param pWorld World, the entities belongs to.
 * @param pBatch The entitiy group to check.
 *
 * @return DT_true if valid.
 * @return DT_false if invalid.
 */
DT_bool EntityGroupIsValid(FECS_World *pWorld,
                           const FECS_EntityGroupId *pGroup);
/**
 * Kills the given entity, and invalidates it.
 *
 * @param pWorld World, the entity belongs to.
 * @param entity The entitiy to kill.
 */
DT_void EntityKill(FECS_World *pWorld, FECS_EntityId *pEntity);
/**
 * Kills the given entity and nullifies the pointer.
 *
 * @param pWorld  World, the entities belongs to.
 * @param ppGroup The pointer to the entities to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entities is invalid INTERNALLY.
 */
PRP_Result EntityGroupKill(FECS_World *pWorld, FECS_EntityGroupId **ppGroup);
/**
 * Fetches the pointer to the specific component of an entity.
 *
 * @param pWorld     World the entity belongs to.
 * @param entity     The entity whose component to get.
 * @param comp_id    The component to fetch of the entity.
 * @param ppComp_ptr Output pointer of the component data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entity doesn't have the component.
 */
PRP_Result EntityGetComp(FECS_World *pWorld, const FECS_EntityId entity,
                         FECS_CompId comp_id, DT_void **ppComp_ptr);
/**
 * Sets the value to the specific component of an entity.
 *
 * @param pWorld     World the entity belongs to.
 * @param entity     The entity whose component to set.
 * @param comp_id    The component to set of the entity.
 * @param pComp_data The pointer to the value to set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if entity doesn't have the component.
 */
PRP_Result EntitySetComp(FECS_World *pWorld, FECS_EntityId entity,
                         FECS_CompId comp_id, const DT_void *pComp_data);
/**
 * Iterates over all entities of a batch.
 *
 * @param pWorld     World, the entities belongs to.
 * @param pGroup     The entities to operate on.
 * @param comp_id    The component to operate on.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if the entities don't have the component or the batch
 *                         is invalid.
 */
PRP_Result EntityGroupForEach(FECS_World *pWorld, FECS_EntityGroupId *pGroup,
                              FECS_CompId comp_id,
                              PRP_Result (*cb)(DT_void *pComp_data,
                                               DT_void *pUser_data),
                              DT_void *pUser_data);

/* ----  SYSTEM INSTANCE EXEC ---- */

/**
 * Executes the given system instance.
 *
 * @param pWorld World, the system instance belongs to.
 */
DT_void SystemInstanceExec(FECS_World *pWorld,
                           FECS_SystemInstanceId system_instance_id,
                           DT_void *pUser_data);
/**
 * Fetches pointer of the component array during system exec using exec
 * internals.
 *
 * @param pExec_internals The internal data needed for system execution.
 * @param idx             The index into the strides array to fetch comp array.
 *                        This index corresponds to the user provided
 *                        pComp_ids_needed array during system registration.
 *
 * @return Valid component array ptr on success.
 * @return DT_null if idx is out of bounds.
 *
 * @note:
 * - The user is not to explictly interact with the returned ptr. It is for
 * internal use only.
 */
DT_void *
SystemInstanceFetchComp(const FECS_SystemExecInternalData *pExec_internals,
                        DT_size idx);

#ifdef __cplusplus
}
#endif
