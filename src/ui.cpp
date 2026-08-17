#include "ui.h"
#include "raylib_wrapper.h"
#include "helpers.h"
#include "config.h"

#define CHARACTER_PANEL_HEIGHT 150.0f
#define ALARM_PANEL_HEIGHT 70.0f

namespace UI
{
    static void ResetTimerForm(UIState& uiState)
    {
        uiState.hours = 0;
        uiState.minutes = 0;
        uiState.seconds = 0;
        uiState.hoursEdit = false;
        uiState.minsEdit = false;
        uiState.secsEdit = false;
        uiState.activeResource = 0;
        uiState.resourceEditMode = false;
        uiState.activeLevel = 1;
        uiState.levelEditMode = false;
        uiState.needSave = false;
    }

    void InitUI(Store::SaveData user_save) {

        rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
        rl::InitWindow(user_save.window_width, user_save.window_height, APP_NAME);

        rl::SetTargetFPS(15);

        UpdateWindowIcon();

        InitTheme();

        rl::SetExitKey(rl::KEY_NULL);

    }

    void SetWorkingDirectory() {
        rl::ChangeDirectory(rl::GetApplicationDirectory());
    }

    void UpdateWindowIcon() {
        rl::Image appIcon = rl::LoadImage("resources/icon.png");

        if (appIcon.data != nullptr) {
            rl::SetWindowIcon(appIcon);
            rl::UnloadImage(appIcon);
        }

    }

    HWND GetWindowHandle() {
        return (HWND)rl::GetWindowHandle();
    }

    void RenderResourceDropdownOverlay(Navigation& nav_state, float startX, float startY, UIState& uiState)
    {
        if (nav_state.activeCharacter == nullptr)
            return;

        float innerY = startY + 35.0f;
        float currentX = startX + 10.0f;

        currentX += 35 + 5;
        currentX += 35 + 5;
        currentX += 35 + 5;
        currentX += 35 + 15;
        currentX += 30 + 5;
        currentX += 80 + 15;
        currentX += 30 + 5;

        if (rl::GuiDropdownBox(rl::Rectangle{ currentX, innerY, 110, 25 }, "Food;Wood;Stone;Gold;Gems", &uiState.activeResource, uiState.resourceEditMode))
        {
            uiState.resourceEditMode = !uiState.resourceEditMode;
        }
    }

    void RenderUI(Navigation& nav_state, std::vector<ROK::Account>& accounts, UIState& uiState, const ROK::ResourceImages& resourceImages)
    {
        rl::BeginDrawing();
        rl::ClearBackground(rl::GRAY);

        int currentCharacterId = nav_state.activeCharacter ? nav_state.activeCharacter->id : -1;
        if (currentCharacterId != uiState.lastActiveCharacterId)
        {
            ResetTimerForm(uiState);
            uiState.lastActiveCharacterId = currentCharacterId;
        }

        float leftPanelWidth = uiState.windowWidth / 3.0f;
        float rightPanelX = leftPanelWidth;
        float rightPanelWidth = uiState.windowWidth - leftPanelWidth;
        float timersPanelHeight = uiState.windowHeight - CHARACTER_PANEL_HEIGHT - ALARM_PANEL_HEIGHT;

        RenderAccountPanel(nav_state, accounts, uiState);
        RenderCharacterSelectPanel(nav_state, rightPanelX, 0, rightPanelWidth, CHARACTER_PANEL_HEIGHT, uiState);
        RenderAddAlarmPanel(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT, rightPanelWidth, ALARM_PANEL_HEIGHT, uiState);
        RenderActiveTimersPanel(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT + ALARM_PANEL_HEIGHT, rightPanelWidth, timersPanelHeight, uiState, resourceImages);

        RenderResourceDropdownOverlay(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT, uiState);

        RenderAddAccountDialog(nav_state, accounts, uiState);
        RenderAddCharacterDialog(nav_state, uiState);

#ifdef _DEBUG
        DrawDebug(uiState);
#endif

        rl::EndDrawing();
    }

