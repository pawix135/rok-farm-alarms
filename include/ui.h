#pragma once
#include "raylib_wrapper.h"
#include "navigation_state.h"
#include "store.h"

namespace UI
{
    struct UIState
    {
        bool showAddCharDialog = false;
        char charNameBuffer[128] = "";
        bool charEditMode = false;

        bool needSave = false;

        bool showAddAccountDialog = false;
        char accountEmailBuffer[128] = "";
        bool accountEditMode = false;

        rl::Vector2 accountScrollPos = { 0, 0 };
        rl::Rectangle accountViewScrollRect = { 0 };

        rl::Vector2 characterScrollPos = { 0, 0 };
        rl::Rectangle characterViewScrollRect = { 0 };

        int hours = 0;
        int minutes = 0;
        int seconds = 0;
        bool hoursEdit = false;
        bool minsEdit = false;
        bool secsEdit = false;

        int activeResource = 0;
        bool resourceEditMode = false;

        int activeLevel = 1;
        bool levelEditMode = false;

        rl::Vector2 timersScrollPos = { 0, 0 };
        rl::Rectangle timersViewScrollRect = { 0 };

        int lastActiveCharacterId = -1;
    };

    void InitUI(Store::SaveData user_save);
    void InitTheme();
    void UpdateWindowIcon();
    void SetWorkingDirectory();
    HWND GetWindowHandle();
    void RenderUI(Navigation& nav_state, std::vector<ROK::Account>& accounts, int width, int height, UIState& uiState);
    void RenderAccountPanel(Navigation& nav_state, std::vector<ROK::Account>& accounts, int width, int height, UIState& uiState);
    void RenderCharacterSelectPanel(Navigation& nav_state, float startX, float startY, float width, float height, UIState& uiState);
    void RenderAddAlarmPanel(Navigation& nav_state, float startX, float startY, float width, float height, UIState& uiState);
    void RenderActiveTimersPanel(Navigation& nav_state, float startX, float startY, float width, float height, UIState& uiState);
    void RenderAddCharacterDialog(Navigation& nav_state, int screenWidth, int screenHeight, UIState& uiState);
    void RenderAddAccountDialog(Navigation& nav_state, std::vector<ROK::Account>& accounts, int screenWidth, int screenHeight, UIState& uiState);
    void MarkDirty(UIState& uiState);
}