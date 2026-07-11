#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/DSArr.h"
#include "DataTypes/StringArr.h"
#include "ForgeECS/Internals/Typedefs.h"

/**
 * All function declared in this header expect all the parameter to be valid and
 * in perfect condition.
 */

/* ----  INTERNAL CONTEXT ---- */

typedef struct {
    DT_Arr *pComp_sizes;
    DT_StrArr *pComp_names;

    DT_Arr *pSystem_funcs;
    DT_StrArr *pSystem_names;

    DT_DSArr *pWorlds;
} FECS_InternalCtx;

extern FECS_InternalCtx *g_ctx;

#define CTX_INVARIANT_EXPR                                                     \
    (g_ctx != DT_null && DT_ArrIsValid(g_ctx->pComp_sizes) &&                  \
     DT_ArrIsValid(g_ctx->pSystem_funcs) && DT_DSArrIsValid(g_ctx->pWorlds) && \
     DT_StrArrIsValid(g_ctx->pComp_names) &&                                   \
     DT_StrArrIsValid(g_ctx->pSystem_names) &&                                 \
     DT_ArrLen(g_ctx->pComp_sizes) == DT_StrArrLen(g_ctx->pComp_names) &&      \
     DT_ArrLen(g_ctx->pSystem_funcs) == DT_StrArrLen(g_ctx->pSystem_names))

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
PRP_Result CompRegister(DT_char *pName, DT_size name_len, DT_size comp_size,
                        FECS_CompId *pComp_id);

/* ----  SYTEMS ---- */

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
 */
PRP_Result SystemRegister(DT_char *pName, DT_size name_len,
                          FECS_SystemFunc system_func,
                          FECS_SystemId *pSystem_id);

#ifdef __cplusplus
}
#endif
