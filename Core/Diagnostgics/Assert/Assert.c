#include "Assert.h"
#include "Core/Logging/Log.h"

PRP_API void PRP_CALL
PRP_DiagHandleFailure(const PRP_DiagFailureInfo *pFailure_info) {
    if (!pFailure_info) {
        return;
    }

    /**
     * char x[200] = "some string %d %s";

     x will contain:

     s o m e   s t r i n g   % d   % s \0

     The %d and %s have no special meaning here because this is just a string
     literal being copied into a character array. They are not interpreted as
     format specifiers until you pass the string to something like printf.

     For example:

     char x[200] = "some string %d %s";

     printf("%s\n", x);
     */

    switch (pFailure_info->type) {
    case PRP_DIAG_FAILURE_TYPE_VERIFY:
        return;
    case PRP_DIAG_FAILURE_TYPE_PANIC:
        return;
    case PRP_DIAG_FAILURE_TYPE_ASSERTION:
        return;
    case PRP_DIAG_FAILURE_TYPE_UNREACHABLE:
        return;
    case PRP_DIAG_FAILURE_TYPE_UNIMPLEMENTED:
        return;
    default:
        return;
    }
}
