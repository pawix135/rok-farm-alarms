#include "app_manager.h"
#include "store.h"
#include "win_app.h"
#include "navigation_state.h"
#include "helpers.h"
#include "config.h" 
#include <iostream>

namespace APP {

	namespace {
		Store::SaveData user_save;
		std::vector<ROK::Account> accounts;
        UI::UIState ui_state = {};
        Navigation navigation_state = { 0 };
        ROK::ResourceImages resourceImages;
	}

	void RunApp() {

        UI::SetWorkingDirectory();

		user_save = Store::LoadAll();
		accounts = user_save.accounts;
        ui_state.windowWidth = user_save.window_width;
        ui_state.windowHeight = user_save.window_height;
		
		UI::InitUI(user_save);

        resourceImages.LoadAll();

		WinApp::Init(UI::GetWindowHandle());

		while (WinApp::ShouldExit())
		{
			Update();

			if (!rl::IsWindowHidden())
			{
				UI::RenderUI(navigation_state, accounts, ui_state, resourceImages);

				if (ui_state.needSave)
				{
					Store::SaveAccountsOnly(accounts);
                    ui_state.needSave = false;
				}
			}
			else
			{
				rl::WaitTime(0.016);
			}
		}

        Cleanup();

	}

    void Update()
    {
        long long now = static_cast<long long>(std::time(nullptr));
        
        ROK::ForEachGatherer(accounts, [&](ROK::Account& account, ROK::Character& character, ROK::Gatherer& gatherer)
            {
                if (!gatherer.isActive) return;

                if (now >= gatherer.targetTimestamp)
                {
                    gatherer.isActive = false;
                    UI::MarkDirty(ui_state);

                    std::string title = "Gathering Complete!";
                    std::string msg = "Account: (" + account.email + "), " + character.name + " finished gathering!";
                    WinApp::SendNotification(title.c_str(), msg.c_str());
                } });

        if (ui_state.needSave)
        {
            Store::SaveAccountsOnly(accounts);
        }

        if (rl::IsWindowResized())
        {
            int w = rl::GetRenderWidth();
            int h = rl::GetRenderHeight();
            ui_state.windowWidth = w;
            ui_state.windowHeight = h;
            Store::SaveWindowSizeOnly(w, h);
        }

        if (rl::WindowShouldClose())
        {
            WinApp::HideToTray();
            rl::ClearWindowState(rl::FLAG_WINDOW_HIDDEN);
        }

        bool isEditingMode = ui_state.charEditMode || ui_state.accountEditMode;

        if (!isEditingMode) {

            if (rl::IsKeyPressed(rl::KEY_Q))
            {
                WinApp::Exit();
            }
        }

    }
    
    void Cleanup() {
        resourceImages.UnloadAll();
        WinApp::Cleanup();
        UI::Cleanup();
    }
}