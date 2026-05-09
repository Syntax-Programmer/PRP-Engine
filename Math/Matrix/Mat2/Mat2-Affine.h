#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Utils/Defs.h"
#include "../../Vector/Vec2.h"
#include "Defs.h"

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

static inline MATH_Vec2 MATH_Mat2GetRight(MATH_Mat2 a) {
    return (MATH_Vec2){
        .x = a.membs[0],
        .y = a.membs[1],
    };
}

static inline MATH_Vec2 MATH_Mat2GetUp(MATH_Mat2 a) {
    return (MATH_Vec2){
        .x = a.membs[2],
        .y = a.membs[3],
    };
}

#ifdef __cplusplus
}
#endif
