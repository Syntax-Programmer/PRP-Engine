#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Data-Types/Typedefs.h"

#define MATH_MAT2_SIZE (2)
#define MATH_MAT2_ELEM_COUNT (4)

// Its a column major matrix.
typedef struct {
    DT_f32 membs[MATH_MAT2_ELEM_COUNT];
} MATH_Mat2;

#ifdef __cplusplus
}
#endif
