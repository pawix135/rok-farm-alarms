#pragma once
#include "raylib_wrapper.h"

typedef struct HWND__* HWND;

namespace WinApp {
    void Init(HWND windowHandle, int width, int height);
    void Cleanup();
    void HideToTray();
    void RestoreFromTray();
    void ToggleVisibility();
    bool ShouldExit();
    void SendNotification(const char* title, const char* message);
    void SetWindowSize(int width, int height);
    int GetWindowWidth();
    int GetWindowHeight();
    void Exit();
}
