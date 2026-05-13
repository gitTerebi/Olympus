#ifndef EREPLACECATTLEACTION_H
#define EREPLACECATTLEACTION_H

#include "eactionwithcomeback.h"

#include "characters/echaracter.h"
#include "characters/actions/eanimalaction.h"
#include "buildings/eanimalpen.h"

class eSaveArchive;

class eReplaceCattleAction : public eActionWithComeback {
public:
    eReplaceCattleAction(eCharacter* const c,
                         eCharacter* const cc = nullptr);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void serialize(eSaveArchive& ar);

    void goCattle();

    stdptr<eCharacter> mCattle;
};

class eRC_finishAction : public eCharActFunc {
public:
    eRC_finishAction(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::RC_finishAction) {}
    eRC_finishAction(eGameBoard& board,
                     eReplaceCattleAction* const a,
                     eCharacter* const c,
                     eCharacter* const cc) :
        eCharActFunc(board, eCharActFuncType::RC_finishAction),
        mButcherA(a), mButcher(c), mCattle(cc) {}

    void call() override;

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const a) {
            mButcherA = static_cast<eReplaceCattleAction*>(a);
        });
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mButcher = c;
        });
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCattle = c;
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mButcherA);
        dst.writeCharacter(mButcher);
        dst.writeCharacter(mCattle);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mButcherA ? mButcherA->ioID() : -1;
            ar.field("mButcherA", ioid);
            int ioid2 = mButcher ? mButcher->ioID() : -1;
            ar.field("mButcher", ioid2);
            int ioid3 = mCattle ? mCattle->ioID() : -1;
            ar.field("mCattle", ioid3);
        } else {
            int ioid = -1;
            ar.field("mButcherA", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mButcherA = static_cast<eReplaceCattleAction*>(resolveCharAction(ioid));
                });
            }
            int ioid2 = -1;
            ar.field("mButcher", ioid2);
            if(ioid2 >= 0) {
                ar.addPostFunc([this, ioid2]() {
                    mButcher = resolveChar(ioid2);
                });
            }
            int ioid3 = -1;
            ar.field("mCattle", ioid3);
            if(ioid3 >= 0) {
                ar.addPostFunc([this, ioid3]() {
                    mCattle = resolveChar(ioid3);
                });
            }
        }
    }
private:
    stdptr<eReplaceCattleAction> mButcherA;
    stdptr<eCharacter> mButcher;
    stdptr<eCharacter> mCattle;
};

class eRC_finishWalkingAction : public eCharActFunc {
public:
    eRC_finishWalkingAction(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::RC_finishWalkingAction) {}
    eRC_finishWalkingAction(eGameBoard& board,
                            eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::RC_finishWalkingAction),
        mCattle(c) {}

    void call() override;

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCattle = c;
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCattle);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mCattle ? mCattle->ioID() : -1;
            ar.field("mCattle", ioid);
        } else {
            int ioid = -1;
            ar.field("mCattle", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mCattle = resolveChar(ioid);
                });
            }
        }
    }
private:
    stdptr<eCharacter> mCattle;
};

#endif // EREPLACECATTLEACTION_H
