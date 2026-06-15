#include "boar-spawner.h"

#include "characters/eboar.h"
#include "numbers.h"
#include "characters/soldier-banner.h"

BoarSpawner::BoarSpawner(const int id,
                           eTile* const tile,
                           GameBoard& board) :
    AnimalSpawner(BannerTypeS::boar, id, tile,
                   Numbers::sBoarMaxCount,
                   Numbers::sBoarSpawnPeriod, board) {}

stdsptr<eWildAnimal> BoarSpawner::create(GameBoard& board) {
    const auto b = e::make_shared<eBoar>(board);
    b->setOnCityId(cityId());
    return b;
}
