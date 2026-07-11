#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Internals/Typedefs.h"

/* ----  COMPS ---- */

/**
 * Registers a new component to the FECS registry.
 *
 * @param pName     The name of the component.
 * @param name_len  The len of the name.
 * @param comp_size The size of the component struct.
 * @param pComp_id  Output pointer to the component id.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_ALREADY_EXISTS if the component name is already used.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_CompRegister(DT_char *pName,
                                                    DT_size name_len,
                                                    DT_size comp_size,
                                                    FECS_CompId *pComp_id);

/* ----  SYSTEMS ---- */

/**
 * Registers a new system to the FECS registry.
 *
 * @param pName       The name of the system.
 * @param name_len    The len of the name.
 * @param system_func The function pointer to the system func.
 * @param pSystem_id  Output pointer to the component id.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_ALREADY_EXISTS if the system name is already used.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_SystemRegister(DT_char *pName, DT_size name_len,
                    FECS_SystemFunc system_func, FECS_SystemId *pSystem_id);

/* ----  WORLD ---- */

/**
 * Loads a given world into the FECS.
 *
 * @param pFile_path The fine path to the world to load.
 * @param pWorld_id  Output world id if the loading succeeds.
 *
 * @return PRP_OK on full/partial success.
 * @return PRP_ERR_IO on file opening/indexing errors.
 * @return PRP_ERR_PARSE on invalid file structure/syntax.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_WorldLoad(const DT_char *pFile_path, FECS_WorldId *pWorld_id);
/**
 * Unloads an existing world from the FECS.
 *
 * @param pWorld_id Id to the world to unload.
 *
 * @return PRP_OK on usccess.
 * @return PRP_ERR_OOB if given id is invalid.
 * @return PRP_ERR_INV_STATE if the given id is stale/already deleted.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_WorldUnload(FECS_WorldId *pWorld_id);

/**
 * Finds if a world contains a specific layout seached by its name.
 *
 * @param world_id   The id of the world to find the layout in.
 * @param pName      The name of the layout to find.
 * @param name_len   The len of the name.
 * @parma pLayout_id Output pointer to the id if found.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if the given name doesn't exist/failed to load
 *                           during world load.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_WorldFindLayoutId(FECS_WorldId world_id, const char *pName,
                                  DT_size name_len, FECS_LayoutId *pLayout_id);
/**
 * Finds if a world contains a specific system instance seached by its name.
 *
 * @param world_id            The id of the world to find the system instance
 *                            in.
 * @param pName               The name of the system instance to find.
 * @param name_len            The len of the name.
 * @parma pSystem_instance_id Output pointer to the id if found.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if the given name doesn't exist/failed to load
 *                           during world load.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result
FECS_WorldFindSystemInstanceId(FECS_WorldId world_id, const char *pName,
                               DT_size name_len,
                               FECS_SystemInstanceId *pSystem_instance_id);

/* ----  ENTITIES  ---- */

/**
 * Spawns a new entity inside the given world and layout.
 *
 * @param world_id  The id to the world in which the layout lies.
 * @param layout_id The id to the layout in which to spawn the entity.
 * @param pEntity   Output pointer to the entity filled with data on success.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntitySpawn(FECS_WorldId world_id, FECS_LayoutId layout_id,
                            FECS_EntityId *pEntity);
/**
 * Spawns a new group of entities inside the given world and layout.
 *
 * @param world_id     The id to the world in which the layout lies.
 * @param layout_id    The id to the layout in which to spawn the entities.
 * @param entity_count The number of entities to spawn.
 * @param ppGroup      Output pointer to the group filled with data on success.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntityGroupSpawn(FECS_WorldId world_id, FECS_LayoutId layout_id,
                                 DT_size entity_count,
                                 FECS_EntityGroupId **ppGroup);

/**
 * Checks if the given entity is valid.
 *
 * @param world_id The world in which the entity exists.
 * @param entity   The entity to check the validity of.
 * @param pRslt    The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntityIsValid(FECS_WorldId world_id, const FECS_EntityId entity,
                              DT_bool *pRslt);
/**
 * Checks if the given entity group is valid.
 *
 * @param world_id The world in which the entity group exists.
 * @param pGroup   The entity group to check the validity of.
 * @param pRslt    The pointer to where the result is stored.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntityGroupIsValid(FECS_WorldId world_id,
                                   const FECS_EntityGroupId *pGroup,
                                   DT_bool *pRslt);

/**
 * Kills the given entity and corrupts the entity struct.
 *
 * @param world_id The world in which the entity exists.
 * @param pEntity  Pointer to the entity to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntityKill(FECS_WorldId world_id, FECS_EntityId *pEntity);

/**
 * Kills the given entity group and nullifies it.
 *
 * @param world_id The world in which the entity group exists.
 * @param ppGroup  Pointer to the entity group to kill.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid or *ppGroup is invalid
 *                         internally.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntityGroupKill(FECS_WorldId world_id,
                                FECS_EntityGroupId **ppGroup);

/**
 * Fetches the specified component's pointer that belongs to the entity.
 *
 * @param world_id   The world in which the entity exists.
 * @param entity     The entity to get comp ptr from.
 * @param comp_id    The id of the component to fetch.
 * @param ppComp_ptr Output pointer to store the pointer of the component.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid or entity doesn't have the
 *                         specified component.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntityGetComp(FECS_WorldId world_id, const FECS_EntityId entity,
                              FECS_CompId comp_id, DT_void **ppComp_ptr);
/**
 * Sets the specified component's pointer that belongs to the entity.
 *
 * @param world_id   The world in which the entity exists.
 * @param entity     The entity to set comp of.
 * @param comp_id    The id of the component to fetch.
 * @param pComp_data Pointer to the data that will be set.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid or entity doesn't have the
 *                         specified component.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_Result FECS_EntitySetComp(FECS_WorldId world_id, FECS_EntityId entity,
                              FECS_CompId comp_id, const DT_void *pComp_data);

/**
 * Iterates over the specified component belonging to entities of the group.
 *
 * @param world_id   The world in which the entity exists.
 * @param pGroup     The group of entities to iterate over.
 * @param comp_id    The id of the component to iterate.
 * @param cb         Callback invoked per element.
 * @param pUser_data User-provided context.
 *
 * @return PRP_OK if iteration completes.
 * @return Callback error if cb returns non-PRP_OK.
 * @return PRP_ERR_INV_ARG if arguments are invalid or *ppGroup is invalid
 *                         internally or the entities don't have the specified
 *                         component.
 */
PRP_Result FECS_EntityGroupForEach(
    FECS_WorldId world_id, FECS_EntityGroupId *pGroup, FECS_CompId comp_id,
    PRP_Result (*cb)(DT_void *pComp_data, DT_void *pUser_data),
    DT_void *pUser_data);

/* ----  SYSTEM INSTANCE ---- */

/* ----  FECS ---- */

/**
 * Initialize FECS.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note
 * - If FECS is already initialized this function will just return PRP_OK
 *   without doing anything.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void);
/**
 * Destroyes and exits FECS.
 *
 * @note:
 * -Panics and exits if FECS not initialized correctly.
 */
PRP_FN_API DT_void PRP_FN_CALL FECS_Exit(DT_void);

#ifdef __cplusplus
}
#endif
