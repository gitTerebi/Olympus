#include "edeerspawner.h"

#include "characters/deer.h"
#include "enumbers.h"
#include "characters/soldier-banner.h"

eDeerSpawner::eDeerSpawner(const int id,
                           eTile* const tile,
                           GameBoard& board) :
    eAnimalSpawner(eBannerTypeS::deer, id, tile,
                   eNumbers::sDeerMaxCount,
                   eNumbers::sDeerSpawnPeriod, board) {}


stdsptr<eWildAnimal> eDeerSpawner::create(GameBoard& board) {
    const auto b = e::make_shared<Deer>(board);
    b->setOnCityId(cityId());
    return b;
}
