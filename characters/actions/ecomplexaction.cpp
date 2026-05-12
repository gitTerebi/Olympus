#include "ecomplexaction.h"

#include "ewaitaction.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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
