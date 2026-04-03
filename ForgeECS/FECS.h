#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Arr.h"
#include "Defs.h"

/* ----  COMPS ---- */

PRP_FN_API DT_size PRP_FN_CALL FECS_CompRegisterUnchecked(const DT_char *name,
                                                          DT_size size);
PRP_FN_API DT_size PRP_FN_CALL FECS_CompRegisterChecked(const DT_char *name,
                                                        DT_size size);
// This method will prove unusable givven current context i have.

// #define FECS_COMPONENT(name) \
//     typedef struct name name; \ struct name

// #if defined(_MSC_VER)
// #pragma section(".CRT$XCU", read)
// #define FECS_CONSTRUCTOR(func) \
//     static DT_void func(DT_void); \
//     static __declspec(allocate(".CRT$XCU")) DT_void (*func##_ptr)(DT_void) =
//     \
//         func; \
//     static DT_void func(DT_void)
// #elif defined(__GNUC__) || defined(__clang__)
// #define FECS_CONSTRUCTOR(func) \
//     static DT_void func(DT_void) __attribute__((constructor)); \ static
//     DT_void func(DT_void)
// #endif

// #define FECS_REGISTER_COMPONENT(name) \
//     FECS_CONSTRUCTOR(__fecs_register_##name) { \
//         FECS_CompRegisterChecked(name, sizeof(name)); \
//     }

/* ----  BEHAVIOR ---- */

PRP_FN_API DT_size PRP_FN_CALL
FECS_BehaviorRegisterUnchecked(DT_Arr *comp_idxs);
PRP_FN_API DT_size PRP_FN_CALL FECS_BehaviorRegisterChecked(DT_Arr *comp_idxs);

/* ----  QUERY ---- */

PRP_FN_API DT_size PRP_FN_CALL FECS_QueryRegisterUnchecked(DT_Arr *inc_comps,
                                                           DT_Arr *exc_comps);
PRP_FN_API DT_size PRP_FN_CALL FECS_QueryRegisterChecked(DT_Arr *inc_comps,
                                                         DT_Arr *exc_comps);

/* ----  SYSTEMS ---- */

PRP_FN_API DT_size PRP_FN_CALL FECS_SystemRegisterUnchecked(FECS_System system);
PRP_FN_API DT_size PRP_FN_CALL FECS_SystemRegisterChecked(FECS_System system);

/* ----  LAYOUTS ---- */

/* ----  WORLD ---- */

/* ----  FECS ---- */

PRP_FN_API PRP_Result PRP_FN_CALL FECS_GetLastErrCode(DT_void);

PRP_FN_API PRP_Result PRP_FN_CALL FECS_Init(DT_void);
PRP_FN_API PRP_Result PRP_FN_CALL FECS_Exit(DT_void);

#ifdef __cplusplus
}
#endif
