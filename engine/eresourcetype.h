#ifndef ERESOURCETYPE_H
#define ERESOURCETYPE_H

#include <vector>
#include <string>

#include "etexture.h"
#include "widgets/eresolution.h"

enum class eResourceType {
    none = 0,

    urchin = 1 << 0,
    fish = 1 << 1,
    meat = 1 << 2,
    cheese = 1 << 3,
    carrots = 1 << 4,
    onions = 1 << 5,
    wheat = 1 << 6,
    oranges = 1 << 7,

    food = (1 << 8) - 1,

    grapes = 1 << 8,
    olives = 1 << 9,
    wine = 1 << 10,
    oliveOil = 1 << 11,
    fleece = 1 << 12,

    wood = 1 << 13,
    bronze = 1 << 14,
    marble = 1 << 15,

    armor = 1 << 16,
    sculpture = 1 << 17,

    orichalc = 1 << 18,
    blackMarble = 1 << 19,

    allBasic = (1 << 20) - 1,
    warehouse = (allBasic & ~food) | wheat,
    tradePost = allBasic,

    horse = 1 << 20,
    chariot = 1 << 21,

    allTransportable = (1 << 22) - 1,

    silver = 1 << 22,
    drachmas = 1 << 23
};

inline eResourceType operator|(const eResourceType a, const eResourceType b) {
    return static_cast<eResourceType>(static_cast<int>(a) | static_cast<int>(b));
}

inline eResourceType operator&(const eResourceType a, const eResourceType b) {
    return static_cast<eResourceType>(static_cast<int>(a) & static_cast<int>(b));
}

inline eResourceType operator~(const eResourceType a) {
    return static_cast<eResourceType>(~static_cast<int>(a));
}

namespace eResourceTypeHelpers {
    std::vector<eResourceType> extractResourceTypes(
            const eResourceType from);
    std::string typeName(const eResourceType type);
    std::string typeLongName(const eResourceType type);
    std::shared_ptr<eTexture> icon(const eUIScale scale, const eResourceType type);
    inline int transportSize(const eResourceType type,
                             const bool doubleCapacity = false) {
        int base = 0;
        switch(type) {
        case eResourceType::urchin:
        case eResourceType::fish:
        case eResourceType::meat:
        case eResourceType::cheese:
        case eResourceType::carrots:
        case eResourceType::onions:
        case eResourceType::wheat:
        case eResourceType::oranges:
        case eResourceType::food:
        case eResourceType::grapes:
        case eResourceType::olives:
        case eResourceType::wine:
        case eResourceType::oliveOil:
        case eResourceType::fleece:
        case eResourceType::wood:
        case eResourceType::bronze:
        case eResourceType::marble:
        case eResourceType::orichalc:
        case eResourceType::blackMarble:
        case eResourceType::armor:
            base = 4;
            break;
        case eResourceType::sculpture:
            base = 1;
            break;
        default:
            break;
        }
        return doubleCapacity ? base * 2 : base;
    }
    int defaultPrice(const eResourceType type);
    bool isSingleType(const eResourceType type);
}

#endif // ERESOURCETYPE_H
