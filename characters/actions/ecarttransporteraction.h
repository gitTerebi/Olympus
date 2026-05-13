#ifndef ECARTTRANSPORTERACTION_H
#define ECARTTRANSPORTERACTION_H

#include "eactionwithcomeback.h"

#include "characters/ecarttransporter.h"
#include "buildings/ebuildingwithresource.h"
#include "fileIO/esavearchive.h"

class eSaveArchive;

class eCartTransporterAction : public eActionWithComeback {
    friend class eCTA_findTargetFinish;
    friend class eCTA_waitOutsideFinish;
public:
    eCartTransporterAction(eCharacter* const c,
                           eBuildingWithResource* const b);
    eCartTransporterAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    eCartActionTypeSupport support() const;
    eResourceType supportsResource() const;

    eBuilding* src() const { return mBuilding; }
    eBuilding* target() const { return mTarget; }
    bool noDestination() const { return mWaitOutside; }
    void setBuilding(eBuildingWithResource* b) { mBuilding = b; }

    bool waiting();
protected:
    void findTarget();
    void findTarget(const eCartTask& task);
    void findTarget(const std::vector<eCartTask>& tasks);
    void goBack();

    void targetResourceAction(const int bx, const int by);
    void targetResourceAction(eBuildingWithResource* const rb);
    int targetProcessTask(eBuildingWithResource* const rb,
                          const eCartTask& task);

    void startResourceAction(const eCartTask& task);
    void finishResourceAction(const eCartTask& task);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);

    stdsptr<eWalkableObject> getWalkable() const;

    void updateWaiting();

    void waitOutside();
    void spread();
    void clearTask();

    void disappear();

    stdptr<eBuildingWithResource> mBuilding;
    stdptr<eBuilding> mTarget;

    eCartTask mTask;

    int mUpdateWaiting = 0;

    bool mNoTarget = false;
    bool mWaitOutside = false;
};

class eCTA_findTargetFinish : public eCharActFunc {
public:
    eCTA_findTargetFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_findTargetFinish) {}
    eCTA_findTargetFinish(eGameBoard& board,
                          eCartTransporterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::CTA_findTargetFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->targetResourceAction(mBx, mBy);
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eCartTransporterAction*>(ca);
        });
        eSaveArchive ar(src);
        ar.field("buildingX", mBx);
        ar.field("buildingY", mBy);
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        eSaveArchive ar(dst);
        ar.field("buildingX", const_cast<int&>(mBx));
        ar.field("buildingY", const_cast<int&>(mBy));
    }

    void setXY(const int x, const int y) {
        mBx = x;
        mBy = y;
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
                    mTptr = static_cast<eCartTransporterAction*>(resolveCharAction(ioid));
                });
            }
        }
        ar.field("mBx", mBx);
        ar.field("mBy", mBy);
    }
private:
    stdptr<eCartTransporterAction> mTptr;
    int mBx = -1;
    int mBy = -1;
};

class eCTA_waitOutsideFinish : public eCharActFunc {
public:
    eCTA_waitOutsideFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_waitOutsideFinish) {}
    eCTA_waitOutsideFinish(eGameBoard& board,
                           eCartTransporterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::CTA_waitOutsideFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->spread();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eCartTransporterAction*>(ca);
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
                    mTptr = static_cast<eCartTransporterAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eCartTransporterAction> mTptr;
};

class eCTA_spreadFinish : public eCharActFunc {
public:
    eCTA_spreadFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_spreadFinish) {}
    eCTA_spreadFinish(eGameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::CTA_spreadFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        const auto c = mCptr.get();
        c->setActionType(eCharacterActionType::stand);
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = c;
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

#endif // ECARTTRANSPORTERACTION_H
