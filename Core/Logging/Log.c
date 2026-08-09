#include "Log.h"
#include <stdarg.h>

PRP_API PRP_Size PRP_CALL PRP_LogGetFinalFormatSize(const PRP_Char8 *pMsg, ...);
PRP_API void PRP_CALL PRP_LogFormat(PRP_Char8 *pDest, PRP_Size dest_size,
                                    const PRP_Char8 *pMsg, ...);
PRP_API void PRP_CALL PRP_LogFormatF(FILE *pDest, const PRP_Char8 *pMsg, ...);

PRP_API void PRP_CALL PRP_LogWrite(const PRP_Char8 *pMsg, PRP_Size len);
PRP_API void PRP_CALL PRP_LogWriteLn(const PRP_Char8 *pMsg, PRP_Size len);
PRP_API void PRP_CALL PRP_LogWriteChar(PRP_Char8 c);

PRP_API void PRP_CALL PRP_LogWriteF(const PRP_Char8 *pMsg, PRP_Size len,
                                    FILE *pFile);
PRP_API void PRP_CALL PRP_LogWriteLnF(const PRP_Char8 *pMsg, PRP_Size len,
                                      FILE *pFile);
PRP_API void PRP_CALL PRP_LogWriteCharF(PRP_Char8 c, FILE *pFile);
