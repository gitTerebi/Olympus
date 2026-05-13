#ifndef EKILLCHARACTERFINISHFAIL_H
#define EKILLCHARACTERFINISHFAIL_H

#include "echaracteractionfunction.h"

#include "characters/echaracter.h"
#include "fileIO/ejsonarchive.h"

class eKillCharacterFinishFail : public eCharActFunc {
public:
    eKillCharacterFinishFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::killCharacterFinishFail) {}
    eKillCharacterFinishFail(eGameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::killCharacterFinishFail),
        mCptr(c) {}

    void call() override;

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = static_cast<eCharacter*>(c);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mCptr ? mCptr->ioID() : -1;
            ar.field("mCptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mCptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mCptr = resolveChar(ioid);
                });
            }
        }
    }
private:
    stdptr<eCharacter> mCptr;
};

#endif // EKILLCHARACTERFINISHFAIL_H
