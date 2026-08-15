#pragma once
#include <vector>
#include "rok.h"
#include "json.hpp"
#include "config.h"

namespace Store {

    struct SaveData {
        int version = APP_VERSION;
        int window_width = DEFAULT_WINDOW_WIDTH;
        int window_height = DEFAULT_WINDOW_HEIGHT;
        std::vector<ROK::Account> accounts = {};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
        SaveData,
        version,
        window_width,
        window_height,
        accounts
    );

    std::vector<ROK::Account> LoadAccounts();
    SaveData LoadAll();

    bool SaveAll(const SaveData& data);
    bool SaveAccountsOnly(const std::vector<ROK::Account>& accounts);
    bool SaveWindowSizeOnly(int width, int height);
    bool UpdateVersionOnly(int newVersion);
}