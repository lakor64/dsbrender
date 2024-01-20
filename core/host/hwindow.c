/*
 *
 * $Id: hwindow.c 1.1 2023/14/08 17:08:50 chry Exp $
 * $Locker: $
 *
 * Window managment (brender fork)
 */
#include "host.h"
#include "hstvk.h"

#ifdef __WIN_32__
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

extern br_uint_8 show_count;
extern br_uint_8 cursor_state;

// top window
HWND top_window = NULL;

// custom id of the window status
#define GWLP_WINDOWSTATUS 0

static BR_FORCEINLINE void HostGenClassName(LPCTSTR title, LPTSTR className)
{
    BrMemCpy(className, title, 50 * sizeof(TCHAR));
    className[50] = _T('_');
    // TODO: random
}

//****************************************************************************
//*                                                                          *
//*  Return rectangle containing client-area boundaries in screenspace       *
//*                                                                          *
//****************************************************************************

static LPRECT client_screen_rect(HWND hwnd, LPRECT rect)
{
    POINT        ul, lr;

    GetClientRect(hwnd, rect);

    ul.x = rect->left;
    ul.y = rect->top;
    lr.x = rect->right;
    lr.y = rect->bottom;

    if (GetMenu(hwnd) != NULL)
    {
        ul.y -= GetSystemMetrics(SM_CYMENU);
    }

    ClientToScreen(hwnd, &ul);
    ClientToScreen(hwnd, &lr);

    SetRect(rect, ul.x, ul.y,
        lr.x - 1, lr.y - 1);

    return rect;
}

static LRESULT CALLBACK EmptyWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

