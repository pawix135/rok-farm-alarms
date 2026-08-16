#include "store.h"
#include "raylib_wrapper.h"
#include <fstream>
#include "config.h"
#include <iostream>

namespace Store {

    constexpr const char* SAVE_FILE_PATH = "save.json";

    static SaveData GetCurrentSaveOrDefault() {
        std::ifstream file(SAVE_FILE_PATH);
        if (!file.is_open()) {
            return SaveData();
        }

        try {
            nlohmann::json j;
            file >> j;
            return j.get<SaveData>();
        }
        catch (...) {
            return SaveData();
        }
    }

    SaveData LoadAll() {
        return GetCurrentSaveOrDefault();
    }

    std::vector<ROK::Account> LoadAccounts() {
        return GetCurrentSaveOrDefault().accounts;
    }

    bool SaveAll(const SaveData& data) {
        try {
            nlohmann::json j = data;
            std::ofstream file(SAVE_FILE_PATH);
            if (!file.is_open()) return false;

            file << j.dump(4);
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool SaveAccountsOnly(const std::vector<ROK::Account>& accounts) {
        SaveData current = GetCurrentSaveOrDefault();
        current.accounts = accounts;
        return SaveAll(current);
    }

    bool SaveWindowSizeOnly(int width, int height) {
        SaveData current = GetCurrentSaveOrDefault();
        current.window_width = width;
        current.window_height = height;
        return SaveAll(current);
    }

    bool UpdateVersionOnly(int newVersion) {
        SaveData current = GetCurrentSaveOrDefault();
        current.version = newVersion;
        return SaveAll(current);
    }
}