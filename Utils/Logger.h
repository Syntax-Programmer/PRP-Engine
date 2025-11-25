#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Defs.h"

/* ----  GENERIC LOGGER  ---- */

PRP_FN_API PRP_void PRP_FN_CALL PRP_Log(const PRP_char *file,
                                        const PRP_char *func, PRP_u32 line,
                                        const PRP_char *fmt, ...);

#define PRP_LOG(fmt, ...)                                                      \
  PRP_Log(__FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/* ----  CODE LOGGER  ---- */

PRP_FN_API PRP_void PRP_FN_CALL PRP_LogFnCode(PRP_FnCode code,
                                              const PRP_char *file,
                                              const PRP_char *func,
                                              PRP_u32 line, const PRP_char *fmt,
                                              ...);

#define PRP_LOG_FN_CODE(code, fmt, ...)                                        \
  PRP_LogFnCode(code, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#define PRP_LOG_FN_NULL_ERROR(var)                                             \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Unexpected PRP_null value: '%s' encountered.", #var)

#define PRP_LOG_FN_MALLOC_ERROR(var)                                           \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Failed to allocate mem for the var: '%s'.", #var)

#define PRP_LOG_FN_UAF_ERROR(var)                                              \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Tried using an already freed var: '%s'.", #var)

#define PRP_LOG_FN_INV_ARG_ERROR(var)                                          \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Invalid/Corrupted function argument: '%s' encountered.",    \
                  #var)

#ifdef __cplusplus
}
#endif