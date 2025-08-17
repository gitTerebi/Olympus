#include "ecitybecomesevent.h"


#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "estringhelpers.h"
#include "elanguage.h"

eCityBecomesEvent::eCityBecomesEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::cityBecomes, branch, board) {}

void eCityBecomesEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
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
    }
}

std::string eCityBecomesEvent::longName() const {
    auto text = eLanguage::text("city_becomes_long_name");
    std::string key;
    switch(mType) {
    case eCityBecomesType::ally: {
        key = "ally";
    } break;
    case eCityBecomesType::rival: {
        key = "rival";
    } break;
    case eCityBecomesType::vassal: {
        key = "vassal";
    } break;
    case eCityBecomesType::active: {
        key = "active";
    } break;
    case eCityBecomesType::inactive: {
        key = "inactive";
    } break;
    case eCityBecomesType::visible: {
        key = "visible";
    } break;
    case eCityBecomesType::invisible: {
        key = "invisible";
    } break;
    }

    const auto cname = mCity ? mCity->name() : eLanguage::text("city");
    eStringHelpers::replace(text, "%1", cname);
    eStringHelpers::replace(text, "%2", eLanguage::text(key));
    return text;
}

void eCityBecomesEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst << mType;
    dst.writeCity(mCity.get());
}

void eCityBecomesEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    src >> mType;
    src.readCity(worldBoard(), [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });
}
