#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

#define MATH_MAT2_SIZE (2)
#define MATH_MAT2_ELEM_COUNT (4)

// Its a column major matrix.
typedef struct {
    PRP_F32 membs[MATH_MAT2_ELEM_COUNT];
} MATH_Mat2;

#ifdef __cplusplus
}
#endif
