#pragma once
#include "raylib_wrapper.h"

typedef struct HWND__* HWND;

namespace WinApp {
    void Init(HWND windowHandle);
    void Cleanup();
    void HideToTray();
    void RestoreFromTray();
    void ToggleVisibility();
    bool ShouldExit();
    void SendNotification(const char* title, const char* message);
    void Exit();
}
