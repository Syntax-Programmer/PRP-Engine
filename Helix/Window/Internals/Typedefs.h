#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/* ---- WINDOW FLAGS ---- */

typedef enum HLIX_WindowFlagBit {
    HLIX_WINDOW_FLAG_RESIZABLE_BIT = (1 << 0),
    HLIX_WINDOW_FLAG_FULLSCREEN_BIT = (1 << 1),
    HLIX_WINDOW_FLAG_BORDERLESS_BIT = (1 << 2),
    HLIX_WINDOW_FLAG_MAXIMIZED_BIT = (1 << 3),
    HLIX_WINDOW_FLAG_HIDDEN_BIT = (1 << 4),

    HLIX_WINDOW_FLAG_MINIMIZED_BIT = (1 << 5),
    HLIX_WINDOW_FLAG_FOCUSED_BIT = (1 << 6),
    HLIX_WINDOW_FLAG_FLOATING_BIT = (1 << 7),
    HLIX_WINDOW_FLAG_TRANSPARENT_BIT = (1 << 8),
    HLIX_WINDOW_FLAG_HIGH_DPI_BIT = (1 << 9),

    HLIX_WINDOW_FLAG_MOUSE_PASSTHROUGH_BIT = (1 << 10),
    HLIX_WINDOW_FLAG_CENTERED_BIT = (1 << 11),
} HLIX_WindowFlagBit;

typedef PRP_U32 HLIX_WindowFlags;

/* ---- WINDOW FORWARD DECL ---- */

#define HLIX_MAX_WINDOW_TITLE_SIZE (64)

typedef struct HLIX_Window HLIX_Window;

/* ---- CALLBACKS ---- */

typedef void (*HLIX_WindowResizeCb)(HLIX_Window *pWindow, PRP_U32 width,
                                    PRP_U32 height, void *pUser_data);
typedef void (*HLIX_WindowMoveCb)(HLIX_Window *pWindow, PRP_I32 x, PRP_I32 y,
                                  void *pUser_data);
typedef void (*HLIX_WindowFocusCb)(HLIX_Window *pWindow, PRP_Bool focused,
                                   void *pUser_data);
typedef void (*HLIX_WindowCloseCb)(HLIX_Window *pWindow, void *pUser_data);
typedef void (*HLIX_MouseCb)(HLIX_Window *pWindow, PRP_I32 button,
                             PRP_I32 action, PRP_I32 mods, void *pUser_data);
typedef void (*HLIX_KeyCb)(HLIX_Window *pWindow, PRP_I32 key, PRP_I32 scancode,
                           PRP_I32 action, PRP_I32 mods, void *pUser_data);
typedef void (*HLIX_ScrollCb)(HLIX_Window *pWindow, PRP_F64 xOffset,
                              PRP_F64 yOffset, void *pUser_data);
typedef void (*HLIX_DropFileCb)(HLIX_Window *pWindow, PRP_Size count,
                                const PRP_Char8 *const *ppPaths,
                                void *pUser_data);

#ifdef __cplusplus
}
#endif
