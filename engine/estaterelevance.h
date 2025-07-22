#ifndef ESTATERELEVANCE_H
#define ESTATERELEVANCE_H

enum class eStateRelevance {
    none = 0,
    terrain = 1 << 0, // except forests
    forests = 1 << 1,
    buildings = 1 << 2,
    resourcesInBuildings = 1 << 3,
    houseVacancies = 1 << 4,
    treesAndVines = 1 << 5,
    domesticatedAnimals = 1 << 6,
    resources = 1 << 7,
    all = (1 << 8) - 1,
};

inline eStateRelevance operator|(const eStateRelevance a, const eStateRelevance b) {
    return static_cast<eStateRelevance>(static_cast<int>(a) | static_cast<int>(b));
}

inline eStateRelevance operator&(const eStateRelevance a, const eStateRelevance b) {
    return static_cast<eStateRelevance>(static_cast<int>(a) & static_cast<int>(b));
}

#endif // ESTATERELEVANCE_H