static LRESULT CALLBACK HostWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    host_window_status* status = GetWindowLongPtr(hWnd, GWLP_WINDOWSTATUS);

    switch (Msg)
    {
    case WM_SETFOCUS:
        if ((status->current_display_mode == BR_MODE_WINDOW) && status->constrain_state)
            status->constrain_request = 1;

        top_window = hWnd;

        if (status->callbacks.on_focus)
            status->callbacks.on_focus(BR_TRUE);

        break; // pass to callback

    case WM_KILLFOCUS:
        if ((status->current_display_mode == BR_MODE_WINDOW) && status->constrain_state)
        {
            br_rectangle* r2 = &status->unconstrained_rect;
            RECT r = { r2->x, r2->y, r2->w, r2->h };
            ClipCursor(&r);
            status->constrain_request = 0;
        }

        if (status->callbacks.on_focus)
            status->callbacks.on_focus(BR_FALSE);

        top_window = NULL;

        break; // pass to callback

    case WM_ACTIVATEAPP:
        if (wParam)
        {
            if (!status->app_active && (!status->WPS_lock))
            {
                status->app_active = BR_TRUE;
                if (status->current_display_mode != BR_MODE_WINDOW)
                {
                    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SYSMENU & ~WS_CAPTION);
                }
            }
        }
        else
        {
            if (status->app_active && (!status->WPS_lock))
            {
                status->app_active = BR_FALSE;
                if (status->current_display_mode != BR_MODE_WINDOW)
                {
                    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_SYSMENU | WS_CAPTION);
                }

               //
               // We are switching tasks, so if the application has hidden
               // the mouse cursor, restore it so that it will not vanish
               // if the user moves the mouse over the deactivated app window
               //
               // Cursor will be hidden again the first time the user moves 
               // the mouse inside the app window's client area, after the 
               // app has received the input focus once again.
               //

                if (cursor_state)
                {
                    cursor_state = 1;
                    ShowCursor(TRUE);
                }
            }
        }
        break; // pass to callback

    case WM_WINDOWPOSCHANGED:
        if ((status->current_display_mode == BR_MODE_WINDOW) && (!status->WPS_lock))
        {
            // If deactivating or minimizing, unconstrain the cursor

            if (((LPWINDOWPOS)lParam)->flags & SWP_NOACTIVATE)
            {
                if (status->constrain_state)
                {
                    br_rectangle* r2 = &status->unconstrained_rect;
                    RECT r = { r2->x, r2->y, r2->w, r2->h };
                    ClipCursor(&r);
                    status->constrain_request = 0;
                }
            }
            else {
                if (status->constrain_state)
                    status->constrain_request = 1;
            }

        }
        break; // pass to callback

    case WM_MENUCHAR:
        // Inhibit beep when requesting fullscreen/windowed-mode toggle
        return MNC_CLOSE << 16;

    case WM_SYSCOMMAND:
        switch (wParam & 0xFFFF0)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            // Inhibit all screen-saver activity
            return 0;

        case SC_MINIMIZE:
            //
            // Beware of Win95 bug: This message is NOT sent if the 
            // user selects "Minimize All Windows" from the taskbar menu.
            // An alternate way to detect minimization in general appears
            // to be the SWP_NOACTIVATE flag test in WM_WINDOWPOSCHANGED
            // above.
            //
            status->app_minimized = BR_TRUE;
            break;

        case SC_RESTORE:
            status->app_minimized = BR_FALSE;
            break;

        case SC_MAXIMIZE:
            status->app_minimized = BR_FALSE;

            if (status->current_display_mode == BR_MODE_WINDOW)
            {
                if (status->flags & BR_WINDOW_FLAG_ENABLE_MODE_TOGGLE)
                {
                    status->mode_change_request = BR_TRUE;
                    SetFocus(hWnd);
                    return 0;
                }
                else
                {
                    SetFocus(hWnd);
                }
            }
            top_window = hWnd;
            break;

        default:
            break;
        }

        return FALSE;

    case WM_NCMOUSEMOVE:
        if (status->current_display_mode == BR_MODE_WINDOW)
        {
            // Mouse has moved into menu area of active window -- show it 
            // regardless of show_count, so that menu items can be selected

            if (status->app_active && !cursor_state)
            {
                cursor_state = 1;
                ShowCursor(TRUE);
            }
        }
        
        return FALSE;

    case WM_MOUSEMOVE:
        if (status->current_display_mode == BR_MODE_WINDOW)
        {
            if (status->app_active)
            {
                //
                // Mouse has moved into window client area -- hide it if 
                // application is active, cursor is currently visible,
                // and show_count is less than 1
                //
                // Fall through to process MOUSE_event() case
                //

                if (cursor_state && (show_count < 1))
                {
                    cursor_state = 0;
                    ShowCursor(FALSE);
                }

                //
                // If mouse constraint request is pending, constrain the mouse
                // as soon as it moves into the client area
                //
                // We do not want to constrain the mouse unless the pointer 
                // is in the client area, because problems would occur when 
                // the user right-clicks on the app's taskbar entry and causes 
                // the app to regain focus indirectly
                //

                if (status->constrain_request)
                {
                    RECT rect;
                    ClipCursor(client_screen_rect(hWnd, &rect));
                    status->constrain_request = 0;
                }
            }
        }

        // IT'S INTENTIONAL THAT THERE IS NO BREAK HERE!
        // We want to execute the LBUTTONDOWN callback

    case WM_LBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_RBUTTONDOWN:
        if (status->callbacks.on_mouse_move)
            status->callbacks.on_mouse_move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        if (status->callbacks.on_mouse_key_down)
        {
            if (wParam & MK_LBUTTON)
                status->callbacks.on_mouse_key_down(BR_MOUSE_BUTTON_LEFT);
            if (wParam & MK_RBUTTON)
                status->callbacks.on_mouse_key_down(BR_MOUSE_BUTTON_RIGHT);
            if (wParam & MK_MBUTTON)
                status->callbacks.on_mouse_key_down(BR_MOUSE_BUTTON_MIDDLE);
            if (wParam & MK_XBUTTON1)
                status->callbacks.on_mouse_key_down(BR_MOUSE_BUTTON_X1);
            if (wParam & MK_XBUTTON2)
                status->callbacks.on_mouse_key_down(BR_MOUSE_BUTTON_X2);
        }
        
        return FALSE;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_XBUTTONUP:
        if (status->callbacks.on_mouse_move)
            status->callbacks.on_mouse_move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        if (status->callbacks.on_mouse_key_up)
        {
            if (wParam & MK_LBUTTON)
                status->callbacks.on_mouse_key_up(BR_MOUSE_BUTTON_LEFT);
            if (wParam & MK_RBUTTON)
                status->callbacks.on_mouse_key_up(BR_MOUSE_BUTTON_RIGHT);
            if (wParam & MK_MBUTTON)
                status->callbacks.on_mouse_key_up(BR_MOUSE_BUTTON_MIDDLE);
            if (wParam & MK_XBUTTON1)
                status->callbacks.on_mouse_key_up(BR_MOUSE_BUTTON_X1);
            if (wParam & MK_XBUTTON2)
                status->callbacks.on_mouse_key_up(BR_MOUSE_BUTTON_X2);
        }
        
        return FALSE;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        if (status->callbacks.on_key_down)
            status->callbacks.on_key_down(HostVirtualKeyToBrKeys(wParam), HostVirtualKeyToBrModes(wParam));

        if (wParam == VK_MENU && (status->flags & BR_WINDOW_FLAG_PREVENT_ALT_MENU_POPUP))
        {
            //
            // If key pressed was ALT by itself, set wParam to 0 (invalid
            // key code) to prevent system menu from popping up and 
            // interfering with app
            //

            wParam = 0;
        }
        
        return FALSE;

    case WM_KEYUP:
        if (wParam == VK_F11 && (status->flags & BR_WINDOW_FLAG_ENABLE_MODE_TOGGLE))
        {
            // IE compatibility
            status->mode_change_request = BR_TRUE;
        }

        if (status->callbacks.on_key_up)
            status->callbacks.on_key_up(HostVirtualKeyToBrKeys(wParam), HostVirtualKeyToBrModes(wParam));

        return FALSE;

    case WM_CHAR:
        if (status->callbacks.on_text)
            status->callbacks.on_text(wParam);

        return FALSE;

    case WM_SYSKEYUP:
        if (wParam == VK_RETURN && (status->flags & BR_WINDOW_FLAG_ENABLE_MODE_TOGGLE))
        {
            status->mode_change_request = BR_TRUE;
        }

        if (status->callbacks.on_key_up)
            status->callbacks.on_key_up(HostVirtualKeyToBrKeys(wParam), HostVirtualKeyToBrModes(wParam));

        if (wParam == VK_MENU && (status->flags & BR_WINDOW_FLAG_PREVENT_ALT_MENU_POPUP))
        {
            //
            // If key pressed was ALT by itself, set wParam to 0 (invalid
            // key code) to prevent system menu from popping up and 
            // interfering with app
            //

            wParam = 0;
        }
        return FALSE;

    case WM_DESTROY:
        status->app_active = BR_FALSE;
        break;

    case WM_QUERYNEWPALETTE:
        break; // request wndproc custom handling

    case WM_PALETTECHANGED:
        break; // request wndproc custom handling

    case WM_CREATE: // setup the window status
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)wParam;
        host_window_init_info* init = (host_window_init_info*)cs->lpCreateParams;
        BrMemSet(status, 0, sizeof(host_window_status));

        status->app_active = BR_TRUE;
        status->window_proc = EmptyWindowProc; // default fallback
        status->callbacks = init->callbacks;
        status->current_display_mode = init->mode;
        status->flags = init->flags;
        status->app_minimized = (br_boolean)init->state == BR_WINDOW_STATE_MINIMIZED;
        status->current_height = cs->cy;
        status->current_width = cs->cx;

        return FALSE; // avoid passing to status as it's just an empty callback
    }

    case WM_SIZE:
        status->current_width = LOWORD(lParam);
        status->current_width = HIWORD(lParam);
        break; // pass to callback

    case WM_PAINT:
        //
        // If part of window is invalidated while app is not in foreground,
        // re-copy last DIB image to window
        //
        if (status->current_display_mode == BR_MODE_WINDOW && !status->app_active && (status->flags & BR_WINDOW_FLAG_REFRESH_WHILE_SLEEP) > 0)
        {
            break; // pass WM_PAINT
        }

        //
        // Windows sends a spurious WM_PAINT to the app when task-switching
        // away from a DirectDraw fullscreen app.  Swallow it, so the app
        // doesn't try to re-lock the surface to refresh the screen...
        //

        if (!status->app_active)
        {
            return DefWindowProc(hWnd, Msg, wParam, lParam);
        }

        return FALSE; // don't pass WM_PAINT to app (note: this might break GDI draw? that's what SAL was doing)

    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return ((WNDPROC)status->window_proc)(hWnd, Msg, wParam, lParam);
}


