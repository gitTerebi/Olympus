#ifndef ESTATERELEVANCE_H
#define ESTATERELEVANCE_H

enum class eStateRelevance {
    none = 0,
    terrain = 1 << 0,
    buildings = 1 << 1,
    resourcesInBuildings = 1 << 2,
    characters = 1 << 3,
    all = (1 << 4) - 1,
};

inline eStateRelevance operator|(const eStateRelevance a, const eStateRelevance b) {
    return static_cast<eStateRelevance>(static_cast<int>(a) | static_cast<int>(b));
}

inline eStateRelevance operator&(const eStateRelevance a, const eStateRelevance b) {
    return static_cast<eStateRelevance>(static_cast<int>(a) & static_cast<int>(b));
}

#endif // ESTATERELEVANCE_H
