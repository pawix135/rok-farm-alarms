#include "ui.h"
#include "raylib_wrapper.h"
#include "helpers.h"

#define CHARACTER_PANEL_HEIGHT 150.0f
#define ALARM_PANEL_HEIGHT 70.0f

namespace UI
{

    static void ResetTimerForm(UIState &uiState)
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
    }

    bool RenderUI(Navigation &nav_state, std::vector<ROK::Account> &accounts, int width, int height, UIState &uiState)
    {
        rl::BeginDrawing();
        rl::ClearBackground(rl::GRAY);

        bool dataChanged = false;

        int currentCharacterId = nav_state.activeCharacter ? nav_state.activeCharacter->id : -1;
        if (currentCharacterId != uiState.lastActiveCharacterId)
        {
            ResetTimerForm(uiState);
            uiState.lastActiveCharacterId = currentCharacterId;
        }

        float leftPanelWidth = width / 3.0f;
        float rightPanelX = leftPanelWidth;
        float rightPanelWidth = width - leftPanelWidth;
        float timersPanelHeight = height - CHARACTER_PANEL_HEIGHT - ALARM_PANEL_HEIGHT;

        RenderAccountPanel(nav_state, accounts, width, height, uiState);
        RenderCharacterSelectPanel(nav_state, rightPanelX, 0, rightPanelWidth, CHARACTER_PANEL_HEIGHT, uiState);
        RenderAddAlarmPanel(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT, rightPanelWidth, ALARM_PANEL_HEIGHT, uiState, dataChanged);
        RenderActiveTimersPanel(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT + ALARM_PANEL_HEIGHT, rightPanelWidth, timersPanelHeight, uiState, dataChanged);

        RenderAddAccountDialog(nav_state, accounts, width, height, uiState, dataChanged);
        RenderAddCharacterDialog(nav_state, width, height, uiState, dataChanged);

        rl::EndDrawing();

        return dataChanged;
    }

    void RenderAccountPanel(Navigation &nav_state, std::vector<ROK::Account> &accounts, int width, int height, UIState &uiState)
    {

        float panelWidth = width / 3.0f;
        float panelHeight = (float)height;

        rl::GuiPanel(rl::Rectangle{0, 0, panelWidth, panelHeight}, "Accounts");

        rl::Rectangle viewRect = {5, 25, panelWidth - 10, panelHeight - 90};
        float totalContentHeight = accounts.size() * 50.0f + 10.0f;

        if (totalContentHeight < viewRect.height)
        {
            totalContentHeight = viewRect.height;
        }

        rl::Rectangle contentRect = {0, 0, panelWidth - 26, totalContentHeight};

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &uiState.accountScrollPos, &uiState.accountViewScrollRect);

        rl::BeginScissorMode((int)uiState.accountViewScrollRect.x, (int)uiState.accountViewScrollRect.y, (int)uiState.accountViewScrollRect.width, (int)uiState.accountViewScrollRect.height);
        {
            float startX = uiState.accountViewScrollRect.x + 5.0f;
            float startY = uiState.accountViewScrollRect.y + uiState.accountScrollPos.y + 5.0f;
            float buttonWidth = contentRect.width - 10.0f;

            for (auto &acc : accounts)
            {
                bool isSelected = (nav_state.activeAccount != nullptr && nav_state.activeAccount->id == acc.id);

                if (isSelected)
                {
                    rl::GuiSetState(rl::STATE_PRESSED);
                }

                if (rl::GuiButton(rl::Rectangle{startX, startY, buttonWidth, 40}, acc.email.c_str()))
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
                startY += 50.0f;
            }
        }
        rl::EndScissorMode();

        float addBtnY = panelHeight - 55.0f;
        float addBtnWidth = panelWidth - 20.0f;

        if (rl::GuiButton(rl::Rectangle{10, addBtnY, addBtnWidth, 40}, "Add Account +"))
        {
            uiState.showAddAccountDialog = true;
            uiState.accountEmailBuffer[0] = '\0';
            uiState.accountEditMode = true;
        }
    }

    void RenderCharacterSelectPanel(Navigation &nav_state, float startX, float startY, float width, float height, UIState &uiState)
    {

        rl::GuiPanel(rl::Rectangle{startX, startY, width, height}, "Characters");

        if (nav_state.activeAccount == nullptr)
            return;

        float padding = 10.0f;
        float titleBarHeight = 30.0f;

        rl::Rectangle viewRect = {startX + padding, startY + titleBarHeight, width - 2 * padding, height - titleBarHeight - padding};

        float buttonWidth = 100.0f;
        float buttonHeight = viewRect.height - 25.0f;

        int charCount = (int)nav_state.activeAccount->characters.size();

        float totalContentWidth = (charCount + 1) * (buttonWidth + padding) + padding;
        if (totalContentWidth < viewRect.width)
        {
            totalContentWidth = viewRect.width;
        }

        rl::Rectangle contentRect = {0, 0, totalContentWidth, viewRect.height - 16.0f};

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &uiState.characterScrollPos, &uiState.characterViewScrollRect);

        rl::BeginScissorMode((int)uiState.characterViewScrollRect.x, (int)uiState.characterViewScrollRect.y, (int)uiState.characterViewScrollRect.width, (int)uiState.characterViewScrollRect.height);
        {
            float currentX = uiState.characterViewScrollRect.x + uiState.characterScrollPos.x + padding;
            float currentY = uiState.characterViewScrollRect.y + 5.0f;

            for (auto &character : nav_state.activeAccount->characters)
            {
                bool isSelected = (nav_state.activeCharacter != nullptr && nav_state.activeCharacter->id == character.id);

                if (isSelected)
                {
                    rl::GuiSetState(rl::STATE_PRESSED);
                }

                if (rl::GuiButton(rl::Rectangle{currentX, currentY, buttonWidth, buttonHeight}, character.name.c_str()))
                {
                    nav_state.activeCharacter = &character;
                }

                rl::GuiSetState(rl::STATE_NORMAL);
                currentX += buttonWidth + padding;
            }

            if (rl::GuiButton(rl::Rectangle{currentX, currentY, buttonWidth, buttonHeight}, "ADD +"))
            {
                uiState.showAddCharDialog = true;
                uiState.charNameBuffer[0] = '\0';
                uiState.charEditMode = true;
            }
        }
        rl::EndScissorMode();
    }

    void RenderAddAlarmPanel(Navigation &nav_state, float startX, float startY, float width, float height, UIState &uiState, bool &dataChanged)
    {
        rl::GuiPanel(rl::Rectangle{startX, startY, width, height}, "Section For Adding Timers");

        if (nav_state.activeCharacter == nullptr)
            return;

        float innerY = startY + 35.0f;
        float currentX = startX + 10.0f;

        rl::GuiLabel(rl::Rectangle{currentX, innerY, 35, 25}, "Time:");
        currentX += 40;

        if (rl::GuiValueBox(rl::Rectangle{currentX, innerY, 35, 25}, nullptr, &uiState.hours, 0, 99, uiState.hoursEdit))
            uiState.hoursEdit = !uiState.hoursEdit;
        currentX += 40;
        if (rl::GuiValueBox(rl::Rectangle{currentX, innerY, 35, 25}, nullptr, &uiState.minutes, 0, 59, uiState.minsEdit))
            uiState.minsEdit = !uiState.minsEdit;
        currentX += 40;
        if (rl::GuiValueBox(rl::Rectangle{currentX, innerY, 35, 25}, nullptr, &uiState.seconds, 0, 59, uiState.secsEdit))
            uiState.secsEdit = !uiState.secsEdit;
        currentX += 50;

        rl::GuiLabel(rl::Rectangle{currentX, innerY, 30, 25}, "Lvl:");
        currentX += 30;
        if (rl::GuiSpinner(rl::Rectangle{currentX, innerY, 80, 25}, nullptr, &uiState.activeLevel, 1, 9, uiState.levelEditMode))
            uiState.levelEditMode = !uiState.levelEditMode;
        currentX += 95;

        float resX = currentX;
        currentX += 115;

        if (rl::GuiButton(rl::Rectangle{currentX, innerY, 80, 25}, "START"))
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
                dataChanged = true;
                uiState.hours = 0;
                uiState.minutes = 0;
                uiState.seconds = 0;
            }
        }

        rl::GuiLabel(rl::Rectangle{resX, innerY, 30, 25}, "Res:");
        if (rl::GuiDropdownBox(rl::Rectangle{resX + 30, innerY, 70, 25}, "Food;Wood;Stone;Gold;Gems", &uiState.activeResource, uiState.resourceEditMode))
        {
            uiState.resourceEditMode = !uiState.resourceEditMode;
        }
    }

    void RenderActiveTimersPanel(Navigation &nav_state, float startX, float startY, float width, float height, UIState &uiState, bool &dataChanged)
    {
        rl::GuiPanel(rl::Rectangle{startX, startY, width, height}, "Active Timers");

        if (nav_state.activeCharacter == nullptr)
            return;

        float padding = 10.0f;
        rl::Rectangle viewRect = {startX + padding, startY + 25.0f, width - 2 * padding, height - 35.0f};

        float itemHeight = 40.0f;
        float totalContentHeight = nav_state.activeCharacter->gatherers.size() * (itemHeight + padding) + padding;

        if (totalContentHeight < viewRect.height)
        {
            totalContentHeight = viewRect.height;
        }

        rl::Rectangle contentRect = {0, 0, viewRect.width - 16.0f, totalContentHeight};

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &uiState.timersScrollPos, &uiState.timersViewScrollRect);

        rl::BeginScissorMode((int)uiState.timersViewScrollRect.x, (int)uiState.timersViewScrollRect.y, (int)uiState.timersViewScrollRect.width, (int)uiState.timersViewScrollRect.height);
        {
            float currentX = uiState.timersViewScrollRect.x + padding;
            float currentY = uiState.timersViewScrollRect.y + uiState.timersScrollPos.y + padding;
            float itemWidth = contentRect.width - padding;

            auto &gatherers = nav_state.activeCharacter->gatherers;

            for (size_t i = 0; i < gatherers.size();)
            {
                auto &gatherer = gatherers[i];

                rl::GuiPanel(rl::Rectangle{currentX, currentY, itemWidth, itemHeight}, nullptr);

                rl::GuiLabel(rl::Rectangle{currentX + 10, currentY + 5, 200, 30}, gatherer.GetFormattedTime().c_str());

                bool isFinished = !gatherer.isActive || (gatherer.GetRemainingSeconds() <= 0.0f);
                const char *buttonLabel = isFinished ? "REMOVE" : "STOP";

                if (rl::GuiButton(rl::Rectangle{currentX + itemWidth - 80, currentY + 5, 70, 30}, buttonLabel))
                {
                    gatherers.erase(gatherers.begin() + i);
                    dataChanged = true;

                    continue;
                }

                currentY += itemHeight + padding;
                i++;
            }
        }
        rl::EndScissorMode();
    }

    void RenderAddAccountDialog(Navigation &nav_state, std::vector<ROK::Account> &accounts, int screenWidth, int screenHeight, UIState &uiState, bool &dataChanged)
    {
        if (!uiState.showAddAccountDialog)
            return;

        rl::GuiLock();

        float dialogW = 320.0f;
        float dialogH = 170.0f;
        float dialogX = (screenWidth - dialogW) / 2.0f;
        float dialogY = (screenHeight - dialogH) / 2.0f;

        if (rl::GuiWindowBox(rl::Rectangle{dialogX, dialogY, dialogW, dialogH}, "New Account"))
        {
            uiState.showAddAccountDialog = false;
            uiState.accountEditMode = false;
            uiState.accountEmailBuffer[0] = '\0';
        }

        rl::GuiUnlock();

        rl::GuiLabel(rl::Rectangle{dialogX + 20, dialogY + 45, dialogW - 40, 20}, "Enter Email Address:");

        if (rl::GuiTextBox(rl::Rectangle{dialogX + 20, dialogY + 70, dialogW - 40, 30}, uiState.accountEmailBuffer, 128, uiState.accountEditMode))
        {
            uiState.accountEditMode = !uiState.accountEditMode;
        }

        if (rl::GuiButton(rl::Rectangle{dialogX + 20, dialogY + 115, 130, 35}, "Save"))
        {
            if (uiState.accountEmailBuffer[0] != '\0')
            {
                int currentActiveId = nav_state.activeAccount ? nav_state.activeAccount->id : -1;

                ROK::Account newAccount;
                newAccount.id = accounts.empty() ? 1 : accounts.back().id + 1;
                newAccount.email = std::string(uiState.accountEmailBuffer);

                accounts.push_back(newAccount);

                nav_state.activeAccount = nullptr;
                for (auto &acc : accounts)
                {
                    if (acc.id == currentActiveId)
                    {
                        nav_state.activeAccount = &acc;
                        break;
                    }
                }
                dataChanged = true;
            }
            uiState.showAddAccountDialog = false;
            uiState.accountEditMode = false;
            uiState.accountEmailBuffer[0] = '\0';
        }

        if (rl::GuiButton(rl::Rectangle{dialogX + 170, dialogY + 115, 130, 35}, "Cancel"))
        {
            uiState.showAddAccountDialog = false;
            uiState.accountEditMode = false;
            uiState.accountEmailBuffer[0] = '\0';
        }
    }

    void RenderAddCharacterDialog(Navigation &nav_state, int screenWidth, int screenHeight, UIState &uiState, bool &dataChanged)
    {
        if (!uiState.showAddCharDialog || nav_state.activeAccount == nullptr)
            return;

        rl::GuiLock();

        float dialogW = 320.0f;
        float dialogH = 170.0f;
        float dialogX = (screenWidth - dialogW) / 2.0f;
        float dialogY = (screenHeight - dialogH) / 2.0f;

        if (rl::GuiWindowBox(rl::Rectangle{dialogX, dialogY, dialogW, dialogH}, "New Character"))
        {
            uiState.showAddCharDialog = false;
            uiState.charEditMode = false;
            uiState.charNameBuffer[0] = '\0';
        }

        rl::GuiUnlock();

        rl::GuiLabel(rl::Rectangle{dialogX + 20, dialogY + 45, dialogW - 40, 20}, "Enter Character Name:");

        if (rl::GuiTextBox(rl::Rectangle{dialogX + 20, dialogY + 70, dialogW - 40, 30}, uiState.charNameBuffer, 128, uiState.charEditMode))
        {
            uiState.charEditMode = !uiState.charEditMode;
        }

        if (rl::GuiButton(rl::Rectangle{dialogX + 20, dialogY + 115, 130, 35}, "Save"))
        {
            if (uiState.charNameBuffer[0] != '\0')
            {
                ROK::Character newChar;
                newChar.id = nav_state.activeAccount->characters.empty() ? 101 : nav_state.activeAccount->characters.back().id + 1;
                newChar.name = std::string(uiState.charNameBuffer);

                int activeCharId = nav_state.activeCharacter ? nav_state.activeCharacter->id : -1;

                nav_state.activeAccount->characters.push_back(newChar);

                nav_state.activeCharacter = nullptr;
                for (auto &ch : nav_state.activeAccount->characters)
                {
                    if (ch.id == activeCharId)
                    {
                        nav_state.activeCharacter = &ch;
                        break;
                    }
                }
                dataChanged = true;
            }
            uiState.showAddCharDialog = false;
            uiState.charEditMode = false;
            uiState.charNameBuffer[0] = '\0';
        }

        if (rl::GuiButton(rl::Rectangle{dialogX + 170, dialogY + 115, 130, 35}, "Cancel"))
        {
            uiState.showAddCharDialog = false;
            uiState.charEditMode = false;
            uiState.charNameBuffer[0] = '\0';
        }
    }

    void InitTheme()
    {
        const unsigned char *styleData = nullptr;
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
}