br_window BR_RESIDENT_ENTRY HostWindowCreate(host_window_init_info* info)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    DWORD style = WS_OVERLAPPED;
    TCHAR className[100];

    HostGenClassName(info->window_title, className);

    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc = HostWindowProc;
    wcex.lpszClassName = className;
    wcex.hInstance = (HINSTANCE)info->instance;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(host_window_status);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hIcon = LoadCursor(NULL, IDI_APPLICATION);
    wcex.hIconSm = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszMenuName = NULL;
    wcex.style = CS_VREDRAW | CS_HREDRAW;

    if (!RegisterClassEx(&wcex))
        return NULL;

    if (info->mode == BR_MODE_DESKTOP)
    {
        info->rect.w = GetSystemMetrics(SM_CXSCREEN);
        info->rect.h = GetSystemMetrics(SM_CYSCREEN);
        info->rect.x = 0;
        info->rect.y = 0;
    }

    if (info->mode == BR_MODE_WINDOW)
      style |= WS_CAPTION | WS_SYSMENU;
    if (info->flags & BR_WINDOW_FLAG_RESIZABLE)
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    if (info->flags & BR_WINDOW_FLAG_MINIMIZABLE)
        style |= WS_MINIMIZEBOX;
    if (info->flags & BR_WINDOW_FLAG_POPUP)
        style |= WS_POPUP;

    hwnd = CreateWindow(className, info->window_title, style, info->rect.x, info->rect.y, info->rect.w, info->rect.h, (HWND)info->parent, NULL, wcex.hInstance, (LPARAM)info);
    
    if (!hwnd)
    {
        UnregisterClass(className, wcex.hInstance);
        return NULL;
    }

    UpdateWindow(hwnd);

    switch (info->state)
    {
    case BR_WINDOW_STATE_HIDDEN:
        ShowWindow(hwnd, SW_HIDE);
        break;
    case BR_WINDOW_STATE_NORMAL:
        ShowWindow(hwnd, SW_SHOW);
        SetFocus(hwnd);
        top_window = hwnd;
        break;
    case BR_WINDOW_STATE_MAXIMIZED:
        ShowWindow(hwnd, SW_MAXIMIZE);
        SetFocus(hwnd);
        top_window = hwnd;
        break;
    case BR_WINDOW_STATE_MINIMIZED:
        ShowWindow(hwnd, SW_MINIMIZE);
        break;
    default:
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        break;
    }

    return hwnd;
}

