#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compile-time configuration macros recognized by PRP.
 *
 * PRP_NDEBUG
 *     Builds PRP in release mode. Disables debug-only diagnostics,
 *     assertions, and validation checks.
 *
 * PRP_BUILD_DLL
 *     Indicates that PRP itself is being built as a shared library.
 *     Public symbols are exported.
 *
 * PRP_USE_DLL
 *     Indicates that PRP is being linked against as a shared library.
 *     Public symbols are imported.
 */

#ifdef PRP_NDEBUG
#define PRP_RELEASE_MODE 1
#else
#define PRP_DEBUG_MODE 1
#endif

/* ----  OS DETECTION ---- */

#if defined(_WIN32) || defined(_WIN64)
#define PRP_PLATFORM_WINDOWS 1

#elif defined(__ANDROID__)
#define PRP_PLATFORM_ANDROID 1

#elif defined(__EMSCRIPTEN__)
#define PRP_PLATFORM_WEB 1

#elif defined(__linux__)
#define PRP_PLATFORM_LINUX 1

#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define PRP_PLATFORM_IOS 1
#else
#define PRP_PLATFORM_MACOS 1
#endif

#else
#error Unsupported Platform Detected
#endif

/* ----  COMPILER DETECTION ---- */

#if defined(__clang__)
#define PRP_COMPILER_CLANG 1

#elif defined(__GNUC__)
#define PRP_COMPILER_GCC 1

#elif defined(_MSC_VER)
#define PRP_COMPILER_MSVC 1

#else
#error Unsupported Compiler Detected
#endif

/* ---- C STANDARD DETECTION ---- */

#ifdef __cplusplus

/* Compiling as C++, skip C standard checks. */

#else

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 201112L)
#error PRP requires C11 or newer.
#elif __STDC_VERSION__ >= 202311L
#define PRP_C23 1
#elif __STDC_VERSION__ >= 201710L
#define PRP_C17 1
#else
#define PRP_C11 1
#endif

#endif

/* ----  CPU ARCHITECTURE DETECTION ---- */

#if defined(__x86_64__) || defined(_M_X64)
#define PRP_CPU_ARCH_X64 1

#elif defined(__i386__) || defined(_M_IX86)
#define PRP_CPU_ARCH_X86 1

#elif defined(__aarch64__) || defined(_M_ARM64)
#define PRP_CPU_ARCH_ARM64 1

#elif defined(__arm__) || defined(_M_ARM)
#define PRP_CPU_ARCH_ARM32 1

#elif defined(__riscv)
#define PRP_CPU_ARCH_RISCV 1

#else
#error Unsupported CPU Architecture Detected
#endif

/* ----  BITNESS DETECTION ---- */

#if defined(PRP_CPU_ARCH_X64) || defined(PRP_CPU_ARCH_ARM64)
#define PRP_BITNESS64 1

#else
#define PRP_BITNESS32 1
#endif

/* ----  ENDIANNESS DETECTION ---- */

#if defined(PRP_CPU_ARCH_X86) || defined(PRP_CPU_ARCH_X64) ||                  \
    defined(PRP_CPU_ARCH_ARM64)
#define PRP_ENDIANNESS_LITTLE_ENDIAN 1

#else
#define PRP_ENDIANNESS_BIG_ENDIAN 1
#endif

/* ----  THREADING DETECTION ---- */

#if defined(PRP_PLATFORM_WINDOWS)
#define PRP_THREADING_WINTHREADS 1

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_MACOS) ||            \
    defined(PRP_PLATFORM_IOS) || defined(PRP_PLATFORM_ANDROID)
#define PRP_THREADING_PTHREADS 1
#endif

/* ---- BIUILTIN DETECTION ---- */

#ifdef __has_builtin

#if __has_builtin(__builtin_popcount)
#define PRP_HAS_BUILTIN_POPCOUNT 1
#endif

#if __has_builtin(__builtin_popcountl)
#define PRP_HAS_BUILTIN_POPCOUNTL 1
#endif

#if __has_builtin(__builtin_popcountll)
#define PRP_HAS_BUILTIN_POPCOUNTLL 1
#endif

#if __has_builtin(__builtin_parity)
#define PRP_HAS_BUILTIN_PARITY 1
#endif

#if __has_builtin(__builtin_parityl)
#define PRP_HAS_BUILTIN_PARITYL 1
#endif

