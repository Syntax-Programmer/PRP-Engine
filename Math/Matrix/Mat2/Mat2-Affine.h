#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Defs.h"
#include "Math/Vector/Vec2.h"
#include "Utils/Defs.h"

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat2 MATH_Mat2CreateRotation(DT_f32 rad) {
    DT_f32 c = MATH_CosF32(rad);
    DT_f32 s = MATH_SinF32(rad);

    return (MATH_Mat2){.membs = {c, s, -s, c}};
}

static inline MATH_Mat2 MATH_Mat2CreateScale(MATH_Vec2 scale) {
    return (MATH_Mat2){.membs = {scale.x, 0.0f, 0.0f, scale.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateRS(DT_f32 rot_rad, MATH_Vec2 scale) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat2){
        .membs = {c * scale.x, s * scale.x, -s * scale.y, c * scale.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateShear(MATH_Vec2 shear) {
    return (MATH_Mat2){.membs = {1.0f, shear.x, shear.y, 1.0f}};
}

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat2ExtractRotation(MATH_Mat2 a);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Mat2ExtractScale(MATH_Mat2 a);
PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2NormBasis(MATH_Mat2 a);

static inline MATH_Vec2 MATH_Mat2Right(MATH_Mat2 a) {
    return (MATH_Vec2){
        .x = a.membs[0],
        .y = a.membs[1],
    };
}

static inline DT_f32 MATH_Mat2RightLen(MATH_Mat2 a) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1];

    return MATH_SqrtF32((a00 * a00) + (a10 * a10));
}

static inline DT_f32 MATH_Mat2RightLenSq(MATH_Mat2 a) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1];

    return (a00 * a00) + (a10 * a10);
}

static inline MATH_Vec2 MATH_Mat2Up(MATH_Mat2 a) {
    return (MATH_Vec2){
        .x = a.membs[2],
        .y = a.membs[3],
    };
}

static inline DT_f32 MATH_Mat2UpLen(MATH_Mat2 a) {
    DT_f32 a01 = a.membs[2], a11 = a.membs[3];

    return MATH_SqrtF32((a01 * a01) + (a11 * a11));
}

static inline DT_f32 MATH_Mat2UpLenSq(MATH_Mat2 a) {
    DT_f32 a01 = a.membs[2], a11 = a.membs[3];

    return (a01 * a01) + (a11 * a11);
}

#ifdef __cplusplus
}
#endif
