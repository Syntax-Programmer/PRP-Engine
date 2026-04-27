#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "World/World.h"

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
 * @param name The name of the component.
 * @param size Size (in bytes) of the component.
 * @param pIdx The pointer to hold the index of the behavior.
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
                                                    DT_size *pIdx);

/* ----  BEHAVIOR ---- */

/**
 * Registers a new behavior into the FECS.
 *
 * @param comp_idxs Array of comp behavior includes. Array will mutate.
 * @param pIdx      The pointer to hold the index of the behavior.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of behaviors or query matches is
 *         reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_BehaviorRegister(DT_Arr *comp_idxs,
                                                        DT_size *pIdx);

/* ----  QUERY ---- */

/**
 * Registers a new query into the FECS.
 *
 * @param inc_comps Array of comps query includes.
 * @param exc_comps Array of comps query excludes.
 * @param pIdx      The pointer to hold the index of the query.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of behaviors or query matches is
 *         reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_QueryRegister(const DT_Arr *inc_comps,
                                                     const DT_Arr *exc_comps,
                                                     DT_size *pIdx);

/* ----  SYSTEMS ---- */

/**
 * Registers a new system into the FECS.
 *
 * @param system The system function to register.
 * @param pIdx   The pointer to hold the index of the system.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_STATE if the schema lock is already initiated.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_RES_EXHAUSTED if max cap of behaviors or query matches is
 *         reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemRegister(FECS_System system,
                                                      DT_size *pIdx);

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
