#include "character-finish-actions.h"

#include "characters/echaracter.h"
#include "edieaction.h"
#include "fileIO/save-archive.h"

eChar_fightFinish::eChar_fightFinish(GameBoard& board) :
    eCharActFunc(board, eCharActFuncType::Char_fightFinish) {}

eChar_fightFinish::eChar_fightFinish(GameBoard& board, eCharacter* const t) :
    eCharActFunc(board, eCharActFuncType::Char_fightFinish),
    mTptr(t) {}

void eChar_fightFinish::call() {
    if(!mTptr) return;
    const auto t = mTptr.get();
    if(t->dead()) {
        const auto d = e::make_shared<eDieAction>(t);
        t->setAction(d);
    } else {
        t->resumeAction();
    }
}

void eChar_fightFinish::serializeFields(SaveArchive& ar) {
    ar.characterField("character", &board(), mTptr);
}

eChar_killWithCorpseFinish::eChar_killWithCorpseFinish(GameBoard& board) :
    eCharActFunc(board, eCharActFuncType::Char_killWithCorpseFinish),
    mWithCorpse(false) {}

eChar_killWithCorpseFinish::eChar_killWithCorpseFinish(
        GameBoard& board, eCharacter* const t, const bool withCorpse) :
    eCharActFunc(board, eCharActFuncType::Char_killWithCorpseFinish),
    mWithCorpse(withCorpse), mTptr(t) {}

void eChar_killWithCorpseFinish::call() {
    if(!mTptr) return;
    const auto t = mTptr.get();
    if(mWithCorpse && !t->dead()) t->killWithCorpse();
    else t->kill();
}

void eChar_killWithCorpseFinish::serializeFields(SaveArchive& ar) {
    ar.field("withCorpse", mWithCorpse);
    ar.characterField("character", &board(), mTptr);
}
