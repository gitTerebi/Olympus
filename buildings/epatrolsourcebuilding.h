#ifndef EPATROLSOURCEBUILDING_H
#define EPATROLSOURCEBUILDING_H

#include "epatrolbuilding.h"
#include "textures/ebuildingtextures.h"
#include "pointers/estdpointer.h"

class eCharacter;

struct eTargetData {
    int fSpawnTime;
    int fLastId;
    stdptr<eCharacter> fWalker;
};

class eSaveArchive;

class ePatrolSourceBuilding : public ePatrolBuilding {
public:
    using eBaseTex = std::shared_ptr<eTexture> eBuildingTextures::*;
    using eOverlays = eTextureCollection eBuildingTextures::*;
    using eTarget = std::pair<eCharacterType, eBuildingType>;
    using eTargets = std::vector<eTarget>;
    ePatrolSourceBuilding(eGameBoard& board,
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
