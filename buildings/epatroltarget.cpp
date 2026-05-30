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
#include "engine/game-board.h"

ePatrolTarget::ePatrolTarget(GameBoard& board,
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
    const bool wasIdle = !isActive();
    mShowDays = eNumbers::sCultureShowDays;
    mDayAccum = 0;
    if(wasIdle) resetSpawnTimer();
    setSpawnPatrolers(true);
}

int ePatrolTarget::spawnCooldown() const {
    return eNumbers::sCulturePatrolSpawnCooldown;
}

void ePatrolTarget::timeChanged(const int by) {
    ePatrolBuilding::timeChanged(by);
    if(mShowDays > 0) {
        mDayAccum += by;
        const int dayLen = eNumbers::sDayLength;
        if(dayLen > 0) {
            const int days = mDayAccum / dayLen;
            if(days > 0) {
                mDayAccum -= days * dayLen;
                mShowDays -= days;
                if(mShowDays < 0) mShowDays = 0;
            }
        }
        if(mShowDays <= 0) setSpawnPatrolers(false); // timer expired, stop
    }
}

void ePatrolTarget::serializeFields(eSaveArchive& ar) {
    ePatrolBuildingBase::serializeFields(ar);
    ar.field("showDays", mShowDays);
    // mDayAccum is a transient sub-day remainder; not serialized.
}
