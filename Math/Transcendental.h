#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Consts.h"
#include <math.h>

/* ----  OPS  ---- */

static inline PRP_F32 MATH_SqrtF32(PRP_F32 a) { return sqrtf(a); }
static inline PRP_F64 MATH_SqrtF64(PRP_F64 a) { return sqrt(a); }

static inline PRP_F32 MATH_InvSqrtF32(PRP_F32 a) {
    return ((PRP_F32)1.0 / sqrtf(a));
}

static inline PRP_F64 MATH_InvSqrtF64(PRP_F64 a) {
    return ((PRP_F64)1.0 / sqrt(a));
}

static inline PRP_F32 MATH_CbrtF32(PRP_F32 a) { return cbrtf(a); }
static inline PRP_F64 MATH_CbrtF64(PRP_F64 a) { return cbrt(a); }

static inline PRP_F32 MATH_PowF32(PRP_F32 x, PRP_F32 y) { return powf(x, y); }
static inline PRP_F64 MATH_PowF64(PRP_F64 x, PRP_F64 y) { return pow(x, y); }

static inline PRP_F32 MATH_ExpF32(PRP_F32 x) { return expf(x); }
static inline PRP_F64 MATH_ExpF64(PRP_F64 x) { return exp(x); }

static inline PRP_F32 MATH_LogF32(PRP_F32 x) { return logf(x); }
static inline PRP_F64 MATH_LogF64(PRP_F64 x) { return log(x); }

static inline PRP_F32 MATH_Log2F32(PRP_F32 x) { return log2f(x); }
static inline PRP_F64 MATH_Log2F64(PRP_F64 x) { return log2(x); }

/* ----  TRIGONOMETRY  ---- */

static inline PRP_F32 MATH_SinF32(PRP_F32 rad) { return sinf(rad); }
static inline PRP_F64 MATH_SinF64(PRP_F64 rad) { return sin(rad); }

static inline PRP_F32 MATH_CosF32(PRP_F32 rad) { return cosf(rad); }
static inline PRP_F64 MATH_CosF64(PRP_F64 rad) { return cos(rad); }

static inline PRP_F32 MATH_TanF32(PRP_F32 rad) { return tanf(rad); }
static inline PRP_F64 MATH_TanF64(PRP_F64 rad) { return tan(rad); }

static inline PRP_F32 MATH_ASinF32(PRP_F32 x) { return asinf(x); }
static inline PRP_F64 MATH_ASinF64(PRP_F64 x) { return asin(x); }

static inline PRP_F32 MATH_ACosF32(PRP_F32 x) { return acosf(x); }
static inline PRP_F64 MATH_ACosF64(PRP_F64 x) { return acos(x); }

static inline PRP_F32 MATH_ATanF32(PRP_F32 x) { return atanf(x); }
static inline PRP_F64 MATH_ATanF64(PRP_F64 x) { return atan(x); }

static inline PRP_F32 MATH_ATan2F32(PRP_F32 y, PRP_F32 x) {
    return atan2f(y, x);
}
static inline PRP_F64 MATH_ATan2F64(PRP_F64 y, PRP_F64 x) {
    return atan2(y, x);
}

static inline PRP_F32 MATH_SinHF32(PRP_F32 rad) { return sinhf(rad); }
static inline PRP_F64 MATH_SinHF64(PRP_F64 rad) { return sinh(rad); }

static inline PRP_F32 MATH_CosHF32(PRP_F32 rad) { return coshf(rad); }
static inline PRP_F64 MATH_CosHF64(PRP_F64 rad) { return cosh(rad); }

static inline PRP_F32 MATH_TanHF32(PRP_F32 rad) { return tanhf(rad); }
static inline PRP_F64 MATH_TanHF64(PRP_F64 rad) { return tanh(rad); }

static inline PRP_F32 MATH_ASinHF32(PRP_F32 x) { return asinhf(x); }
static inline PRP_F64 MATH_ASinHF64(PRP_F64 x) { return asinh(x); }

static inline PRP_F32 MATH_ACosHF32(PRP_F32 x) { return acosf(x); }
static inline PRP_F64 MATH_ACosHF64(PRP_F64 x) { return acosh(x); }

static inline PRP_F32 MATH_ATanHF32(PRP_F32 x) { return atanhf(x); }
static inline PRP_F64 MATH_ATanHF64(PRP_F64 x) { return atanh(x); }

static inline PRP_F32 MATH_WrapAngleToPiF32(PRP_F32 rad) {
    PRP_F32 x = fmodf(rad + MATH_PI_F32, MATH_TAU_F32);
    if (x < (PRP_F32)0.0) {
        x += MATH_TAU_F32;
    }

    return x - MATH_PI_F32;
}

static inline PRP_F64 MATH_WrapAngleToPiF64(PRP_F64 rad) {
    PRP_F64 x = fmod(rad + MATH_PI_F64, MATH_TAU_F64);
    if (x < (PRP_F64)0.0) {
        x += MATH_TAU_F64;
    }

    return x - MATH_PI_F64;
}

static inline PRP_F32 MATH_WrapAngleToTauF32(PRP_F32 rad) {
    PRP_F32 x = fmodf(rad, MATH_TAU_F32);
    if (x < (PRP_F32)0.0) {
        x += MATH_TAU_F32;
    }

    return x;
}

static inline PRP_F64 MATH_WrapAngleToTauF64(PRP_F64 rad) {
    PRP_F64 x = fmod(rad, MATH_TAU_F64);
    if (x < (PRP_F64)0.0) {
        x += MATH_TAU_F64;
    }

    return x;
}

static inline PRP_F32 MATH_DeltaAngleF32(PRP_F32 a, PRP_F32 b) {
    return MATH_WrapAngleToPiF32(b - a);
}
static inline PRP_F64 MATH_DeltaAngleF64(PRP_F64 a, PRP_F64 b) {
    return MATH_WrapAngleToPiF64(b - a);
}

#ifdef __cplusplus
}
#endif
