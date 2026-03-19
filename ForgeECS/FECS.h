#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"

/* ----  COMPS ---- */

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

/* ----  LAYOUTS ---- */

#define FECS_BEHAVIOR(comp_idxs)

/* ----  QUERY ---- */

/* ----  SYSTEMS ---- */

/* ----  WORLD ---- */

/* ----  FECS ---- */

#ifdef __cplusplus
}
#endif
