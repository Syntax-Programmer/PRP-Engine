#include "ForgeECS/Internals/World-Compiler/Compiler-Internals.h"

PRP_Result CompilerCompile(const DT_char *pFile_path,
                           FECS_WorldCreateInfo *pCreate_info) {
    FECS_WCTokStream tok_stream;
    PRP_Result code = LexerTokenizeFile(pFile_path, &tok_stream);
    if (code != PRP_OK) {
        return code;
    }

    FECS_WCParseTable parse_table;
    code = ParserParseTokStream(&tok_stream, &parse_table);
    LexerTokStreamDelete(&tok_stream);
    if (code != PRP_OK) {
        return code;
    }

    code = ResolverResolveParseTables(&parse_table, pCreate_info);
    ParserParseTableDelete(&parse_table);

    return code;
}
