#include "ForgeECS/Internals/World-Compiler/Compiler-Internals.h"

#define TOKS_PER_FIELD (2)
#define EMPTY_DECL_TOK_COUNT (4)

typedef struct {
    const FECS_WCTokType *pTypes;
    DT_size types_len;
    DT_size types_idx;

    const FECS_WCIdentifierTok *pIdentifiers;
    DT_size identifiers_len;
    DT_size identifiers_idx;

    const DT_size *pRbrace_idxs;
    DT_size rbrace_len;
    DT_size rbrace_idx;

    DT_size identifier_bffr_ofs;

    const DT_char *pSrc_bffr;
    DT_size src_bffr_size;
} ParserState;

/**
 * Initializes the parse tables for parsing.
 *
 * @param pParse_table The parse table to initialize.
 * @param pTok_stream  The tok stream to initialze the parse table with.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ParseTableInit(FECS_WCParseTable *pParse_table,
                                 const FECS_WCTokStream *pTok_stream);

/**
 * Registers a new identifier into the parse table.
 *
 * @param pParser_state The parsing state that defines current state of parsing.
 *                      The internals will be updated to match new state.
 * @param pParse_table  The parse table to register identifier into.
 *
 * @return The identifier token metadata of the identifier.
 */
static FECS_WCIdentifierTok RegisterIdentifier(ParserState *pParser_state,
                                               FECS_WCParseTable *pParse_table);

/**
 * Performs common initial validity check of a layout/system-instance decl.
 *
 * @param pParser_state       The current state of the parsing stage. The
 *                            internals will be updated to match new state.
 * @param pTok_count_to_parse Output pointer to number of tokens to parse ahead
 *                            of the newly updated state.
 *
 * @return DT_true if initial checks pass, otherwise DT_false.
 */
static DT_bool DeclIsValidInitCheck(ParserState *pParser_state,
                                    DT_size *pTok_count_to_parse);

/**
 * Used to free layout decl internals.
 * Called via DT_ArrForEach_...
 *
 * @param pVal The layout decl to delete.
 *
 * @return PRP_OK on success.
 */
static PRP_Result LayoutDelCb(DT_void *pVal, DT_void *_);
/**
 * Parses a set of component to check if they match layout decl syntax.
 *
 * @param pParser_state The current state of the parsing stage. The internals
 *                      will be updated to match new state.
 * @param pParse_table  The parsing table to add the layout decl if we can match
 *                      and validate it.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_PARSE if initial decl validity check fails.
 * @return PRP_ERR_PARSE if the layout decl contains no components.
 * @return PRP_ERR_PARSE if component decl doesn't match syntax.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ParseLayoutDecl(ParserState *pParser_state,
                                  FECS_WCParseTable *pParse_table);

/**
 * Used to free system instance decl internals.
 * Called via DT_ArrForEach_...
 *
 * @param pVal The system instance decl to delete.
 *
 * @return PRP_OK on success.
 */
