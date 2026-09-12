#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Helix/Internals/Typedefs.h"

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
    void *pResize_cb_user_data;

    HLIX_WindowMoveCb move_cb;
    void *pMove_cb_user_data;

    HLIX_WindowFocusCb focus_cb;
    void *pFocus_cb_user_data;

    HLIX_WindowCloseCb close_cb;
    void *pClose_cb_user_data;

    HLIX_MouseCb mouse_cb;
    void *pMouse_cb_user_data;

    HLIX_KeyCb key_cb;
    void *pKey_cb_user_data;

    HLIX_ScrollCb scroll_cb;
    void *pScroll_cb_user_data;

    HLIX_DropFileCb drop_file_cb;
    void *pDrop_file_cb_user_data;

    /* ---- GENERIC BACKEND ---- */

    void *pBackend;
    void *pWindow;
} HLIX_Window;

#ifdef __cplusplus
}
#endif
