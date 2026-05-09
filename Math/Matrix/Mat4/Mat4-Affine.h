#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Diagnostics/Assert.h"
#include "../../../Utils/Defs.h"
#include "../../Vector/Vec3.h"
#include "../Mat3/Defs.h"
#include "Defs.h"

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat4 MATH_Mat4CreateTranslation(MATH_Vec3 pos) {
    return (MATH_Mat4){.membs = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f, pos.x, pos.y, pos.z,
                                 1.0f}};
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4CreateRotationAxis(MATH_Vec3 axis,
                                                             DT_f32 angle);

static inline MATH_Mat4 MATH_Mat4CreateRotationAxisX(DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Mat4){.membs = {1.0f, 0.0f, 0.0f, 0.0f,

                                 0.0f, c, s, 0.0f,

                                 0.0f, -s, c, 0.0f,

                                 0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateRotationAxisY(DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Mat4){.membs = {c, 0.0f, -s, 0.0f,

                                 0.0f, 1.0f, 0.0f, 0.0f,

                                 s, 0.0f, c, 0.0f,

                                 0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateRotationAxisZ(DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Mat4){.membs = {c, s, 0.0f, 0.0f,

                                 -s, c, 0.0f, 0.0f,

                                 0.0f, 0.0f, 1.0f, 0.0f,

                                 0.0f, 0.0f, 0.0f, 1.0f}};
}

typedef enum {
    MATH_EULER_XYZ,
    MATH_EULER_ZXY,
    MATH_EULER_YZX,
    MATH_EULER_YXZ,
    MATH_EULER_ZYX,
    MATH_EULER_XZY,
} MATH_Mat4EulerRotOrder;

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerXYZ(MATH_Vec3 angles);
PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerZXY(MATH_Vec3 angles);
PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerYZX(MATH_Vec3 angles);
PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerYXZ(MATH_Vec3 angles);
PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerZYX(MATH_Vec3 angles);
PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerXZY(MATH_Vec3 angles);

static inline MATH_Mat4
MATH_Mat4CreateRotationEuler(MATH_Vec3 angles, MATH_Mat4EulerRotOrder order) {
    switch (order) {
    case MATH_EULER_XYZ:
        return MATH_Mat4CreateRotationEulerXYZ(angles);
    case MATH_EULER_ZXY:
        return MATH_Mat4CreateRotationEulerZXY(angles);
    case MATH_EULER_YZX:
        return MATH_Mat4CreateRotationEulerYZX(angles);
    case MATH_EULER_YXZ:
        return MATH_Mat4CreateRotationEulerYXZ(angles);
    case MATH_EULER_ZYX:
        return MATH_Mat4CreateRotationEulerZYX(angles);
    case MATH_EULER_XZY:
        return MATH_Mat4CreateRotationEulerXZY(angles);
    default:
        DIAG_ASSERT(0 && "Invalid MATH_EulerOrder provided.");
        // Identity matrix.
        return (MATH_Mat4){.membs = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f}};
    }
}

static inline MATH_Mat4 MATH_Mat4CreateScale(MATH_Vec3 scale) {
    return (MATH_Mat4){.membs = {scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f,
                                 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f}};
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4CreateTRSAxis(MATH_Vec3 pos,
                                                        MATH_Vec3 rot_axis,
                                                        DT_f32 rot_rad,
                                                        MATH_Vec3 scale);
PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateTRSEuler(MATH_Vec3 pos, MATH_Vec3 rot_angles,
                        MATH_Mat4EulerRotOrder rot_order, MATH_Vec3 scale);

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_bool MATH_Mat4IsAffine(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[3]) && MATH_IsZeroF32(a.membs[7]) &&
                     MATH_IsZeroF32(a.membs[11]) &&
                     MATH_AlmostEqF32(a.membs[15], 1.0f));
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat4IsAffineOrthonormal(MATH_Mat4 a);

/* ----  BASIC OPS  ---- */

static inline MATH_Vec3 MATH_Mat4MulVec3Affine(MATH_Mat4 a, MATH_Vec3 v) {
    return (MATH_Vec3){
        .x = (a.membs[0] * v.x) + (a.membs[4] * v.y) + (a.membs[8] * v.z) +
             (a.membs[12]),
        .y = (a.membs[1] * v.x) + (a.membs[5] * v.y) + (a.membs[9] * v.z) +
             (a.membs[13]),
        .z = (a.membs[2] * v.x) + (a.membs[6] * v.y) + (a.membs[10] * v.z) +
             (a.membs[14]),
    };
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4InvAffine(MATH_Mat4 a, MATH_Mat4 det_zero_fallback);
PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4InvAffineOrthonormal(MATH_Mat4 a);
PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4AffineOrthonormalize(MATH_Mat4 a);

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

static inline MATH_Vec3 MATH_Mat4ExtractTranslation(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.membs[12],
        .y = a.membs[13],
        .z = a.membs[14],
    };
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat4ExtractRotation(MATH_Mat4 a);
PRP_FN_API MATH_Vec3 PRP_FN_CALL MATH_Mat4ExtractScale(MATH_Mat4 a);
PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4NormBasis(MATH_Mat4 a);

static inline MATH_Vec3 MATH_Mat4GetRight(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.membs[0],
        .y = a.membs[1],
        .z = a.membs[2],
    };
}

static inline MATH_Vec3 MATH_Mat4GetUp(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.membs[4],
        .y = a.membs[5],
        .z = a.membs[6],
    };
}

static inline MATH_Vec3 MATH_Mat4GetForward(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.membs[8],
        .y = a.membs[9],
        .z = a.membs[10],
    };
}

#ifdef __cplusplus
}
#endif
