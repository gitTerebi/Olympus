#include "deer-spawner.h"

#include "characters/deer.h"
#include "numbers.h"
#include "characters/soldier-banner.h"

DeerSpawner::DeerSpawner(const int id,
                           eTile* const tile,
                           GameBoard& board) :
    AnimalSpawner(BannerTypeS::deer, id, tile,
                   Numbers::sDeerMaxCount,
                   Numbers::sDeerSpawnPeriod, board) {}


stdsptr<eWildAnimal> DeerSpawner::create(GameBoard& board) {
    const auto b = e::make_shared<Deer>(board);
    b->setOnCityId(cityId());
    return b;
}
