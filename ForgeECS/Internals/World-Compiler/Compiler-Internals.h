#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/ByteBffr.h"
#include "ForgeECS/Internals/FECS-World/World-Internals.h"

/* ----  LEXER ---- */

typedef enum {
    // A name token
    WC_TOK_IDENTIFIER,

    // Decl start-enders
    WC_TOK_LBRACE,
    WC_TOK_RBRACE,

    // Punctuators
    WC_TOK_COLON,
    WC_TOK_SEMICOLON,

    // Sub-decl keywords
    WC_TOK_SYSTEM,
    WC_TOK_INC,
    WC_TOK_EXC,

    // Decl keywords
    WC_TOK_LAYOUT,
    WC_TOK_SYSTEM_INSTANCE,
} FECS_WCTokType;

typedef struct {
    DT_size ofs;
    DT_size size;
} FECS_WCIdentifierTok;

typedef struct {
    DT_Arr *pTypes;
    DT_Arr *pIdentifiers;
    DT_Arr *pRbrace_idxs;
    DT_size total_identifier_size;
    DT_ByteBffr *pSrc_bffr;
} FECS_WCTokStream;

#define WC_SYSTEM_TOK_STR "system"
#define WC_SYSTEM_TOK_STRLEN (sizeof(WC_SYSTEM_TOK_STR) - 1)

#define WC_INC_TOK_STR "inc"
#define WC_INC_TOK_STRLEN (sizeof(WC_INC_TOK_STR) - 1)

#define WC_EXC_TOK_STR "exc"
#define WC_EXC_TOK_STRLEN (sizeof(WC_EXC_TOK_STR) - 1)

#define WC_LAYOUT_TOK_STR "layout"
#define WC_LAYOUT_TOK_STRLEN (sizeof(WC_LAYOUT_TOK_STR) - 1)

#define WC_SYSTEM_INSTANCE_TOK_STR "system_instance"
#define WC_SYSTEM_INSTANCE_TOK_STRLEN (sizeof(WC_SYSTEM_INSTANCE_TOK_STR) - 1)

PRP_Result LexerTokenizeFile(const DT_char *pFile_path,
                             FECS_WCTokStream *pTok_stream);
DT_void LexerTokStreamDelete(FECS_WCTokStream *pTok_stream);

/* ----  PARSER ---- */

typedef struct {
    FECS_WCIdentifierTok layout_name;
    DT_Arr *pComp_names;
} FECS_WCLayoutDecl;

typedef struct {
    FECS_WCIdentifierTok system_instance_name;
    FECS_WCIdentifierTok system_name;
    DT_Arr *pInc_comp_names;
    DT_Arr *pExc_comp_names;
} FECS_WCSystemInstanceDecl;

typedef struct {
    DT_Arr *pLayout_table;
    DT_Arr *pSystem_instance_table;
    DT_size layout_names_size;
    DT_size system_instance_names_size;
    DT_ByteBffr *pIdentifiers_bffr;
} FECS_WCParseTable;

PRP_Result ParserParseTokStream(const FECS_WCTokStream *pTok_stream,
                                FECS_WCParseTable *pParse_table);
DT_void ParserParseTableDelete(FECS_WCParseTable *pParse_table);

/* ----  RESOLVER ---- */

PRP_Result ResolverResolveParseTables(const FECS_WCParseTable *pParse_table,
                                      FECS_WorldCreateInfo *pCreate_info);

/* ----  COMPILER ---- */

PRP_Result CompilerCompile(const DT_char *pFile_path,
                           FECS_WorldCreateInfo *pCreate_info);

#ifdef __cplusplus
}
#endif
