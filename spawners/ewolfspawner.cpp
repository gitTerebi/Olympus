#include "ewolfspawner.h"

#include "characters/ewolf.h"
#include "enumbers.h"

eWolfSpawner::eWolfSpawner(const int id,
                           eTile* const tile,
                           eGameBoard& board) :
    eAnimalSpawner(eBannerTypeS::wolf, id, tile,
                   eNumbers::sWolfMaxCount,
                   eNumbers::sWolfSpawnPeriod, board) {}


stdsptr<eWildAnimal> eWolfSpawner::create(eGameBoard& board) {
    const auto b = e::make_shared<eWolf>(board);
    b->setOnCityId(cityId());
    return b;
}
