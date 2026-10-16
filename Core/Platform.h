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
 * PRP_WINDOW_BACKEND_USE_WAYLAND
 *     Indicates that PRP is run on linux, and the user specifically wants to
 *     use Wayland window backend, otherwise default window backend for linux is
 *     x11.
 * PRP_BUILD_DLL
 *     Indicates that PRP itself is being built as a shared library.
 *     Public symbols are exported.
 *
 * PRP_USE_DLL
 *     Indicates that PRP is being linked against as a shared library.
 *     Public symbols are imported.
 */

/* ----  COMPILE MODE SELECTION ---- */

#ifdef PRP_NDEBUG
    #define PRP_RELEASE_MODE 1
#else
    #define PRP_DEBUG_MODE 1
#endif

/* ----  OS DETECTION ---- */

#if defined(_WIN32) || defined(_WIN64)
    #define PRP_PLATFORM_WINDOW 1

#elifdef __ANDROID__
    #define PRP_PLATFORM_ANDROID 1

#elifdef __EMSCRIPTEN__
    #define PRP_PLATFORM_WEB 1

#elifdef __linux__
    #define PRP_PLATFORM_LINUX 1

#elifdef __APPLE__
    #include <TargetConditionals.h>
    #ifdef TARGET_OS_IPHONE
        #define PRP_PLATFORM_IOS 1
    #else
        #define PRP_PLATFORM_MACOS 1
    #endif

#else
    #error Unsupported platform detected.
#endif

/* ---- WINDOW SYSTEM BACKEND DETECTION ---- */

#ifdef PRP_PLATFORM_WINDOWS
    #define PRP_WINDOW_BACKEND_WIN32 1

#elifdef PRP_PLATFORM_MACOS
    #define PRP_WINDOW_BACKEND_COCOA 1

#elifdef PRP_PLATFORM_LINUX
    #ifdef PRP_WINDOW_BACKEND_USE_WAYLAND
        #define PRP_WINDOW_BACKEND_WAYLAND 1
    #else
        #define PRP_WINDOW_BACKEND_USE_X11 1
    #endif

#elifdef PRP_PLATFORM_ANDROID
    #define PRP_WINDOW_BACKEND_ANDROID 1

#elifdef PRP_PLAFORM_IOS
    #define PRP_WINDOW_BACKEND_UIKIT 1

#elifdef PRP_PLATFORM_WEB
    #define PRP_WINDOW_BACKEND_EMSCRIPTEN 1

#else
    #error Unsupported window backend detected.
#endif

/* ---- COMPILER DETECTION ---- */

#ifdef __clang__
    #define PRP_COMPILER_CLANG 1
#elifdef __GNUC__
    #define PRP_COMPILER_GCC 1
#elifdef _MSC_VER
    #define PPR_COMPILER_MSVC 1
#else
    #error Unsupported compiler detected.
#endif

/* ---- C STANDARD DETECTION ---- */

#ifdef __cplusplus
/* Compiling as C++, skip C standard checks. */
#else
    #if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L)
        #error PRP requires C23 or newer.
    #elif __STDC_VERSION__ >= 202311L
        #define PRP_STANDARD_C23 1
    #endif
#endif

/* ----  SHARED LIBRARY SUPPORT ---- */

#ifdef PRP_PLATFORM_WINDOWS
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

#ifdef PRP_COMPILER_MSVC
    #define PRP_CDECL    __cdecl
    #define PRP_STDCALL  __stdcall
    #define PRP_FASTCALL __fastcall
#else
    #define PRP_CDECL
    #define PRP_STDCALL
    #define PRP_FASTCALL
#endif

#define PRP_CALL PRP_CDECL

/* ----  VARADIC ARG ATTR THING ---- */

#if defined(PRP_COMPILER_CLANG) || defined(PRP_COMPILER_GCC)
    #define PRP_PRINTF_FORMAT(fmt_idx, arg_idx)                                \
        __attribute__((format(printf, fmt_idx, arg_idx)))
#else
    #define PRP_PRINTF_FORMAT(fmt_idx, arg_idx)
#endif

/* ----  FORCEINLINE and NOINLINE ---- */

#ifdef PRP_COMPILER_MSVC
    #define PRP_ATTR_FORCEINLINE __forceinline
    #define PRP_ATTR_NOINLINE    __declspec(noinline)

#elif defined(PRP_COMPILER_CLANG) || defined(PRP_COMPILER_GCC)
    #define PRP_ATTR_FORCEINLINE __attribute__((always_inline))
    #define PRP_ATTR_NOINLINE    __attribute__((noinline))

#else
    #define PRP_ATTR_FORCEINLINE
    #define PRP_ATTR_NOINLINE
#endif

/* ---- RESTRICT ---- */

#if defined(PRP_COMPILER_MSVC)
    #define PRP_RESTRICT __restrict
#else
    #define PRP_RESTRICT restrict
#endif

/* ---- ABORT ---- */

#define PRP_ABORT() abort()

/* ---- UNREACHABLE ---- */

#if defined(PRP_COMPILER_MSVC)
    #define PRP_UNREACHABLE() __assume(0)

#elif defined(PRP_COMPILER_CLANG) || defined(PRP_COMPILER_GCC)
    #define PRP_UNREACHABLE() __builtin_unreachable()

#else
    #define PRP_UNREACHABLE() abort()
#endif

/* ---- DEBUG BREAK ---- */

#ifdef _MSC_VER
    #include <intrin.h>
    #define PRP_DEBUG_BREAK() __debugbreak()

#elifdef __has_builtin
    #if __has_builtin(__builtin_debugtrap)
        #define PRP_DEBUG_BREAK() __builtin_debugtrap()
    #elif __has_builtin(__builtin_trap)
        #define PRP_DEBUG_BREAK() __builtin_trap()
    #else
        #define PRP_DEBUG_BREAK() abort()
    #endif

#elif defined(__clang__) || defined(__GNUC__)
    #define PRP_DEBUG_BREAK() __builtin_trap()

#else
    #define PRP_DEBUG_BREAK() abort()
#endif

/* ---- BRANCH PREDICTION ---- */

#if defined(PRP_COMPILER_GCC) || defined(PRP_COMPILER_CLANG)
    #define PRP_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define PRP_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define PRP_LIKELY(x)   (x)
    #define PRP_UNLIKELY(x) (x)
#endif

/* ---- ASSUMPTION ---- */

#if defined(PRP_COMPILER_MSVC)
    #define PRP_ASSUME(x) __assume(x)
#else
    #define PRP_ASSUME(x)                                                      \
        do {                                                                   \
            if (!(x))                                                          \
                PRP_UNREACHABLE();                                             \
        } while (0)
#endif

#ifdef __cplusplus
}
#endif
