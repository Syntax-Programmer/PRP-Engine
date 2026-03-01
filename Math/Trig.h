#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Scalar.h"

/* ----  ANGLE UTILS  ---- */

static inline DT_f32 MATH_DegToRadF32(DT_f32 deg) {
    return deg * MATH_DEG2RAD_F32;
}

static inline DT_f64 MATH_DegToRadF64(DT_f64 deg) {
    return deg * MATH_DEG2RAD_F64;
}

static inline DT_f32 MATH_RadToDegF32(DT_f32 rad) {
    return rad * MATH_RAD2DEG_F32;
}

static inline DT_f64 MATH_RadToDegF64(DT_f64 rad) {
    return rad * MATH_RAD2DEG_F64;
}

/* ----  BASIC TRIG  ---- */

static inline DT_f32 MATH_SinF32(DT_f32 x) { return sinf(x); }
static inline DT_f64 MATH_SinF64(DT_f64 x) { return sin(x); }

static inline DT_f32 MATH_CosF32(DT_f32 x) { return cosf(x); }
static inline DT_f64 MATH_CosF64(DT_f64 x) { return cos(x); }

static inline DT_f32 MATH_TanF32(DT_f32 x) { return tanf(x); }
static inline DT_f64 MATH_TanF64(DT_f64 x) { return tan(x); }

/* ----  INV TRIG (DOMAIN GUARDED)  ---- */

static inline DT_f32 MATH_ASinF32(DT_f32 x) {
    x = MATH_ClampF32(x, -1.0f, 1.0f);
    return asinf(x);
}

static inline DT_f64 MATH_ASinF64(DT_f64 x) {
    x = MATH_ClampF64(x, -1.0, 1.0);
    return asin(x);
}

static inline DT_f32 MATH_ACosF32(DT_f32 x) {
    x = MATH_ClampF32(x, -1.0f, 1.0f);
    return acosf(x);
}

static inline DT_f64 MATH_ACosF64(DT_f64 x) {
    x = MATH_ClampF64(x, -1.0, 1.0);
    return acos(x);
}

static inline DT_f32 MATH_ATanF32(DT_f32 x) { return atanf(x); }
static inline DT_f64 MATH_ATanF64(DT_f64 x) { return atan(x); }

static inline DT_f32 MATH_ATan2F32(DT_f32 y, DT_f32 x) { return atan2f(y, x); }
static inline DT_f64 MATH_ATan2F64(DT_f64 y, DT_f64 x) { return atan2(y, x); }

/* ----  HYPERBOLIC  ---- */

static inline DT_f32 MATH_SinhF32(DT_f32 x) { return sinhf(x); }
static inline DT_f64 MATH_SinhF64(DT_f64 x) { return sinh(x); }

static inline DT_f32 MATH_CoshF32(DT_f32 x) { return coshf(x); }
static inline DT_f64 MATH_CoshF64(DT_f64 x) { return cosh(x); }

static inline DT_f32 MATH_TanhF32(DT_f32 x) { return tanhf(x); }
static inline DT_f64 MATH_TanhF64(DT_f64 x) { return tanh(x); }

/* ----  EXP LOG  ---- */

static inline DT_f32 MATH_ExpF32(DT_f32 x) { return expf(x); }
static inline DT_f64 MATH_ExpF64(DT_f64 x) { return exp(x); }

static inline DT_f32 MATH_Exp2F32(DT_f32 x) { return exp2f(x); }
static inline DT_f64 MATH_Exp2F64(DT_f64 x) { return exp2(x); }

static inline DT_f32 MATH_LogF32(DT_f32 x) { return logf(x); }
static inline DT_f64 MATH_LogF64(DT_f64 x) { return log(x); }

static inline DT_f32 MATH_Log10F32(DT_f32 x) { return log10f(x); }
static inline DT_f64 MATH_Log10F64(DT_f64 x) { return log10(x); }

static inline DT_f32 MATH_Log2F32(DT_f32 x) { return log2f(x); }
static inline DT_f64 MATH_Log2F64(DT_f64 x) { return log2(x); }

/* ----  POWER/ROOT  ---- */

static inline DT_f32 MATH_PowF32(DT_f32 a, DT_f32 b) { return powf(a, b); }
static inline DT_f64 MATH_PowF64(DT_f64 a, DT_f64 b) { return pow(a, b); }

static inline DT_f32 MATH_SqrtF32(DT_f32 x) { return sqrtf(x); }
static inline DT_f64 MATH_SqrtF64(DT_f64 x) { return sqrt(x); }

static inline DT_f32 MATH_CbrtF32(DT_f32 x) { return cbrtf(x); }
static inline DT_f64 MATH_CbrtF64(DT_f64 x) { return cbrt(x); }

/* ----  SAFE INVERSE  ---- */

static inline DT_f32 MATH_InvF32(DT_f32 x) {
    return (MATH_AbsF32(x) > MATH_NORMALIZE_EPS_F32) ? (1.0f / x) : 0.0f;
}

static inline DT_f64 MATH_InvF64(DT_f64 x) {
    return (MATH_AbsF64(x) > MATH_NORMALIZE_EPS_F64) ? (1.0 / x) : 0.0;
}

/* ----  SAFE SQRT  ---- */

static inline DT_f32 MATH_SafeSqrtF32(DT_f32 x) {
    return (x > 0.0f) ? sqrtf(x) : 0.0f;
}

static inline DT_f64 MATH_SafeSqrtF64(DT_f64 x) {
    return (x > 0.0) ? sqrt(x) : 0.0;
}

/* ----  FAST INVERSE SQRT ( QUAKE III )  ---- */

static inline DT_f32 MATH_FastInvSqrtF32(DT_f32 number) {
    DT_f32 x2 = number * 0.5f;
    DT_f32 y = number;

    union {
        DT_f32 f;
        DT_u32 i;
    } conv;

    conv.f = y;
    conv.i = 0x5f3759df - (conv.i >> 1);
    y = conv.f;

    /* Newton iteration */
    y = y * (1.5f - (x2 * y * y));

    return y;
}

static inline DT_f32 MATH_FastInvF32(DT_f32 x) {
    return MATH_FastInvSqrtF32(x * x);
}

#ifdef __cplusplus
}
#endif
