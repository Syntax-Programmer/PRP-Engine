#include "Assert.h"
#include "Core/Logging/Log.h"

PRP_API void PRP_CALL
PRP_DiagHandleFailure(const PRP_DiagFailureInfo *pFailure_info) {
    if (!pFailure_info) {
        return;
    }

    const PRP_Char8 *initial_str;
    switch (pFailure_info->type) {
    case PRP_DIAG_FAILURE_TYPE_VERIFY:
        initial_str = "VERIFY FAILED:";
        break;
    case PRP_DIAG_FAILURE_TYPE_PANIC:
        initial_str = "ENGINE PANIC:";
        break;
    case PRP_DIAG_FAILURE_TYPE_ASSERTION:
        initial_str = "ASSERTION FAILED:";
        break;
    case PRP_DIAG_FAILURE_TYPE_UNREACHABLE:
        initial_str = "UNREACHABLE REACHED:";
        break;
    case PRP_DIAG_FAILURE_TYPE_UNIMPLEMENTED:
        initial_str = "UNIMPLEMENTED AREA REACHED:";
        break;
    default:
        initial_str = "INVALID FAILURE CODE:";
        break;
    }

    PRP_LogFormatF(stderr, "%s [File: %s][Line: %d][Func: %s] |%s|: %s\n",
                   initial_str, pFailure_info->pFile, pFailure_info->line,
                   pFailure_info->pFunc, pFailure_info->pExpr,
                   pFailure_info->pMsg);
}
