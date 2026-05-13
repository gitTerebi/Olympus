#ifndef ESETTLERACTION_H
#define ESETTLERACTION_H

#include "eactionwithcomeback.h"

class eSaveArchive;

class eSettlerAction : public eActionWithComeback {
    friend class eSA_findHouseFail;
    friend class eSA_findHouseFinish;
public:
    eSettlerAction(eCharacter* const c);
    ~eSettlerAction();

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    void setNumberPeople(const int p);
    void setInitialWait(const int w);
    int nPeople() const { return mNPeople; }
protected:
    void findHouse();
    void goBack2();
    void leave();
    bool enterHouse();
    void serializeJson(eJsonArchive& ar) override {
        eActionWithComeback::serializeJson(ar);
        ar.field("mNPeople", mNPeople);
        ar.field("mNoHouses", mNoHouses);
    }

private:
    void serialize(eSaveArchive& ar);

    int mNPeople = 0;
    int mInitialWait = 0;
    bool mNoHouses = false;
};

class eSA_findHouseFail : public eCharActFunc {
public:
    eSA_findHouseFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFail) {}
    eSA_findHouseFail(eGameBoard& board, eSettlerAction* const t) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFail),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->findHouse();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eSettlerAction*>(ca);
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
                    mTptr = static_cast<eSettlerAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eSettlerAction> mTptr;
};

class eSA_findHouseFinish : public eCharActFunc {
public:
    eSA_findHouseFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFinish) {}
    eSA_findHouseFinish(eGameBoard& board, eSettlerAction* const t) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        const bool r = t->enterHouse();
        const int nPeople = t->nPeople();
        if(r && nPeople <= 0) {
            t->setState(eCharacterActionState::finished);
        }
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eSettlerAction*>(ca);
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
                    mTptr = static_cast<eSettlerAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eSettlerAction> mTptr;
};

#endif // ESETTLERACTION_H