    void RenderAccountPanel(Navigation& nav_state, std::vector<ROK::Account>& accounts, UIState& uiState)
    {
        float panelWidth = uiState.windowWidth / 3.0f;
        float panelHeight = (float)uiState.windowHeight;

        rl::GuiPanel(rl::Rectangle{ 0, 0, panelWidth, panelHeight }, "Accounts");

        rl::Rectangle viewRect = { 5, 25, panelWidth - 10, panelHeight - 90 };
        float totalContentHeight = accounts.size() * 50.0f + 10.0f;

        if (totalContentHeight < viewRect.height)
        {
            totalContentHeight = viewRect.height;
        }

        rl::Rectangle contentRect = { 0, 0, panelWidth - 26, totalContentHeight };

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &uiState.accountScrollPos, &uiState.accountViewScrollRect);

        rl::BeginScissorMode((int)uiState.accountViewScrollRect.x, (int)uiState.accountViewScrollRect.y, (int)uiState.accountViewScrollRect.width, (int)uiState.accountViewScrollRect.height);
        {
            float startX = uiState.accountViewScrollRect.x + 5.0f;
            float startY = uiState.accountViewScrollRect.y + uiState.accountScrollPos.y + 5.0f;
            float deleteBtnWidth = 45.0f;
            float totalRowWidth = contentRect.width - 10.0f;
            float accountBtnWidth = totalRowWidth - deleteBtnWidth - 5.0f;

            int accountToRemoveIndex = -1;

            for (size_t i = 0; i < accounts.size(); ++i)
            {
                auto& acc = accounts[i];
                bool isSelected = (nav_state.activeAccount != nullptr && nav_state.activeAccount->id == acc.id);

                if (isSelected)
                {
                    rl::GuiSetState(rl::STATE_PRESSED);
                }

                if (rl::GuiButton(rl::Rectangle{ startX, startY, accountBtnWidth, 40 }, acc.email.c_str()))
                {
                    nav_state.activeAccount = &acc;

                    if (!acc.characters.empty())
                    {
                        nav_state.activeCharacter = &acc.characters[0];
                    }
                    else
                    {
                        nav_state.activeCharacter = nullptr;
                    }
                }

                rl::GuiSetState(rl::STATE_NORMAL);

                if (rl::GuiButton(rl::Rectangle{ startX + accountBtnWidth + 5.0f, startY, deleteBtnWidth, 40 }, "#143#"))
                {
                    accountToRemoveIndex = static_cast<int>(i);
                }

                startY += 50.0f;
            }

            if (accountToRemoveIndex != -1)
            {
                if (nav_state.activeAccount != nullptr && nav_state.activeAccount->id == accounts[accountToRemoveIndex].id)
                {
                    nav_state.activeAccount = nullptr;
                    nav_state.activeCharacter = nullptr;
                }

                accounts.erase(accounts.begin() + accountToRemoveIndex);
                MarkDirty(uiState);
            }
        }
        rl::EndScissorMode();

        float addBtnY = panelHeight - 55.0f;
        float addBtnWidth = panelWidth - 20.0f;

