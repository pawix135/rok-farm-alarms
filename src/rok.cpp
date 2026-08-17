#include "rok.h"
#include <cstdio>
#include <ctime>

#include "images/img_gem.h"
#include "images/img_food.h"
#include "images/img_wood.h"
#include "images/img_stone.h"
#include "images/img_gold.h"

namespace ROK
{
    void ResourceImages::LoadAll() {
        food = LoadTextureFromMemory(".png", food_png, food_png_len);
        wood = LoadTextureFromMemory(".png", wood_png, wood_png_len);
        stone = LoadTextureFromMemory(".png", stone_png, stone_png_len);
        gold = LoadTextureFromMemory(".png", gold_png, gold_png_len);
        gem = LoadTextureFromMemory(".png", gem_png, gem_png_len);
    }

    const rl::Texture2D& GetResourceTexture(ROK::ResourceType type, const ROK::ResourceImages& images) {
        switch (type) {
        case ROK::ResourceType::Food: return images.food;
        case ROK::ResourceType::Wood: return images.wood;
        case ROK::ResourceType::Stone: return images.stone;
        case ROK::ResourceType::Gold: return images.gold;
        case ROK::ResourceType::Gems: return images.gem;
        default: return images.food;
        }
    }

    void ResourceImages::UnloadAll() {
        UnloadTexture(gem);
        UnloadTexture(food);
        UnloadTexture(wood);
        UnloadTexture(stone);
        UnloadTexture(gold);
    }

    std::string Gatherer::GetFormattedTime() const
    {
        float totalSecs = GetRemainingSeconds();
        int h = static_cast<int>(totalSecs) / 3600;
        int m = (static_cast<int>(totalSecs) % 3600) / 60;
        int s = static_cast<int>(totalSecs) % 60;

        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", h, m, s);
        return std::string(buffer);
    }

    float Gatherer::GetRemainingSeconds() const
    {
        if (!isActive)
            return 0.0f;
        long long now = static_cast<long long>(std::time(nullptr));
        long long diff = targetTimestamp - now;
        return diff > 0 ? static_cast<float>(diff) : 0.0f;
    }

    void Gatherer::SetTimer(int hours, int minutes, int seconds)
    {
        long long duration = (hours * 3600LL) + (minutes * 60LL) + seconds;
        targetTimestamp = static_cast<long long>(std::time(nullptr)) + duration;
        isActive = true;
    }

    void Account::AddCharacter(int charId, std::string charName)
    {
        characters.push_back({charId, charName, {}});
    }

    void ForEachGatherer(std::vector<Account> &accounts, const GathererVisitor &visitor)
    {
        for (auto &account : accounts)
        {
            for (auto &character : account.characters)
            {
                for (auto &gatherer : character.gatherers)
                {
                    visitor(account, character, gatherer);
                }
            }
        }
    }

}