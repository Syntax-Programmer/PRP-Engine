#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/**
    Tier 1 (implement now)
    Window System
     Win32
     X11
     Wayland
     Cocoa (macos only not ios)


     Tier 2 (later)
     Android
     iOS
     Web (Emscripten)


     Tier 3 (very far future)
     PS5
     Xbox
     Nintendo Switch
     Steam Deck specific APIs

     Those require vendor SDKs.

     Ignore them.
 */

typedef enum HLIX_WindowCreateFlagBits {
    HLIX_WINDOW_CREATE_FLAG_RESIZABLE_BIT = (1 << 0),
    HLIX_WINDOW_CREATE_FLAG_FULLSCREEN_BIT = (1 << 1),
    HLIX_WINDOW_CREATE_FLAG_BORDERLESS_BIT = (1 << 2),
    HLIX_WINDOW_CREATE_FLAG_MAXIMIZED_BIT = (1 << 3),
    HLIX_WINDOW_CREATE_FLAG_HIDDEN_BIT = (1 << 4),

    HLIX_WINDOW_CREATE_FLAG_MINIMIZED_BIT = (1 << 5),
    HLIX_WINDOW_CREATE_FLAG_FOCUSED_BIT = (1 << 6),
    HLIX_WINDOW_CREATE_FLAG_FLOATING_BIT = (1 << 7),
    HLIX_WINDOW_CREATE_FLAG_TRANSPARENT_BIT = (1 << 8),
    HLIX_WINDOW_CREATE_FLAG_HIGH_DPI_BIT = (1 << 9),

    HLIX_WINDOW_CREATE_FLAG_MOUSE_PASSTHROUGH_BIT = (1 << 10),
    HLIX_WINDOW_CREATE_FLAG_CENTERED_BIT = (1 << 11),
} HLIX_WindowCreateFlagBits;

typedef PRP_U32 HLIX_WindowCreateFlags;

typedef struct HLIX_WindowCreateInfo {
    const PRP_Char8 *pTitle;

    PRP_U32 width;
    PRP_U32 height;

    HLIX_WindowCreateFlags flags;
} HLIX_WindowCreateInfo;

typedef struct HLIX_Window HLIX_Window;
/* ---- CREATE / DESTROY ---- */

