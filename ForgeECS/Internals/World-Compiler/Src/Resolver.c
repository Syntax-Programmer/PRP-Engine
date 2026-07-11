#include "ForgeECS/Internals/FECS/FECS-Internals.h"
#include "ForgeECS/Internals/World-Compiler/Compiler-Internals.h"

typedef struct {
    DT_ByteBffr *pIdentifier_bffr;
    FECS_WorldCreateInfo *pCreate_info;
} DeclResolveData;

typedef struct {
    DT_ByteBffr *pIdentifier_bffr;

    DT_Bitmap *pComp_set;

    // Used for debugging.
    DT_size comp_name_len;
    DT_char *pName;
} CompResolveData;

/**
 * Destroyes the create info be it partial created or fully.
 *
 * @param pCreate_info The create info to delete.
 */
static DT_void DestroyCreateInfo(FECS_WorldCreateInfo *pCreate_info);
/**
 * Initializes the create info for resolving.
 *
 * @param pParse_table The parse table to initialize create info with.
 * @param pCreate_info The create info to initialize.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result CreateInfoInit(const FECS_WCParseTable *pParse_table,
                                 FECS_WorldCreateInfo *pCreate_info);

/**
 * Resolves a component name, and adds it to a comp bitset.
 * Called via DT_ArrForEach_...
 *
 * @param pVal       The identifier tok of the comp name.
 * @param pUser_data CompResolveData instance with all relevant data.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_NOT_FOUND if the component name couldn't be resolved.
 */
static PRP_Result ResolveCompName(DT_void *pVal, DT_void *pUser_data);

/**
 * Resolve an entire layout decl to create pLayout_create_info bitmap.
 * Called via DT_ArrForEach_...
 *
 * @param pVal       The layout decl to resolve.
 * @param pUser_data DeclResolveData instance with all relevant data.
 *
 * @return PRP_OK on success.
 * @return PRP_OK if layout already exists. This is to not halt foreach exec.
 * @return PRP_OK if unregisterd comps exist. This is to not halt foreach exec.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ResolveLayoutDecl(DT_void *pVal, DT_void *pUser_data);

/**
 * Resolves system instance decl into inc and exc comp bitsets.
 *
 * @param pSystem_instance_name    The name of the system instance to resolve.
 * @param system_instance_name_len The len of the system instance name.
 * @param pSystem_instance_decl    The system instance decl to resolve.
 * @param pIdentifier_bffr         The identifier bbfr that stores names of
 *                                 comps in the pParse_table.
 * @param ppInc_comp_set           Output resolved inc comp set bitmap.
 * @param ppExc_comp_set           Output resolved exc comp set bitmap.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 * @return PRP_ERR_NOT_FOUND if the component name couldn't be resolved.
 */
static PRP_Result CreateSystemInstanceCompSets(
    const DT_char *pSystem_instance_name, DT_size system_instance_name_len,
    FECS_WCSystemInstanceDecl *pSystem_instance_decl,
    DT_ByteBffr *pIdentifier_bffr, DT_Bitmap **ppInc_comp_set,
    DT_Bitmap **ppExc_comp_set);
