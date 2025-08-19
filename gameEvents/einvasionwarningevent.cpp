#include "einvasionwarningevent.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eworldcity.h"
#include "engine/egameboard.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "einvasionevent.h"

eInvasionWarningEvent::eInvasionWarningEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::invasionWarning, branch, board) {}

void eInvasionWarningEvent::initialize(
    const eInvasionWarningType type) {
    mType = type;
}

void eInvasionWarningEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    eEventData ed(cid);

    const auto p = parent();
    if(const auto i = dynamic_cast<eInvasionEvent*>(p)) {
        ed.fCity = i->city();
        const bool w = i->warned();
        if(!w) {
            const auto& date = board->date();
            i->setFirstWarning(date);
        }
    }

    switch(mType) {
    case eInvasionWarningType::warning36: {
        board->event(eEvent::invasion36, ed);
    } break;
    case eInvasionWarningType::warning24: {
        board->event(eEvent::invasion24, ed);
    } break;
    case eInvasionWarningType::warning12: {
        board->event(eEvent::invasion12, ed);
    } break;
    case eInvasionWarningType::warning6: {
        board->event(eEvent::invasion6, ed);
    } break;
    case eInvasionWarningType::warning1: {
        board->event(eEvent::invasion1, ed);
    } break;
    }
}

std::string eInvasionWarningEvent::longName() const {
    auto tmpl = eLanguage::text("invasion_by");
    const auto none = eLanguage::text("none");
    stdsptr<eWorldCity> city;

    const auto p = parent();
    if(const auto i = dynamic_cast<eInvasionEvent*>(p)) {
        city = i->city();
    }

    const auto cstr = city ? city->name() : none;
    eStringHelpers::replace(tmpl, "%1", cstr);
    return tmpl;
}

void eInvasionWarningEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst << mType;
}

void eInvasionWarningEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    src >> mType;
}
