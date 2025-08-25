#include "einvasionwarning.h"

#include "engine/egameboard.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "einvasionevent.h"

eInvasionWarning::eInvasionWarning(const int warningDays,
                                   eGameEvent &parent,
                                   const eCityId cid,
                                   eGameBoard &board,
                                   const eInvasionWarningType type) :
    eWarning(warningDays, parent, cid, board),
    mType(type) {}

void eInvasionWarning::trigger() {
    auto& board = eWarning::board();
    const auto cid = cityId();
    eEventData ed(cid);

    auto& p = parent();
    if(const auto i = dynamic_cast<eInvasionEvent*>(&p)) {
        const bool w = i->warned();
        if(!w) {
            const auto& date = board.date();
            i->setFirstWarning(date);
        }
        const auto c = i->city();
        if(!c) return;
        ed.fCity = c;
    }

    switch(mType) {
    case eInvasionWarningType::warningInitial: {
        ed.fTime = warningMonths();
        board.event(eEvent::invasionInitial, ed);
    } break;
    case eInvasionWarningType::warning24: {
        board.event(eEvent::invasion24, ed);
    } break;
    case eInvasionWarningType::warning12: {
        board.event(eEvent::invasion12, ed);
    } break;
    case eInvasionWarningType::warning6: {
        board.event(eEvent::invasion6, ed);
    } break;
    case eInvasionWarningType::warning1: {
        board.event(eEvent::invasion1, ed);
    } break;
    }
}
