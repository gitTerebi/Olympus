#include "spawner.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

Spawner::Spawner(const BannerTypeS type,
                   const int id, eTile* const tile,
                   const int maxCount,
                   const int spawnFreq,
                   GameBoard& board) :
    Banner(type, id, tile, board),
    mMaxCount(maxCount), mSpawnPeriod(spawnFreq) {
    board.registerSpawner(this);
}

Spawner::~Spawner() {
    board().unregisterSpawner(this);
}

void Spawner::serialize(SaveArchive& ar) {
    Banner::serialize(ar);
    ar.field("count", mCount, 0);
    ar.field("time", mTime, 0);
    ar.field("spawningEnabled", mSpawningEnabled, true);
}

void Spawner::incTime(const int by) {
    if(!mSpawningEnabled) return;
    mTime += by;
    if(mTime >= mSpawnPeriod && mCount < mMaxCount) {
        spawn(tile());
        mCount++;
        mTime -= mSpawnPeriod;
    }
}

void Spawner::decCount() {
    mCount--;
}

void Spawner::spawnMax() {
    const auto center = tile();
    const int cx = center->x();
    const int cy = center->y();
    const int dist = 5;
    const int iMax = mMaxCount - mCount;
    auto& board = this->board();
    for(int i = 0; i < iMax; i++) {
        const int tx = cx + (Rand::rand() % dist);
        const int ty = cy + (Rand::rand() % dist);
        const auto tile = board.tile(tx, ty);
        if(!tile) {
            i--;
            continue;
        }
        spawn(tile);
        mCount++;
    }
}

void Spawner::setSpawnPeriod(const int p) {
    mSpawnPeriod = p;
}

void Spawner::disableSpawning() {
    mSpawningEnabled = false;
}
