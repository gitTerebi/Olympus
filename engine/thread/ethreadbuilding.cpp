#include "ethreadbuilding.h"

#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "buildings/eresourcebuildingbase.h"
#include "buildings/eresourcebuilding.h"
#include "buildings/sanctuaries/esanctbuilding.h"
#include "buildings/eanimalbuilding.h"
#include "buildings/echariotfactory.h"
#include "buildings/ehorseranch.h"
#include "buildings/ehorseranchenclosure.h"
#include "buildings/sanctuaries/esanctuary.h"
#include "buildings/eaestheticsbuilding.h"
#include "buildings/pyramids/epyramidelement.h"

void eThreadBuilding::load(eBuilding* const src) {
    if(!mCleared) {
        mVacancies = 0;

        mResourcesAvailable = false;
        mWorkedOn = false;

        mSpaceCount = 0;
        memset(mResourceCount, 0, sizeof(mResourceCount));
        memset(mResource, 0, sizeof(mResource));
        mMaxCount.clear();
        mGet = eResourceType::none;
        mEmpty = eResourceType::none;
        mAccepts = eResourceType::none;
        mCleared = true;
    }

    if(src) {
        mType = src->type();
        mTileRect = src->tileRect();
        mCleared = false;
        switch(mType) {
        case eBuildingType::road:
        case eBuildingType::park:
        case eBuildingType::doricColumn:
        case eBuildingType::ionicColumn:
        case eBuildingType::corinthianColumn:
        case eBuildingType::avenue:

        case eBuildingType::commemorative:
        case eBuildingType::godMonument:
        case eBuildingType::godMonumentTile:

        case eBuildingType::bench:
        case eBuildingType::flowerGarden:
        case eBuildingType::gazebo:
        case eBuildingType::hedgeMaze:
        case eBuildingType::fishPond:

        case eBuildingType::waterPark:

        case eBuildingType::birdBath:
        case eBuildingType::shortObelisk:
        case eBuildingType::tallObelisk:
        case eBuildingType::shellGarden:
        case eBuildingType::sundial:
        case eBuildingType::dolphinSculpture:
        case eBuildingType::orrery:
        case eBuildingType::spring:
        case eBuildingType::topiary:
        case eBuildingType::baths:
        case eBuildingType::stoneCircle:
            mCleared = true;
            break;
        case eBuildingType::commonHouse: {
            const auto h = static_cast<eSmallHouse*>(src);
            mVacancies = h->vacancies();
        } break;
        case eBuildingType::eliteHousing: {
            const auto h = static_cast<eEliteHousing*>(src);
            mVacancies = h->vacancies();
        } break;
        case eBuildingType::granary:
        case eBuildingType::warehouse:
        case eBuildingType::tradePost: {
            const auto s = static_cast<eStorageBuilding*>(src);
            mAccepts = s->accepts();
            mGet = s->get();
            mEmpty = s->empties();
            const auto rc = s->resourceCounts();
            const auto rt = s->resourceTypes();
            mSpaceCount = s->spaceCount();
            for(int i = 0; i < mSpaceCount; i++) {
                mResourceCount[i] = rc[i];
                mResource[i] = rt[i];
            }
            mMaxCount = s->maxCount();
        } break;
        case eBuildingType::corral:
        case eBuildingType::carrotsFarm:
        case eBuildingType::onionsFarm:
        case eBuildingType::wheatFarm:
        case eBuildingType::dairy:
        case eBuildingType::cardingShed:
        case eBuildingType::armory:
        case eBuildingType::olivePress:
        case eBuildingType::sculptureStudio:
        case eBuildingType::winery:
        case eBuildingType::fishery:
        case eBuildingType::huntingLodge:
        case eBuildingType::urchinQuay:
        case eBuildingType::foundry:
        case eBuildingType::masonryShop:
        case eBuildingType::refinery:
        case eBuildingType::blackMarbleWorkshop:
        case eBuildingType::mint:
        case eBuildingType::timberMill: {
            const auto b = static_cast<eResourceBuildingBase*>(src);
            mResource[0] = b->resourceType();
            mResourceCount[0] = b->resource();
            mSpaceCount = 1;
        } break;
        case eBuildingType::horseRanch: {
            const auto b = static_cast<eHorseRanch*>(src);
            mResource[0] = eResourceType::horse;
            mResourceCount[0] = b->horseCount();
            mSpaceCount = 1;
        } break;
        case eBuildingType::horseRanchEnclosure: {
            const auto e = static_cast<eHorseRanchEnclosure*>(src);
            mResource[0] = eResourceType::horse;
            mResourceCount[0] = e->horseCount();
            mSpaceCount = 1;
        } break;
        case eBuildingType::chariotFactory: {
            const auto b = static_cast<eChariotFactory*>(src);
            mResource[0] = eResourceType::chariot;
            mResourceCount[0] = b->chariotCount();
            mSpaceCount = 1;
        } break;
        case eBuildingType::vine:
        case eBuildingType::oliveTree:
        case eBuildingType::orangeTree: {
            const auto b = static_cast<eResourceBuilding*>(src);
            mWorkedOn = b->workedOn();
        } break;
        case eBuildingType::temple:
        case eBuildingType::templeAltar:
        case eBuildingType::templeMonument:
        case eBuildingType::templeStatue:
        case eBuildingType::templeTile:

        case eBuildingType::pyramidPart:
        case eBuildingType::pyramidWall:
        case eBuildingType::pyramidTop:
        case eBuildingType::pyramidStatue:
        case eBuildingType::pyramidMonument:
        case eBuildingType::pyramidTile:
        case eBuildingType::pyramidAltar:
        case eBuildingType::pyramidTemple:
        case eBuildingType::pyramidObservatory:
        case eBuildingType::pyramidMuseum: {
            const auto b = static_cast<eSanctBuilding*>(src);
            mWorkedOn = b->workedOn();
            const auto s = b->monument();
            const bool h = s->constructionHalted() || b->halted();
            if(h) {
                mResourcesAvailable = false;
            } else {
                mResourcesAvailable = b->resourcesAvailable();
            }
        } break;
        case eBuildingType::sheep:
        case eBuildingType::goat:
        case eBuildingType::cattle: {
            const auto b = static_cast<eAnimalBuilding*>(src);
            mWorkedOn = b->animal();
        } break;
        default:
            break;
        }
    } else {
        mType = eBuildingType::none;
    }
}

