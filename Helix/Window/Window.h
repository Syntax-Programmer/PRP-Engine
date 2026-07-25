#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

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

typedef struct _Window HLIX_Window;

PRP_API PRP_Result PRP_CALL
HLIX_WindowCreate(HLIX_WindowCreateInfo *pCreate_info, HLIX_Window **ppWindow);
PRP_API PRP_Result PRP_CALL HLIX_WindowDestroy(HLIX_Window **ppWindow);

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

/*
 * void HLIX_WindowShow();
 void HLIX_WindowHide();

 void HLIX_WindowMinimize();
 void HLIX_WindowMaximize();

 void HLIX_WindowRestore();

 void HLIX_WindowFocus();
 void HLIX_WindowSetSize(
     HLIX_Window*,
     PRP_U32 width,
     PRP_U32 height);

 void HLIX_WindowGetSize(
     HLIX_Window*,
     PRP_U32* width,
     PRP_U32* height);

void HLIX_WindowGetFramebufferSize(...);
void HLIX_WindowSetPosition(...);
void HLIX_WindowGetPosition(...);
void HLIX_WindowCenter(...);

void HLIX_WindowSetTitle(
    HLIX_Window*,
    const PRP_Char8* title);

const PRP_Char8 *
HLIX_WindowGetTitle(...);

void HLIX_WindowSetResizable(...);
void HLIX_WindowSetBorderless(...);
void HLIX_WindowSetFloating(...);

PRP_Bool HLIX_WindowIsResizable(...);

void HLIX_WindowSetUserData(
    HLIX_Window*,
    void* data);

void *HLIX_WindowGetUserData(...);

void HLIX_WindowSetCursorVisible(...);

void HLIX_WindowSetCursorPos(...);

void HLIX_WindowGetCursorPos(...);

void HLIX_WindowCaptureCursor(...);

void HLIX_WindowReleaseCursor(...);

void HLIX_WindowSetClipboardString(...);

const PRP_Char8 *
HLIX_WindowGetClipboardString(...);

void HLIX_PollEvents();
void HLIX_WaitEvents();

PRP_Size HLIX_GetMonitorCount();

HLIX_Monitor *
HLIX_GetPrimaryMonitor();

const PRP_Char8 *
HLIX_MonitorGetName();

Resolution
Refresh Rate
Physical Size
DPI

VkSurfaceKHR
HLIX_WindowCreateSurface(
    HLIX_Window*,
    VkInstance);
PRP_Bool
HLIX_WindowSupportsVulkan();

void *
HLIX_WindowGetNativeHandle();

ResizeCallback
MoveCallback
FocusCallback
CloseCallback
MouseCallback
KeyCallback
ScrollCallback
DropFileCallback

void HLIX_WindowSetResizeCallback(...);
void HLIX_WindowSetCloseCallback(...);
*/

#ifdef __cplusplus
}
#endif
