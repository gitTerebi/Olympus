#include "ecomplexaction.h"

#include "ewaitaction.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"
#include <algorithm>

void eComplexAction::increment(const int by) {
    int remaining = by;
    for(int i = 0; i < 16; i++) {
        if(!mCurrentAction) {
            decide();
            if(!mCurrentAction) return;
        }
        const auto state = mCurrentAction->state();
        if(state == eCharacterActionState::running) {
            int step = remaining;
            if(const auto w = dynamic_cast<eWaitAction*>(mCurrentAction.get())) {
                step = std::min(remaining, w->remainingTime());
            }
            mCurrentAction->increment(step);
            remaining -= step;
            if(!mCurrentAction ||
               mCurrentAction->state() == eCharacterActionState::running) return;
        }
        mCurrentAction = nullptr;
        if(this->state() != eCharacterActionState::running) return;
        if(remaining <= 0) return;
        decide();
    }
}

void eComplexAction::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eComplexAction::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eComplexAction*>(this)->serialize(ar);
}

void eComplexAction::serializeJson(eJsonArchive& ar) {
    eCharacterAction::serializeJson(ar);
    if(ar.writing()) {
        int hasCA = mCurrentAction ? 1 : 0;
        ar.field("hasCurrentAction", hasCA);
        if(mCurrentAction) {
            int type = static_cast<int>(mCurrentAction->type());
            ar.field("currentActionType", type);
            auto sub = ar.child("currentAction");
            mCurrentAction->serializeJson(sub);
        }
    } else {
        int hasCA = 0;
        ar.field("hasCurrentAction", hasCA);
        if(hasCA) {
            int type = 0;
            ar.field("currentActionType", type);
            mCurrentAction = eCharacterAction::sCreate(
                character(), static_cast<eCharActionType>(type));
            if(mCurrentAction) {
                auto sub = ar.child("currentAction");
                mCurrentAction->serializeJson(sub);
            }
        }
    }
}

void eComplexAction::serialize(eSaveArchive& ar) {
    eCharacterAction::serialize(ar);
    ar.characterAction<eCharacterAction>(mCurrentAction, [this](const eCharActionType type) {
        return eCharacterAction::sCreate(character(), type);
    });
}

void eComplexAction::setCurrentAction(const stdsptr<eCharacterAction>& a) {
    mCurrentAction = a;
}

void eComplexAction::wait(const int t) {
    const auto c = character();
    const auto w = e::make_shared<eWaitAction>(c);
    w->setTime(t);
    setCurrentAction(w);
}

void eComplexAction::wait() {
    const auto c = character();
    const auto w = e::make_shared<eWaitAction>(c);
    setCurrentAction(w);
}
