#if defined(DM_PLATFORM_WINDOWS) && !defined(DM_HEADLESS)

#include "filedrop.h"
#include <dmsdk/sdk.h>

#include <Windows.h>


WNDPROC engineWinProc;

OnEventCallback draggingCallback;


// https://github.com/gametutorials/tutorials/blob/master/Win32/Drag%20And%20Drop/Main.cpp
void HandleFiles(WPARAM wParam)
{
    // DragQueryFile() takes a LPWSTR for the name so we need a TCHAR string
    TCHAR szName[MAX_PATH];

    // Here we cast the wParam as a HDROP handle to pass into the next functions
    HDROP hDrop = (HDROP)wParam;

    int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

    for (int i = 0; i < count; i++)
    {
        DragQueryFile(hDrop, i, szName, MAX_PATH);

        dmLogInfo("Dragged %s", szName);
        draggingCallback("drop", szName, 0, 0);
    }

    // Finally, we destroy the HDROP handle so the extra memory
    // allocated by the application is released.
    DragFinish(hDrop);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        case WM_DROPFILES:
            HandleFiles(wParam);
            break;
    }
    return CallWindowProc (engineWinProc, hwnd, iMsg, wParam, lParam);
}

void FileDrop_PlatformSetEventListener(OnEventCallback callback) {
    draggingCallback = callback;
}

void FileDrop_PlatformAppInitialize()
{
    LONG_PTR style = GetWindowLongPtrA(dmGraphics::GetNativeWindowsHWND(), GWL_STYLE);
    style |= WS_EX_ACCEPTFILES;
    if (!SetWindowLongPtrA(dmGraphics::GetNativeWindowsHWND(), GWL_STYLE, style))
    {
        dmLogWarning("Unable to add WS_EX_ACCEPTFILES to window style");
        return;
    }

    engineWinProc = (WNDPROC)GetWindowLongPtrA(dmGraphics::GetNativeWindowsHWND(), GWLP_WNDPROC);
    if (!SetWindowLongPtrA(dmGraphics::GetNativeWindowsHWND(), GWLP_WNDPROC, (LONG_PTR)WndProc))
    {
        dmLogWarning("Unable to set GWLP_WNDPROC for window");
        return;
    }
}

void FileDrop_PlatformAppFinalize() {}
void FileDrop_PlatformFinalize() {}

#endif