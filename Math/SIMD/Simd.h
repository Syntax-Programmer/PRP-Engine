#pragma once

#include "Platform-Defs/Config.h"

#define MATH_SIMD_INTERNAL

#if defined(MATH_SIMD_BACKEND_AVX2)
#include "Platform-Defs/AVX2.h"
#elif defined(MATH_SIMD_BACKEND_SSE2)
#include "Platform-Defs/SSE2.h"
#elif defined(MATH_SIMD_BACKEND_NEON)
#include "Platform-Defs/NEON.h"
#else
#include "Platform-Defs/Scalar.h"
#endif

#undef MATH_SIMD_INTERNAL
