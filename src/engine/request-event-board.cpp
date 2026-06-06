#include "game-board.h"

#include "e-board-player.h"
#include "gameEvents/requests/send-resources-to-city-event.h"
#include "gameEvents/requests/receive-tribute-event.h"
#include "evectorhelpers.h"

GameBoard::eRequests GameBoard::cityRequests(const ePlayerId pid) const
{
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->cityRequests();
}

void GameBoard::addCityRequest(SendResourcesToCityEvent* const q)
{
    if(!q) return;
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addCityRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void GameBoard::removeCityRequest(SendResourcesToCityEvent* const q)
{
    if(!q) return;
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->removeCityRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

GameBoard::eTributeRequests GameBoard::tributeRequests(const ePlayerId pid) const
{
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->tributeRequests();
}

void GameBoard::addTributeRequest(ReceiveTributeEvent* const q)
{
    if(!q) return;
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addTributeRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void GameBoard::removeTributeRequest(ReceiveTributeEvent* const q)
{
    if(!q) return;
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->removeTributeRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void GameBoard::advanceRequestEvents()
{
    const auto gameEvents = mAllGameEvents;
    for(const auto event : gameEvents) {
        const auto tribute = dynamic_cast<ReceiveTributeEvent*>(event);
        if(tribute && tribute->isMainEvent()) {
            tribute->advanceIfNeeded(mDate);
        }
        const auto request = dynamic_cast<SendResourcesToCityEvent*>(event);
        if(request && request->isMainEvent()) {
            request->advanceIfNeeded(mDate);
        }
    }
}

void GameBoard::processYearlyRequestEvents()
{
    const auto playerCities = personPlayerCitiesOnBoard();
    for(const auto playerCityId : playerCities) {
        const auto playerCity = world().cityWithId(playerCityId);
        if(!playerCity) continue;

        for(const auto& p : mConqueredBy) {
            const auto parentCity = world().cityWithId(p.first);
            if(!parentCity || !parentCity->isRival()) continue;
            if(!eVectorHelpers::contains(p.second, playerCity)) continue;

            const auto e = e::make_shared<ReceiveTributeEvent>(
                playerCityId, eGameEventBranch::root, *this);
            e->initialize(parentCity);
            e->initializeDate(date());
            addRootGameEvent(e);
        }
    }
}
