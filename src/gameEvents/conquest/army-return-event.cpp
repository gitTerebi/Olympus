#include "army-return-event.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"
#include "buildings/eheroshall.h"
#include "characters/actions/eheroaction.h"
#include "characters/gods/actions/god-worshipped-action.h"
#include "buildings/etriremewharf.h"

#include <algorithm>
#include <cstdio>

namespace {

eTriremeWharf* findReturningTriremeWharf(
        GameBoard& board,
        const eCityId cid,
        std::vector<eTriremeWharf*>& used) {
    const auto wharves = board.buildings(cid, eBuildingType::triremeWharf);
    for(const auto b : wharves) {
        const auto wharf = static_cast<eTriremeWharf*>(b);
        if(!wharf->abroad()) continue;
        if(std::find(used.begin(), used.end(), wharf) != used.end()) continue;
        used.push_back(wharf);
        return wharf;
    }
    return nullptr;
}

void returnTriremeBanner(GameBoard& board,
                         SoldierBanner& banner,
                         int& wait,
                         std::vector<eTriremeWharf*>& usedWharves) {
    const auto wharf = findReturningTriremeWharf(
        board, banner.cityId(), usedWharves);
    if(!wharf) {
        printf("[trireme-return] no abroad trireme wharf for city=%i banner=%i\n",
               static_cast<int>(banner.cityId()), banner.id());
        return;
    }

    banner.backFromAbroad(wait);
    wharf->triremeCameBack();
}

}

ArmyReturnEvent::ArmyReturnEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    ArmyEventBase(cid, eGameEventType::armyReturnEvent, branch, board) {}

void ArmyReturnEvent::initialize(
        const eEnlistedForces& forces,
        const stdsptr<WorldCity>& city) {
    mForces = forces;
    mCity = city;
}

void ArmyReturnEvent::trigger() {
    removeArmyEvent();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    const auto entryPoint = board->entryPoint(cid);

    int wait = 0;

    if(mForces.fAres) {
        const auto as = board->sanctuary(cid, GodType::ares);
        if(as) {
            as->godComeback();
            if(entryPoint) {
                const auto god = as->spawnGod();
                if(god) {
                    const auto ga = e::make_shared<GodWorshippedAction>(god);
                    god->setAction(ga);
                    god->changeTile(entryPoint);
                    ga->goBackToSanctuary();
                    wait += 150;
                }
            }
        }
    }

    for(const auto h : mForces.fHeroes) {
        const auto hh = board->heroHall(h.first, h.second);
        if(!hh) continue;
        if(!hh->heroOnQuest()) continue;
        hh->setHeroOnQuest(false);
        if(!entryPoint) continue;
        const auto hero = hh->spawnHero();
        const auto a = hero->action();
        const auto ha = dynamic_cast<eHeroAction*>(a);
        if(!ha) continue;
        hero->changeTile(entryPoint);
        ha->waitAndGoBackToHall(wait);
        wait += 150;
    }

    std::vector<eTriremeWharf*> returnedTriremeWharves;
    for(const auto& s : mForces.fSoldiers) {
        s->killAll();
        s->setBothCityIds(cid);
        if(s->type() == eBannerType::trireme) {
            returnTriremeBanner(*board, *s, wait, returnedTriremeWharves);
        } else {
            s->backFromAbroad(wait);
        }
    }

    for(const auto& a : mForces.fAllies) {
        a->setAbroad(false);
    }

    eEventData ed(cityId());
    ed.fTile = entryPoint;
    ed.fCity = mCity;
    board->event(eEvent::armyReturns, ed);
}

std::string ArmyReturnEvent::longName() const {
    return Language::text("army_returns_event_long_name");
}
