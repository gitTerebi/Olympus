#include "walkable-object.h"

#include "buildings/ebuilding.h"
#include "buildings/eroad.h"
#include "engine/etile.h"
#include "fileIO/esavearchive.h"

#include "erectwalkableobject.h"
#include "ehasresourcewalkableobject.h"

namespace {

bool ranchTileWalkable(eTileBase* const t) {
    const auto tt = t->underBuildingType();
    if(tt != eBuildingType::horseRanchEnclosure) return false;

    const auto bl = t->bottomLeft();
    if(!bl) return false;
    const auto blt = bl->underBuildingType();
    if(blt != eBuildingType::horseRanchEnclosure) return false;

    const auto br = t->bottomRight();
    if(!br) return false;
    const auto brt = br->underBuildingType();
    if(brt != eBuildingType::horseRanchEnclosure) return false;

    return true;
}

class RanchRectWalkableObject : public WalkableObject {
public:
    RanchRectWalkableObject() :
        WalkableObject(eWalkableObjectType::ranchRect) {}
    RanchRectWalkableObject(const SDL_Rect& rect) :
        RanchRectWalkableObject() {
        mRect = rect;
    }

    bool walkable(eTileBase* const t) const override {
        const SDL_Point p{t->x(), t->y()};
        if(!SDL_PointInRect(&p, &mRect)) return false;
        return ranchTileWalkable(t);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.field("rect", mRect);
    }
private:
    SDL_Rect mRect{0, 0, 0, 0};
};

}

bool WalkableObject::walkable(eTileBase* const t) const {
    switch(mType) {
    case eWalkableObjectType::ddefault:
        return t->walkable();
    case eWalkableObjectType::artisan: {
        const auto type = t->underBuildingType();
        const bool p = eBuilding::sPyramidBuilding(type);
        if(p) return true;
        return t->walkable();
    } break;
    case eWalkableObjectType::road:
        return t->hasRoad();
    case eWalkableObjectType::roadAvenue: {
        const auto type = t->underBuildingType();
        const bool hr = type == eBuildingType::road;
        if(hr) return true;
        const bool a = type == eBuildingType::avenue;
        if(a) return true;
        return false;
    } break;
    case eWalkableObjectType::roadblock: {
        const bool hr = t->hasRoad();
        if(!hr) return false;
        return !t->hasRoadblock();
    }
    case eWalkableObjectType::roadblockNoAgora: {
        if(!t->hasRoad()) return false;
        if(t->hasRoadblock()) return false;
        const auto tt = static_cast<const eTile*>(t);
        const auto r = dynamic_cast<const eRoad*>(tt->underBuilding());
        return !r || !r->underAgora();
    }
    case eWalkableObjectType::terrain: {
        if(!t->walkableElev() && t->isElevationTile()) return false;
        const auto terr = t->terrain() & eTerrain::walkable;
        return static_cast<bool>(terr);
    }
    case eWalkableObjectType::attacker: {
        const auto type = t->underBuildingType();
        if(type == eBuildingType::temple ||
           type == eBuildingType::templeAltar ||
           type == eBuildingType::templeStatue ||
           type == eBuildingType::templeMonument) return false;
        if(type == eBuildingType::road) return true;
        if(!t->walkableElev() && t->isElevationTile()) return false;
        const auto terr = t->terrain() & eTerrain::walkable;
        return static_cast<bool>(terr);
    }
    case eWalkableObjectType::waterAndDefault: {
        const auto type = t->underBuildingType();
        if(type == eBuildingType::road) return true;
        const auto terr = t->terrain();
        const auto terrW = terr & eTerrain::walkable;
        if(!static_cast<bool>(terrW) && terr != eTerrain::water) return false;
        if(!t->walkableElev() && t->isElevationTile()) return false;
        return eBuilding::sWalkableBuilding(type);
    }
    case eWalkableObjectType::fertile: {
        const bool r = t->walkable();
        if(!r) return false;
        return t->terrain() == eTerrain::fertile;
    }
    case eWalkableObjectType::wall: {
        const auto checker = [](eTileBase* const t) {
            if(!t) return false;
            const auto ubt = t->underBuildingType();
            return ubt == eBuildingType::wall ||
                   ubt == eBuildingType::tower;
        };
        if(!checker(t)) return false;
        const auto bl = t->tileRel(0, 1);
        if(!checker(bl)) return false;
        const auto br = t->tileRel(1, 0);
        if(!checker(br)) return false;
        const auto b = t->tileRel(1, 1);
        if(!checker(b)) return false;
        return true;
    }
    case eWalkableObjectType::deepWater: {
        return t->hasDeepWater();
    }
    case eWalkableObjectType::water: {
        return t->hasWater();
    }
    case eWalkableObjectType::ranch: {
        return ranchTileWalkable(t);
    }
    default:
        return true;
    }
}