#if __has_builtin(__builtin_parityll)
#define PRP_HAS_BUILTIN_PARITYLL 1
#endif

#if __has_builtin(__builtin_clz)
#define PRP_HAS_BUILTIN_CLZ 1
#endif

#if __has_builtin(__builtin_clzl)
#define PRP_HAS_BUILTIN_CLZL 1
#endif

#if __has_builtin(__builtin_clzll)
#define PRP_HAS_BUILTIN_CLZLL 1
#endif

#if __has_builtin(__builtin_ctz)
#define PRP_HAS_BUILTIN_CTZ 1
#endif

#if __has_builtin(__builtin_ctzl)
#define PRP_HAS_BUILTIN_CTZL 1
#endif

#if __has_builtin(__builtin_ctzll)
#define PRP_HAS_BUILTIN_CTZLL 1
#endif

#if __has_builtin(__builtin_ffs)
#define PRP_HAS_BUILTIN_FFS 1
#endif

#if __has_builtin(__builtin_ffsl)
#define PRP_HAS_BUILTIN_FFSL 1
#endif

#if __has_builtin(__builtin_ffsll)
#define PRP_HAS_BUILTIN_FFSLL 1
#endif

#if __has_builtin(__builtin_prefetch)
#define PRP_HAS_BUILTIN_PREFETCH 1
#endif

#if __has_builtin(__builtin_memcpy)
#define PRP_HAS_BUILTIN_MEMCPY 1
#endif

#if __has_builtin(__builtin_memmove)
#define PRP_HAS_BUILTIN_MEMMOVE 1
#endif

#if __has_builtin(__builtin_memset)
#define PRP_HAS_BUILTIN_MEMSET 1
#endif

#if __has_builtin(__builtin_memcpy_inline)
#define PRP_HAS_BUILTIN_MEMCPY_INLINE 1
#endif

#if __has_builtin(__builtin_expect)
#define PRP_HAS_BUILTIN_EXPECT 1
#endif

#if __has_builtin(__builtin_assume)
#define PRP_HAS_BUILTIN_ASSUME 1
#endif

#if __has_builtin(__builtin_unreachable)
#define PRP_HAS_BUILTIN_UNREACHABLE 1
#endif

#if __has_builtin(__builtin_debugtrap)
#define PRP_HAS_BUILTIN_DEBUG_TRAP 1
#endif

#if __has_builtin(__builtin_trap)
#define PRP_HAS_BUILTIN_TRAP 1
#endif

#if __has_builtin(__builtin_cpu_init)
#define PRP_HAS_BUILTIN_CPU_INIT 1
#endif

#if __has_builtin(__builtin_cpu_supports)
#define PRP_HAS_BUILTIN_CPU_SUPPORTS 1
#endif

#if __has_builtin(__builtin_cpu_is)
#define PRP_HAS_BUILTIN_CPU_IS 1
#endif

#if __has_builtin(__atomic_load_n)
#define PRP_HAS_BUILTIN_ATOMIC_LOAD_N 1
#endif

#if __has_builtin(__atomic_store_n)
#define PRP_HAS_BUILTIN_ATOMIC_STORE_N 1
#endif

#if __has_builtin(__atomic_fetch_add)
#define PRP_HAS_BUILTIN_ATOMIC_FETCH_ADD 1
#endif

#if __has_builtin(__atomic_fetch_sub)
#define PRP_HAS_BUILTIN_ATOMIC_FETCH_SUB 1
#endif

#if __has_builtin(__atomic_compare_exchange_n)
#define PRP_HAS_BUILTIN_ATOMIC_COMPARE_EXCHANGE_N 1
#endif

#if __has_builtin(__atomic_thread_fence)
#define PRP_HAS_BUILTIN_ATOMIC_THREAD_FENCE 1
#endif

#if __has_builtin(__sync_fetch_and_add)
#define PRP_HAS_BUILTIN_SYNC_FETCH_AND_ADD 1
#endif

#if __has_builtin(__sync_fetch_and_sub)
#define PRP_HAS_BUILTIN_SYNC_FETCH_AND_SUB 1
#endif

#if __has_builtin(__sync_bool_compare_and_swap)
#define PRP_HAS_BUILTIN_SYNC_BOOL_COMPARE_AND_SWAP 1
#endif

#if __has_builtin(__builtin_fabs)
#define PRP_HAS_BUILTIN_FABS 1
#endif

