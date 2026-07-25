#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Containers/Arr.h"
#include "Containers/DSArr.h"
#include "Containers/StringArr.h"
#include "Forge/Internals/Typedefs.h"

/**
 * All function declared in this header expect all the parameter to be valid and
 * in perfect condition.
 */

/* ----  INTERNAL CONTEXT ---- */

typedef struct {
    CONT_Arr *pComp_sizes;
    CONT_StrArr *pComp_names;

    CONT_Arr *pSystem_infos;
    CONT_StrArr *pSystem_names;

    CONT_DSArr *pWorlds;
} FECS_InternalCtx;

extern FECS_InternalCtx *g_ctx;

#define CTX_INVARIANT_EXPR                                                     \
    (g_ctx != NULL && CONT_ArrIsValid(g_ctx->pComp_sizes) &&                     \
     CONT_ArrIsValid(g_ctx->pSystem_infos) && CONT_DSArrIsValid(g_ctx->pWorlds) && \
     CONT_StrArrIsValid(g_ctx->pComp_names) &&                                   \
     CONT_StrArrIsValid(g_ctx->pSystem_names) &&                                 \
     CONT_ArrLen(g_ctx->pComp_sizes) == CONT_StrArrLen(g_ctx->pComp_names) &&      \
     CONT_ArrLen(g_ctx->pSystem_infos) == CONT_StrArrLen(g_ctx->pSystem_names))

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
 */
PRP_Result CompRegister(PRP_Char8 *pName, PRP_Size name_len, PRP_Size comp_size,
                        FECS_CompId *pComp_id);

/* ----  SYTEMS ---- */

typedef struct {
    FECS_SystemFunc systmem_func;
    PRP_Size comp_ids_needed_count;
    FECS_CompId *pComp_ids_needed;
} FECS_SystemInfo;

/**
 * Registers a new system to the FECS registry.
 *
 * @param pName                 The name of the system.
 * @param name_len              The len of the name.
 * @param system_func           The function pointer to the system func.
 * @param comp_ids_needed_count The len of the pComp_ids_needed array.
 * @param pComp_ids_needed      The array of component ids the system will use.
 * @param pSystem_id            Output pointer to the component id.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_ALREADY_EXISTS if the system name is already used.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_INV_ARG if pComp_ids_needed contains invalid comp id(s).
 */
PRP_Result SystemRegister(PRP_Char8 *pName, PRP_Size name_len,
                          FECS_SystemFunc system_func,
                          PRP_Size comp_ids_needed_count,
                          FECS_CompId *pComp_ids_needed,
                          FECS_SystemId *pSystem_id);
/**
 * Deletes a given system's internals.
 * Called via CONT_ArrForEach_...

 *
 * @param pVal System to delete internals of..
 *
 * @return PRP_OK on success.
 */
PRP_Result SystemInfoDeleteCb(void *pVal, void *_);

#ifdef __cplusplus
}
#endif
