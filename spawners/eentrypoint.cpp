#include "eentrypoint.h"

#include "engine/e-game-board.h"
#include "characters/esettler.h"
#include "characters/actions/esettleraction.h"

eEntryPoint::eEntryPoint(const int id,
                         eTile* const tile,
                         GameBoard& board) :
    eSpawner(eBannerTypeS::entryPoint, id, tile,
             __INT_MAX__, 500, board) {}

// Augustus-style sentiment curve: popularity (avg house satisfaction)
// throttles immigration, freezing below 50. Unemployment already drags
// popularity down via house work satisfaction, so this breaks the
// jobless immigration churn cycle.
int eEntryPoint::sImmigrationFreezePopularity = 50;

void eEntryPoint::incTime(const int by) {
    auto& board = eEntryPoint::board();
    const auto tile = this->tile();
    const auto cid = tile->cityId();
    const int pop = board.popularity(cid);
    const int base = 500*(115 - pop)/15;
    // Augustus update_status step curve mapped onto base spawn rate.
    int period;
    if(pop > 70) {
        period = base;             // 100% rate
    } else if(pop > 60) {
        period = base*4/3;         // 75% rate
    } else if(pop >= sImmigrationFreezePopularity) {
        period = base*2;           // 50% rate
    } else {
        period = base*2;           // frozen (spawn() returns early)
    }
    setSpawnPeriod(period);
    eSpawner::incTime(by);
}

void eEntryPoint::spawn(eTile* const tile) {
    auto& board = eEntryPoint::board();
    const auto cid = tile->cityId();
    const auto& ivs = board.invasionHandlers(cid);
    if(!ivs.empty()) return;
    // Freeze immigration when sentiment (popularity) is low.
    if(board.popularity(cid) < sImmigrationFreezePopularity) return;
    const auto limit = board.immigrationLimit(cid);
    if(limit != eImmigrationLimitedBy::none &&
       limit != eImmigrationLimitedBy::lackOfVacancies) {
        return;
    }
    const auto popData = board.populationData(cid);
    if(!popData) return;
    board.validateSettlers(cid);
    const int v = popData->vacancies();
    const int s = popData->settlers();
    if(s >= v) return;
    const auto b = e::make_shared<eSettler>(board);
    b->setBothCityIds(cid);
    b->setVisible(false);
    b->changeTile(tile);
    const auto a = e::make_shared<eSettlerAction>(b.get());
    a->setNumberPeople(std::min(4, v - s));
    b->setAction(a);
}

void eSS_spawnFinish::call() {}

eRiverEntryPoint::eRiverEntryPoint(const int id, eTile * const tile,
                                   GameBoard &board) :
    eBanner(eBannerTypeS::riverEntryPoint, id, tile, board) {}
