#include "Assert.h"
#include "Log.h"

PRP_API void PRP_CALL DIAG_FailureLog(const DIAG_FailureInfo *pFail_info) {
    if (!pFail_info) {
        return;
    }

    const PRP_char8 *init_str;
    switch (pFail_info->type) {
    case DIAG_FAILURE_TYPE_VERIFY:
        init_str = "VERIFY FAILED";
        break;
    case DIAG_FAILURE_TYPE_PANIC:
        init_str = "ENGINE PANIC";
        break;
    case DIAG_FAILURE_TYPE_ASSERTION:
        init_str = "ASSERTION FAILED";
        break;
    case DIAG_FAILURE_TYPE_UNREACHABLE:
        init_str = "UNREACHABLE REACHED";
        break;
    case DIAG_FAILURE_TYPE_UNIMPLEMENTED:
        init_str = "UNIMPLEMENTED CODE REACHED";
        break;
    default:
        init_str = "NO FAILURE CODE";
        break;
    }

    DIAG_Log(DIAG_FAILURE_DEFAULT_LOG_FILE, DIAG_LOG_LVL_ERROR,
             pFail_info->pFile, pFail_info->pFunc, pFail_info->line,
             "%s: (%s) : %s", init_str, pFail_info->pExpr, pFail_info->pMsg);
}