        if (rl::GuiButton(rl::Rectangle{ 10, addBtnY, addBtnWidth, 40 }, "Add Account +"))
        {
            uiState.showAddAccountDialog = true;
            uiState.accountEmailBuffer[0] = '\0';
            uiState.accountEditMode = true;
        }
    }

    void MarkDirty(UIState& uiState) {
        uiState.needSave = true;
    }

    void RenderCharacterSelectPanel(Navigation& nav_state, float startX, float startY, float width, float height, UIState& uiState)
    {
        rl::GuiPanel(rl::Rectangle{ startX, startY, width, height }, "Characters");

        if (nav_state.activeAccount == nullptr)
            return;

        float padding = 10.0f;
        float titleBarHeight = 30.0f;

        rl::Rectangle viewRect = { startX + padding, startY + titleBarHeight, width - 2 * padding, height - titleBarHeight - padding };

        float buttonWidth = 100.0f;
        float buttonHeight = viewRect.height - 25.0f;

        int charCount = (int)nav_state.activeAccount->characters.size();

        float totalContentWidth = (charCount + 1) * (buttonWidth + padding) + padding;
        if (totalContentWidth < viewRect.width)
        {
            totalContentWidth = viewRect.width;
        }

        rl::Rectangle contentRect = { 0, 0, totalContentWidth, viewRect.height - 16.0f };

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &uiState.characterScrollPos, &uiState.characterViewScrollRect);

        rl::BeginScissorMode((int)uiState.characterViewScrollRect.x, (int)uiState.characterViewScrollRect.y, (int)uiState.characterViewScrollRect.width, (int)uiState.characterViewScrollRect.height);
        {
            float currentX = uiState.characterViewScrollRect.x + uiState.characterScrollPos.x + padding;
            float currentY = uiState.characterViewScrollRect.y + 5.0f;

            int charToRemoveIndex = -1;

            for (size_t i = 0; i < nav_state.activeAccount->characters.size(); ++i)
            {
                auto& character = nav_state.activeAccount->characters[i];
                bool isSelected = (nav_state.activeCharacter != nullptr && nav_state.activeCharacter->id == character.id);

                if (isSelected)
                {
                    rl::GuiSetState(rl::STATE_PRESSED);
                }

                if (rl::GuiButton(rl::Rectangle{ currentX, currentY, buttonWidth, buttonHeight }, character.name.c_str()))
                {
                    nav_state.activeCharacter = &character;
                }

                rl::GuiSetState(rl::STATE_NORMAL);

                float deleteSize = 22.0f;
                rl::Rectangle deleteRect = { currentX + buttonWidth - deleteSize - 2.0f, currentY + 2.0f, deleteSize, deleteSize };

                if (rl::GuiButton(deleteRect, "#143#"))
                {
                    charToRemoveIndex = static_cast<int>(i);
                }

                currentX += buttonWidth + padding;
            }

            if (charToRemoveIndex != -1)
            {
                if (nav_state.activeCharacter != nullptr && nav_state.activeCharacter->id == nav_state.activeAccount->characters[charToRemoveIndex].id)
                {
                    nav_state.activeCharacter = nullptr;
                }

                nav_state.activeAccount->characters.erase(nav_state.activeAccount->characters.begin() + charToRemoveIndex);
                MarkDirty(uiState);
            }

            if (rl::GuiButton(rl::Rectangle{ currentX, currentY, buttonWidth, buttonHeight }, "ADD +"))
            {
                uiState.showAddCharDialog = true;
                uiState.charNameBuffer[0] = '\0';
                uiState.charEditMode = true;
            }
        }
        rl::EndScissorMode();
    }

    void RenderAddAlarmPanel(Navigation& nav_state, float startX, float startY, float width, float height, UIState& uiState)
    {
        rl::GuiPanel(rl::Rectangle{ startX, startY, width, height }, "Section For Adding Timers");

        if (nav_state.activeCharacter == nullptr)
            return;

        float innerY = startY + 35.0f;
        float currentX = startX + 10.0f;

        rl::GuiLabel(rl::Rectangle{ currentX, innerY, 35, 25 }, "Time:");
        currentX += 40;

        if (rl::GuiValueBox(rl::Rectangle{ currentX, innerY, 35, 25 }, nullptr, &uiState.hours, 0, 99, uiState.hoursEdit))
            uiState.hoursEdit = !uiState.hoursEdit;
        currentX += 40;
        if (rl::GuiValueBox(rl::Rectangle{ currentX, innerY, 35, 25 }, nullptr, &uiState.minutes, 0, 59, uiState.minsEdit))
            uiState.minsEdit = !uiState.minsEdit;
        currentX += 40;
        if (rl::GuiValueBox(rl::Rectangle{ currentX, innerY, 35, 25 }, nullptr, &uiState.seconds, 0, 59, uiState.secsEdit))
            uiState.secsEdit = !uiState.secsEdit;
        currentX += 50;

        rl::GuiLabel(rl::Rectangle{ currentX, innerY, 30, 25 }, "Lvl:");
        currentX += 35;
        if (rl::GuiSpinner(rl::Rectangle{ currentX, innerY, 80, 25 }, nullptr, &uiState.activeLevel, 1, 9, uiState.levelEditMode))
            uiState.levelEditMode = !uiState.levelEditMode;
        currentX += 95;

        rl::GuiLabel(rl::Rectangle{ currentX, innerY, 30, 25 }, "Res:");

        float startBtnWidth = 80.0f;
        float startBtnX = startX + width - startBtnWidth - 10.0f;

        if (rl::GuiButton(rl::Rectangle{ startBtnX, innerY, startBtnWidth, 25 }, "START"))
        {
            int totalSecs = (uiState.hours * 3600) + (uiState.minutes * 60) + uiState.seconds;
            if (totalSecs > 0)
            {
                ROK::Gatherer g;
                g.id = static_cast<int>(nav_state.activeCharacter->gatherers.size()) + 1;
                g.resourceLvl = uiState.activeLevel;
                g.resource = static_cast<ROK::ResourceType>(uiState.activeResource);
                g.SetTimer(uiState.hours, uiState.minutes, uiState.seconds);

                nav_state.activeCharacter->gatherers.push_back(g);
                MarkDirty(uiState);
                uiState.hours = 0;
                uiState.minutes = 0;
                uiState.seconds = 0;
            }
        }
    }

    void RenderActiveTimersPanel(Navigation& nav_state, float startX, float startY, float width, float height, UIState& uiState, const ROK::ResourceImages& resourceImages)
    {
        rl::GuiPanel(rl::Rectangle{ startX, startY, width, height }, "Active Timers");

        if (nav_state.activeCharacter == nullptr)
            return;

        float padding = 10.0f;
        rl::Rectangle viewRect = { startX + padding, startY + 25.0f, width - 2 * padding, height - 35.0f };

        auto& gatherers = nav_state.activeCharacter->gatherers;

        float itemHeight = 100.0f;
        float totalContentHeight = gatherers.size() * (itemHeight + padding) + padding;

        if (totalContentHeight < viewRect.height)
        {
            totalContentHeight = viewRect.height;
        }

        rl::Rectangle contentRect = { 0, 0, viewRect.width - 16.0f, totalContentHeight };

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &uiState.timersScrollPos, &uiState.timersViewScrollRect);

        rl::BeginScissorMode((int)uiState.timersViewScrollRect.x, (int)uiState.timersViewScrollRect.y, (int)uiState.timersViewScrollRect.width, (int)uiState.timersViewScrollRect.height);
        {
            float currentX = uiState.timersViewScrollRect.x + padding;
            float currentY = uiState.timersViewScrollRect.y + uiState.timersScrollPos.y + padding;
            float itemWidth = uiState.timersViewScrollRect.width - (2 * padding);

            int indexToRemove = -1;

            for (size_t i = 0; i < gatherers.size(); i++)
            {
                auto& gatherer = gatherers[i];

                rl::GuiPanel(rl::Rectangle{ currentX, currentY, itemWidth, itemHeight }, nullptr);

                float imgSize = 68.0f;
                float imgX = currentX + 16.0f;
                float imgY = currentY + (itemHeight - imgSize) / 2.0f;

                const rl::Texture2D& tex = GetResourceTexture(gatherer.resource, resourceImages);

                rl::DrawTexturePro(
                    tex,
                    rl::Rectangle{ 0, 0, (float)tex.width, (float)tex.height },
                    rl::Rectangle{ imgX, imgY, imgSize, imgSize },
                    rl::Vector2{ 0, 0 }, 0.0f, rl::WHITE
                );

                std::string lvlText = std::to_string(gatherer.resourceLvl);
                float badgeWidth = 24.0f;
                float badgeHeight = 24.0f;
                float badgeX = imgX + imgSize - badgeWidth;
                float badgeY = imgY + imgSize - badgeHeight;

                rl::DrawRectangle((int)badgeX, (int)badgeY, (int)badgeWidth, (int)badgeHeight, rl::BLACK);
                rl::DrawRectangleLines((int)badgeX, (int)badgeY, (int)badgeWidth, (int)badgeHeight, rl::DARKGRAY);
                rl::DrawText(lvlText.c_str(), (int)badgeX + 7, (int)badgeY + 4, 16, rl::WHITE);

                float totalSecs = gatherer.GetRemainingSeconds();
                if (totalSecs < 0.0f) totalSecs = 0.0f;

                int h = static_cast<int>(totalSecs) / 3600;
                int m = (static_cast<int>(totalSecs) % 3600) / 60;
                int s = static_cast<int>(totalSecs) % 60;

                float squareSize = 70.0f;
                float boxY = currentY + (itemHeight - squareSize) / 2.0f;
                float startBoxX = imgX + imgSize + 25.0f;
                float gap = 12.0f;

                char hBuf[8], mBuf[8], sBuf[8];
                snprintf(hBuf, sizeof(hBuf), "%02dh", h);
                snprintf(mBuf, sizeof(mBuf), "%02dm", m);
                snprintf(sBuf, sizeof(sBuf), "%02ds", s);

                const char* timeStrings[3] = { hBuf, mBuf, sBuf };

                for (int t = 0; t < 3; t++)
                {
                    float boxX = startBoxX + t * (squareSize + gap);
                    rl::Rectangle boxRect = { boxX, boxY, squareSize, squareSize };

                    rl::DrawRectangleLinesEx(boxRect, 2.0f, rl::GRAY);

                    int fontSize = 20;
                    int textWidth = rl::MeasureText(timeStrings[t], fontSize);
                    int textX = (int)(boxX + (squareSize - textWidth) / 2.0f);
                    int textY = (int)(boxY + (squareSize - fontSize) / 2.0f);

                    rl::DrawText(timeStrings[t], textX, textY, fontSize, rl::LIGHTGRAY);
                }

                float deleteSize = 36.0f;
                rl::Rectangle deleteRect = { currentX + itemWidth - deleteSize - 16.0f, currentY + (itemHeight - deleteSize) / 2.0f, deleteSize, deleteSize };

                if (rl::GuiButton(deleteRect, "#143#"))
                {
                    indexToRemove = static_cast<int>(i);
                }

                currentY += itemHeight + padding;
            }

            if (indexToRemove != -1)
            {
                gatherers.erase(gatherers.begin() + indexToRemove);
                MarkDirty(uiState);
            }
        }
        rl::EndScissorMode();
    }

    void RenderAddAccountDialog(Navigation& nav_state, std::vector<ROK::Account>& accounts, UIState& uiState)
    {
        if (!uiState.showAddAccountDialog)
            return;

        rl::GuiLock();

        float dialogW = 320.0f;
        float dialogH = 170.0f;
        float dialogX = (uiState.windowWidth - dialogW) / 2.0f;
        float dialogY = (uiState.windowHeight - dialogH) / 2.0f;

        if (rl::GuiWindowBox(rl::Rectangle{ dialogX, dialogY, dialogW, dialogH }, "New Account"))
        {
            uiState.showAddAccountDialog = false;
            uiState.accountEditMode = false;
            uiState.accountEmailBuffer[0] = '\0';
        }

        rl::GuiUnlock();

        rl::GuiLabel(rl::Rectangle{ dialogX + 20, dialogY + 45, dialogW - 40, 20 }, "Enter Account Name:");

        if (rl::GuiTextBox(rl::Rectangle{ dialogX + 20, dialogY + 70, dialogW - 40, 30 }, uiState.accountEmailBuffer, 128, uiState.accountEditMode))
        {
            uiState.accountEditMode = !uiState.accountEditMode;
        }

        if (rl::GuiButton(rl::Rectangle{ dialogX + 20, dialogY + 115, 130, 35 }, "Save"))
        {
            if (uiState.accountEmailBuffer[0] != '\0')
            {
                int currentActiveId = nav_state.activeAccount ? nav_state.activeAccount->id : -1;

                ROK::Account newAccount;
                newAccount.id = accounts.empty() ? 1 : accounts.back().id + 1;
                newAccount.email = std::string(uiState.accountEmailBuffer);

                accounts.push_back(newAccount);

                nav_state.activeAccount = nullptr;

                for (auto& acc : accounts)
                {
                    if (acc.id == currentActiveId)
                    {
                        nav_state.activeAccount = &acc;
                        break;
                    }
                }

                MarkDirty(uiState);
            }
            uiState.showAddAccountDialog = false;
            uiState.accountEditMode = false;
            uiState.accountEmailBuffer[0] = '\0';
        }

        if (rl::GuiButton(rl::Rectangle{ dialogX + 170, dialogY + 115, 130, 35 }, "Cancel"))
        {
            uiState.showAddAccountDialog = false;
            uiState.accountEditMode = false;
            uiState.accountEmailBuffer[0] = '\0';
        }
    }

    void RenderAddCharacterDialog(Navigation& nav_state, UIState& uiState)
    {
        if (!uiState.showAddCharDialog || nav_state.activeAccount == nullptr)
            return;

        rl::GuiLock();

        float dialogW = 320.0f;
        float dialogH = 170.0f;
        float dialogX = (uiState.windowWidth - dialogW) / 2.0f;
        float dialogY = (uiState.windowHeight - dialogH) / 2.0f;

        if (rl::GuiWindowBox(rl::Rectangle{ dialogX, dialogY, dialogW, dialogH }, "New Character"))
        {
            uiState.showAddCharDialog = false;
            uiState.charEditMode = false;
            uiState.charNameBuffer[0] = '\0';
        }

        rl::GuiUnlock();

        rl::GuiLabel(rl::Rectangle{ dialogX + 20, dialogY + 45, dialogW - 40, 20 }, "Enter Character Name:");

        if (rl::GuiTextBox(rl::Rectangle{ dialogX + 20, dialogY + 70, dialogW - 40, 30 }, uiState.charNameBuffer, 128, uiState.charEditMode))
        {
            uiState.charEditMode = !uiState.charEditMode;
        }

        if (rl::GuiButton(rl::Rectangle{ dialogX + 20, dialogY + 115, 130, 35 }, "Save"))
        {
            if (uiState.charNameBuffer[0] != '\0')
            {
                ROK::Character newChar;
                newChar.id = nav_state.activeAccount->characters.empty() ? 101 : nav_state.activeAccount->characters.back().id + 1;
                newChar.name = std::string(uiState.charNameBuffer);

                int activeCharId = nav_state.activeCharacter ? nav_state.activeCharacter->id : -1;

                nav_state.activeAccount->characters.push_back(newChar);

                nav_state.activeCharacter = nullptr;
                for (auto& ch : nav_state.activeAccount->characters)
                {
                    if (ch.id == activeCharId)
                    {
                        nav_state.activeCharacter = &ch;
                        break;
                    }
                }
                MarkDirty(uiState);
            }
            uiState.showAddCharDialog = false;
            uiState.charEditMode = false;
            uiState.charNameBuffer[0] = '\0';
        }

        if (rl::GuiButton(rl::Rectangle{ dialogX + 170, dialogY + 115, 130, 35 }, "Cancel"))
        {
            uiState.showAddCharDialog = false;
            uiState.charEditMode = false;
            uiState.charNameBuffer[0] = '\0';
        }
    }

    void InitTheme()
    {
        const unsigned char* styleData = nullptr;
        unsigned int styleSize = 0;

        if (GetEmbeddedBuffer(102, &styleData, &styleSize))
        {
            rl::GuiLoadStyleFromMemory(styleData, styleSize);
        }
        else
        {
            rl::GuiLoadStyle("resources/style_dark.rgs");
        }
    }

    void DrawDebug(UIState& uiState) {
        rl::DrawFPS(rl::GetScreenWidth() - 75, 10);
        rl::DrawText("DEBUG", rl::GetScreenWidth() - rl::MeasureText("DEBUG", 25) - 50, rl::GetScreenHeight() - 15 - 25, 25, rl::RED);
    }

    void Cleanup() {
        rl::CloseWindow();
    }
}