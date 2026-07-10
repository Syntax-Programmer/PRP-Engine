#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/DSArr.h"
#include "DataTypes/StringArr.h"
#include "ForgeECS/Internals/Typedefs.h"

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

DT_bool FindName(DT_char *pName, DT_size name_len, DT_size max_search_ofs,
                 DT_StrArr *pName_bffr, DT_size *pStr_idx);

/* ----  COMPS ---- */

PRP_Result CompRegister(DT_char *pName, DT_size name_len, DT_size comp_size,
                        FECS_CompId *pComp_id);

/* ----  SYTEMS ---- */

PRP_Result SystemRegister(DT_char *pName, DT_size name_len,
                          FECS_SystemFunc system_func,
                          FECS_SystemId *pSystem_id);

#ifdef __cplusplus
}
#endif
