#pragma once
#include <vector>
#include <string>
#include <functional>
#include "json.hpp"
#include "helpers.h"

namespace ROK
{

    enum class ResourceType
    {
        Food,
        Wood,
        Stone,
        Gold,
        Gems
    };

    struct ResourceImages {
        rl::Texture2D gem;
        rl::Texture2D food;
        rl::Texture2D wood;
        rl::Texture2D stone;
        rl::Texture2D gold;

        void LoadAll();
        void UnloadAll();
    };

    const rl::Texture2D& GetResourceTexture(ROK::ResourceType type, const ROK::ResourceImages& images);

    struct Gatherer
    {
        int id;
        int resourceLvl;
        ResourceType resource;
        long long targetTimestamp;
        bool isActive;

        Gatherer() = default;

        std::string GetFormattedTime() const;
        float GetRemainingSeconds() const;
        void SetTimer(int hours, int minutes, int seconds);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
        Gatherer,
        id,
        resourceLvl,
        resource,
        targetTimestamp,
        isActive)

    struct Character
    {
        int id;
        std::string name;
        std::vector<Gatherer> gatherers;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Character, id, name, gatherers)

    struct Account
    {
        int id;
        std::string email;
        std::vector<Character> characters;

        void AddCharacter(int charId, std::string charName);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Account, id, email, characters)

    using GathererVisitor = std::function<void(Account &, Character &, Gatherer &)>;
    void ForEachGatherer(std::vector<Account> &accounts, const GathererVisitor &visitor);
}