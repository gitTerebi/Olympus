#include "espawner.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"

eSpawner::eSpawner(const eBannerTypeS type,
                   const int id, eTile* const tile,
                   const int maxCount,
                   const int spawnFreq,
                   eGameBoard& board) :
    eBanner(type, id, tile, board),
    mMaxCount(maxCount), mSpawnPeriod(spawnFreq) {
    board.registerSpawner(this);
}

eSpawner::~eSpawner() {
    board().unregisterSpawner(this);
}

void eSpawner::serialize(eSaveArchive& ar) {
    eBanner::serialize(ar);
    ar.field("count", mCount, 0);
    ar.field("time", mTime, 0);
    ar.field("spawningEnabled", mSpawningEnabled, true);
}

void eSpawner::incTime(const int by) {
    if(!mSpawningEnabled) return;
    mTime += by;
    if(mTime >= mSpawnPeriod && mCount < mMaxCount) {
        spawn(tile());
        mCount++;
        mTime -= mSpawnPeriod;
    }
}

void eSpawner::decCount() {
    mCount--;
}

void eSpawner::spawnMax() {
    const auto center = tile();
    const int cx = center->x();
    const int cy = center->y();
    const int dist = 5;
    const int iMax = mMaxCount - mCount;
    auto& board = this->board();
    for(int i = 0; i < iMax; i++) {
        const int tx = cx + (eRand::rand() % dist);
        const int ty = cy + (eRand::rand() % dist);
        const auto tile = board.tile(tx, ty);
        if(!tile) {
            i--;
            continue;
        }
        spawn(tile);
        mCount++;
    }
}

void eSpawner::setSpawnPeriod(const int p) {
    mSpawnPeriod = p;
}

void eSpawner::disableSpawning() {
    mSpawningEnabled = false;
}