#if __has_builtin(__builtin_sqrt)
#define PRP_HAS_BUILTIN_SQRT 1
#endif

#if __has_builtin(__builtin_sqrtf)
#define PRP_HAS_BUILTIN_SQRTF 1
#endif

#if __has_builtin(__builtin_sin)
#define PRP_HAS_BUILTIN_SIN 1
#endif

#if __has_builtin(__builtin_cos)
#define PRP_HAS_BUILTIN_COS 1
#endif

#if __has_builtin(__builtin_exp)
#define PRP_HAS_BUILTIN_EXP 1
#endif

#if __has_builtin(__builtin_log)
#define PRP_HAS_BUILTIN_LOG 1
#endif

#if __has_builtin(__builtin_add_overflow)
#define PRP_HAS_BUILTIN_ADD_OVERFLOW 1
#endif

#if __has_builtin(__builtin_sub_overflow)
#define PRP_HAS_BUILTIN_SUB_OVERFLOW 1
#endif

#if __has_builtin(__builtin_mul_overflow)
#define PRP_HAS_BUILTIN_MUL_OVERFLOW 1
#endif

#if __has_builtin(__builtin_bswap16)
#define PRP_HAS_BUILTIN_BSWAP16 1
#endif

#if __has_builtin(__builtin_bswap32)
#define PRP_HAS_BUILTIN_BSWAP32 1
#endif

#if __has_builtin(__builtin_bswap64)
#define PRP_HAS_BUILTIN_BSWAP64 1
#endif

#if __has_builtin(__builtin_bit_cast)
#define PRP_HAS_BUILTIN_BIT_CAST 1
#endif

#if __has_builtin(__builtin_constant_p)
#define PRP_HAS_BUILTIN_CONSTANT_P 1
#endif

#if __has_builtin(__builtin_object_size)
#define PRP_HAS_BUILTIN_OBJECT_SIZE 1
#endif

#endif

/* ----  ATTRIBUTE DETECTION ---- */

#ifdef __has_attribute

#if __has_attribute(always_inline)
#define PRP_HAS_ATTR_ALWAYS_INLINE 1
#endif

#if __has_attribute(noinline)
#define PRP_HAS_ATTR_NO_INLINE 1
#endif

#if __has_attribute(flatten)
#define PRP_HAS_ATTR_FLATTEN 1
#endif

#if __has_attribute(aligned)
#define PRP_HAS_ATTR_ALIGNED 1
#endif

#if __has_attribute(packed)
#define PRP_HAS_ATTR_PACKED 1
#endif

#if __has_attribute(hot)
#define PRP_HAS_ATTR_HOT 1
#endif

#if __has_attribute(cold)
#define PRP_HAS_ATTR_COLD 1
#endif

#if __has_attribute(optimize)
#define PRP_HAS_ATTR_OPTIMIZE 1
#endif

#if __has_attribute(visibility)
#define PRP_HAS_ATTR_VISIBILITY 1
#endif

#if __has_attribute(deprecated)
#define PRP_HAS_ATTR_DEPRECATED 1
#endif

#if __has_attribute(warn_unused_result)
#define PRP_HAS_ATTR_WARN_UNUSED_RESULT 1
#endif

#if __has_attribute(unused)
#define PRP_HAS_ATTR_UNUSED 1
#endif

#if __has_attribute(fallthrough)
#define PRP_HAS_ATTR_FALLTHROUGH 1
#endif

#if __has_attribute(malloc)
#define PRP_HAS_ATTR_MALLOC 1
#endif

#if __has_attribute(pure)
#define PRP_HAS_ATTR_PURE 1
#endif

#if __has_attribute(const)
#define PRP_HAS_ATTR_CONST 1
#endif

#if __has_attribute(returns_nonnull)
#define PRP_HAS_ATTR_RETURNS_NO_NULL 1
#endif

#if __has_attribute(noreturn)
#define PRP_HAS_ATTR_NO_RETURN 1
#endif

#if __has_attribute(no_sanitize)
#define PRP_HAS_ATTR_NO_SANITIZE 1
#endif

#if __has_attribute(no_sanitize_address)
#define PRP_HAS_ATTR_NO_SANITIZE_ADDRESS 1
#endif

#if __has_attribute(no_sanitize_thread)
#define PRP_HAS_ATTR_NO_SANITIZE_THREAD 1
#endif

