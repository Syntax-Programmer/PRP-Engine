#include "ForgeECS/Internals/World-Compiler/Compiler-Internals.h"

/**
 * Initializes the tok stream to accomodate for lexing the file.
 *
 * @param pTok_stream The token stream to initalize.
 * @param file        The file pointer to tokenize.
 * @param file_size   The size of the file buffer.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result TokStreamInit(FECS_WCTokStream *pTok_stream, DT_file file,
                                DT_size file_size);

/**
 * Checks if a character can be a valid start for an identifier/keyword.
 *
 * @param start_char The character to check.
 *
 * @return DT_true if valid, otherwise DT_false.
 */
static inline DT_bool IsIdentifierValidStart(DT_char start_char);
/**
 * Checks if a character can be a valid character for an identifier/keyword.
 *
 * @param tok_char The character to check.
 *
 * @return DT_true if valid, otherwise DT_false.
 */
static inline DT_bool IsIdentifierValid(DT_char tok_char);
/**
 * Checks if a character can be a valid delimiter after an identifier/keyword.
 *
 * @param last_char The character to check.
 *
 * @return DT_true if valid, otherwise DT_false.
 */
static inline DT_bool IsIdentifierValidDelim(DT_char last_char);

/**
 * Helper function to tokenize identifiers or keywords.
 *
 * @param pSrc_bffr     The src buffer to tokenize.
 * @param src_bffr_size The size of the src buffer.
 * @param pIdx          The file pointer index to be updated.
 * @param pTok_stream   The tok stream to store the tokens into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_PARSE if the file contains an identifier that contains
 *                       invalid character following it.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result TokenizeMultiCharTok(const DT_char *pSrc_bffr,
                                       DT_size src_bffr_size, DT_size *pIdx,
                                       FECS_WCTokStream *pTok_stream);
/**
 * Tokenizes the entire src bffr.
 *
 * @param pTok_stream The tok stream to store the tokens into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_PARSE if the file contains an identifier that contains
 *                       invalid character following it.
 * @return PRP_ERR_PARSE if the file contains an invalid character that doesn't
 *                       being an identifier or is not one of: ' ', '\t', '\n',
 *                       '\r'.
 * @return PRP_ERR_RES_EXHAUSTED if max cap is reached.
 * @return PRP_ERR_OOM if allocation fails.
 */
static PRP_Result TokenizeSrcBffr(FECS_WCTokStream *pTok_stream);

static PRP_Result TokStreamInit(FECS_WCTokStream *pTok_stream, DT_file file,
                                DT_size file_size) {
    PRP_Result code = DT_ArrCreateUnchecked(
        sizeof(FECS_WCTokType), DT_ARR_DEFAULT_CAP, &pTok_stream->pTypes);
    if (code != PRP_OK) {
        return code;
    }
    code =
        DT_ArrCreateUnchecked(sizeof(FECS_WCIdentifierTok), DT_ARR_DEFAULT_CAP,
                              &pTok_stream->pIdentifiers);
    if (code != PRP_OK) {
        DT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        return code;
    }
    code = DT_ArrCreateUnchecked(sizeof(DT_size), DT_ARR_DEFAULT_CAP,
                                 &pTok_stream->pRbrace_idxs);
    if (code != PRP_OK) {
        DT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        DT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
        return code;
    }
    code = DT_ByteBffrCreateUnchecked(file_size, &pTok_stream->pSrc_bffr);
    if (code != PRP_OK) {
        DT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        DT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
        DT_ArrDeleteUnchecked(&pTok_stream->pRbrace_idxs);
        return code;
    }
    DT_char *pStart = DT_ByteBffrGetUnchecked(pTok_stream->pSrc_bffr, 0);
    if (file_size != fread(pStart, 1, file_size, file)) {
        DT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        DT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
        DT_ArrDeleteUnchecked(&pTok_stream->pRbrace_idxs);
        DT_ByteBffrDeleteUnchecked(&pTok_stream->pSrc_bffr);
        code = PRP_ERR_IO;
        return code;
    }
    pTok_stream->total_identifier_size = 0;

    return PRP_OK;
}

static inline DT_bool IsIdentifierValidStart(DT_char start_char) {
    return (start_char >= 'a' && start_char <= 'z') ||
           (start_char >= 'A' && start_char <= 'Z') || (start_char == '_');
}

static inline DT_bool IsIdentifierValid(DT_char tok_char) {
    return (tok_char >= 'a' && tok_char <= 'z') ||
           (tok_char >= 'A' && tok_char <= 'Z') ||
           (tok_char >= '0' && tok_char <= '9') || (tok_char == '_');
}

static inline DT_bool IsIdentifierValidDelim(DT_char last_char) {
    return (last_char == ' ' || last_char == '\t' || last_char == '\r' ||
            last_char == '\n' || last_char == '\0' || last_char == '{' ||
            last_char == '}' || last_char == ':' || last_char == ';');
}

