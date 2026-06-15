#ifndef WALKABLE_OBJECT_H
#define WALKABLE_OBJECT_H

#include "fileIO/streams.h"
#include "pointers/estdpointer.h"
#include "engine/eorientation.h"

class eTileBase;
class SaveArchive;

enum class eWalkableObjectType {
    ddefault,
    waterAndDefault,
    road,
    roadAvenue,
    roadblock,
    terrain,
    fertile,
    wall,
    rect,
    hasResource,
    deepWater,
    water,
    ranch,
    all,
    attacker,
    artisan,
    ranchRect,
    roadblockNoAgora
};

class eHasResourceObject;

class WalkableObject {
public:
    WalkableObject(const eWalkableObjectType t) :
        mType(t) {}
    virtual ~WalkableObject() = default;

    virtual bool walkable(eTileBase* const t) const;

    void serialize(SaveArchive& ar) { serializeFields(ar); }

    virtual eWalkableObjectType rootType() const {
        return mType;
    }

    eWalkableObjectType type() const { return mType; }

    static stdsptr<WalkableObject> sCreate(
            const eWalkableObjectType type);
    static stdsptr<WalkableObject> sCreateDefault();
    static stdsptr<WalkableObject> sCreateArtisan();
    static stdsptr<WalkableObject> sCreateRoad();
    static stdsptr<WalkableObject> sCreateRoadAvenue();
    static stdsptr<WalkableObject> sCreateRoadblock();
    static stdsptr<WalkableObject> sCreateRoadblockNoAgora();
    static stdsptr<WalkableObject> sCreateTerrain();
    static stdsptr<WalkableObject> sCreateAttacker();
    static stdsptr<WalkableObject> sCreateFertile();
    static stdsptr<WalkableObject> sCreateWall();
    static stdsptr<WalkableObject> sCreateDeepWater();
    static stdsptr<WalkableObject> sCreateWater();
    static stdsptr<WalkableObject> sCreateWaterAndDefault();
    static stdsptr<WalkableObject> sCreateAll();
    static stdsptr<WalkableObject> sCreateRanch(const SDL_Rect& rect);
    static stdsptr<WalkableObject> sCreateRect(const SDL_Rect& rect);
    static stdsptr<WalkableObject> sCreateRect(eBuilding* const b);
    static stdsptr<WalkableObject> sCreateRect(
            const SDL_Rect& rect,
            const stdsptr<WalkableObject>& other);
    static stdsptr<WalkableObject> sCreateRect(
            eBuilding* const b,
            const stdsptr<WalkableObject>& other);
    static stdsptr<WalkableObject> sCreateHasResource(
            const stdsptr<eHasResourceObject>& hr,
            const stdsptr<WalkableObject>& w);
protected:
    virtual void serializeFields(SaveArchive& ar) { (void)ar; }
private:
    const eWalkableObjectType mType;
};

#endif // WALKABLE_OBJECT_H
