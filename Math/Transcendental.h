#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Consts.h"
#include <math.h>

/* ----  OPS  ---- */

static inline DT_f32 MATH_SqrtF32(DT_f32 a) { return sqrtf(a); }
static inline DT_f64 MATH_SqrtF64(DT_f64 a) { return sqrt(a); }

static inline DT_f32 MATH_InvSqrtF32(DT_f32 a) {
    return ((DT_f32)1.0 / sqrtf(a));
}

static inline DT_f64 MATH_InvSqrtF64(DT_f64 a) {
    return ((DT_f64)1.0 / sqrt(a));
}

static inline DT_f32 MATH_CbrtF32(DT_f32 a) { return cbrtf(a); }
static inline DT_f64 MATH_CbrtF64(DT_f64 a) { return cbrt(a); }

static inline DT_f32 MATH_PowF32(DT_f32 x, DT_f32 y) { return powf(x, y); }
static inline DT_f64 MATH_PowF64(DT_f64 x, DT_f64 y) { return pow(x, y); }

static inline DT_f32 MATH_ExpF32(DT_f32 x) { return expf(x); }
static inline DT_f64 MATH_ExpF64(DT_f64 x) { return exp(x); }

static inline DT_f32 MATH_LogF32(DT_f32 x) { return logf(x); }
static inline DT_f64 MATH_LogF64(DT_f64 x) { return log(x); }

static inline DT_f32 MATH_Log2F32(DT_f32 x) { return log2f(x); }
static inline DT_f64 MATH_Log2F64(DT_f64 x) { return log2(x); }

/* ----  TRIGONOMETRY  ---- */

static inline DT_f32 MATH_SinF32(DT_f32 rad) { return sinf(rad); }
static inline DT_f64 MATH_SinF64(DT_f64 rad) { return sin(rad); }

static inline DT_f32 MATH_CosF32(DT_f32 rad) { return cosf(rad); }
static inline DT_f64 MATH_CosF64(DT_f64 rad) { return cos(rad); }

static inline DT_f32 MATH_TanF32(DT_f32 rad) { return tanf(rad); }
static inline DT_f64 MATH_TanF64(DT_f64 rad) { return tan(rad); }

static inline DT_f32 MATH_ASinF32(DT_f32 x) { return asinf(x); }
static inline DT_f64 MATH_ASinF64(DT_f64 x) { return asin(x); }

static inline DT_f32 MATH_ACosF32(DT_f32 x) { return acosf(x); }
static inline DT_f64 MATH_ACosF64(DT_f64 x) { return acos(x); }

static inline DT_f32 MATH_ATanF32(DT_f32 x) { return atanf(x); }
static inline DT_f64 MATH_ATanF64(DT_f64 x) { return atan(x); }

static inline DT_f32 MATH_ATan2F32(DT_f32 y, DT_f32 x) { return atan2f(y, x); }
static inline DT_f64 MATH_ATan2F64(DT_f64 y, DT_f64 x) { return atan2(y, x); }

static inline DT_f32 MATH_SinHF32(DT_f32 rad) { return sinhf(rad); }
static inline DT_f64 MATH_SinHF64(DT_f64 rad) { return sinh(rad); }

static inline DT_f32 MATH_CosHF32(DT_f32 rad) { return coshf(rad); }
static inline DT_f64 MATH_CosHF64(DT_f64 rad) { return cosh(rad); }

static inline DT_f32 MATH_TanHF32(DT_f32 rad) { return tanhf(rad); }
static inline DT_f64 MATH_TanHF64(DT_f64 rad) { return tanh(rad); }

static inline DT_f32 MATH_ASinHF32(DT_f32 x) { return asinhf(x); }
static inline DT_f64 MATH_ASinHF64(DT_f64 x) { return asinh(x); }

static inline DT_f32 MATH_ACosHF32(DT_f32 x) { return acosf(x); }
static inline DT_f64 MATH_ACosHF64(DT_f64 x) { return acosh(x); }

static inline DT_f32 MATH_ATanHF32(DT_f32 x) { return atanhf(x); }
static inline DT_f64 MATH_ATanHF64(DT_f64 x) { return atanh(x); }

static inline DT_f32 MATH_WrapAngleToPiF32(DT_f32 rad) {
    DT_f32 x = fmodf(rad + MATH_PI_F32, MATH_TAU_F32);
    if (x < (DT_f32)0.0) {
        x += MATH_TAU_F32;
    }

    return x - MATH_PI_F32;
}

static inline DT_f64 MATH_WrapAngleToPiF64(DT_f64 rad) {
    DT_f64 x = fmod(rad + MATH_PI_F64, MATH_TAU_F64);
    if (x < (DT_f64)0.0) {
        x += MATH_TAU_F64;
    }

    return x - MATH_PI_F64;
}

static inline DT_f32 MATH_WrapAngleToTauF32(DT_f32 rad) {
    DT_f32 x = fmodf(rad, MATH_TAU_F32);
    if (x < (DT_f32)0.0) {
        x += MATH_TAU_F32;
    }

    return x;
}

static inline DT_f64 MATH_WrapAngleToTauF64(DT_f64 rad) {
    DT_f64 x = fmod(rad, MATH_TAU_F64);
    if (x < (DT_f64)0.0) {
        x += MATH_TAU_F64;
    }

    return x;
}

static inline DT_f32 MATH_DeltaAngleF32(DT_f32 a, DT_f32 b) {
    return MATH_WrapAngleToPiF32(b - a);
}
static inline DT_f64 MATH_DeltaAngleF64(DT_f64 a, DT_f64 b) {
    return MATH_WrapAngleToPiF64(b - a);
}

#ifdef __cplusplus
}
#endif