static PRP_Result SystemInstanceDelCb(DT_void *pVal, DT_void *_);
/**
 * Parses a set of component to check if they match system instance decl syntax.
 *
 * @param pParser_state The current state of the parsing stage. The internals
 *                      will be updated to match new state.
 * @param pParse_table  The parsing table to add the system instance decl if we
 *                      can match and validate it.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_PARSE if initial decl validity check fails.
 * @return PRP_ERR_PARSE if the system instance decl contains no sub decls.
 * @return PRP_ERR_PARSE if no system func name is defined.
 * @return PRP_ERR_PARSE if invalid decl inside system instance exist.
 * @return PRP_ERR_PARSE if not all sub decls exist, where system func and inc
 *                       comps must not be empty.
 * @return PRP_ERR_PARSE if component decl doesn't match syntax.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result ParseSystemInstanceDecl(ParserState *pParser_state,
                                          FECS_WCParseTable *pParse_table);

static PRP_Result ParseTableInit(FECS_WCParseTable *pParse_table,
                                 const FECS_WCTokStream *pTok_stream) {
    PRP_Result code =
        DT_ArrCreateUnchecked(sizeof(FECS_WCLayoutDecl), DT_ARR_DEFAULT_CAP,
                              &pParse_table->pLayout_table);
    if (code != PRP_OK) {
        return code;
    }
    code = DT_ArrCreateUnchecked(sizeof(FECS_WCSystemInstanceDecl),
                                 DT_ARR_DEFAULT_CAP,
                                 &pParse_table->pSystem_instance_table);
    if (code != PRP_OK) {
        DT_ArrDeleteUnchecked(&pParse_table->pLayout_table);
        return code;
    }
    code = DT_ByteBffrCreateUnchecked(pTok_stream->total_identifier_size,
                                      &pParse_table->pIdentifiers_bffr);
    if (code != PRP_OK) {
        DT_ArrDeleteUnchecked(&pParse_table->pLayout_table);
        DT_ArrDeleteUnchecked(&pParse_table->pSystem_instance_table);
        return code;
    }
    pParse_table->layout_names_size = 0;
    pParse_table->system_instance_names_size = 0;

    return PRP_OK;
}

static FECS_WCIdentifierTok
RegisterIdentifier(ParserState *pParser_state,
                   FECS_WCParseTable *pParse_table) {
    DIAG_ASSERT(pParser_state->identifiers_idx <
                pParser_state->identifiers_len);

    FECS_WCIdentifierTok old_identifier =
        pParser_state->pIdentifiers[pParser_state->identifiers_idx++];

    DT_ByteBffrUploadUnchecked(
        pParse_table->pIdentifiers_bffr, pParser_state->identifier_bffr_ofs,
        old_identifier.size,
        (DT_void *)(pParser_state->pSrc_bffr + old_identifier.ofs));

    old_identifier.ofs = pParser_state->identifier_bffr_ofs;
    pParser_state->identifier_bffr_ofs += old_identifier.size;

    return old_identifier;
}

static DT_bool DeclIsValidInitCheck(ParserState *pParser_state,
                                    DT_size *pTok_count_to_parse) {
    if (pParser_state->rbrace_idx == pParser_state->rbrace_len) {
        // Decl started, but not closed.
        return DT_false;
    }

    DT_size rbrace_types_idx =
        pParser_state->pRbrace_idxs[pParser_state->rbrace_idx++];
    DT_size toks_this_decl = rbrace_types_idx - pParser_state->types_idx + 1;
    if (toks_this_decl < EMPTY_DECL_TOK_COUNT) {
        // Not enough toks for even an empty decl.
        return DT_false;
    }
    DT_size remaining_toks = toks_this_decl - EMPTY_DECL_TOK_COUNT;
    if (remaining_toks % TOKS_PER_FIELD != 0) {
        // Extra/missing tokens exist.
        return DT_false;
    }
    DT_size type_idx = pParser_state->types_idx;
    const FECS_WCTokType *pTypes = pParser_state->pTypes;
    if (pTypes[++type_idx] != WC_TOK_IDENTIFIER ||
        pTypes[++type_idx] != WC_TOK_LBRACE) {
        // Decl syntax missing.
        return DT_false;
    }
    pParser_state->types_idx = ++type_idx;
    *pTok_count_to_parse = remaining_toks;

    return DT_true;
}

static PRP_Result LayoutDelCb(DT_void *pVal, DT_void *_) {
    FECS_WCLayoutDecl *pLayout_decl = pVal;

    DT_ArrDeleteUnchecked(&pLayout_decl->pComp_names);

    return PRP_OK;
}

static PRP_Result ParseLayoutDecl(ParserState *pParser_state,
                                  FECS_WCParseTable *pParse_table) {
    DT_size toks_to_parse = 0;
    if (!DeclIsValidInitCheck(pParser_state, &toks_to_parse) ||
        toks_to_parse == 0) {
        // Extra check to reject empty decl.
        return PRP_ERR_PARSE;
    }

    FECS_WCLayoutDecl layout_decl = {0};
    PRP_Result code =
        DT_ArrCreateUnchecked(sizeof(FECS_WCIdentifierTok), DT_ARR_DEFAULT_CAP,
                              &layout_decl.pComp_names);
    if (code != PRP_OK) {
        return code;
    }

    layout_decl.layout_name = RegisterIdentifier(pParser_state, pParse_table);
    pParse_table->layout_names_size += layout_decl.layout_name.size;

    FECS_WCTokType pMatch[TOKS_PER_FIELD] = {WC_TOK_IDENTIFIER,
                                             WC_TOK_SEMICOLON};
    for (DT_size i = 0; i < toks_to_parse;
         i += TOKS_PER_FIELD, pParser_state->types_idx += TOKS_PER_FIELD) {
        if (memcmp(&pParser_state->pTypes[pParser_state->types_idx], pMatch,
                   sizeof(FECS_WCTokType) * TOKS_PER_FIELD) != 0) {
            code = PRP_ERR_PARSE;
            goto err_path;
        }
        FECS_WCIdentifierTok comp_name =
            RegisterIdentifier(pParser_state, pParse_table);
        code = DT_ArrPushUnchecked(layout_decl.pComp_names, &comp_name);
        if (code != PRP_OK) {
            goto err_path;
        }
    }
    DT_ArrShrinkFitUnchecked(layout_decl.pComp_names);
    code = DT_ArrPushUnchecked(pParse_table->pLayout_table, &layout_decl);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    DT_ArrDeleteUnchecked(&layout_decl.pComp_names);

    return code;
}

static PRP_Result SystemInstanceDelCb(DT_void *pVal, DT_void *_) {
    FECS_WCSystemInstanceDecl *pSystem_instance_decl = pVal;

    DT_ArrDeleteUnchecked(&pSystem_instance_decl->pInc_comp_names);
    DT_ArrDeleteUnchecked(&pSystem_instance_decl->pExc_comp_names);

    return PRP_OK;
}

static PRP_Result ParseSystemInstanceDecl(ParserState *pParser_state,
                                          FECS_WCParseTable *pParse_table) {
    DT_size toks_to_parse;
    if (!DeclIsValidInitCheck(pParser_state, &toks_to_parse) ||
        toks_to_parse == 0) {
        // Extra check to reject empty decl.
        return PRP_ERR_PARSE;
    }

    FECS_WCSystemInstanceDecl system_instance_decl = {0};
    PRP_Result code =
        DT_ArrCreateUnchecked(sizeof(FECS_WCIdentifierTok), DT_ARR_DEFAULT_CAP,
                              &system_instance_decl.pInc_comp_names);
    if (code != PRP_OK) {
        return code;
    }
    code =
        DT_ArrCreateUnchecked(sizeof(FECS_WCIdentifierTok), DT_ARR_DEFAULT_CAP,
                              &system_instance_decl.pExc_comp_names);
    if (code != PRP_OK) {
        goto err_path;
    }

    system_instance_decl.system_instance_name =
        RegisterIdentifier(pParser_state, pParse_table);
    pParse_table->system_instance_names_size +=
        system_instance_decl.system_instance_name.size;

    DT_Arr *pAttached_arr = DT_null;
    DT_bool found_inc = DT_false, found_exc = DT_false, found_system = DT_false;
    for (DT_size i = 0; i < toks_to_parse;
         i += TOKS_PER_FIELD, pParser_state->types_idx += TOKS_PER_FIELD) {
        FECS_WCTokType curr_tok =
            pParser_state->pTypes[pParser_state->types_idx];
        FECS_WCTokType next_tok =
            pParser_state->pTypes[pParser_state->types_idx + 1];

        if (curr_tok == WC_TOK_INC && next_tok == WC_TOK_COLON && !found_inc) {
            found_inc = DT_true;
            pAttached_arr = system_instance_decl.pInc_comp_names;
        } else if (curr_tok == WC_TOK_EXC && next_tok == WC_TOK_COLON &&
                   !found_exc) {
            found_exc = DT_true;
            pAttached_arr = system_instance_decl.pExc_comp_names;
        } else if (curr_tok == WC_TOK_SYSTEM && next_tok == WC_TOK_COLON &&
                   !found_system) {
            pParser_state->types_idx += TOKS_PER_FIELD;
            i += TOKS_PER_FIELD;
            curr_tok = pParser_state->pTypes[pParser_state->types_idx];
            next_tok = pParser_state->pTypes[pParser_state->types_idx + 1];
            if (curr_tok != WC_TOK_IDENTIFIER || next_tok != WC_TOK_SEMICOLON) {
                // No function name defined.
                code = PRP_ERR_PARSE;
                goto err_path;
            }
            system_instance_decl.system_name =
                RegisterIdentifier(pParser_state, pParse_table);
            found_system = DT_true;
            pAttached_arr = DT_null;
        } else if (curr_tok == WC_TOK_IDENTIFIER &&
                   next_tok == WC_TOK_SEMICOLON && pAttached_arr) {
            FECS_WCIdentifierTok comp_name =
                RegisterIdentifier(pParser_state, pParse_table);
            code = DT_ArrPushUnchecked(pAttached_arr, &comp_name);
            if (code != PRP_OK) {
                goto err_path;
            }
        } else {
            code = PRP_ERR_PARSE;
            goto err_path;
        }
    }
    if (!found_inc || !found_exc || !found_system ||
        !DT_ArrLen(system_instance_decl.pInc_comp_names)) {
        // All sub decls must exist and inc can't be empty sub decl.
        code = PRP_ERR_PARSE;
        goto err_path;
    }
    DT_ArrShrinkFitUnchecked(system_instance_decl.pInc_comp_names);
    DT_ArrShrinkFitUnchecked(system_instance_decl.pExc_comp_names);
    code = DT_ArrPushUnchecked(pParse_table->pSystem_instance_table,
                               &system_instance_decl);
    if (code != PRP_OK) {
        goto err_path;
    }

    return PRP_OK;

err_path:
    if (system_instance_decl.pInc_comp_names) {
        DT_ArrDeleteUnchecked(&system_instance_decl.pInc_comp_names);
    }
    if (system_instance_decl.pExc_comp_names) {
        DT_ArrDeleteUnchecked(&system_instance_decl.pExc_comp_names);
    }

    return code;
}

PRP_Result ParserParseTokStream(const FECS_WCTokStream *pTok_stream,
                                FECS_WCParseTable *pParse_table) {
    PRP_Result code = ParseTableInit(pParse_table, pTok_stream);
    if (code != PRP_OK) {
        return code;
    }

    ParserState parser_state = {0};
    parser_state.pTypes =
        DT_ArrRawUnchecked(pTok_stream->pTypes, &parser_state.types_len);
    parser_state.pIdentifiers = DT_ArrRawUnchecked(
        pTok_stream->pIdentifiers, &parser_state.identifiers_len);
    parser_state.pRbrace_idxs =
        DT_ArrRawUnchecked(pTok_stream->pRbrace_idxs, &parser_state.rbrace_len);
    parser_state.pSrc_bffr = DT_ByteBffrGetUnchecked(pTok_stream->pSrc_bffr, 0);
    parser_state.src_bffr_size = DT_ByteBffrSize(pTok_stream->pSrc_bffr);

    for (; parser_state.types_idx < parser_state.types_len;
         parser_state.types_idx++) {
        FECS_WCTokType type = parser_state.pTypes[parser_state.types_idx];
        switch (type) {
        case WC_TOK_LAYOUT:
            code = ParseLayoutDecl(&parser_state, pParse_table);
            break;
        case WC_TOK_SYSTEM_INSTANCE:
            code = ParseSystemInstanceDecl(&parser_state, pParse_table);
            break;
        default:
            code = PRP_ERR_PARSE;
            break;
        }
        if (code != PRP_OK) {
            ParserParseTableDelete(pParse_table);
            return code;
        }
    }

    return PRP_OK;
}

DT_void ParserParseTableDelete(FECS_WCParseTable *pParse_table) {
    DT_ArrForEachUnchecked(pParse_table->pLayout_table, LayoutDelCb, DT_null);
    DT_ArrDeleteUnchecked(&pParse_table->pLayout_table);

    DT_ArrForEachUnchecked(pParse_table->pSystem_instance_table,
                           SystemInstanceDelCb, DT_null);
    DT_ArrDeleteUnchecked(&pParse_table->pSystem_instance_table);

    DT_ByteBffrDeleteUnchecked(&pParse_table->pIdentifiers_bffr);
}