/**
 * Filters existing layouts based on the inc and exc comp sets.
 *
 * @param pResolve_data                DeclResolveData instance with all
 *                                     relevant data.
 * @param pInc_comp_set                The inc component bit set.
 * @param pExc_comp_set                The exc component bit set.
 * @param pSystem_instance_create_info Create info to where load the filtered
 *                                     layouts.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result
FilterLayouts(DeclResolveData *pResolve_data, DT_Bitmap *pInc_comp_set,
              DT_Bitmap *pExc_comp_set,
              FECS_SystemInstanceCreateInfo *pSystem_instance_create_info);
/**
 * Resolve an entire system instance decl to create system_instance_create_info.
 * Called via DT_ArrForEach_...
 *
 * @param pVal       The system instance decl to resolve.
 * @param pUser_data DeclResolveData instance with all relevant data.
 *
 * @return PRP_OK on success.
 * @return PRP_OK if system instance already exists. This is to not halt foreach
 *                exec.
 * @return PRP_OK if unregisterd comps/system_func exist. This is to not halt
 *                foreach exec.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ResolveSystemInstanceDecl(DT_void *pVal, DT_void *pUser_data);

static DT_void DestroyCreateInfo(FECS_WorldCreateInfo *pCreate_info) {
    if (pCreate_info->ppLayout_create_infos) {
        for (DT_size i = 0; i < pCreate_info->layout_count; i++) {
            DT_Bitmap *pLayout_create_info =
                pCreate_info->ppLayout_create_infos[i];
            DT_BitmapDeleteUnchecked(&pLayout_create_info);
        }
        free(pCreate_info->ppLayout_create_infos);
        DT_StrArrDeleteUnchecked(&pCreate_info->pLayout_names);
        pCreate_info->layout_count = 0;
    }
    if (pCreate_info->pSystem_instance_create_infos) {
        for (DT_size i = 0; i < pCreate_info->system_instance_count; i++) {
            FECS_SystemInstanceCreateInfo *pSystem_instance_create_info =
                &pCreate_info->pSystem_instance_create_infos[i];
            free(pSystem_instance_create_info->pLayout_id_matches);
        }
        free(pCreate_info->pSystem_instance_create_infos);
        DT_StrArrDeleteUnchecked(&pCreate_info->pSystem_instance_names);
        pCreate_info->system_instance_count = 0;
    }
}

static PRP_Result CreateInfoInit(const FECS_WCParseTable *pParse_table,
                                 FECS_WorldCreateInfo *pCreate_info) {
    // This will be incremented as we parse, this is to account for failed init.
    pCreate_info->layout_count = 0;
    pCreate_info->system_instance_count = 0;

    DT_size layout_count = DT_ArrLen(pParse_table->pLayout_table);
    pCreate_info->ppLayout_create_infos =
        malloc(sizeof(DT_Bitmap *) * layout_count);
    if (!pCreate_info->ppLayout_create_infos) {
        return PRP_ERR_OOM;
    }
    PRP_Result code =
        DT_StrArrCreateUnchecked(pParse_table->layout_names_size, layout_count,
                                 &pCreate_info->pLayout_names);
    if (code != PRP_OK) {
        DestroyCreateInfo(pCreate_info);
        return code;
    }

    DT_size system_instance_count =
        DT_ArrLen(pParse_table->pSystem_instance_table);
    pCreate_info->pSystem_instance_create_infos =
        malloc(sizeof(FECS_SystemInstanceCreateInfo) * system_instance_count);
    if (!pCreate_info->pSystem_instance_create_infos) {
        DestroyCreateInfo(pCreate_info);
        return PRP_ERR_OOM;
    }
    code = DT_StrArrCreateUnchecked(pParse_table->system_instance_names_size,
                                    system_instance_count,
                                    &pCreate_info->pSystem_instance_names);
    if (code != PRP_OK) {
        DestroyCreateInfo(pCreate_info);
        return code;
    }

    return PRP_OK;
}

static PRP_Result ResolveCompName(DT_void *pVal, DT_void *pUser_data) {
    FECS_WCIdentifierTok *pTok = pVal;
    CompResolveData *pComp_resolve_data = pUser_data;

    pComp_resolve_data->comp_name_len = pTok->size;
    pComp_resolve_data->pName = DT_ByteBffrGetUnchecked(
        pComp_resolve_data->pIdentifier_bffr, pTok->ofs);
    DT_size idx;
    if (!DT_StrArrSearchUnchecked(g_ctx->pComp_names, pComp_resolve_data->pName,
                                  pTok->size, &idx)) {
        return PRP_ERR_NOT_FOUND;
    }

    DT_BitmapSetUnchecked(pComp_resolve_data->pComp_set, idx);

    return PRP_OK;
}

static PRP_Result ResolveLayoutDecl(DT_void *pVal, DT_void *pUser_data) {
    FECS_WCLayoutDecl *pLayout_decl = pVal;
    DeclResolveData *pResolve_data = pUser_data;

    DT_size layout_name_len = pLayout_decl->layout_name.size;
    DT_char *pLayout_name = DT_ByteBffrGetUnchecked(
        pResolve_data->pIdentifier_bffr, pLayout_decl->layout_name.ofs);
    if (DT_StrArrSearchUnchecked(pResolve_data->pCreate_info->pLayout_names,
                                 pLayout_name, layout_name_len, DT_null)) {
        DIAG_LOG_INFO(DIAG_LOG_CODE_NONE,
                      "Layout: %.*s, already exists, the entire layout "
                      "declaration will be skipped.",
                      (int)layout_name_len, pLayout_name);
        return PRP_OK;
    }

    DT_Bitmap *pLayout_create_info;
    PRP_Result code = DT_BitmapCreateUnchecked(DT_ArrLen(g_ctx->pComp_sizes),
                                               &pLayout_create_info);
    if (code != PRP_OK) {
        return PRP_ERR_OOM;
    }
    CompResolveData comp_resolve_data = {.pIdentifier_bffr =
                                             pResolve_data->pIdentifier_bffr,
                                         .pComp_set = pLayout_create_info};
    code = DT_ArrForEachUnchecked(pLayout_decl->pComp_names, ResolveCompName,
                                  &comp_resolve_data);
    if (code != PRP_OK) {
        DT_BitmapDeleteUnchecked(&pLayout_create_info);
        DIAG_LOG_INFO(
            DIAG_LOG_CODE_INVALID_STATE,
            "Layout: %.*s, contains unregistered component "
            "name: %.*s, the entire layout declaration will be skipped.",
            (int)layout_name_len, pLayout_name,
            (int)comp_resolve_data.comp_name_len, comp_resolve_data.pName);
        return PRP_OK;
    }

    code = DT_StrArrPushUnchecked(pResolve_data->pCreate_info->pLayout_names,
                                  pLayout_name, layout_name_len);
    if (code != PRP_OK) {
        DT_BitmapDeleteUnchecked(&pLayout_create_info);
        return code;
    }
    DT_Bitmap **ppLayout_create_infos =
        pResolve_data->pCreate_info->ppLayout_create_infos;
    ppLayout_create_infos[pResolve_data->pCreate_info->layout_count++] =
        pLayout_create_info;

    return PRP_OK;
}

static PRP_Result CreateSystemInstanceCompSets(
    const DT_char *pSystem_instance_name, DT_size system_instance_name_len,
    FECS_WCSystemInstanceDecl *pSystem_instance_decl,
    DT_ByteBffr *pIdentifier_bffr, DT_Bitmap **ppInc_comp_set,
    DT_Bitmap **ppExc_comp_set) {
    PRP_Result code =
        DT_BitmapCreateUnchecked(DT_ArrLen(g_ctx->pComp_sizes), ppInc_comp_set);
    if (code != PRP_OK) {
        return code;
    }
    code =
        DT_BitmapCreateUnchecked(DT_ArrLen(g_ctx->pComp_sizes), ppExc_comp_set);
    if (code != PRP_OK) {
        DT_BitmapDeleteUnchecked(ppInc_comp_set);
        return code;
    }
    CompResolveData comp_resolve_data = {.pIdentifier_bffr = pIdentifier_bffr,
                                         .pComp_set = *ppInc_comp_set};
    code = DT_ArrForEachUnchecked(pSystem_instance_decl->pInc_comp_names,
                                  ResolveCompName, &comp_resolve_data);
    if (code != PRP_OK) {
        DT_BitmapDeleteUnchecked(ppInc_comp_set);
        DT_BitmapDeleteUnchecked(ppExc_comp_set);
        DIAG_LOG_INFO(
            DIAG_LOG_CODE_INVALID_STATE,
            "System Instance: %.*s, contains unregistered include component "
            "name: %.*s, the entire system instance declaration will be "
            "skipped.",
            (int)system_instance_name_len, pSystem_instance_name,
            (int)comp_resolve_data.comp_name_len, comp_resolve_data.pName);
        return PRP_ERR_NOT_FOUND;
    }
    comp_resolve_data.pComp_set = *ppExc_comp_set;
    code = DT_ArrForEachUnchecked(pSystem_instance_decl->pExc_comp_names,
                                  ResolveCompName, &comp_resolve_data);
    if (code != PRP_OK) {
        DT_BitmapDeleteUnchecked(ppInc_comp_set);
        DT_BitmapDeleteUnchecked(ppExc_comp_set);
        DIAG_LOG_INFO(
            DIAG_LOG_CODE_INVALID_STATE,
            "System Instance: %.*s, contains unregistered exclude component "
            "name: %.*s, the entire system instance declaration will be "
            "skipped.",
            (int)system_instance_name_len, pSystem_instance_name,
            (int)comp_resolve_data.comp_name_len, comp_resolve_data.pName);
        return PRP_ERR_NOT_FOUND;
    }

    return PRP_OK;
}

static PRP_Result
FilterLayouts(DeclResolveData *pResolve_data, DT_Bitmap *pInc_comp_set,
              DT_Bitmap *pExc_comp_set,
              FECS_SystemInstanceCreateInfo *pSystem_instance_create_info) {
    // Allocate all in one go, we can shrink fit if needed.
    pSystem_instance_create_info->pLayout_id_matches = malloc(
        sizeof(FECS_LayoutId) * pResolve_data->pCreate_info->layout_count);
    if (!pSystem_instance_create_info->pLayout_id_matches) {
        return PRP_ERR_OOM;
    }
    pSystem_instance_create_info->layout_id_match_count = 0;

    for (DT_size i = 0; i < pResolve_data->pCreate_info->layout_count; i++) {
        DT_Bitmap *pLayout_create_info =
            pResolve_data->pCreate_info->ppLayout_create_infos[i];

        if (DT_BitmapHasAnyUnchecked(pLayout_create_info, pExc_comp_set)) {
            continue;
        } else if (DT_BitmapHasAllUnchecked(pLayout_create_info,
                                            pInc_comp_set)) {
            pSystem_instance_create_info->pLayout_id_matches
                [pSystem_instance_create_info->layout_id_match_count++] = i;
        }
    }

    if (pSystem_instance_create_info->layout_id_match_count == 0) {
        free(pSystem_instance_create_info->pLayout_id_matches);
        pSystem_instance_create_info->pLayout_id_matches = DT_null;
    } else if (pSystem_instance_create_info->layout_id_match_count <
               pResolve_data->pCreate_info->layout_count) {
        FECS_LayoutId *pMatches =
            realloc(pSystem_instance_create_info->pLayout_id_matches,
                    sizeof(FECS_LayoutId) *
                        pSystem_instance_create_info->layout_id_match_count);
        if (pMatches) {
            pSystem_instance_create_info->pLayout_id_matches = pMatches;
        }
        // We don't care if the shrink fit fails.
    }

    return PRP_OK;
}

static PRP_Result ResolveSystemInstanceDecl(DT_void *pVal,
                                            DT_void *pUser_data) {
    FECS_WCSystemInstanceDecl *pSystem_instance_decl = pVal;
    DeclResolveData *pResolve_data = pUser_data;

    DT_size system_instance_name_len =
        pSystem_instance_decl->system_instance_name.size;
    DT_char *pSystem_instance_name = DT_ByteBffrGetUnchecked(
        pResolve_data->pIdentifier_bffr,
        pSystem_instance_decl->system_instance_name.ofs);
    if (DT_StrArrSearchUnchecked(
            pResolve_data->pCreate_info->pSystem_instance_names,
            pSystem_instance_name, system_instance_name_len, DT_null)) {
        DIAG_LOG_INFO(
            DIAG_LOG_CODE_NONE,
            "System Instance: %.*s, already exists, the entire system instance "
            "declaration will be skipped.",
            (int)system_instance_name_len, pSystem_instance_name);
        return PRP_OK;
    }

    FECS_SystemId system_id;
    DT_char *pSystem_name =
        DT_ByteBffrGetUnchecked(pResolve_data->pIdentifier_bffr,
                                pSystem_instance_decl->system_name.ofs);
    DT_size system_name_len = pSystem_instance_decl->system_name.size;
    if (!DT_StrArrSearchUnchecked(g_ctx->pSystem_names, pSystem_name,
                                  system_name_len, &system_id)) {
        DIAG_LOG_INFO(
            DIAG_LOG_CODE_INVALID_STATE,
            "System Instance: %.*s, contains unregistered system function "
            "name: %.*s, the entire system instance declaration will be "
            "skipped.",
            (int)system_instance_name_len, pSystem_instance_name,
            (int)system_name_len, pSystem_name);
        return PRP_OK;
    }
    DT_Bitmap *pInc_comp_set, *pExc_comp_set;
    PRP_Result code = CreateSystemInstanceCompSets(
        pSystem_instance_name, system_instance_name_len, pSystem_instance_decl,
        pResolve_data->pIdentifier_bffr, &pInc_comp_set, &pExc_comp_set);
    if (code == PRP_ERR_NOT_FOUND) {
        return PRP_OK;
    } else if (code != PRP_OK) {
        return code;
    }

    FECS_SystemInstanceCreateInfo system_instance_create_info = {
        .system_id = system_id,
        .layout_id_match_count = 0,
        .pLayout_id_matches = DT_null};
    code = PRP_OK; // Never hurts to be explicit.
    if (DT_BitmapHasAnyUnchecked(pExc_comp_set, pInc_comp_set)) {
        DIAG_LOG_WARN(DIAG_LOG_CODE_INIT_FAIL,
                      "System Instance: %.*s, has overlapping include and "
                      "exclude components, this will not match any layouts.",
                      system_instance_name_len, pSystem_instance_name);
    } else if (pResolve_data->pCreate_info->layout_count != 0) {
        code = FilterLayouts(pResolve_data, pInc_comp_set, pExc_comp_set,
                             &system_instance_create_info);
    }
    DT_BitmapDeleteUnchecked(&pInc_comp_set);
    DT_BitmapDeleteUnchecked(&pExc_comp_set);
    if (code != PRP_OK) {
        return code;
    }

    code = DT_StrArrPushUnchecked(
        pResolve_data->pCreate_info->pSystem_instance_names,
        pSystem_instance_name, system_instance_name_len);
    if (code != PRP_OK) {
        free(system_instance_create_info.pLayout_id_matches);
        return code;
    }
    FECS_SystemInstanceCreateInfo *pSystem_instance_create_infos =
        pResolve_data->pCreate_info->pSystem_instance_create_infos;
    pSystem_instance_create_infos[pResolve_data->pCreate_info
                                      ->system_instance_count++] =
        system_instance_create_info;

    return PRP_OK;
}

PRP_Result ResolverResolveParseTables(const FECS_WCParseTable *pParse_table,
                                      FECS_WorldCreateInfo *pCreate_info) {
    PRP_Result code = CreateInfoInit(pParse_table, pCreate_info);
    if (code != PRP_OK) {
        return code;
    }

    DeclResolveData resolve_data = {.pIdentifier_bffr =
                                        pParse_table->pIdentifiers_bffr,
                                    .pCreate_info = pCreate_info};
    code = DT_ArrForEachUnchecked(pParse_table->pLayout_table,
                                  ResolveLayoutDecl, &resolve_data);
    if (code != PRP_OK) {
        DestroyCreateInfo(pCreate_info);
        return code;
    }
    code = DT_ArrForEachUnchecked(pParse_table->pSystem_instance_table,
                                  ResolveSystemInstanceDecl, &resolve_data);
    if (code != PRP_OK) {
        DestroyCreateInfo(pCreate_info);
        return code;
    }

    return PRP_OK;
}
