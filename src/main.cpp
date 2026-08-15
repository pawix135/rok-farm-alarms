#define RAYGUI_IMPLEMENTATION
#include "raylib_wrapper.h"
#include "win_app.h"
#include "navigation_state.h"
#include "ui.h"
#include "store.h"

void UpdateLogic(std::vector<ROK::Account> &accounts);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // TODO: ADD ICON
    Store::SaveData user_save = Store::LoadAll();

    rl::ChangeDirectory(rl::GetApplicationDirectory());
    rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
    rl::InitWindow(user_save.window_width, user_save.window_height, "Farm Alarms");
    rl::SetTargetFPS(60);

    rl::SetExitKey(rl::KEY_NULL);

    UI::InitTheme();

    WinApp::Init((HWND)rl::GetWindowHandle(), user_save.window_width, user_save.window_height);

    Navigation navigation_state = {0};
    UI::UIState ui_state = {};
    std::vector<ROK::Account> accounts = Store::LoadAccounts();
    while (WinApp::ShouldExit())
    {
        UpdateLogic(accounts);

        if (!rl::IsWindowHidden())
        {
            bool dataChanged = UI::RenderUI(navigation_state, accounts, WinApp::GetWindowWidth(), WinApp::GetWindowHeight(), ui_state);
            if (dataChanged)
            {
                Store::SaveAccountsOnly(accounts);
            }
        }
        else
        {
            rl::WaitTime(0.016);
        }
    }

    WinApp::Cleanup();
    rl::CloseWindow();
    return 0;
}

void UpdateLogic(std::vector<ROK::Account> &accounts)
{
    long long now = static_cast<long long>(std::time(nullptr));
    bool stateChanged = false;

    ROK::ForEachGatherer(accounts, [&](ROK::Account &account, ROK::Character &character, ROK::Gatherer &gatherer)
                         {
        if (!gatherer.isActive) return;

        if (now >= gatherer.targetTimestamp)
        {
            gatherer.isActive = false;
            stateChanged = true;

            std::string title = "Gathering Complete!";
            std::string msg = character.name + " (" + account.email + ") finished gathering!";
            WinApp::SendNotification(title.c_str(), msg.c_str());
        } });

    if (stateChanged)
    {
        Store::SaveAccountsOnly(accounts);
    }

    if (rl::IsWindowResized())
    {
        WinApp::SetWindowSize(rl::GetRenderWidth(), rl::GetRenderHeight());
        Store::SaveWindowSizeOnly(rl::GetRenderWidth(), rl::GetRenderHeight());
    }

    if (rl::WindowShouldClose())
    {
        WinApp::HideToTray();
        rl::ClearWindowState(rl::FLAG_WINDOW_HIDDEN);
    }

    /*if (rl::IsKeyPressed(rl::KEY_H)) {
        WinApp::HideToTray();
        rl::ClearWindowState(rl::FLAG_WINDOW_HIDDEN);
    }*/

    if (rl::IsKeyPressed(rl::KEY_N))
    {
        WinApp::SendNotification("Timer Alert!", "Your farm is ready to be harvested.");
    }

    if (rl::IsKeyPressed(rl::KEY_Q))
    {
        WinApp::Exit();
    }
}