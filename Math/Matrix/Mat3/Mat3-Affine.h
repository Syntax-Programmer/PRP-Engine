#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Utils/Defs.h"
#include "../../Vector/Vec2.h"
#include "Defs.h"

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat3 MATH_Mat3CreateTranslation(MATH_Vec2 pos) {
    return (MATH_Mat3){.membs = {1.0f, 0.0f, 0.0f,

                                 0.0f, 1.0f, 0.0f,

                                 pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateRotation(DT_f32 rad) {
    DT_f32 c = MATH_CosF32(rad);
    DT_f32 s = MATH_SinF32(rad);

    return (MATH_Mat3){.membs = {c, s, 0.0f,

                                 -s, c, 0.0f,

                                 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateScale(MATH_Vec2 scale) {
    return (MATH_Mat3){.membs = {scale.x, 0.0f, 0.0f,

                                 0.0f, scale.y, 0.0f,

                                 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateTR(MATH_Vec2 pos, DT_f32 rot_rad) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat3){.membs = {c, s, 0.0f,

                                 -s, c, 0.0f,

                                 pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateTS(MATH_Vec2 pos, MATH_Vec2 scale) {
    return (MATH_Mat3){.membs = {scale.x, 0.0f, 0.0f,

                                 0.0f, scale.y, 0.0f,

                                 pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateRS(DT_f32 rot_rad, MATH_Vec2 scale) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat3){.membs = {c * scale.x, s * scale.x, 0.0f,

                                 -s * scale.y, c * scale.y, 0.0f,

                                 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateTRS(MATH_Vec2 pos, DT_f32 rot_rad,
                                           MATH_Vec2 scale) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat3){.membs = {c * scale.x, s * scale.x, 0.0f,

                                 -s * scale.y, c * scale.y, 0.0f,

                                 pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateShear(MATH_Vec2 shear) {

    return (MATH_Mat3){.membs = {1.0f, shear.x, 0.0f,

                                 shear.y, 1.0f, 0.0f,

                                 0.0f, 0.0f, 1.0f}};
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_bool MATH_Mat3IsAffine(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[5]) &&
                     MATH_AlmostEqF32(a.membs[8], 1.0f));
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat3IsAffineOrthonormal(MATH_Mat3 a);

/* ----  BASIC OPS  ---- */

static inline MATH_Vec2 MATH_Mat3MulVec2Affine(MATH_Mat3 a, MATH_Vec2 v) {
    return (MATH_Vec2){
        .x = (a.membs[0] * v.x) + (a.membs[3] * v.y) + a.membs[6],
        .y = (a.membs[1] * v.x) + (a.membs[4] * v.y) + a.membs[7],
    };
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL
MATH_Mat3InvAffine(MATH_Mat3 a, MATH_Mat3 det_zero_fallback);
PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3InvAffineOrthonormal(MATH_Mat3 a);
PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3AffineOrthonormalize(MATH_Mat3 a);

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

static inline MATH_Vec2 MATH_Mat3ExtractTranslation(MATH_Mat3 a) {
    return (MATH_Vec2){
        .x = a.membs[6],
        .y = a.membs[7],
    };
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat3ExtractRotation(MATH_Mat3 a);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Mat3ExtractScale(MATH_Mat3 a);
PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3NormBasis(MATH_Mat3 a);

static inline MATH_Vec2 MATH_Mat3GetRight(MATH_Mat3 a) {
    return (MATH_Vec2){
        .x = a.membs[0],
        .y = a.membs[1],
    };
}

static inline MATH_Vec2 MATH_Mat3GetUp(MATH_Mat3 a) {
    return (MATH_Vec2){
        .x = a.membs[3],
        .y = a.membs[4],
    };
}

#ifdef __cplusplus
}
#endif
