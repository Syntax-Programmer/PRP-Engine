#include "Forge/Internals/World-Compiler/Compiler-Internals.h"

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
static PRP_Result TokStreamInit(FECS_WCTokStream *pTok_stream, FILE *file,
                                PRP_Size file_size);

/**
 * Checks if a character can be a valid start for an identifier/keyword.
 *
 * @param start_char The character to check.
 *
 * @return PRP_True if valid, otherwise PRP_False.
 */
static inline PRP_Bool IsIdentifierValidStart(PRP_Char8 start_char);
/**
 * Checks if a character can be a valid character for an identifier/keyword.
 *
 * @param tok_char The character to check.
 *
 * @return PRP_True if valid, otherwise PRP_False.
 */
static inline PRP_Bool IsIdentifierValid(PRP_Char8 tok_char);
/**
 * Checks if a character can be a valid delimiter after an identifier/keyword.
 *
 * @param last_char The character to check.
 *
 * @return PRP_True if valid, otherwise PRP_False.
 */
static inline PRP_Bool IsIdentifierValidDelim(PRP_Char8 last_char);

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
static PRP_Result TokenizeMultiCharTok(const PRP_Char8 *pSrc_bffr,
                                       PRP_Size src_bffr_size, PRP_Size *pIdx,
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

static PRP_Result TokStreamInit(FECS_WCTokStream *pTok_stream, FILE *file,
                                PRP_Size file_size) {
    PRP_Result code = CONT_ArrCreateUnchecked(
        sizeof(FECS_WCTokType), CONT_ARR_DEFAULT_CAP, &pTok_stream->pTypes);
    if (code != PRP_OK) {
        return code;
    }
    code =
        CONT_ArrCreateUnchecked(sizeof(FECS_WCIdentifierTok), CONT_ARR_DEFAULT_CAP,
                              &pTok_stream->pIdentifiers);
    if (code != PRP_OK) {
        CONT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        return code;
    }
    code = CONT_ArrCreateUnchecked(sizeof(PRP_Size), CONT_ARR_DEFAULT_CAP,
                                 &pTok_stream->pRbrace_idxs);
    if (code != PRP_OK) {
        CONT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        CONT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
        return code;
    }
    code = CONT_ByteBffrCreateUnchecked(file_size, &pTok_stream->pSrc_bffr);
    if (code != PRP_OK) {
        CONT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        CONT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
        CONT_ArrDeleteUnchecked(&pTok_stream->pRbrace_idxs);
        return code;
    }
    PRP_Char8 *pStart = CONT_ByteBffrGetUnchecked(pTok_stream->pSrc_bffr, 0);
    if (file_size != fread(pStart, 1, file_size, file)) {
        CONT_ArrDeleteUnchecked(&pTok_stream->pTypes);
        CONT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
        CONT_ArrDeleteUnchecked(&pTok_stream->pRbrace_idxs);
        CONT_ByteBffrDeleteUnchecked(&pTok_stream->pSrc_bffr);
        code = PRP_ERR_IO;
        return code;
    }
    pTok_stream->total_identifier_size = 0;

    return PRP_OK;
}

static inline PRP_Bool IsIdentifierValidStart(PRP_Char8 start_char) {
    return (start_char >= 'a' && start_char <= 'z') ||
           (start_char >= 'A' && start_char <= 'Z') || (start_char == '_');
}

static inline PRP_Bool IsIdentifierValid(PRP_Char8 tok_char) {
    return (tok_char >= 'a' && tok_char <= 'z') ||
           (tok_char >= 'A' && tok_char <= 'Z') ||
           (tok_char >= '0' && tok_char <= '9') || (tok_char == '_');
}

static inline PRP_Bool IsIdentifierValidDelim(PRP_Char8 last_char) {
    return (last_char == ' ' || last_char == '\t' || last_char == '\r' ||
            last_char == '\n' || last_char == '\0' || last_char == '{' ||
            last_char == '}' || last_char == ':' || last_char == ';');
}

static PRP_Result TokenizeMultiCharTok(const PRP_Char8 *pSrc_bffr,
                                       PRP_Size src_bffr_size, PRP_Size *pIdx,
                                       FECS_WCTokStream *pTok_stream) {
    // Validity of start is already verified.
    PRP_Size start_idx = *pIdx;
    PRP_Size size = 1;
    while ((++start_idx) < src_bffr_size &&
           IsIdentifierValid(pSrc_bffr[start_idx])) {
        size++;
    }
    if (start_idx < src_bffr_size &&
        !IsIdentifierValidDelim(pSrc_bffr[start_idx])) {
        return PRP_ERR_PARSE;
    }

    const PRP_Char8 *pIdentifier = &pSrc_bffr[*pIdx];
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

    PRP_Result code = CONT_ArrPushUnchecked(pTok_stream->pTypes, &type);
    if (code != PRP_OK) {
        return code;
    }
    if (type == WC_TOK_IDENTIFIER) {
        FECS_WCIdentifierTok identifier_tok = {.ofs = *pIdx, .size = size};
        code = CONT_ArrPushUnchecked(pTok_stream->pIdentifiers, &identifier_tok);
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
    PRP_Size src_bffr_size;
    const PRP_Char8 *pSrc_bffr =
        CONT_ByteBffrRawUnchecked(pTok_stream->pSrc_bffr, &src_bffr_size);
    FECS_WCTokType type;

    for (PRP_Size i = 0; i < src_bffr_size;) {
        PRP_Char8 curr = pSrc_bffr[i];
        PRP_Bool append = PRP_True;
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
            append = PRP_False;
            break;
        }
        if (append) {
            PRP_Result code = CONT_ArrPushUnchecked(pTok_stream->pTypes, &type);
            if (code != PRP_OK) {
                return code;
            }
            if (type == WC_TOK_RBRACE) {
                PRP_Size idx = CONT_ArrLen(pTok_stream->pTypes) - 1;
                code = CONT_ArrPushUnchecked(pTok_stream->pRbrace_idxs, &idx);
                if (code != PRP_OK) {
                    return code;
                }
            }
        }
        i++;
    }

    return PRP_OK;
}

PRP_Result LexerTokenizeFile(const PRP_Char8 *pFile_path,
                             FECS_WCTokStream *pTok_stream) {
    FILE *file = fopen(pFile_path, "rb");
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
    PRP_Size file_size = (PRP_Size)size;
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

void LexerTokStreamDelete(FECS_WCTokStream *pTok_stream) {
    CONT_ArrDeleteUnchecked(&pTok_stream->pTypes);
    CONT_ArrDeleteUnchecked(&pTok_stream->pIdentifiers);
    CONT_ArrDeleteUnchecked(&pTok_stream->pRbrace_idxs);
    CONT_ByteBffrDeleteUnchecked(&pTok_stream->pSrc_bffr);
}
