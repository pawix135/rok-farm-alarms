#include "ui.h"
#include "raylib_wrapper.h"
#include "helpers.h"
#include "store.h"

#define CHARACTER_PANEL_HEIGHT 150.0f
#define ALARM_PANEL_HEIGHT 70.0f

static bool showAddCharDialog = false;
static char charNameBuffer[128] = "";
static bool charEditMode = false;

static bool showAddAccountDialog = false;
static char accountEmailBuffer[128] = "";
static bool accountEditMode = false;

namespace UI {

    void RenderUI(Navigation& nav_state, std::vector<ROK::Account>& accounts, int width, int height) {
        rl::BeginDrawing();
        rl::ClearBackground(rl::GRAY);

        float leftPanelWidth = width / 3.0f;
        float rightPanelX = leftPanelWidth;
        float rightPanelWidth = width - leftPanelWidth;
        float timersPanelHeight = height - CHARACTER_PANEL_HEIGHT - ALARM_PANEL_HEIGHT;

        RenderAccountPanel(nav_state, accounts, width, height);
        RenderCharacterSelectPanel(nav_state, rightPanelX, 0, rightPanelWidth, CHARACTER_PANEL_HEIGHT);
        RenderAddAlarmPanel(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT, rightPanelWidth, ALARM_PANEL_HEIGHT, accounts);
        RenderActiveTimersPanel(nav_state, rightPanelX, CHARACTER_PANEL_HEIGHT + ALARM_PANEL_HEIGHT, rightPanelWidth, timersPanelHeight, accounts);

        RenderAddAccountDialog(nav_state, accounts, width, height);
        RenderAddCharacterDialog(nav_state, accounts, width, height);

        rl::EndDrawing();
    }

    void RenderAccountPanel(Navigation& nav_state, std::vector<ROK::Account>& accounts, int width, int height) {

        float panelWidth = width / 3.0f;
        float panelHeight = (float)height;

        rl::GuiPanel(rl::Rectangle{ 0, 0, panelWidth, panelHeight }, "Accounts");

        rl::Rectangle viewRect = { 5, 25, panelWidth - 10, panelHeight - 90 };
        float totalContentHeight = accounts.size() * 50.0f + 10.0f;

        if (totalContentHeight < viewRect.height) {
            totalContentHeight = viewRect.height;
        }

        rl::Rectangle contentRect = { 0, 0, panelWidth - 26, totalContentHeight };

        static rl::Vector2 scrollPos = { 0, 0 };
        static rl::Vector2 viewScrollPos = { 0, 0 };
        static rl::Rectangle viewScrollRect = { 0 };

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &scrollPos, &viewScrollRect);

