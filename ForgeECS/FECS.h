#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "Defs.h"

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

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_CompRegisterUnchecked(const DT_char *name, DT_size size, DT_size *pIdx);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_CompRegisterChecked(const DT_char *name,
                                                           DT_size size,
                                                           DT_size *pIdx);

/* ----  BEHAVIOR ---- */

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_BehaviorRegisterUnchecked(DT_Arr *comp_idxs, DT_size *pIdx);
PRP_FN_API PRP_Result PRP_FN_CALL
FECS_BehaviorRegisterChecked(DT_Arr *comp_idxs, DT_size *pIdx);

/* ----  QUERY ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_QueryRegisterUnchecked(
    const DT_Arr *inc_comps, const DT_Arr *exc_comps, DT_size *pIdx);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_QueryRegisterChecked(
    const DT_Arr *inc_comps, const DT_Arr *exc_comps, DT_size *pIdx);

/* ----  SYSTEMS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL
FECS_SystemRegisterUnchecked(FECS_System system, DT_size *pIdx);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_SystemRegisterChecked(FECS_System system,
                                                             DT_size *pIdx);

/* ----  LAYOUTS ---- */

/* ----  WORLD ---- */

/* ----  FECS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_LockSchemaDefs(DT_void);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_Exit(DT_void);

#ifdef __cplusplus
}
#endif
