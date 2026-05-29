#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Defs.h"
#include "ForgeWorld/World.h"

/**
 * In this ECS library, the order you define things changes how performance is
 * affected.
 *
 * Since some cascading of metadata needs to be done when something is created,
 * if we do things in the below specified order there will be minimal to no
 * cascading done reducing initialization time.
 *
 * FECS_Init()
 * Register Comps
 * Register Systems
 * Register Behaviors
 * Register Queries
 * FECS_LockSchemaDefs()
 * Create worlds
 * Create layouts
 * Create system caches
 */

/* ----  COMPS ---- */

/**
 * Registers a new component into the FECS.
 *
 * @param name     The name of the component.
 * @param size     Size (in bytes) of the component.
 * @param pComp_id The pointer to hold the index of the comp.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_ALREADY_EXISTS if given comp already exists.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of comps is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_CompRegister(const DT_char *name,
                                                    DT_size size,
                                                    FECS_CompId *pComp_id);

/* ----  BEHAVIOR ---- */

/**
 * Registers a new behavior into the FECS.
 *
 * @param pComp_ids    Array of comp behavior includes. *Array will be sorted.*
 * @param comp_count   Len of the comp_idxs array.
 * @param pBehavior_id The pointer to hold the index of the behavior.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid or pComp_ids contains
 *                         invalid/duplicate entries.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of behaviors or query matches is
 *         reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_BehaviorRegister(
    FECS_CompId *pComp_ids, DT_size comp_count, FECS_BehaviorId *pBehavior_id);

/* ----  QUERY ---- */

/**
 * Registers a new query into the FECS.
 *
 * @param pInc_comp_ids   Array of comps query includes.
 * @param inc_comps_count Len of the inc_comps array.
 * @param pExc_comp_ids   Array of comps query excludes.
 * @param exc_comps_count Len of the exc_comps array.
 * @param pQuery_id       The pointer to hold the index of the query.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid or pInc_comp_ids or
 *                         pExc_comp_ids contains invalid/duplicate entries.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of behaviors or query matches is
 *         reached.
 * @return PRP_ERR_OOM if allocation fails.
 *
 * @note:
 * - If pInc_comp_ids or pExc_comp_ids contain duplicate elements, they will be
 *   de-duplicated since checking for duplicates is expensive in this case.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_QueryRegister(const FECS_CompId *pInc_comp_ids, DT_size inc_comps_count,
                   const FECS_CompId *pExc_comp_ids, DT_size exc_comps_count,
                   FECS_QueryId *pQuery_id);

/* ----  SYSTEMS ---- */

/**
 * Registers a new system into the FECS.
 *
 * @param system_func The system function to register.
 * @param pSystem_id  The pointer to hold the index of the system.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of behaviors or query matches is
 *         reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_SystemRegister(FECS_SystemFunc system_func, FECS_SystemId *pSystem_id);

/* ----  FECS ---- */

/**
 * Locks the schema definition for the FECS.
 * Cannot register anything after this.
 */
PRP_FN_API DT_void PRP_FN_CALL FECS_LockSchemaDefs(DT_void);
/**
 * Initializes the FECS.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void);
/**
 * Exits the FECS. Cleaning up every resource allocated.
 */
PRP_FN_API DT_void PRP_FN_CALL FECS_Exit(DT_void);

#ifdef __cplusplus
}
#endif
