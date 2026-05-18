#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Data-Types/Typedefs.h"

#define MATH_MAT4_SIZE (4)
#define MATH_MAT4_ELEM_COUNT (16)

// Its a column major matrix.
typedef struct {
    DT_f32 membs[MATH_MAT4_ELEM_COUNT];
} MATH_Mat4;

#ifdef __cplusplus
}
#endif
