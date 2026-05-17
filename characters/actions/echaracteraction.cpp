#include "echaracteraction.h"

#include "engine/e-game-board.h"
#include "characters/echaracter.h"
#include "fileIO/esavearchive.h"

eCharacterAction::eCharacterAction(
        eCharacter* const c, const eCharActionType type) :
    mBoard(c->getBoard()), mCharacter(c), mType(type) {
    mBoard.registerCharacterAction(this);
}

eCharacterAction::~eCharacterAction() {
    mBoard.unregisterCharacterAction(this);
    if(mState == eCharacterActionState::running) {
        if(mDeleteFailAction) mDeleteFailAction->call();
    }
}

eCityId eCharacterAction::cityId() const {
    const auto c = character();
    if(!c) return eCityId::neutralFriendly;
    return c->cityId();
}

eCityId eCharacterAction::onCityId() const {
    const auto c = character();
    if(!c) return eCityId::neutralFriendly;
    return c->onCityId();
}

void eCharacterAction::setState(const eCharacterActionState state) {
    if(state == mState) return;
    mState = state;
    if(mState == eCharacterActionState::failed) {
        if(mFailAction) mFailAction->call();
    } else if(mState == eCharacterActionState::finished) {
        if(mFinishAction) mFinishAction->call();
    }
}

void eCharacterAction::setFinishAction(const stdsptr<eCharActFunc>& f) {
    mFinishAction = f;
}

void eCharacterAction::setFailAction(const stdsptr<eCharActFunc>& f) {
    mFailAction = f;
}

void eCharacterAction::setDeleteFailAction(const stdsptr<eCharActFunc>& d) {
    mDeleteFailAction = d;
}

void eCharacterAction::serializeFields(eSaveArchive& ar) {
    ar.field("ioID", mIOID);
    ar.field("state", mState);
}

void eCharacterAction::read(eReadStream& src) {
    eSaveArchive ar(src);
    serializeFields(ar);
    const stdptr<eCharacterAction> tptr(this);
    ar.addPostFunc([tptr]() {
        if(tptr) tptr->resumeFromSavedState();
    }, "resumeFromSavedState");
}

void eCharacterAction::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eCharacterAction*>(this)->serializeFields(ar);
}
