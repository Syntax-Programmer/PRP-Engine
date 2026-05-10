#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Float.h"

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

#ifdef __cplusplus
}
#endif
