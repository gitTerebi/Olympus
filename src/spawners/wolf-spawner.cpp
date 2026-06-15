#include "wolf-spawner.h"

#include "characters/wolf.h"
#include "numbers.h"
#include "characters/soldier-banner.h"

WolfSpawner::WolfSpawner(const int id,
                           eTile* const tile,
                           GameBoard& board) :
    AnimalSpawner(BannerTypeS::wolf, id, tile,
                   Numbers::sWolfMaxCount,
                   Numbers::sWolfSpawnPeriod, board) {}


stdsptr<eWildAnimal> WolfSpawner::create(GameBoard& board) {
    const auto b = e::make_shared<Wolf>(board);
    b->setOnCityId(cityId());
    return b;
}

void WolfSpawner::incTime(const int by) {
    Spawner::incTime(by);
    if(count() >= maxCount()) disableSpawning();
}

void WolfSpawner::spawnMax() {
    Spawner::spawnMax();
    disableSpawning();
}
