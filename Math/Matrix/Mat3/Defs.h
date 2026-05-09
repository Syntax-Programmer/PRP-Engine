#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Data-Types/Typedefs.h"

#define MATH_MAT3_SIZE (3)
#define MATH_MAT3_ELEM_COUNT (9)

// Its a column major matrix.
typedef struct {
    DT_f32 membs[MATH_MAT3_ELEM_COUNT];
} MATH_Mat3;

#ifdef __cplusplus
}
#endif
