#ifndef EPATROLSOURCEBUILDING_H
#define EPATROLSOURCEBUILDING_H

#include "epatrolbuilding.h"
#include "textures/ebuildingtextures.h"
#include "pointers/estdpointer.h"

class eCharacter;

struct eTargetData {
    int fSpawnTime;
    stdptr<eCharacter> fWalker;
};

class eSaveArchive;

class ePatrolSourceBuilding : public ePatrolBuilding {
public:
    using eBaseTex = std::shared_ptr<eTexture> eBuildingTextures::*;
    using eOverlays = eTextureCollection eBuildingTextures::*;
    using eTarget = std::pair<eCharacterType, eBuildingType>;
    using eTargets = std::vector<eTarget>;
    ePatrolSourceBuilding(GameBoard& board,
                          const eBaseTex baseTex,
                          const double overlayX,
                          const double overlayY,
                          const eOverlays overlays,
                          const eTargets& targets,
                          const eBuildingType type,
                          const int sw, const int sh,
                          const int maxEmployees,
                          const eCityId cid,
                          const eCharGenerator& charGen = nullptr);

    void timeChanged(const int by) override;

    const eTargets& targets() const { return mTargets; }

    // Augustus-style dispatch: target score = straight-line distance + load
    // penalty; pick the single lowest. shared by dispatch (spawn) and the
    // patrol target highlight so the preview shows where the next walker goes.
    // mirrors Augustus 2*days_left: a recently-served venue is penalized so
    // the next walker prefers an idle one.
    static int sLoadPenalty(const int showDays);
    // distance bias (tiles) added per remaining show-day at a target.
    static constexpr int sLoadBias = 2;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool targetWalkerInFlight(const int id) const;

    void spawn(const int id);
    void spawn(const int id, eBuilding* const targetBuilding);

    const eTargets mTargets;

    const int mSpawnInterval = eNumbers::sDestinationWalkerSpawnCooldown;
    std::vector<eTargetData> mTargetData;
};

#endif // EPATROLSOURCEBUILDING_H
