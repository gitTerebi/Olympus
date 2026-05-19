#include "epatroltarget.h"
#include "fileIO/esavearchive.h"

#include "characters/actions/epatrolaction.h"
#include "textures/egametextures.h"
#include "characters/actions/emovepathaction.h"
#include "engine/epathfinder.h"
#include "epathfindtask.h"
#include "engine/ethreadpool.h"
#include "characters/actions/ewaitaction.h"
#include "characters/echaracter.h"
#include "engine/e-game-board.h"

ePatrolTarget::ePatrolTarget(eGameBoard& board,
                             const eBaseTex baseTex,
                             const double overlayX,
                             const double overlayY,
                             const eOverlays overlays,
                             const eCharGenerator& charGen,
                             const eBuildingType type,
                             const int sw, const int sh,
                             const int maxEmployees,
                             const eCityId cid) :
    ePatrolBuilding(board, baseTex,
                    overlayX, overlayY,
                    overlays, charGen,
                    type, sw, sh,
                    maxEmployees, cid),
    mCharGen(charGen) {
    setSpawnPatrolers(false);
    setOverlayEnabledFunc([this]() {
        return enabled() && isActive();
    });
}

void ePatrolTarget::arrived() {
    const bool startPatrolCooldown = mSpawnPool == 0 && patroler() == nullptr;
    mSpawnPool++;
    mActiveTimer = eNumbers::sCultureActiveTime;
    if(startPatrolCooldown) resetSpawnTimer();
    setSpawnPatrolers(true);
}

int ePatrolTarget::spawnCooldown() const {
    return eNumbers::sCulturePatrolSpawnCooldown;
}

void ePatrolTarget::timeChanged(const int by) {
    const bool hadBefore = mHadPatroler;
    ePatrolBuilding::timeChanged(by);
    const bool hasNow = patroler() != nullptr;
    if(hasNow && !hadBefore) {
        if(mSpawnPool > 0) mSpawnPool--;
        if(mSpawnPool <= 0) setSpawnPatrolers(false);
    }
    mHadPatroler = hasNow;
    if(mActiveTimer > 0) {
        mActiveTimer -= by;
        if(mActiveTimer < 0) mActiveTimer = 0;
    }
}

void ePatrolTarget::serializeFields(eSaveArchive& ar) {
    ePatrolBuildingBase::serializeFields(ar);
    ar.field("spawnPool", mSpawnPool);
    ar.field("hadPatroler", mHadPatroler);
    ar.field("activeTimer", mActiveTimer);
}
