#include "rok.h"
#include <cstdio>
#include <ctime>

namespace ROK {

    std::string Gatherer::GetFormattedTime() const {
        float totalSecs = GetRemainingSeconds();
        int h = static_cast<int>(totalSecs) / 3600;
        int m = (static_cast<int>(totalSecs) % 3600) / 60;
        int s = static_cast<int>(totalSecs) % 60;

        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", h, m, s);
        return std::string(buffer);
    }

    float Gatherer::GetRemainingSeconds() const {
        if (!isActive) return 0.0f;
        long long now = static_cast<long long>(std::time(nullptr));
        long long diff = targetTimestamp - now;
        return diff > 0 ? static_cast<float>(diff) : 0.0f;
    }

    void Gatherer::SetTimer(int hours, int minutes, int seconds) {
        long long duration = (hours * 3600LL) + (minutes * 60LL) + seconds;
        targetTimestamp = static_cast<long long>(std::time(nullptr)) + duration;
        isActive = true;
    }

    void Account::AddCharacter(int charId, std::string charName) {
        characters.push_back({ charId, charName, {} });
    }

}