#if __has_attribute(no_sanitize_memory)
#define PRP_HAS_ATTR_NO_SANITIZE_MEMORY 1
#endif

#if __has_attribute(ms_abi)
#define PRP_HAS_ATTR_MS_ABI 1
#endif

#if __has_attribute(sysv_abi)
#define PRP_HAS_ATTR_SYSV_ABI 1
#endif

#if __has_attribute(constructor)
#define PRP_HAS_ATTR_CONSTRUCTOR 1
#endif

#if __has_attribute(destructor)
#define PRP_HAS_ATTR_DESTRUCTOR 1
#endif

#if __has_attribute(used)
#define PRP_HAS_ATTR_USED 1
#endif

#if __has_attribute(weak)
#define PRP_HAS_ATTR_WEAK 1
#endif

#if __has_attribute(alias)
#define PRP_HAS_ATTR_ALIAS 1
#endif

#endif

/* ----  INCLUDE DETECTION ---- */

#ifdef __has_include

#if __has_include(<stdalign.h>)
#define PRP_HAS_INCLUDE_STDALIGN 1
#endif

#if __has_include(<stdarg.h>)
#define PRP_HAS_INCLUDE_STDARG 1
#endif

#if __has_include(<stdatomic.h>)
#define PRP_HAS_INCLUDE_STDATOMIC 1
#endif

#if __has_include(<stdbit.h>)
#define PRP_HAS_INCLUDE_STDBIT 1
#endif

#if __has_include(<stdbool.h>)
#define PRP_HAS_INCLUDE_STDBOOL 1
#endif

#if __has_include(<stddef.h>)
#define PRP_HAS_INCLUDE_STDDEF 1
#endif

#if __has_include(<stdint.h>)
#define PRP_HAS_INCLUDE_STDINT 1
#endif

#if __has_include(<stdnoreturn.h>)
#define PRP_HAS_INCLUDE_STDNORETURN 1
#endif

#if __has_include(<threads.h>)
#define PRP_HAS_INCLUDE_THREADS 1
#endif

#if __has_include(<uchar.h>)
#define PRP_HAS_INCLUDE_UCHAR 1
#endif

#if __has_include(<immintrin.h>)
#define PRP_HAS_INCLUDE_IMMINTRIN 1
#endif

#if __has_include(<xmmintrin.h>)
#define PRP_HAS_INCLUDE_XMMINTRIN 1
#endif

#if __has_include(<emmintrin.h>)
#define PRP_HAS_INCLUDE_EMMINTRIN 1
#endif

#if __has_include(<pmmintrin.h>)
#define PRP_HAS_INCLUDE_PMMINTRIN 1
#endif

#if __has_include(<tmmintrin.h>)
#define PRP_HAS_INCLUDE_TMMINTRIN 1
#endif

#if __has_include(<smmintrin.h>)
#define PRP_HAS_INCLUDE_SMMINTRIN 1
#endif

#if __has_include(<nmmintrin.h>)
#define PRP_HAS_INCLUDE_NMMINTRIN 1
#endif

#if __has_include(<wmmintrin.h>)
#define PRP_HAS_INCLUDE_WMMINTRIN 1
#endif

#if __has_include(<arm_neon.h>)
#define PRP_HAS_INCLUDE_ARM_NEON 1
#endif

#if __has_include(<arm_sve.h>)
#define PRP_HAS_INCLUDE_ARM_SVE 1
#endif

#if __has_include(<arm_sve2.h>)
#define PRP_HAS_INCLUDE_ARM_SVE2 1
#endif

#if __has_include(<cpuid.h>)
#define PRP_HAS_INCLUDE_CPUID 1
#endif

#if __has_include(<x86intrin.h>)
#define PRP_HAS_INCLUDE_X86INTRIN 1
#endif

#if __has_include(<windows.h>)
#define PRP_HAS_INCLUDE_WINDOWS 1
#endif

#if __has_include(<pthread.h>)
#define PRP_HAS_INCLUDE_PTHREAD 1
#endif

#if __has_include(<unistd.h>)
#define PRP_HAS_INCLUDE_UNISTD 1
#endif

#if __has_include(<sys/mman.h>)
#define PRP_HAS_INCLUDE_SYS_MMAN 1
#endif

#if __has_include(<sys/sysinfo.h>)
#define PRP_HAS_INCLUDE_SYS_SYSINFO 1
#endif

