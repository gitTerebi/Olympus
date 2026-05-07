#include "echaracteraction.h"

#include "engine/egameboard.h"
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

void eCharacterAction::serialize(eSaveArchive& ar) {
    ar.field("mIOID", mIOID);

    ar.field("mState", mState);

    if(ar.reading()) {
        mFinishAction = ar.readStream().readCharActFunc(board());
        mFailAction = ar.readStream().readCharActFunc(board());
        mDeleteFailAction = ar.readStream().readCharActFunc(board());
    } else {
        ar.writeStream().writeCharActFunc(mFinishAction.get());
        ar.writeStream().writeCharActFunc(mFailAction.get());
        ar.writeStream().writeCharActFunc(mDeleteFailAction.get());
    }
}

void eCharacterAction::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eCharacterAction::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eCharacterAction*>(this)->serialize(ar);
}