stdsptr<WalkableObject> WalkableObject::sCreate(
        const eWalkableObjectType type) {
    switch(type) {
    case eWalkableObjectType::rect:
        return std::make_shared<eRectWalkableObject>();
    case eWalkableObjectType::hasResource:
        return std::make_shared<eHasResourceWalkableObject>();
    case eWalkableObjectType::ranchRect:
        return std::make_shared<RanchRectWalkableObject>();
    default:
        return std::make_shared<WalkableObject>(type);
    }
}

stdsptr<WalkableObject> WalkableObject::sCreateDefault() {
    return sCreate(eWalkableObjectType::ddefault);
}

stdsptr<WalkableObject> WalkableObject::sCreateArtisan() {
    return sCreate(eWalkableObjectType::artisan);
}

stdsptr<WalkableObject> WalkableObject::sCreateRoad() {
    return sCreate(eWalkableObjectType::road);
}

stdsptr<WalkableObject> WalkableObject::sCreateRoadAvenue() {
    return sCreate(eWalkableObjectType::roadAvenue);
}

stdsptr<WalkableObject> WalkableObject::sCreateRoadblock() {
    return sCreate(eWalkableObjectType::roadblock);
}

stdsptr<WalkableObject> WalkableObject::sCreateRoadblockNoAgora() {
    return sCreate(eWalkableObjectType::roadblockNoAgora);
}

stdsptr<WalkableObject> WalkableObject::sCreateTerrain() {
    return sCreate(eWalkableObjectType::terrain);
}

stdsptr<WalkableObject> WalkableObject::sCreateAttacker() {
    return sCreate(eWalkableObjectType::attacker);
}

stdsptr<WalkableObject> WalkableObject::sCreateFertile() {
    return sCreate(eWalkableObjectType::fertile);
}

stdsptr<WalkableObject> WalkableObject::sCreateWall() {
    return sCreate(eWalkableObjectType::wall);
}

stdsptr<WalkableObject> WalkableObject::sCreateDeepWater() {
    return sCreate(eWalkableObjectType::deepWater);
}

stdsptr<WalkableObject> WalkableObject::sCreateWater() {
    return sCreate(eWalkableObjectType::water);
}

stdsptr<WalkableObject> WalkableObject::sCreateWaterAndDefault() {
    return sCreate(eWalkableObjectType::waterAndDefault);
}

stdsptr<WalkableObject> WalkableObject::sCreateAll() {
    return sCreate(eWalkableObjectType::all);
}

stdsptr<WalkableObject> WalkableObject::sCreateRanch(
        const SDL_Rect& rect) {
    return std::make_shared<RanchRectWalkableObject>(rect);
}

stdsptr<WalkableObject> WalkableObject::sCreateRect(const SDL_Rect& rect) {
    return std::make_shared<eRectWalkableObject>(rect);
}

stdsptr<WalkableObject> WalkableObject::sCreateRect(eBuilding* const b) {
    return sCreateRect(b->tileRect());
}

stdsptr<WalkableObject> WalkableObject::sCreateRect(
        const SDL_Rect& rect,
        const stdsptr<WalkableObject>& other) {
    return std::make_shared<eRectWalkableObject>(other, rect);
}

stdsptr<WalkableObject> WalkableObject::sCreateRect(
        eBuilding* const b,
        const stdsptr<WalkableObject>& other) {
    return sCreateRect(b->tileRect(), other);
}

stdsptr<WalkableObject> WalkableObject::sCreateHasResource(
        const stdsptr<eHasResourceObject>& hr,
        const stdsptr<WalkableObject>& w) {
    return std::make_shared<eHasResourceWalkableObject>(hr, w);
}
