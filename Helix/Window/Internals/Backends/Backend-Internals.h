#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"
#include "Helix/Window/Internals/Typedefs.h"

typedef struct HLIX_Window {
    /* ---- GENERIC STATE ---- */

    HLIX_WindowFlags flags;

    PRP_I32 x, y;
    PRP_U32 w, h;
    PRP_Bool should_close;

    PRP_Char8 pTitle[HLIX_MAX_WINDOW_TITLE_SIZE];
    void *pUser_data;

    /* ---- CALLBACKS ---- */

    HLIX_WindowResizeCb resize_cb;
    HLIX_WindowMoveCb move_cb;
    HLIX_WindowFocusCb focus_cb;
    HLIX_WindowCloseCb close_cb;
    HLIX_MouseCb mouse_cb;
    HLIX_KeyCb key_cb;
    HLIX_ScrollCb scroll_cb;
    HLIX_DropFileCb drop_file_cb;

    /* ---- CALLBACK USER DATA ---- */

    void *pResize_cb_user_data;
    void *pMove_cb_user_data;
    void *pFocus_cb_user_data;
    void *pClose_cb_user_data;
    void *pMouse_cb_user_data;
    void *pKey_cb_user_data;
    void *pScroll_cb_user_data;
    void *pDrop_file_cb_user_data;

    /* ---- GENERIC BACKEND ---- */

    void *pBackend;
} HLIX_Window;

#ifdef __cplusplus
}
#endif
