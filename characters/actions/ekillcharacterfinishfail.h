#ifndef EKILLCHARACTERFINISHFAIL_H
#define EKILLCHARACTERFINISHFAIL_H

#include "echaracteractionfunction.h"

#include "characters/echaracter.h"

class eKillCharacterFinishFail : public eCharActFunc {
public:
    eKillCharacterFinishFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::killCharacterFinishFail) {}
    eKillCharacterFinishFail(eGameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::killCharacterFinishFail),
        mCptr(c) {}

    void call() override;
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
    }
private:
    stdptr<eCharacter> mCptr;
};

#endif // EKILLCHARACTERFINISHFAIL_H
