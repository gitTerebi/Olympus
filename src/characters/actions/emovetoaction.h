#ifndef EMOVETOACTION_H
#define EMOVETOACTION_H

#include "ecomplexaction.h"

#include "engine/etile.h"
#include "engine/thread/ethreadtile.h"
#include "ewalkablehelpers.h"
#include "engine/estaterelevance.h"
#include "walkable/eobsticlehandler.h"

class eThreadBoard;
class eSaveArchive;
class eBuilding;

using eAction = std::function<void()>;
using eTileGetter = std::function<eThreadTile*(eThreadBoard&)>;

class eMoveToAction : public eComplexAction {
public:
    eMoveToAction(eCharacter* const c);

    bool decide() override { return false; }

    using eTileFinal = std::function<bool(eThreadTile* const)>;
    void start(const eTileFinal& final,
               stdsptr<WalkableObject> pathFindWalkable =
                   WalkableObject::sCreateDefault(),
               stdsptr<WalkableObject> moveWalkable = nullptr,
               const eTileGetter& endTile = nullptr);
    void start(eTile* const final,
               const stdsptr<WalkableObject>& pathFindWalkable =
                    WalkableObject::sCreateDefault(),
               const stdsptr<WalkableObject>& moveWalkable = nullptr,
               const eTileGetter& endTile = nullptr);
    void start(const SDL_Rect& rect,
               stdsptr<WalkableObject> pathFindWalkable =
                    WalkableObject::sCreateDefault(),
               stdsptr<WalkableObject> moveWalkable = nullptr,
               const eTileGetter& endTile = nullptr);
    void start(eBuilding* const final,
               const stdsptr<WalkableObject>& pathFindWalkable =
                    WalkableObject::sCreateDefault(),
               const stdsptr<WalkableObject>& moveWalkable = nullptr,
               const eTileGetter& endTile = nullptr);
    void start(const eBuildingType final,
               const stdsptr<WalkableObject>& pathFindWalkable =
                    WalkableObject::sCreateDefault(),
               const stdsptr<WalkableObject>& moveWalkable = nullptr,
               const eTileGetter& endTile = nullptr);

    void setRemoveLastTurn(const bool r)
    { mRemoveLastTurn = r; }

    void setFoundAction(const eAction& a)
    { mFoundAction = a; }

    void setFindFailAction(const eAction& a)
    { mFindFailAction = a; }

    void setWait(const bool w)
    { mWait = w; }

    void setDiagonalOnly(const bool d)
    { mDiagonalOnly = d; }

    void setMaxFindDistance(const int d)
    { mMaxFindDistance = d; }

    void setMaxWalkDistance(const int d)
    { mMaxWalkDistance = d; };

    void setObsticleHandler(const stdsptr<eObsticleHandler>& oh);

    int pathLength() const
    { return mPathLength; }

    using eTileDistance = std::function<int(eTileBase* const)>;
    void setTileDistance(const eTileDistance& dist);

    void setStateRelevance(const eStateRelevance rel) { mRelevance = rel; }
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    enum class eSavedGoal {
        none, custom, tile, rect, building, buildingType
    };
    void startInternal(const eTileFinal& final,
                       stdsptr<WalkableObject> pathFindWalkable,
                       stdsptr<WalkableObject> moveWalkable,
                       const eTileGetter& endTile);
    bool restartSavedGoal();

    eStateRelevance mRelevance = eStateRelevance::all;
    eAction mFoundAction;
    eAction mFindFailAction;
    bool mRemoveLastTurn = false;
    bool mWait = true;
    bool mDiagonalOnly = false;
    int mMaxFindDistance = 10000;
    int mMaxWalkDistance = 10000;
    int mPathLength = 0;
    stdsptr<eObsticleHandler> mObstHandler;
    eTileDistance mDistance;
    eSavedGoal mSavedGoal = eSavedGoal::none;
    eTile* mSavedTile = nullptr;
    SDL_Rect mSavedRect{};
    stdptr<eBuilding> mSavedBuilding;
    eBuildingType mSavedBuildingType{};
    stdsptr<WalkableObject> mSavedPathFindWalkable;
    stdsptr<WalkableObject> mSavedMoveWalkable;
};

#endif // EMOVETOACTION_H
