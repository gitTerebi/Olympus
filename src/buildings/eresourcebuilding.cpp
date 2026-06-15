#include "eresourcebuilding.h"
#include "fileIO/save-archive.h"

#include "textures/game-textures.h"
#include "textures/building-textures.h"
#include "engine/game-board.h"
#include "engine/edate.h"
#include "numbers.h"

#include <algorithm>

eBuildingType resourceTypeToBuildingType(const eResourceBuildingType r) {
    switch(r) {
    case eResourceBuildingType::oliveTree:
        return eBuildingType::oliveTree;
    case eResourceBuildingType::vine:
        return eBuildingType::vine;
    case eResourceBuildingType::orangeTree:
        return eBuildingType::orangeTree;
    }
    return eBuildingType::oliveTree;
}

int resourceTypeToRipePeriod(const eResourceBuildingType r) {
    switch(r) {
    case eResourceBuildingType::oliveTree:
        return Numbers::sOliveTreeRipePeriod;
    case eResourceBuildingType::vine:
        return Numbers::sVineRipePeriod;
    case eResourceBuildingType::orangeTree:
        return Numbers::sOrangeTreeRipePeriod;
    }
    return Numbers::sOliveTreeRipePeriod;
}

static bool isOliveHarvestMonth(const eMonth m) {
    return m == eMonth::january ||
           m == eMonth::february ||
           m == eMonth::march;
}

static bool isGrapeHarvestMonth(const eMonth m) {
    return m == eMonth::october ||
           m == eMonth::november ||
           m == eMonth::december;
}

static bool isOrangeHarvestMonth(const eMonth m) {
    return m == eMonth::january ||
           m == eMonth::february ||
           m == eMonth::march;
}

eResourceBuilding::eResourceBuilding(
        GameBoard& board, const eResourceBuildingType type,
        const eCityId cid) :
    eBuilding(board, resourceTypeToBuildingType(type), 1, 1, cid),
    mType(type), mRipePeriod(resourceTypeToRipePeriod(type)) {
    switch(type) {
    case eResourceBuildingType::oliveTree:
        GameTextures::loadOliveTree();
        break;
    case eResourceBuildingType::vine:
        GameTextures::loadVine();
        break;
    case eResourceBuildingType::orangeTree:
        //GameTextures::loadOrangeTree();
        break;
    }
}

std::shared_ptr<Texture> eResourceBuilding::getTexture(const eTileSize size) const {
    const auto& texs = GameTextures::buildings();
    const int sizeId = static_cast<int>(size);
    const auto& colls = texs[sizeId];
    switch(mType) {
    case eResourceBuildingType::oliveTree:
        return colls.fOliveTree.getTexture(mRipe);
    case eResourceBuildingType::vine:
        return colls.fVine.getTexture(mRipe);
    case eResourceBuildingType::orangeTree:
        return colls.fOrangeTree.getTexture(mRipe);
    }
    return std::shared_ptr<Texture>();
}

void eResourceBuilding::erase() {
    if(mSanctuary) return;
    eBuilding::erase();
}

void eResourceBuilding::sanctuaryErase() {
    eBuilding::erase();
}

bool eResourceBuilding::sIsResourceBuilding(const eBuildingType type) {
    return type == eBuildingType::oliveTree ||
           type == eBuildingType::orangeTree ||
           type == eBuildingType::vine;
}

int eResourceBuilding::takeResource(const int by) {
    if(mRipe < 3) return 0;
    const auto m = getBoard().date().month();
    if(mType == eResourceBuildingType::oliveTree &&
       !isOliveHarvestMonth(m)) {
        return 0;
    }
    if(mType == eResourceBuildingType::vine &&
       !isGrapeHarvestMonth(m)) {
        return 0;
    }
    if(mType == eResourceBuildingType::orangeTree &&
       !isOrangeHarvestMonth(m)) {
        return 0;
    }
    (void)by;
    mResource = 0;
    mRipe = 0;
    mNextRipe = 0;
    mWorkedOn = false;
    const auto tile = centerTile();
    tile->scheduleTerrainUpdate();
    return 1;
}

void eResourceBuilding::workOn() {
    mWorkedOn = true;
    mNextRipe = 0;
}

void eResourceBuilding::setSanctuary(const bool s) {
    mSanctuary = s;
}

void eResourceBuilding::timeChanged(const int by) {
    mNextRipe += by;
    if(mType == eResourceBuildingType::oliveTree && mRipe >= 5) {
        return;
    }
    const double mult = Numbers::sTreeVineFullyRipePeriodMultiplier;
    int wait = mRipe >= 5 ? mult*mRipePeriod : mRipePeriod;
    if(mSanctuary || blessed()) {
        if(mRipe >= 5) {
            wait *= Numbers::sTreeVineBlessedFullyRipePeriodMultiplier;
        } else {
            wait *= Numbers::sTreeVineBlessedRipePeriodMultiplier;
        }
    } else if(cursed()) {
        if(mRipe >= 5) {
            wait *= Numbers::sTreeVineCursedFullyRipePeriodMultiplier;
        } else {
            wait *= Numbers::sTreeVineCursedRipePeriodMultiplier;
        }
    }
    if((mWorkedOn || mRipe >= 5) && mNextRipe > wait) {
        mNextRipe -= wait;
        if(mType != eResourceBuildingType::oliveTree) {
            mWorkedOn = false;
        }
        if(mRipe >= 5) {
            mRipe = 0;
            mResource = 0;
            mWorkedOn = false;
            return;
        }
        ++mRipe;
        if(mRipe >= 3) {
            mResource = 1;
        }
        const auto tile = centerTile();
        tile->scheduleTerrainUpdate();
    }
}

void eResourceBuilding::nextMonth() {
    const auto m = getBoard().date().month();
    const bool oliveEnd = mType == eResourceBuildingType::oliveTree &&
                          m == eMonth::april;
    const bool orangeEnd = mType == eResourceBuildingType::orangeTree &&
                           m == eMonth::april;
    const bool grapeEnd = mType == eResourceBuildingType::vine &&
                          m == eMonth::january;
    if((oliveEnd || orangeEnd || grapeEnd) && mRipe >= 3) {
        mNextRipe = 0;
        mRipe = 0;
        mResource = 0;
        mWorkedOn = false;
        const auto tile = centerTile();
        tile->scheduleTerrainUpdate();
    }
}

void eResourceBuilding::serializeFields(SaveArchive& ar) {
    eBuilding::serializeFields(ar);
    ar.field("sanctuary", mSanctuary);
    ar.field("workedOn", mWorkedOn);
    ar.field("nextRipe", mNextRipe);
    ar.field("ripe", mRipe);
    ar.field("resource", mResource);
}