        rl::BeginScissorMode((int)viewScrollRect.x, (int)viewScrollRect.y, (int)viewScrollRect.width, (int)viewScrollRect.height);
        {
            float startX = viewScrollRect.x + 5.0f;
            float startY = viewScrollRect.y + scrollPos.y + 5.0f;
            float buttonWidth = contentRect.width - 10.0f;

            for (auto& acc : accounts) {
                bool isSelected = (nav_state.activeAccount != nullptr && nav_state.activeAccount->id == acc.id);

                if (isSelected) {
                    rl::GuiSetState(rl::STATE_PRESSED);
                }

                if (rl::GuiButton(rl::Rectangle{ startX, startY, buttonWidth, 40 }, acc.email.c_str())) {
                    nav_state.activeAccount = &acc;

                    if (!acc.characters.empty()) {
                        nav_state.activeCharacter = &acc.characters[0];
                    }
                    else {
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

        // --- POPUP DIALOG STATE ---
        static bool showAddDialog = false;
        static char emailBuffer[128] = "";
        static bool textBoxEditMode = false;

        // 1. Click "Add Account +" -> Open the dialog
        if (rl::GuiButton(rl::Rectangle{ 10, addBtnY, addBtnWidth, 40 }, "Add Account +")) {
            showAddAccountDialog = true;     // Changed from showAddDialog
            accountEmailBuffer[0] = '\0';    // Changed from emailBuffer
            accountEditMode = true;          // Changed from textBoxEditMode
        }

        // 2. Render Modal Dialog Boxe
        if (showAddDialog) {
            // Dim/Disable the background controls while modal is active
            rl::GuiLock();

            // Center popup in screen
            float dialogW = 320.0f;
            float dialogH = 170.0f;
            float dialogX = (width - dialogW) / 2.0f;
            float dialogY = (height - dialogH) / 2.0f;

            // Window Box (returns true if close button [X] clicked)
            if (rl::GuiWindowBox(rl::Rectangle{ dialogX, dialogY, dialogW, dialogH }, "New Account")) {
                showAddDialog = false;
                textBoxEditMode = false;
            }

            // Unlock raylib GUI so controls inside the dialog work
            rl::GuiUnlock();

            // Email Label
            rl::GuiLabel(rl::Rectangle{ dialogX + 20, dialogY + 45, dialogW - 40, 20 }, "Enter Email Address:");

            // Text Box Input
            if (rl::GuiTextBox(rl::Rectangle{ dialogX + 20, dialogY + 70, dialogW - 40, 30 }, emailBuffer, 128, textBoxEditMode)) {
                textBoxEditMode = !textBoxEditMode;
            }

            // "Save" Button
            if (rl::GuiButton(rl::Rectangle{ dialogX + 20, dialogY + 115, 130, 35 }, "Save")) {
                if (emailBuffer[0] != '\0') { // Ensure not empty

                    // Track active ID before vector push_back so pointer doesn't corrupt
                    int currentActiveId = nav_state.activeAccount ? nav_state.activeAccount->id : -1;

                    ROK::Account newAccount;
                    newAccount.id = accounts.empty() ? 1 : accounts.back().id + 1;
                    newAccount.email = std::string(emailBuffer);

                    ROK::Character mockChar;
                    mockChar.id = 100 + newAccount.id;
                    mockChar.name = "Governor_" + std::to_string(newAccount.id);

                    newAccount.characters.push_back(mockChar);
                    accounts.push_back(newAccount);

                    // Safely restore pointers after potential vector reallocation
                    nav_state.activeAccount = nullptr;
                    for (auto& acc : accounts) {
                        if (acc.id == currentActiveId) {
                            nav_state.activeAccount = &acc;
                            break;
                        }
                    }

                    Store::SaveAccountsOnly(accounts);
                }

                showAddDialog = false;
                textBoxEditMode = false;
            }

            // "Cancel" Button
            if (rl::GuiButton(rl::Rectangle{ dialogX + 170, dialogY + 115, 130, 35 }, "Cancel")) {
                showAddDialog = false;
                textBoxEditMode = false;
            }
        }
    }

    void RenderCharacterSelectPanel(Navigation& nav_state, float startX, float startY, float width, float height) {

        // Background panel
        rl::GuiPanel(rl::Rectangle{ startX, startY, width, height }, "Characters");

        // If no account is selected, we have nothing to render here
        if (nav_state.activeAccount == nullptr) return;

        float padding = 10.0f;
        float titleBarHeight = 30.0f;

        rl::Rectangle viewRect = { startX + padding, startY + titleBarHeight, width - 2 * padding, height - titleBarHeight - padding };

        float buttonWidth = 100.0f;
        float buttonHeight = viewRect.height - 25.0f;

        int charCount = (int)nav_state.activeAccount->characters.size();

        float totalContentWidth = (charCount + 1) * (buttonWidth + padding) + padding;
        if (totalContentWidth < viewRect.width) {
            totalContentWidth = viewRect.width;
        }

        rl::Rectangle contentRect = { 0, 0, totalContentWidth, viewRect.height - 16.0f };

        static rl::Vector2 scrollPos = { 0, 0 };
        static rl::Rectangle viewScrollRect = { 0 };

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &scrollPos, &viewScrollRect);

        rl::BeginScissorMode((int)viewScrollRect.x, (int)viewScrollRect.y, (int)viewScrollRect.width, (int)viewScrollRect.height);
        {
            float currentX = viewScrollRect.x + scrollPos.x + padding;
            float currentY = viewScrollRect.y + 5.0f;

            for (auto& character : nav_state.activeAccount->characters) {
                bool isSelected = (nav_state.activeCharacter != nullptr && nav_state.activeCharacter->id == character.id);

                if (isSelected) {
                    rl::GuiSetState(rl::STATE_PRESSED);
                }

                if (rl::GuiButton(rl::Rectangle{ currentX, currentY, buttonWidth, buttonHeight }, character.name.c_str())) {
                    nav_state.activeCharacter = &character;
                }

                rl::GuiSetState(rl::STATE_NORMAL);
                currentX += buttonWidth + padding;
            }

            // Click "ADD +" button -> Open popup trigger
            if (rl::GuiButton(rl::Rectangle{ currentX, currentY, buttonWidth, buttonHeight }, "ADD +")) {
                showAddCharDialog = true;
                charNameBuffer[0] = '\0'; // Reset text box
                charEditMode = true;      // Auto-focus input
            }
        }
        rl::EndScissorMode();
    }

    void RenderAddAlarmPanel(Navigation& nav_state, float startX, float startY, float width, float height, std::vector<ROK::Account>& accounts) {
        rl::GuiPanel(rl::Rectangle{ startX, startY, width, height }, "Section For Adding Timers");

        if (nav_state.activeCharacter == nullptr) return;

        // Static states for inputs
        static int hours = 0;
        static int minutes = 0;
        static int seconds = 0;
        static bool hoursEdit = false;
        static bool minsEdit = false;
        static bool secsEdit = false;

        static int activeResource = 0;
        static bool resourceEditMode = false;

        static int activeLevel = 1;
        static bool levelEditMode = false;

        float innerY = startY + 35.0f;
        float currentX = startX + 10.0f;

        rl::GuiLabel(rl::Rectangle{ currentX, innerY, 35, 25 }, "Time:");
        currentX += 40;

        if (rl::GuiValueBox(rl::Rectangle{ currentX, innerY, 35, 25 }, nullptr, &hours, 0, 99, hoursEdit)) hoursEdit = !hoursEdit;
        currentX += 40;
        if (rl::GuiValueBox(rl::Rectangle{ currentX, innerY, 35, 25 }, nullptr, &minutes, 0, 59, minsEdit)) minsEdit = !minsEdit;
        currentX += 40;
        if (rl::GuiValueBox(rl::Rectangle{ currentX, innerY, 35, 25 }, nullptr, &seconds, 0, 59, secsEdit)) secsEdit = !secsEdit;
        currentX += 50;

        // Level Picker
        rl::GuiLabel(rl::Rectangle{ currentX, innerY, 30, 25 }, "Lvl:");
        currentX += 30;
        if (rl::GuiSpinner(rl::Rectangle{ currentX, innerY, 80, 25 }, nullptr, &activeLevel, 1, 9, levelEditMode)) levelEditMode = !levelEditMode;
        currentX += 95;

        // Dropdown Res Label spot reserved
        float resX = currentX;
        currentX += 115;

        // START Button
        if (rl::GuiButton(rl::Rectangle{ currentX, innerY, 80, 25 }, "START")) {
            int totalSecs = (hours * 3600) + (minutes * 60) + seconds;
            if (totalSecs > 0) {
                ROK::Gatherer g;
                g.id = static_cast<int>(nav_state.activeCharacter->gatherers.size()) + 1;
                g.resourceLvl = activeLevel;
                g.resource = static_cast<ROK::ResourceType>(activeResource);
                g.SetTimer(hours, minutes, seconds);

                nav_state.activeCharacter->gatherers.push_back(g);

                Store::SaveAccountsOnly(accounts);

                hours = 0; minutes = 0; seconds = 0;
            }
        }

        // Dropdown Box (Drawn last so it renders on top)
        rl::GuiLabel(rl::Rectangle{ resX, innerY, 30, 25 }, "Res:");
        if (rl::GuiDropdownBox(rl::Rectangle{ resX + 30, innerY, 70, 25 }, "Food;Wood;Stone;Gold;Gems", &activeResource, resourceEditMode)) {
            resourceEditMode = !resourceEditMode;
        }
    }

    void RenderActiveTimersPanel(Navigation& nav_state, float startX, float startY, float width, float height, std::vector<ROK::Account> &accounts) {
        rl::GuiPanel(rl::Rectangle{ startX, startY, width, height }, "Active Timers");

        if (nav_state.activeCharacter == nullptr) return;

        float padding = 10.0f;
        rl::Rectangle viewRect = { startX + padding, startY + 25.0f, width - 2 * padding, height - 35.0f };

        float itemHeight = 40.0f;
        float totalContentHeight = nav_state.activeCharacter->gatherers.size() * (itemHeight + padding) + padding;

        if (totalContentHeight < viewRect.height) {
            totalContentHeight = viewRect.height;
        }

        rl::Rectangle contentRect = { 0, 0, viewRect.width - 16.0f, totalContentHeight };

        static rl::Vector2 scrollPos = { 0, 0 };
        static rl::Rectangle viewScrollRect = { 0 };

        rl::GuiScrollPanel(viewRect, nullptr, contentRect, &scrollPos, &viewScrollRect);

        rl::BeginScissorMode((int)viewScrollRect.x, (int)viewScrollRect.y, (int)viewScrollRect.width, (int)viewScrollRect.height);
        {
            float currentX = viewScrollRect.x + padding;
            float currentY = viewScrollRect.y + scrollPos.y + padding;
            float itemWidth = contentRect.width - padding;

            auto& gatherers = nav_state.activeCharacter->gatherers;

            // Use index-based loop for safe deletion during iteration
            for (size_t i = 0; i < gatherers.size(); ) {
                auto& gatherer = gatherers[i];

                // Background box for individual timer
                rl::GuiPanel(rl::Rectangle{ currentX, currentY, itemWidth, itemHeight }, nullptr);

                // Timer display text
                rl::GuiLabel(rl::Rectangle{ currentX + 10, currentY + 5, 200, 30 }, gatherer.GetFormattedTime().c_str());

                // Determine if the timer is done or active
                bool isFinished = !gatherer.isActive || (gatherer.GetRemainingSeconds() <= 0.0f);
                const char* buttonLabel = isFinished ? "REMOVE" : "STOP";

                // Render STOP / REMOVE button
                if (rl::GuiButton(rl::Rectangle{ currentX + itemWidth - 80, currentY + 5, 70, 30 }, buttonLabel)) {
                    // Remove timer from vector
                    gatherers.erase(gatherers.begin() + i);

                    // Save updated accounts vector immediately
                    Store::SaveAccountsOnly(accounts);

                    // Do not increment `i` since element at `i` was erased
                    continue;
                }

                currentY += itemHeight + padding;
                i++; // Advance loop index
            }
        }
        rl::EndScissorMode();
    }

    // Renders the Add Account Popup (Top-most layer)
    void RenderAddAccountDialog(Navigation& nav_state, std::vector<ROK::Account>& accounts, int screenWidth, int screenHeight) {
        if (!showAddAccountDialog) return; // 'showAddAccountDialog' from AccountPanel

        rl::GuiLock(); // Lock lower layers

        float dialogW = 320.0f;
        float dialogH = 170.0f;
        float dialogX = (screenWidth - dialogW) / 2.0f;
        float dialogY = (screenHeight - dialogH) / 2.0f;

        if (rl::GuiWindowBox(rl::Rectangle{ dialogX, dialogY, dialogW, dialogH }, "New Account")) {
            showAddAccountDialog = false;
            accountEditMode = false;
        }

        rl::GuiUnlock();

        rl::GuiLabel(rl::Rectangle{ dialogX + 20, dialogY + 45, dialogW - 40, 20 }, "Enter Email Address:");

        if (rl::GuiTextBox(rl::Rectangle{ dialogX + 20, dialogY + 70, dialogW - 40, 30 }, accountEmailBuffer, 128, accountEditMode)) {
            accountEditMode = !accountEditMode;
        }

        // SAVE BUTTON
        if (rl::GuiButton(rl::Rectangle{ dialogX + 20, dialogY + 115, 130, 35 }, "Save")) {
            if (accountEmailBuffer[0] != '\0') {
                int currentActiveId = nav_state.activeAccount ? nav_state.activeAccount->id : -1;

                ROK::Account newAccount;
                newAccount.id = accounts.empty() ? 1 : accounts.back().id + 1;
                newAccount.email = std::string(accountEmailBuffer);

                accounts.push_back(newAccount);

                // Re-point safely after potential vector reallocation
                nav_state.activeAccount = nullptr;
                for (auto& acc : accounts) {
                    if (acc.id == currentActiveId) {
                        nav_state.activeAccount = &acc;
                        break;
                    }
                }

                Store::SaveAccountsOnly(accounts);
            }
            showAddAccountDialog = false;
            accountEditMode = false;
        }

        // CANCEL BUTTON
        if (rl::GuiButton(rl::Rectangle{ dialogX + 170, dialogY + 115, 130, 35 }, "Cancel")) {
            showAddAccountDialog = false;
            accountEditMode = false;
        }
    }

    // Renders the Add Character Popup (Top-most layer)
    void RenderAddCharacterDialog(Navigation& nav_state, std::vector<ROK::Account>& accounts, int screenWidth, int screenHeight) {
        if (!showAddCharDialog || nav_state.activeAccount == nullptr) return;

        rl::GuiLock();

        float dialogW = 320.0f;
        float dialogH = 170.0f;
        float dialogX = (screenWidth - dialogW) / 2.0f;
        float dialogY = (screenHeight - dialogH) / 2.0f;

        if (rl::GuiWindowBox(rl::Rectangle{ dialogX, dialogY, dialogW, dialogH }, "New Character")) {
            showAddCharDialog = false;
            charEditMode = false;
        }

        rl::GuiUnlock();

        rl::GuiLabel(rl::Rectangle{ dialogX + 20, dialogY + 45, dialogW - 40, 20 }, "Enter Character Name:");

        if (rl::GuiTextBox(rl::Rectangle{ dialogX + 20, dialogY + 70, dialogW - 40, 30 }, charNameBuffer, 128, charEditMode)) {
            charEditMode = !charEditMode;
        }

        // SAVE BUTTON
        if (rl::GuiButton(rl::Rectangle{ dialogX + 20, dialogY + 115, 130, 35 }, "Save")) {
            if (charNameBuffer[0] != '\0') {
                ROK::Character newChar;
                newChar.id = nav_state.activeAccount->characters.empty() ? 101 : nav_state.activeAccount->characters.back().id + 1;
                newChar.name = std::string(charNameBuffer);

                int activeCharId = nav_state.activeCharacter ? nav_state.activeCharacter->id : -1;

                // Add character to active account
                nav_state.activeAccount->characters.push_back(newChar);

                // Re-point activeCharacter in case vector reallocated
                nav_state.activeCharacter = nullptr;
                for (auto& ch : nav_state.activeAccount->characters) {
                    if (ch.id == activeCharId) {
                        nav_state.activeCharacter = &ch;
                        break;
                    }
                }

                Store::SaveAccountsOnly(accounts);
            }
            showAddCharDialog = false;
            charEditMode = false;
        }

        // CANCEL BUTTON
        if (rl::GuiButton(rl::Rectangle{ dialogX + 170, dialogY + 115, 130, 35 }, "Cancel")) {
            showAddCharDialog = false;
            charEditMode = false;
        }
    }

    void InitTheme() {
        const unsigned char* styleData = nullptr;
        unsigned int styleSize = 0;

        if (GetEmbeddedBuffer(102, &styleData, &styleSize)) {
            rl::GuiLoadStyleFromMemory(styleData, styleSize);
        }
        else {
            rl::GuiLoadStyle("resources/style_dark.rgs");
        }
    }
}