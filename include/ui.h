#pragma once
#include "navigation_state.h"
namespace UI {
	void InitTheme();
	void RenderUI(Navigation& nav_state, std::vector<ROK::Account>& accounts, int width, int height);
	void RenderAccountPanel(Navigation& nav_state, std::vector<ROK::Account>& accounts, int width, int height);
	void RenderCharacterSelectPanel(Navigation& nav_state, float startX, float startY, float width, float height);
	void RenderAddAlarmPanel(Navigation& nav_state, float startX, float startY, float width, float height, std::vector<ROK::Account>& accounts);
	void RenderActiveTimersPanel(Navigation& nav_state, float startX, float startY, float width, float height, std::vector<ROK::Account>& accounts);
	void RenderAddCharacterDialog(Navigation& nav_state, std::vector<ROK::Account>& accounts, int screenWidth, int screenHeight);
	void RenderAddAccountDialog(Navigation& nav_state, std::vector<ROK::Account>& accounts, int screenWidth, int screenHeight);
}