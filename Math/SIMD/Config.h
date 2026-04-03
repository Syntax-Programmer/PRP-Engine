#pragma once

/* ----  ARCHITECTURE DETECTION  ---- */

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) ||             \
    defined(_M_IX86)
#define MATH_ARCH_X86 1
#else
#define MATH_ARCH_X86 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) ||           \
    defined(_M_ARM)
#define MATH_ARCH_ARM 1
#else
#define MATH_ARCH_ARM 0
#endif

/* ----  COMPILER DETECTION  ---- */

#if defined(_MSC_VER)
#define MATH_COMPILER_MSVC 1
#else
#define MATH_COMPILER_MSVC 0
#endif

#if defined(__clang__)
#define MATH_COMPILER_CLANG 1
#else
#define MATH_COMPILER_CLANG 0
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define MATH_COMPILER_GCC 1
#else
#define MATH_COMPILER_GCC 0
#endif

/* ----  BACKEND DETECTION  ---- */

/*
   You may define these externally:

   - MATH_SIMD_DISABLE_SSE2
   - MATH_SIMD_DISABLE_AVX2
   - MATH_SIMD_DISABLE_NEON
   - MATH_SIMD_FORCE_SCALAR
*/

#if defined(MATH_SIMD_FORCE_SCALAR)
#define MATH_SIMD_COMPILED_SCALAR 1
#define MATH_SIMD_COMPILED_SSE2 0
#define MATH_SIMD_COMPILED_AVX2 0
#define MATH_SIMD_COMPILED_NEON 0

#else

/* Scalar is always compiled */
#define MATH_SIMD_COMPILED_SCALAR 1

/* SSE2 */
#if MATH_ARCH_X86 && !defined(MATH_SIMD_DISABLE_SSE2)
#define MATH_SIMD_COMPILED_SSE2 1
#else
#define MATH_SIMD_COMPILED_SSE2 0
#endif

/* AVX2 */
#if MATH_ARCH_X86 && !defined(MATH_SIMD_DISABLE_AVX2)
#define MATH_SIMD_COMPILED_AVX2 1
#else
#define MATH_SIMD_COMPILED_AVX2 0
#endif

/* NEON */
#if MATH_ARCH_ARM && !defined(MATH_SIMD_DISABLE_NEON)
#define MATH_SIMD_COMPILED_NEON 1
#else
#define MATH_SIMD_COMPILED_NEON 0
#endif

#endif

/* ----  SIMD ALIGNMENT POLICY  ---- */

#if MATH_SIMD_COMPILED_AVX2
#define MATH_SIMD_HW_WIDTH 8
#elif MATH_SIMD_COMPILED_SSE2 || MATH_SIMD_COMPILED_NEON
#define MATH_SIMD_HW_WIDTH 4
#else
#define MATH_SIMD_HW_WIDTH 1
#endif

#define MATH_SIMD_ALIGNMENT 16
#define MATH_SIMD_WIDTH 4

/* ----  SIMD AVAILABLITY  ---- */

#if MATH_SIMD_COMPILED_SSE2 || MATH_SIMD_COMPILED_AVX2 ||                      \
    MATH_SIMD_COMPILED_NEON
#define MATH_SIMD_AVAILABLE 1
#else
#define MATH_SIMD_AVAILABLE 0
#endif