#if __has_include(<fcntl.h>)
#define PRP_HAS_INCLUDE_FCNTL 1
#endif

#if __has_include(<dlfcn.h>)
#define PRP_HAS_INCLUDE_DLFCN 1
#endif

#if __has_include(<vulkan/vulkan.h>)
#define PRP_HAS_INCLUDE_VULKAN 1
#endif

#if __has_include(<GL/gl.h>)
#define PRP_HAS_INCLUDE_GL 1
#endif

#if __has_include(<GLES3/gl3.h>)
#define PRP_HAS_INCLUDE_GL3 1
#endif

#if __has_include(<Metal/Metal.h>)
#define PRP_HAS_INCLUDE_METAL 1
#endif

#if __has_include(<d3d12.h>)
#define PRP_HAS_INCLUDE_D3D12 1
#endif

#if __has_include(<intrin.h>)
#define PRP_HAS_INCLUDE_INTRIN 1
#endif

#if __has_include(<execinfo.h>)
#define PRP_HAS_INCLUDE_EXEC_INFO 1
#endif

#if __has_include(<signal.h>)
#define PRP_HAS_INCLUDE_SIGNAL 1
#endif

#if __has_include(<setjmp.h>)
#define PRP_HAS_INCLUDE_SETJMP 1
#endif

#if __has_include(<fenv.h>)
#define PRP_HAS_INCLUDE_FENV 1
#endif

#endif

/* ----  SHARED LIBRARY SUPPORT ---- */

#if defined(PRP_PLATFORM_WINDOWS)
#define PRP_ATTR_EXPORT __declspec(dllexport)
#define PRP_ATTR_IMPORT __declspec(dllimport)

#else
#define PRP_ATTR_EXPORT __attribute__((visibility("default")))
#define PRP_ATTR_IMPORT
#endif

/* ---- API ---- */

#if defined(PRP_BUILD_DLL)
#define PRP_API PRP_ATTR_EXPORT

#elif defined(PRP_USE_DLL)
#define PRP_API PRP_ATTR_IMPORT

#else
#define PRP_API
#endif

/* ---- CALLING ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_CDECL __cdecl
#define PRP_STDCALL __stdcall
#define PRP_FASTCALL __fastcall

#else
#define PRP_CDECL
#define PRP_STDCALL
#define PRP_FASTCALL
#endif

#define PRP_CALL PRP_CDECL

/* ----  FORCEINLINE and NOINLINE ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_ATTR_FORCEINLINE __forceinline
#define PRP_ATTR_NOINLINE __declspec(noinline)

#else
#define PRP_ATTR_FORCEINLINE __attribute__((always_inline))
#define PRP_ATTR_NOINLINE __attribute__((noinline))
#endif

/* ---- RESTRICT ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_RESTRICT __restrict

#else
#define PRP_RESTRICT __restrict__
#endif

/* ---- UNREACHABLE ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_UNREACHABLE() __assume(0)

#else
#define PRP_UNREACHABLE() __builtin_unreachable()
#endif

/* ---- DEBUG BREAK ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_DEBUG_BREAK() __debugbreak()

#elif defined(PRP_HAS_BUILTIN_DEBUG_TRAP)
#define PRP_DEBUG_BREAK() __builtin_debugtrap()

#elif defined(PRP_HAS_BUILTIN_TRAP)
#define PRP_DEBUG_BREAK() __builtin_trap()

#else
#include <stdlib.h>
#define PRP_DEBUG_BREAK() abort()

#endif

/* ---- BRANCH PREDICTION ---- */

#if defined(PRP_COMPILER_GCC) || defined(PRP_COMPILER_CLANG)
#define PRP_LIKELY(x) __builtin_expect(!!(x), 1)
#define PRP_UNLIKELY(x) __builtin_expect(!!(x), 0)

#else
#define PRP_LIKELY(x) (x)
#define PRP_UNLIKELY(x) (x)
#endif

/* ---- ALIGNMENT ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_ATTR_ALIGN(x) __declspec(align(x))

#else
#define PRP_ATTR_ALIGN(x) __attribute__((aligned(x)))
#endif

/* ---- ASSUMPTION ---- */

#if defined(PRP_COMPILER_MSVC)
#define PRP_ASSUME(x) __assume(x)

#else
#define PRP_ASSUME(x)                                                          \
    do {                                                                       \
        if (!(x))                                                              \
            PRP_UNREACHABLE();                                                 \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif
