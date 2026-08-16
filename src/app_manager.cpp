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
	}

	void RunApp() {

        // Change CWD to .exe folder 
        UI::SetWorkingDirectory();

		// Load accounts, window size and other user settings
		user_save = Store::LoadAll();
		accounts = user_save.accounts;
		
		UI::InitUI(user_save);

        // Setup Windows notification system
		WinApp::Init(UI::GetWindowHandle());

		while (WinApp::ShouldExit())
		{
			Update();

			if (!rl::IsWindowHidden())
			{
                // Stop rendering if app is in tray
				UI::RenderUI(navigation_state, accounts, user_save.window_width, user_save.window_height, ui_state);

                // Change in accounts or characters - save
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

        if (rl::IsKeyPressed(rl::KEY_Q))
        {
            WinApp::Exit();
        }
    }
}