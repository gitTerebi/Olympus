#include "ecomplexaction.h"

#include "ewaitaction.h"
#include "fileIO/esavearchive.h"

void eComplexAction::increment(const int by) {
    if(mCurrentAction) {
        const auto state = mCurrentAction->state();
        if(state == eCharacterActionState::running) {
            mCurrentAction->increment(by);
            return;
        } else {
            mCurrentAction = nullptr;
            decide();
        }
    } else {
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
