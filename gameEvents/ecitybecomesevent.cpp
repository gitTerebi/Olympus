#include "ecitybecomesevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eCityBecomesEvent::eCityBecomesEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::cityBecomes,
               branch, board),
    eCityEventValue(board) {}

void eCityBecomesEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    chooseCity();
    auto& world = board->world();
    const auto cid = cityId();
    const auto tid = board->cityIdToTeamId(cid);
    eEventData ed(cid);
    ed.fCity = mCity;
    const auto cpid = mCity->playerId();
    switch(mType) {
    case eCityBecomesType::ally: {
        mCity->setRelationship(eForeignCityRelationship::ally);
        world.setPlayerTeam(cpid, tid);
        board->allow(cityId(), eBuildingType::commemorative, 7);
        board->event(eEvent::cityBecomesAlly, ed);
    } break;
    case eCityBecomesType::rival: {
        mCity->setRelationship(eForeignCityRelationship::rival);
        const auto itid = static_cast<int>(tid);
        const auto newTid = static_cast<eTeamId>(itid + 1);
        world.setPlayerTeam(cpid, newTid);
        board->event(eEvent::cityBecomesRival, ed);
    } break;
    case eCityBecomesType::vassal: {
        mCity->setRelationship(eForeignCityRelationship::vassal);
        world.setPlayerTeam(cpid, tid);
        board->event(eEvent::cityBecomesVassal, ed);
    } break;
    case eCityBecomesType::active: {
        mCity->setState(eCityState::active);
        board->event(eEvent::cityBecomesActive, ed);
    } break;
    case eCityBecomesType::inactive: {
        mCity->setState(eCityState::inactive);
        board->event(eEvent::cityBecomesInactive, ed);
    } break;
    case eCityBecomesType::visible: {
        mCity->setVisible(true);
        board->event(eEvent::cityBecomesVisible, ed);
    } break;
    case eCityBecomesType::invisible: {
        mCity->setVisible(false);
        board->event(eEvent::cityBecomesInvisible, ed);
    } break;
    case eCityBecomesType::conquered: {
        ed.fRivalCity = mAttackingCity;
        const auto pid = playerId();
        const auto tid = board->playerIdToTeamId(pid);
        const auto enemyTid = tid == eTeamId::team0 ? eTeamId::team1 :
                                                      eTeamId::team0;
        if(mAttackingCity->playerId() == pid) {
            if(mCity->isColony()) {
                mCity->setConqueredBy(nullptr);
                board->event(eEvent::colonyRestored, ed);
            } else {
                board->event(eEvent::cityConquered, ed);
                const auto cid = cityId();
                board->allow(cid, eBuildingType::commemorative, 4);
                mCity->setRelationship(eForeignCityRelationship::vassal);
                const auto pid = mCity->playerId();
                board->setPlayerTeam(pid, tid);
            }
        } else if(mAttackingCity->isAlly()) {
            if(mCity->isRival()) {
                board->event(eEvent::rivalConqueredByAlly, ed);
                mCity->setRelationship(eForeignCityRelationship::ally);
                board->setPlayerTeam(cpid, tid);
            }
        } else if(mAttackingCity->isVassal()) {
            if(mCity->isRival()) {
                board->event(eEvent::rivalConqueredByVassal, ed);
                mCity->setRelationship(eForeignCityRelationship::vassal);
                board->setPlayerTeam(cpid, tid);
            }
        } else if(mAttackingCity->isRival()) {
            if(mCity->isAlly()) {
                board->event(eEvent::allyConqueredByRival, ed);
                mCity->setRelationship(eForeignCityRelationship::rival);
                board->setPlayerTeam(cpid, enemyTid);
            } else if(mCity->isVassal()) {
                board->event(eEvent::vassalConqueredByRival, ed);
                mCity->setRelationship(eForeignCityRelationship::rival);
                board->setPlayerTeam(cpid, enemyTid);
            } else if(mCity->isColony()) {
                board->event(eEvent::colonyConqueredByRival, ed);
                mCity->setConqueredBy(mAttackingCity);
            } else if(mCity->isParentCity()) {
                board->event(eEvent::parentConqueredByRival, ed);
                board->setEpisodeLost();
            }
            mCity->incAttitude(-50, pid);
        }
    } break;
    }
}

std::string eCityBecomesEvent::longName() const {
    switch(mType) {
    case eCityBecomesType::ally:
        return eLanguage::zeusText(290, 36);
    case eCityBecomesType::rival:
        return eLanguage::zeusText(290, 37);
    case eCityBecomesType::vassal:
        return eLanguage::zeusText(290, 38);
    case eCityBecomesType::active:
        return eLanguage::zeusText(290, 45);
    case eCityBecomesType::inactive:
        return eLanguage::zeusText(290, 46);
    case eCityBecomesType::visible:
        return eLanguage::zeusText(290, 47);
    case eCityBecomesType::invisible:
        return eLanguage::zeusText(290, 48);
    case eCityBecomesType::conquered:
        return eLanguage::zeusText(290, 50);
    }
    return "";
}

void eCityBecomesEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCityEventValue::write(dst);
    eAttackingCityEventValue::write(dst);
    dst << mType;
}

void eCityBecomesEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eCityEventValue::read(src, *gameBoard());
    eAttackingCityEventValue::read(src, *gameBoard());
    src >> mType;
}
