#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Arr.h"
#include "DataTypes/ByteBffr.h"
#include "Forge/Internals/FECS-World/World-Internals.h"

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

/**
 * Tokenizes a given file into the tok stream.
 *
 * @param pFile_path  The file path to tokenize.
 * @param pTok_stream Output pointer of the tok stream of the file.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO on file opening/indexing errors.
 * @return PRP_ERR_PARSE if the file contains an identifier that contains
 *                       invalid character following it.
 * @return PRP_ERR_PARSE if the file contains an invalid character that doesn't
 *                       being an identifier or is not one of: ' ', '\t', '\n',
 *                       '\r'.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result LexerTokenizeFile(const DT_char *pFile_path,
                             FECS_WCTokStream *pTok_stream);
/**
 * Deletes a already created tok stream, and invalidates the internals.
 *
 * @param pTok_stream The tok stream to delete.
 */
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

/**
 * Parses the tok stream into parsing tables.
 *
 * @param pTok_stream  The tok stream to parse.
 * @param pParse_table Output pointer to stroe the parse tables of the tok
 *                     stream.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_PARSE if initial decl validity check fails.
 * @return PRP_ERR_PARSE if the layout decl contains no components.
 * @return PRP_ERR_PARSE if component decl doesn't match syntax.
 * @return PRP_ERR_PARSE if the system instance decl contains no sub decls.
 * @return PRP_ERR_PARSE if no system func name is defined.
 * @return PRP_ERR_PARSE if invalid decl inside system instance exist.
 * @return PRP_ERR_PARSE if not all sub decls exist, where system func and inc
 *                       comps must not be empty.
 * @return PRP_ERR_PARSE if some decl other than layout or system instance
 *                       exists.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result ParserParseTokStream(const FECS_WCTokStream *pTok_stream,
                                FECS_WCParseTable *pParse_table);
/**
 * Deletes a already created parse table, and invalidates the internals.
 *
 * @param pParse_table The parse table to delete.
 */
DT_void ParserParseTableDelete(FECS_WCParseTable *pParse_table);

/* ----  RESOLVER ---- */

/**
 * Resolve a parse table into world create info.
 *
 * @param pParse_table The parse table to resolve.
 * @param pCreate_info Output pointer to store the resolved create info.
 *
 * @return PRP_OK on full/partial success.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result ResolverResolveParseTables(const FECS_WCParseTable *pParse_table,
                                      FECS_WorldCreateInfo *pCreate_info);

/* ----  COMPILER ---- */

/**
 * Abstracts the entire compiler pipeline to a single function.
 *
 * @oaram pFile_path   The file to compile.
 * @param pCreate_info Output pointer to stored the compiled create info.
 *
 * @return PRP_OK on full/partial success.
 * @return PRP_ERR_IO on file opening/indexing errors.
 * @return PRP_ERR_PARSE if the file contains an identifier that contains
 *                       invalid character following it.
 * @return PRP_ERR_PARSE if the file contains an invalid character that doesn't
 *                       being an identifier or is not one of: ' ', '\t', '\n',
 *                       '\r'.
 * @return PRP_ERR_PARSE if initial decl validity check fails.
 * @return PRP_ERR_PARSE if the layout decl contains no components.
 * @return PRP_ERR_PARSE if component decl doesn't match syntax.
 * @return PRP_ERR_PARSE if the system instance decl contains no sub decls.
 * @return PRP_ERR_PARSE if no system func name is defined.
 * @return PRP_ERR_PARSE if invalid decl inside system instance exist.
 * @return PRP_ERR_PARSE if not all sub decls exist, where system func and inc
 *                       comps must not be empty.
 * @return PRP_ERR_PARSE if some decl other than layout or system instance
 *                       exists.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
PRP_Result CompilerCompile(const DT_char *pFile_path,
                           FECS_WorldCreateInfo *pCreate_info);

#ifdef __cplusplus
}
#endif
