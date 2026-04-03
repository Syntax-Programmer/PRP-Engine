#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Data-Types/Typedefs.h"
#include "Config.h"

typedef struct {
    DT_f32 vec[4];
} MATH_SimdF32;

typedef struct {
    // Construction
    MATH_SimdF32 (*Set)(DT_f32 x, DT_f32 y, DT_f32 z, DT_f32 w);
    MATH_SimdF32 (*Set1)(DT_f32 v);
    MATH_SimdF32 (*Zero)(DT_void);

    MATH_SimdF32 (*Load)(const DT_f32 *ptr);
    MATH_SimdF32 (*LoadUnaligned)(const DT_f32 *ptr);
    DT_void (*Store)(DT_f32 *ptr, MATH_SimdF32 v);
    DT_void (*StoreUnaligned)(DT_f32 *ptr, MATH_SimdF32 v);

    // Arithmetic
    MATH_SimdF32 (*Add)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Sub)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Mul)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Div)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Neg)(MATH_SimdF32 a);
    MATH_SimdF32 (*Abs)(MATH_SimdF32 a);

    // Min / Max
    MATH_SimdF32 (*Min)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Max)(MATH_SimdF32 a, MATH_SimdF32 b);

    // Math
    MATH_SimdF32 (*Sqrt)(MATH_SimdF32 a);
    MATH_SimdF32 (*RSqrt)(MATH_SimdF32 a);
    MATH_SimdF32 (*Reciprocal)(MATH_SimdF32 a);

    // Shuffle
    MATH_SimdF32 (*Shuffle)(MATH_SimdF32 v, DT_i32 mask);
    MATH_SimdF32 (*Shuffle2)(MATH_SimdF32 a, MATH_SimdF32 b, DT_i32 mask);

    // Horizontal
    DT_f32 (*HAdd)(MATH_SimdF32 v);
    DT_f32 (*Dot3)(MATH_SimdF32 a, MATH_SimdF32 b);
    DT_f32 (*Dot4)(MATH_SimdF32 a, MATH_SimdF32 b);

    // Comparison
    MATH_SimdF32 (*CmpEQ)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*CmpLT)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*CmpGT)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*CmpLE)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*CmpGE)(MATH_SimdF32 a, MATH_SimdF32 b);

    // Bitwise
    MATH_SimdF32 (*And)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Or)(MATH_SimdF32 a, MATH_SimdF32 b);
    MATH_SimdF32 (*Xor)(MATH_SimdF32 a, MATH_SimdF32 b);

    // Blend
    MATH_SimdF32 (*Blend)(MATH_SimdF32 a, MATH_SimdF32 b, MATH_SimdF32 mask);

    // Mask extraction
    DT_i32 (*MoveMask)(MATH_SimdF32 a);
} MATH_SimdVTable;

extern MATH_SimdVTable g_math_simd;

#ifdef __cplusplus
}
#endif