PRP_API PRP_Result PRP_CALL
HLIX_WindowCreate(HLIX_WindowCreateInfo *pCreate_info, HLIX_Window **ppWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowDestroy(HLIX_Window **ppWindow);

/* ---- WINDOW STATE ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowShouldClose(const HLIX_Window *pWindow,
                                                   PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL HLIX_WindowClose(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetFlags(const HLIX_Window *pWindow,
                                                HLIX_WindowCreateFlags *pFlags);
PRP_API PRP_Result PRP_CALL HLIX_WindowIsFocused(const HLIX_Window *pWindow,
                                                 PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL HLIX_WindowIsMinimized(const HLIX_Window *pWindow,
                                                   PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL HLIX_WindowIsMaximized(const HLIX_Window *pWindow,
                                                   PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL HLIX_WindowIsFullscreen(const HLIX_Window *pWindow,
                                                    PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL HLIX_WindowIsVisible(const HLIX_Window *pWindow,
                                                 PRP_Bool *pRslt);

/* ---- WINDOW VISIBILITY ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowShow(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowHide(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowMinimize(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowMaximize(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowRestore(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowFocus(HLIX_Window *pWindow);

/* ---- WINDOW SIZE & POSITION ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowSetSize(HLIX_Window *pWindow,
                                               PRP_U32 width, PRP_U32 height);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetSize(const HLIX_Window *pWindow,
                                               PRP_U32 *pWidth,
                                               PRP_U32 *pHeight);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetFramebufferSize(
    const HLIX_Window *pWindow, PRP_U32 *pWidth, PRP_U32 *pHeight);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetPosition(HLIX_Window *pWindow,
                                                   PRP_I32 x, PRP_I32 y);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetPosition(const HLIX_Window *pWindow,
                                                   PRP_I32 *pX, PRP_I32 *pY);
PRP_API PRP_Result PRP_CALL HLIX_WindowCenter(HLIX_Window *pWindow);

/* ---- WINDOW TITLE ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowSetTitle(HLIX_Window *pWindow,
                                                const PRP_Char8 *pTitle);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetTitle(const HLIX_Window *pWindow,
                                                const PRP_Char8 **ppTitle);

/* ---- WINDOW PROPERTIES ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowSetResizable(HLIX_Window *pWindow,
                                                    PRP_Bool resizable);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetBorderless(HLIX_Window *pWindow,
                                                     PRP_Bool borderless);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetFloating(HLIX_Window *pWindow,
                                                   PRP_Bool floating);
PRP_API PRP_Result PRP_CALL HLIX_WindowIsResizable(const HLIX_Window *pWindow,
                                                   PRP_Bool *pRslt);

/* ---- USER DATA ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowSetUserData(HLIX_Window *pWindow,
                                                   void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetUserData(const HLIX_Window *pWindow,
                                                   void **ppUser_data);

/* ---- CURSOR ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowSetCursorVisible(HLIX_Window *pWindow,
                                                        PRP_Bool visible);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetCursorPosition(HLIX_Window *pWindow,
                                                         PRP_F64 x, PRP_F64 y);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetCursorPosition(
    const HLIX_Window *pWindow, PRP_F64 *pX, PRP_F64 *pY);
PRP_API PRP_Result PRP_CALL HLIX_WindowCaptureCursor(HLIX_Window *pWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowReleaseCursor(HLIX_Window *pWindow);

/* ---- CLIPBOARD ---- */

PRP_API PRP_Result PRP_CALL
HLIX_WindowSetClipboardString(HLIX_Window *pWindow, const PRP_Char8 *pString);
PRP_API PRP_Result PRP_CALL HLIX_WindowGetClipboardString(
    const HLIX_Window *pWindow, const PRP_Char8 **ppString);

/* ---- EVENT PROCESSING ---- */

PRP_API PRP_Result PRP_CALL HLIX_PollEvents(void);
PRP_API PRP_Result PRP_CALL HLIX_WaitEvents(void);

/* ---- MONITORS ---- */

typedef struct HLIX_Monitor HLIX_Monitor;

PRP_API PRP_Result PRP_CALL HLIX_GetMonitorCount(PRP_Size *pCount);
PRP_API PRP_Result PRP_CALL HLIX_GetPrimaryMonitor(HLIX_Monitor **ppMonitor);
PRP_API PRP_Result PRP_CALL HLIX_MonitorGetName(const HLIX_Monitor *pMonitor,
                                                const PRP_Char8 **ppName);
PRP_API PRP_Result PRP_CALL HLIX_MonitorGetResolution(
    const HLIX_Monitor *pMonitor, PRP_U32 *pWidth, PRP_U32 *pHeight);
PRP_API PRP_Result PRP_CALL HLIX_MonitorGetRefreshRate(
    const HLIX_Monitor *pMonitor, PRP_U32 *pRefresh_rate);
PRP_API PRP_Result PRP_CALL HLIX_MonitorGetPhysicalSize(
    const HLIX_Monitor *pMonitor, PRP_U32 *pWidth_mm, PRP_U32 *pHeight_mm);
PRP_API PRP_Result PRP_CALL HLIX_MonitorGetDPI(const HLIX_Monitor *pMonitor,
                                               PRP_F32 *pDpi_x,
                                               PRP_F32 *pDpi_y);

/* ---- VULKAN ---- */

PRP_API PRP_Result PRP_CALL HLIX_WindowSupportsVulkan(PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL
HLIX_WindowGetNativeHandle(const HLIX_Window *pWindow, void **ppHandle);
PRP_API PRP_Result PRP_CALL HLIX_WindowCreateSurface(const HLIX_Window *pWindow,
                                                     VkInstance instance,
                                                     VkSurfaceKHR *pSurface);

/* ---- CALLBACKS ---- */

typedef void (*HLIX_WindowResizeCallback)(HLIX_Window *pWindow, PRP_U32 width,
                                          PRP_U32 height, void *pUser_data);
typedef void (*HLIX_WindowMoveCallback)(HLIX_Window *pWindow, PRP_I32 x,
                                        PRP_I32 y, void *pUser_data);
typedef void (*HLIX_WindowFocusCallback)(HLIX_Window *pWindow, PRP_Bool focused,
                                         void *pUser_data);
typedef void (*HLIX_WindowCloseCallback)(HLIX_Window *pWindow,
                                         void *pUser_data);
typedef void (*HLIX_MouseCallback)(HLIX_Window *pWindow, PRP_I32 button,
                                   PRP_I32 action, PRP_I32 mods,
                                   void *pUser_data);
typedef void (*HLIX_KeyCallback)(HLIX_Window *pWindow, PRP_I32 key,
                                 PRP_I32 scancode, PRP_I32 action, PRP_I32 mods,
                                 void *pUser_data);
typedef void (*HLIX_ScrollCallback)(HLIX_Window *pWindow, PRP_F64 xOffset,
                                    PRP_F64 yOffset, void *pUser_data);
typedef void (*HLIX_DropFileCallback)(HLIX_Window *pWindow, PRP_Size count,
                                      const PRP_Char8 *const *ppPaths,
                                      void *pUser_data);

PRP_API PRP_Result PRP_CALL HLIX_WindowSetResizeCallback(
    HLIX_Window *pWindow, HLIX_WindowResizeCallback callback, void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetMoveCallback(
    HLIX_Window *pWindow, HLIX_WindowMoveCallback callback, void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetFocusCallback(
    HLIX_Window *pWindow, HLIX_WindowFocusCallback callback, void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetCloseCallback(
    HLIX_Window *pWindow, HLIX_WindowCloseCallback callback, void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetMouseCallback(
    HLIX_Window *pWindow, HLIX_MouseCallback callback, void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetKeyCallback(HLIX_Window *pWindow,
                                                      HLIX_KeyCallback callback,
                                                      void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetScrollCallback(
    HLIX_Window *pWindow, HLIX_ScrollCallback callback, void *pUser_data);
PRP_API PRP_Result PRP_CALL HLIX_WindowSetDropFileCallback(
    HLIX_Window *pWindow, HLIX_DropFileCallback callback, void *pUser_data);

#ifdef __cplusplus
}
#endif
