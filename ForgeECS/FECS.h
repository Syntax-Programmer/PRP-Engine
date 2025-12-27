#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Core/IdMgr.h"
#include "../Utils/Defs.h"

/* ----  COMP ---- */

/**
 * The id to a registered component a.k.a behavior.
 */
typedef DT_size FECS_CompId;
#define FECS_INVALID_COMP_ID ((FECS_CompId)(-1))

/**
 * Registers the component of the given size to the FECS.
 *
 * @param comp_size: The size the comp struct is going to be.
 *
 * @return FECS_INVALID_COMP_ID if the comp_size=0 or we can't create any more
 * components, otherwise the componenet id of the registerted component
 */
PRP_FN_API FECS_CompId PRP_FN_CALL FECS_CompRegister(DT_size comp_size);

/* ----  BEHAVIOR SET ---- */

/**
 * Creates an empty behavior set and returns an id to its.
 *
 * @return The id of the behavior set.
 */
PRP_FN_API CORE_Id PRP_FN_CALL FECS_BehaviorSetCreate(DT_void);
/**
 * Deletes the behavior set and invalidates the original CORE_Id * to
 * CORE_INVALID_ID to prevent use after free bugs.
 *
 * @param pB_set_id: The pointer to the id of the behavior set to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pB_set_id is DT_null or the id it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_BehaviorSetDelete(CORE_Id *pB_set_id);
/**
 * Empties the behavior set the given id is linked to.
 *
 * #param b_set_id; The id to the behavior set to empty,
 *
 * @return PRP_FN_INV_ARG_ERROR id the given id is invalid in some way,
 * otherwise the PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_BehaviorSetClear(CORE_Id b_set_id);
/**
 * Attaches the given component id to the behavior set the given b_set_id
 * points to.
 *
 * @param b_set_id: The id to the behavior set to attach the component to.
 * @param comp_id: The component id to attach to the behavior set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * PRP_FN_RES_EXHAUSTED_ERROR if the behavior_set cannot accommodate anymore
 * comp id, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_BehaviorSetAttachComp(CORE_Id b_set_id, FECS_CompId comp_id);
/**
 * Detaches the given component id from the behavior set the given b_set_id
 * points to.
 *
 * @param b_set_id: The id to the behavior set to detach the component from.
 * @param comp_id: The component id to detach from the behavior set.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL
FECS_BehaviorSetDetachComp(CORE_Id b_set_id, FECS_CompId comp_id);
/**
 * Checks if the given component id is present in the behavior set the b_set_id
 * points to.
 *
 * @param b_set_id: The id to the behavior set to check for.
 * @param comp_id: The component id to check for in the behavior set.
 * @param pRslt: The pointer to the variable where the boolean result will be
 * stored.
 *
 * @return PRP_FN_INV_ARG_ERROR if the parameters are invalid in any way,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_BehaviorSetHasComp(CORE_Id b_set_id,
                                                          FECS_CompId comp_id,
                                                          DT_bool *pRslt);

/* ----  SYSTEM  ---- */

/**
 * A system function is a function that is executed by the engine on a set of
 * components that match a query.
 */
typedef DT_void (*FECS_SysFn)(DT_void *comp_arr, DT_size len,
                              DT_void *user_data, DT_u32 sys_data);

/**
 * Thes macros abstract the method of iteration over the entities of matching
 * the query of the system.
 */
#define FECS_SYS_DATA_LOOP(sys_data) while ((sys_data))
#define FECS_SYS_GET_I(sys_data, i)                                            \
    do {                                                                       \
        DT_Bitword mask = (sys_data) & -(sys_data);                            \
        (i) = DT_BitwordCtz(mask);                                             \
        sys_data ^= mask;                                                      \
    } while (0);

/**
 * Creates the system user specified function and the query it applies to.
 *
 * @param query_id: The id of the query the system will apply to.
 * @param func: The function that will be executed by the system.
 * @param user_data: The user data that will be passed to the function.
 *
 * @return The id of the created system.
 */
PRP_FN_API CORE_Id PRP_FN_CALL FECS_SystemCreate(CORE_Id query_id,
                                                 FECS_SysFn func,
                                                 DT_void *user_data);
/**
 * Deletes the system and invalidates the original CORE_Id * to CORE_INVALID_ID
 * to prevent use after free bugs.
 *
 * @param pSystem_id: The pointer to the id of the system to delete.
 *
 * @return PRP_FN_INV_ARG_ERROR if pSystem_id is DT_null or the id it points to
 * is invalid, otherwise it returns PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_SystemDelete(CORE_Id *pSystem_id);
/**
 * Executes the system with the given id.
 *
 * @param system_id: The id of the system to execute.
 *
 * @return PRP_FN_INV_ARG_ERROR if system_id is invalid, otherwise it returns
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_SystemExec(CORE_Id system_id);

/* ----  STATE  ---- */

/**
 * Initializes the FECS module of the engine.
 *
 * @return PRP_FN_MALLOC_ERROR if there is error allocating internal resources,
 * otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_Init(DT_void);
/**
 * Exits the FECS module of the engine, freeing all the resources used.
 *
 * @return PRP_FN_NULL_ERROR if the Exit is called before proper initialization
 * of the FECS system, otherwise PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL FECS_Exit(DT_void);

#ifdef __cplusplus
}
#endif
