#ifndef EPATROLACTION_H
#define EPATROLACTION_H

#include "eactionwithcomeback.h"
#include "epatrolmoveaction.h"
#include "buildings/ebuilding.h"

class ePatrolBuildingBase;
class eSaveArchive;

class ePatrolAction : public eActionWithComeback {
    friend class ePA_patrolFail;
    friend class ePA_patrolFinish;
public:
    ePatrolAction(eCharacter* const c,
                  ePatrolBuildingBase* const b,
                  const std::vector<eOrientation> &path,
                  const stdsptr<eDirectionTimes>& dirTimes = {},
                  const eCharActionType at = eCharActionType::patrolAction);
    ePatrolAction(eCharacter* const c,
                  const eCharActionType at = eCharActionType::patrolAction);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
protected:
    void patrol();
    void goBackDecision(const stdsptr<eWalkableObject>& w =
                            eWalkableObject::sCreateRoadAvenue());
private:
    void serialize(eSaveArchive& ar);

    std::vector<eOrientation> mPath;
    stdsptr<eDirectionTimes> mDirTimes;
    stdptr<ePatrolBuildingBase> mBuilding;

    bool mDone = false;
};

class ePA_patrolFail : public eCharActFunc {
public:
    ePA_patrolFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::PA_patrolFail) {}
    ePA_patrolFail(eGameBoard& board, ePatrolAction* const t) :
        eCharActFunc(board, eCharActFuncType::PA_patrolFail),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->mDone = true;
        t->setState(eCharacterActionState::failed);
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<ePatrolAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mTptr ? mTptr->ioID() : -1;
            ar.field("mTptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mTptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mTptr = static_cast<ePatrolAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<ePatrolAction> mTptr;
};

class ePA_patrolFinish : public eCharActFunc {
public:
    ePA_patrolFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::PA_patrolFinish) {}
    ePA_patrolFinish(eGameBoard& board, ePatrolAction* const t) :
        eCharActFunc(board, eCharActFuncType::PA_patrolFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->mDone = true;
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<ePatrolAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mTptr ? mTptr->ioID() : -1;
            ar.field("mTptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mTptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mTptr = static_cast<ePatrolAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<ePatrolAction> mTptr;
};

#endif // EPATROLACTION_H
