#ifndef EGODATTACKACTION_H
#define EGODATTACKACTION_H

#include "characters/gods/actions/egodaction.h"
#include "elanguage.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "etilehelper.h"

class eSaveArchive;

enum class eGodAttackStage {
    none, appear, goTo1, patrol1, goTo2, patrol2, disappear
};

class eGodAttackAction : public eGodAction {
    friend class eGodObsticleHandler;
public:
    eGodAttackAction(eCharacter* const c);
    ~eGodAttackAction();

    void increment(const int by) override;
    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    void setSanctuary(const stdptr<eSanctuary>& s);
private:
    void serialize(eSaveArchive& ar);
    void initialize();

    bool lookForAttack(const int dtime, int& time,
                       const int freq, const int range);
    bool lookForTargetedAttack(const int dtime, int& time,
                               const int freq, const int range);

    void goToTarget();
    stdsptr<eObsticleHandler> obsticleHandler();
    void destroyBuilding(eBuilding* const b);

    eGodAttackStage mStage{eGodAttackStage::none};

    int mLookForCurse = eRand::rand() % 2000;
    int mLookForTargetedCurse = eRand::rand() % 2000;
    int mLookForAttack = eRand::rand() % 2000;
    int mLookForTargetedAttack = eRand::rand() % 2000;
    int mLookForGod = eRand::rand() % 2000;
    int mLookForSpecial = eRand::rand() % 2000;

    stdptr<eSanctuary> mSanctuary;
};

class eGAA_loserDisappearFinish : public eCharActFunc {
public:
    eGAA_loserDisappearFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_loserDisappearFinish) {}
    eGAA_loserDisappearFinish(eGameBoard& board, eGodMonsterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GAA_loserDisappearFinish),
        mLoserPtr(ca) {}

    void call() override {
        if(!mLoserPtr) return;
        const auto t = mLoserPtr.get();
        const auto c = t->character();
        c->kill();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mLoserPtr = static_cast<eGodAction*>(ca);
        });;
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mLoserPtr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mLoserPtr ? mLoserPtr->ioID() : -1;
            ar.field("mLoserPtr", ioid);
        } else {
            int ioid = -1;
            ar.field("mLoserPtr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mLoserPtr = static_cast<eGodMonsterAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eGodMonsterAction> mLoserPtr;
};


class eGAA_destroyBuildingFinish : public eCharActFunc {
public:
    eGAA_destroyBuildingFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_destroyBuildingFinish) {}
    eGAA_destroyBuildingFinish(eGameBoard& board,
                               eGodAttackAction* const tptr,
                               eBuilding* const b) :
        eCharActFunc(board, eCharActFuncType::GAA_destroyBuildingFinish),
        mTptr(tptr), mBptr(b) {}

    void call() override {
        if(!mTptr) return;
        const auto b = mBptr;
        mTptr->resumeAction(); // can delete instance
        if(!b) return;
        b->collapse();
        eSounds::playCollapseSound();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodAttackAction*>(ca);
        });
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mBptr = b;
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        dst.writeBuilding(mBptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mTptr ? mTptr->ioID() : -1;
            ar.field("mTptr", ioid);
            eBuilding* braw = mBptr.get();
            ar.buildingRef("mBptr", braw, board());
        } else {
            int ioid = -1;
            ar.field("mTptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mTptr = static_cast<eGodAttackAction*>(resolveCharAction(ioid));
                });
            }
            ar.buildingRef("mBptr", [this](eBuilding* b) {
                mBptr = b;
            }, board());
        }
    }
private:
    stdptr<eGodAttackAction> mTptr;
    stdptr<eBuilding> mBptr;
};

class eTeleportFindFailFunc : public eFindFailFunc {
public:
    eTeleportFindFailFunc(eGameBoard& board) :
        eFindFailFunc(board, eFindFailFuncType::teleport) {}
    eTeleportFindFailFunc(eGameBoard& board, eGodAction* const ca) :
        eFindFailFunc(board, eFindFailFuncType::teleport),
        mTptr(ca) {}

    void call(eTile* const tile) {
        if(!mTptr) return;
        const auto c = mTptr->character();
        auto& board = c->getBoard();
        const auto r = eTileHelper::closestRoad(tile->x(), tile->y(), board);
        mTptr->teleport(r);
    }

    void read(eReadStream& src) {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eGodAction> mTptr;
};

class eGodObsticleHandler : public eObsticleHandler {
public:
    eGodObsticleHandler(eGameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::god) {}
    eGodObsticleHandler(eGameBoard& board,
                        eGodAttackAction* const t) :
        eObsticleHandler(board, eObsticleHandlerType::god),
        mTptr(t) {}

    bool handle(eTile* const tile) override {
        if(!mTptr) return false;
        const auto ub = tile->underBuilding();
        if(!ub) return false;
        const auto ubt = ub->type();
        const bool r = eBuilding::sWalkableBuilding(ubt);
        if(r) return false;
        mTptr->destroyBuilding(ub);
        return true;
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodAttackAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eGodAttackAction> mTptr;
};

#endif // EGODATTACKACTION_H
