#include "eentrypoint.h"

#include "engine/egameboard.h"
#include "characters/esettler.h"
#include "characters/actions/esettleraction.h"

eEntryPoint::eEntryPoint(const int id,
                         eTile* const tile,
                         eGameBoard& board) :
    eSpawner(eBannerTypeS::entryPoint, id, tile,
             __INT_MAX__, 500, board) {}

void eEntryPoint::incTime(const int by) {
    auto& board = eEntryPoint::board();
    const auto tile = this->tile();
    const auto cid = tile->cityId();
    const int pop = board.popularity(cid);
    setSpawnPeriod(500*(115 - pop)/15);
    eSpawner::incTime(by);
}

void eEntryPoint::spawn(eTile* const tile) {
    auto& board = eEntryPoint::board();
    const auto cid = tile->cityId();
    const auto& ivs = board.invasionHandlers(cid);
    if(!ivs.empty()) return;
    const auto limit = board.immigrationLimit(cid);
    if(limit != eImmigrationLimitedBy::none &&
       limit != eImmigrationLimitedBy::lackOfVacancies) {
        return;
    }
    const auto popData = board.populationData(cid);
    if(!popData) return;
    const int v = popData->vacancies();
    const int s = popData->settlers();
    if(s >= v) return;
    const auto b = e::make_shared<eSettler>(board);
    b->setBothCityIds(cid);
    b->setVisible(false);
    b->changeTile(tile);
    const auto a = e::make_shared<eSettlerAction>(b.get());
    a->setNumberPeople(std::min(8, v - s));
    b->setAction(a);
}

void eSS_spawnFinish::call() {}

eRiverEntryPoint::eRiverEntryPoint(const int id, eTile * const tile,
                                   eGameBoard &board) :
    eBanner(eBannerTypeS::riverEntryPoint, id, tile, board) {}
