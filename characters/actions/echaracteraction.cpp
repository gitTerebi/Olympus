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
    board().unregisterCharacterAction(this);
    if(mState == eCharacterActionState::running) {
        if(mDeleteFailAction) mDeleteFailAction->call();
    }
}

eGameBoard& eCharacterAction::board() const {
    if(mCharacter) {
        const auto t = mCharacter->tile();
        if(t) return t->board();
        return mCharacter->getBoard();
    }
    return mBoard;
}

eGameBoard& eCharacterAction::board() {
    return const_cast<const eCharacterAction*>(this)->board();
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
    auto& brd = board();
    ar.charActFuncField("finishAction", brd, mFinishAction);
    ar.charActFuncField("failAction", brd, mFailAction);
    ar.charActFuncField("deleteFailAction", brd, mDeleteFailAction);
}

void eCharacterAction::serialize(eSaveArchive& ar) {
    serializeFields(ar);
    if(ar.reading()) {
        const stdptr<eCharacterAction> tptr(this);
        ar.addPostFunc([tptr]() {
            if(tptr) tptr->resumeFromSavedState();
        }, "resumeFromSavedState");
    }
}