static PRP_Result TokenizeMultiCharTok(const DT_char *pSrc_bffr,
                                       DT_size src_bffr_size, DT_size *pIdx,
                                       FECS_WCTokStream *pTok_stream) {
    // Validity of start is already verified.
    DT_size start_idx = *pIdx;
    DT_size size = 1;
    while ((++start_idx) < src_bffr_size &&
           IsIdentifierValid(pSrc_bffr[start_idx])) {
        size++;
    }
    if (start_idx < src_bffr_size &&
        !IsIdentifierValidDelim(pSrc_bffr[start_idx])) {
        return PRP_ERR_PARSE;
    }

    const DT_char *pIdentifier = &pSrc_bffr[*pIdx];
    FECS_WCTokType type = WC_TOK_IDENTIFIER;
    if (size == WC_SYSTEM_TOK_STRLEN &&
        memcmp(pIdentifier, WC_SYSTEM_TOK_STR, size) == 0) {
        type = WC_TOK_SYSTEM;
    } else if (size == WC_INC_TOK_STRLEN &&
               memcmp(pIdentifier, WC_INC_TOK_STR, size) == 0) {
        type = WC_TOK_INC;
    } else if (size == WC_EXC_TOK_STRLEN &&
               memcmp(pIdentifier, WC_EXC_TOK_STR, size) == 0) {
        type = WC_TOK_EXC;
    } else if (size == WC_LAYOUT_TOK_STRLEN &&
               memcmp(pIdentifier, WC_LAYOUT_TOK_STR, size) == 0) {
        type = WC_TOK_LAYOUT;
    } else if (size == WC_SYSTEM_INSTANCE_TOK_STRLEN &&
               memcmp(pIdentifier, WC_SYSTEM_INSTANCE_TOK_STR, size) == 0) {
        type = WC_TOK_SYSTEM_INSTANCE;
    }

    PRP_Result code = DT_ArrPushUnchecked(pTok_stream->pTypes, &type);
    if (code != PRP_OK) {
        return code;
    }
    if (type == WC_TOK_IDENTIFIER) {
        FECS_WCIdentifierTok identifier_tok = {.ofs = *pIdx, .size = size};
        code = DT_ArrPushUnchecked(pTok_stream->pIdentifiers, &identifier_tok);
        if (code != PRP_OK) {
            return code;
        }
        pTok_stream->total_identifier_size += size;
    }
    /*
     * We subtract to move back a character so that the TokenizeSrcBuffer can
     * behave and correctly parse the next character say if it is a valid token.
     */
    *pIdx = start_idx - 1;

    return PRP_OK;
}

static PRP_Result TokenizeSrcBffr(FECS_WCTokStream *pTok_stream) {
    DT_size src_bffr_size;
    const DT_char *pSrc_bffr =
        DT_ByteBffrRawUnchecked(pTok_stream->pSrc_bffr, &src_bffr_size);
    FECS_WCTokType type;

    for (DT_size i = 0; i < src_bffr_size;) {
        DT_char curr = pSrc_bffr[i];
        DT_bool append = DT_true;
        switch (curr) {
        case ('{'):
            type = WC_TOK_LBRACE;
            break;
        case ('}'):
            type = WC_TOK_RBRACE;
            break;
        case (':'):
            type = WC_TOK_COLON;
            break;
        case (';'):
            type = WC_TOK_SEMICOLON;
            break;
        default:
            if (IsIdentifierValidStart(curr)) {
                PRP_Result code = TokenizeMultiCharTok(pSrc_bffr, src_bffr_size,
                                                       &i, pTok_stream);
                if (code != PRP_OK) {
                    return code;
                }
            } else if (!(curr == ' ' || curr == '\t' || curr == '\r' ||
                         curr == '\n')) {
                return PRP_ERR_PARSE;
            }
            append = DT_false;
            break;
        }
        if (append) {
            PRP_Result code = DT_ArrPushUnchecked(pTok_stream->pTypes, &type);
            if (code != PRP_OK) {
                return code;
            }
            if (type == WC_TOK_RBRACE) {
                DT_size idx = DT_ArrLen(pTok_stream->pTypes) - 1;
                code = DT_ArrPushUnchecked(pTok_stream->pRbrace_idxs, &idx);
                if (code != PRP_OK) {
                    return code;
                }
            }
        }
        i++;
    }

    return PRP_OK;
}

PRP_Result LexerTokenizeFile(const DT_char *pFile_path,
                             FECS_WCTokStream *pTok_stream) {
    DT_file file = fopen(pFile_path, "rb");
    if (!file) {
        return PRP_ERR_IO;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return PRP_ERR_IO;
    }
    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return PRP_ERR_IO;
    }
    DT_size file_size = (DT_size)size;
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return PRP_ERR_IO;
    }

    PRP_Result code = TokStreamInit(pTok_stream, file, file_size);
    fclose(file);
    if (code != PRP_OK) {
        return code;
    }

    code = TokenizeSrcBffr(pTok_stream);
    if (code != PRP_OK) {
        LexerTokStreamDelete(pTok_stream);
        return code;
    }

    return PRP_OK;
}

DT_void LexerTokStreamDelete(FECS_WCTokStream *pTok_stream) {
    DT_ArrDeleteUnchecked(&pTok_stream->pTypes);
    DT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
    DT_ArrDeleteUnchecked(&pTok_stream->pRbrace_idxs);
    DT_ByteBffrDeleteUnchecked(&pTok_stream->pSrc_bffr);
}
