#include "ewolfspawner.h"

#include "characters/wolf.h"
#include "enumbers.h"
#include "characters/soldier-banner.h"

eWolfSpawner::eWolfSpawner(const int id,
                           eTile* const tile,
                           GameBoard& board) :
    eAnimalSpawner(eBannerTypeS::wolf, id, tile,
                   eNumbers::sWolfMaxCount,
                   eNumbers::sWolfSpawnPeriod, board) {}


stdsptr<eWildAnimal> eWolfSpawner::create(GameBoard& board) {
    const auto b = e::make_shared<Wolf>(board);
    b->setOnCityId(cityId());
    return b;
}

void eWolfSpawner::incTime(const int by) {
    eSpawner::incTime(by);
    if(count() >= maxCount()) disableSpawning();
}

void eWolfSpawner::spawnMax() {
    eSpawner::spawnMax();
    disableSpawning();
}