void BR_RESIDENT_ENTRY HostWindowSetPos(br_window window, br_int_32 x,
    br_int_32 y)
{
    SetWindowPos(window, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void BR_RESIDENT_ENTRY HostWindowSetSize(br_window window, br_int_32 width,
    br_int_32 height)
{
    SetWindowPos(window, NULL, 0, 0, width, height, SWP_NOREPOSITION | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void BR_RESIDENT_ENTRY HostWindowSetState(br_window window, br_int_32 state)
{
    switch (state)
    {
    case BR_WINDOW_STATE_HIDDEN:
        ShowWindow((HWND)window, SW_HIDE);
        break;
    case BR_WINDOW_STATE_NORMAL:
        ShowWindow((HWND)window, SW_SHOW);
        break;
    case BR_WINDOW_STATE_MAXIMIZED:
        ShowWindow((HWND)window, SW_MAXIMIZE);
        break;
    case BR_WINDOW_STATE_MINIMIZED:
        ShowWindow((HWND)window, SW_MINIMIZE);
        break;
    default:
        ShowWindow((HWND)window, SW_SHOWDEFAULT);
        break;
    }
}

void BR_RESIDENT_ENTRY HostWindowDestroy(br_window window)
{
    HINSTANCE hInst;
    ATOM atom;

    atom = GetClassLongPtr((HWND)window, GCW_ATOM);
    hInst = GetWindowLongPtr((HWND)window, GWLP_HINSTANCE);
    DestroyWindow((HWND)window);
    UnregisterClass(MAKEINTATOM(atom), hInst);
}

br_boolean BR_RESIDENT_ENTRY HostWindowIsActive(br_window window)
{
    return ((host_window_status*)GetWindowLongPtr((HWND)window, GWLP_WINDOWSTATUS))->app_active;
}

host_window_status* BR_RESIDENT_ENTRY HostWindowGetStatus(br_window window)
{
    return (host_window_status*)GetWindowLongPtr((HWND)window, GWLP_WINDOWSTATUS);
}

br_uint_8 BR_RESIDENT_ENTRY HostWindowGetDisplayMode(br_window window)
{
    return ((host_window_status*)GetWindowLongPtr((HWND)window, GWLP_WINDOWSTATUS))->current_display_mode;
}

void BR_RESIDENT_ENTRY HostWindowSetTop(br_window window)
{
    SetWindowPos((HWND)window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOREPOSITION | SWP_NOSIZE);
    top_window = (HWND)window;
}

void BR_RESIDENT_ENTRY HostWindowGetClientArea(br_window window, br_rectangle* area)
{
    RECT rect;
    host_window_status* status = GetWindowLongPtr((HWND)window, GWLP_WINDOWSTATUS);

    if (status->current_display_mode == BR_MODE_WINDOW)
    {
        client_screen_rect((HWND)window, &rect);

        area->x = rect.left;
        area->y = rect.top;
        area->w = rect.right - rect.left + 1;
        area->h = rect.bottom - rect.top + 1;
    }
    else
    {
        area->x = 0;
        area->y = 0;
        area->w = status->current_width;
        area->h = status->current_height;
    }
}

void BR_RESIDENT_ENTRY HostWindowGetArea(br_window window, br_rectangle* area)
{
    RECT window_rect;
    host_window_status* status = GetWindowLongPtr((HWND)window, GWLP_WINDOWSTATUS);

    if (status->current_display_mode == BR_MODE_WINDOW)
    {
        GetWindowRect((HWND)window, &window_rect);

        area->x = window_rect.left;
        area->y = window_rect.top;
        area->w = window_rect.right - window_rect.left + 1;
        area->h = window_rect.bottom - window_rect.top + 1;
    }
    else
    {
        area->x = 0;
        area->y = 0;
        area->w = status->current_width;
        area->h = status->current_height;
    }
}


void BR_RESIDENT_ENTRY HostAlertBox(const char* title, const char* text)
{
    MessageBoxA(top_window, text, title, MB_ICONERROR);
}

#endif
