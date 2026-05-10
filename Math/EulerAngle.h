#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Consts.h"
#include "Transcendental.h"

typedef enum {
    MATH_EULER_ANGLE_ORDER_XYZ,
    MATH_EULER_ANGLE_ORDER_ZXY,
    MATH_EULER_ANGLE_ORDER_YZX,
    MATH_EULER_ANGLE_ORDER_YXZ,
    MATH_EULER_ANGLE_ORDER_ZYX,
    MATH_EULER_ANGLE_ORDER_XZY,
} MATH_EulerAngleOrder;

typedef struct {
    DT_f32 x_rad;
    DT_f32 y_rad;
    DT_f32 z_rad;
} MATH_EulerAngle;

static inline MATH_EulerAngle MATH_EulerAngleCreateZero(DT_void) {
    return (MATH_EulerAngle){0};
}

static inline MATH_EulerAngle
MATH_EulerAngleCreateFromRad(DT_f32 x_rad, DT_f32 y_rad, DT_f32 z_rad) {
    return (MATH_EulerAngle){
        .x_rad = x_rad,
        .y_rad = y_rad,
        .z_rad = z_rad,
    };
}

static inline MATH_EulerAngle
MATH_EulerAngleCreateFromDeg(DT_f32 x_deg, DT_f32 y_deg, DT_f32 z_deg) {
    return (MATH_EulerAngle){
        .x_rad = MATH_DEG_TO_RAD_F32 * x_deg,
        .y_rad = MATH_DEG_TO_RAD_F32 * y_deg,
        .z_rad = MATH_DEG_TO_RAD_F32 * z_deg,
    };
}

static inline MATH_EulerAngle MATH_EulerAngleWrapToPi(MATH_EulerAngle angle) {
    return (MATH_EulerAngle){
        .x_rad = MATH_WrapAngleToPiF32(angle.x_rad),
        .y_rad = MATH_WrapAngleToPiF32(angle.y_rad),
        .z_rad = MATH_WrapAngleToPiF32(angle.z_rad),
    };
}

static inline MATH_EulerAngle MATH_EulerAngleWrapToTau(MATH_EulerAngle angle) {
    return (MATH_EulerAngle){
        .x_rad = MATH_WrapAngleToTauF32(angle.x_rad),
        .y_rad = MATH_WrapAngleToTauF32(angle.y_rad),
        .z_rad = MATH_WrapAngleToTauF32(angle.z_rad),
    };
}

#ifdef __cplusplus
}
#endif