int eThreadBuilding::resourceCount(const eResourceType type) const {
    if(mType == eBuildingType::granary ||
       mType == eBuildingType::warehouse ||
       mType == eBuildingType::tradePost) {
        return eStorageBuilding::sCount(type, mResourceCount,
                                        mResource, mSpaceCount);
    } else {
        const auto t = mResource[0];
        if(!static_cast<bool>(t & type)) return 0;
        return mResourceCount[0];
    }
}

int eThreadBuilding::resourceSpaceLeft(const eResourceType type) const {
    const bool accepts = static_cast<bool>(mAccepts & type);
    if(!accepts) return 0;
    if(mType == eBuildingType::granary ||
       mType == eBuildingType::warehouse ||
       mType == eBuildingType::tradePost) {
        return eStorageBuilding::sSpaceLeft(type, mResourceCount, mResource,
                                            mAccepts, mMaxCount, mSpaceCount);
    } else {
        return 0;
    }
}

bool eThreadBuilding::resourceHas(const eResourceType type) const {
    for(int i = 0; i < mSpaceCount; i++) {
        const int c = mResourceCount[i];
        if(c <= 0) continue;
        const auto t = mResource[i];
        if(!static_cast<bool>(t & type)) continue;
        return true;
    }
    return false;
}

bool eThreadBuilding::resourceHasSpace(const eResourceType type) const {
    return resourceSpaceLeft(type) > 0;
}

bool eThreadBuilding::gets(const eResourceType res) const {
    return static_cast<bool>(res & mGet);
}

bool eThreadBuilding::empties(const eResourceType res) const {
    return static_cast<bool>(res & mEmpty);
}
