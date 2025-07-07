#include "egodquestfulfilledevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"
#include "emessages.h"

#include "buildings/eheroshall.h"

eGodQuestFulfilledEvent::eGodQuestFulfilledEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGodQuestEventBase(cid, eGameEventType::godQuestFulfilled, branch, board) {}

void eGodQuestFulfilledEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed;
    ed.fHero = hero();
    ed.fQuestId = id();
    ed.fGod = god();
    board->event(eEvent::godQuestFulfilled, ed);
    const auto cid = cityId();
    const auto pid = board->cityIdToPlayerId(cid);
    board->allow(cid, eBuildingType::godMonument, static_cast<int>(god()));
    board->addFulfilledQuest(pid, godQuest());
    const auto hh = board->heroHall(cid, hero());
    if(hh) hh->setHeroOnQuest(false);
    const auto me = mainEvent<eGodQuestEvent>();
    board->removeGodQuest(me);
    me->fulfilled();
}

std::string eGodQuestFulfilledEvent::longName() const {
    return eLanguage::text("god_quest_fulfilled");
}
