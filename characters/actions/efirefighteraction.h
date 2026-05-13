#ifndef EFIREFIGHTERACTION_H
#define EFIREFIGHTERACTION_H

#include "epatrolaction.h"

#include "engine/etile.h"
#include "characters/echaracter.h"

class eSaveArchive;

class eFireFighterAction : public ePatrolAction {
    friend class eFFA_lookForFireFail;
public:
    eFireFighterAction(eCharacter* const c,
                      ePatrolBuildingBase* const b,
                      const std::vector<eOrientation>& path,
                      const stdsptr<eDirectionTimes>& dirTimes = {});
    eFireFighterAction(eCharacter* const c);

    bool decide() override;
    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override {
        ePatrolAction::serializeJson(ar);
        ar.field("mFireFighting", mFireFighting);
        ar.field("mFireCheck", mFireCheck);
        ar.field("mUsedWater", mUsedWater);
    }

private:
    void serialize(eSaveArchive& ar);
    bool lookForFire(const bool second);
    void putOutFire(eTile* const tile);

    bool mFireFighting{false};
    int mFireCheck{0};

    int mUsedWater = 0;
};

class eFFA_lookForFireFail : public eCharActFunc {
public:
    eFFA_lookForFireFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::FFA_lookForFireFail) {}
    eFFA_lookForFireFail(eGameBoard& board,
                         eFireFighterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::FFA_lookForFireFail),
        mPtr(ca) {}

    void call() override {
        if(!mPtr) return;
        const auto t = mPtr.get();
        if(!t->mFireFighting) return;
        t->mFireFighting = false;
        const auto c = t->character();
        c->setActionType(eCharacterActionType::walk);
        t->goBackDecision(eWalkableObject::sCreateDefault());
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mPtr = static_cast<eFireFighterAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mPtr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mPtr ? mPtr->ioID() : -1;
            ar.field("mPtr", ioid);
        } else {
            int ioid = -1;
            ar.field("mPtr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mPtr = static_cast<eFireFighterAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eFireFighterAction> mPtr;
};

class eFFA_putOutFireFinish : public eCharActFunc {
public:
    eFFA_putOutFireFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::FFA_putOutFireFinish) {}
    eFFA_putOutFireFinish(eGameBoard& board, eCharacter* const c,
                          eTile* const t) :
        eCharActFunc(board, eCharActFuncType::FFA_putOutFireFinish),
        mCptr(c), mTile(t) {}

    void call() override {
        if(!mCptr) return;
        mCptr->setActionType(eCharacterActionType::stand);
        if(const auto b = mTile->underBuilding()) {
            b->setOnFire(false);
        }
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = c;
        });
        mTile = src.readTile(board());
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCptr);
        dst.writeTile(mTile);
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
        ar.tile("mTile", mTile, board());
    }
private:
    stdptr<eCharacter> mCptr;
    eTile* mTile = nullptr;
};

#endif // EFIREFIGHTERACTION_H
