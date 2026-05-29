#ifndef ETHREADBUILDING_H
#define ETHREADBUILDING_H

#include "buildings/estoragebuilding.h"

class eThreadBuilding {
public:
    void load(eBuilding* const src);
    eBuildingType type() const { return mType; }

    int houseVacancies() const { return mVacancies; }

    bool resourcesAvailable() const { return mResourcesAvailable; }
    bool workedOn() const { return mWorkedOn; }

    bool hasAnimal() const { return mWorkedOn; }
    int treeResource() const { return mTreeResource; }

    int resourceCount(const eResourceType type) const;
    int resourceSpaceLeft(const eResourceType type) const;

    bool resourceHas(const eResourceType type) const;
    bool resourceHasSpace(const eResourceType type) const;

    eResourceType gets() const { return mGet; }
    eResourceType empties() const { return mEmpty; }
    eResourceType imports() const { return mImports; }

    bool gets(const eResourceType res) const;
    bool empties(const eResourceType res) const;

    const SDL_Rect& tileRect() const { return mTileRect; }
private:
    bool mCleared = false;
    SDL_Rect mTileRect;
    eBuildingType mType{eBuildingType::none};

    int mVacancies{0};

    bool mResourcesAvailable = false; // for sanctuary buildings
    bool mWorkedOn = false;
    int mTreeResource = 0;

    int mSpaceCount;
    int mResourceCount[15] = {0};
    eResourceType mResource[15] = {eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none};
    std::map<eResourceType, int> mMaxCount;

    eResourceType mGet = eResourceType::none;
    eResourceType mEmpty = eResourceType::none;
    eResourceType mImports = eResourceType::none; // trade post imported goods
    eResourceType mAccepts = eResourceType::none; // includes get
    int mRawSpaceLeft{0};
};

#endif // ETHREADBUILDING_H
