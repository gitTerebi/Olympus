#ifndef ESHEPHERDACTION_H
#define ESHEPHERDACTION_H

#include "eactionwithcomeback.h"

#include "characters/eshepherd.h"
#include "buildings/eshepherbuildingbase.h"
#include "characters/edomesticatedanimal.h"

class eDomesticatedAnimal;
class eSaveArchive;

class eShepherdAction : public eActionWithComeback {
    friend class eSA_collectDecisionFinish;
    friend class eSA_groomDecisionFinish;
public:
    eShepherdAction(eShepherBuildingBase* const shed,
                    eResourceCollectorBase* const c,
                    const eCharacterType animalType);
    eShepherdAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    bool findResourceDecision();
    void collectDecision(eDomesticatedAnimal* const a);
    void groomDecision(eDomesticatedAnimal* const a);
    void goBackDecision();
    void waitDecision();
    void serialize(eSaveArchive& ar);

    eCharacterType mAnimalType;

    eResourceCollectorBase* mCharacter = nullptr;
    eShepherBuildingBase* mShed = nullptr;

    bool mFinishOnce = true;
    int mGroomed = 0;
    bool mNoResource = false;
};

class eSA_collectDecisionFinish : public eCharActFunc {
public:
    eSA_collectDecisionFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionFinish) {}
    eSA_collectDecisionFinish(eGameBoard& board, eShepherdAction* const t,
                              eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionFinish),
        mTptr(t), mAptr(a) {}

    void call() override {
        int c = 0;
        if(mAptr) {
            const auto a = mAptr.get();
            c = a->collect();
            a->setBusy(false);
            a->setVisible(true);
        }
        if(!mTptr) return;
        mTptr->mCharacter->incCollected(c);
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eShepherdAction*>(ca);
        });
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mAptr = static_cast<eDomesticatedAnimal*>(c);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        dst.writeCharacter(mAptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mTptr ? mTptr->ioID() : -1;
            ar.field("mTptr", ioid);
            int ioid2 = mAptr ? mAptr->ioID() : -1;
            ar.field("mAptr", ioid2);
        } else {
            int ioid = -1;
            ar.field("mTptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mTptr = static_cast<eShepherdAction*>(resolveCharAction(ioid));
                });
            }
            int ioid2 = -1;
            ar.field("mAptr", ioid2);
            if(ioid2 >= 0) {
                ar.addPostFunc([this, ioid2]() {
                    mAptr = static_cast<eDomesticatedAnimal*>(resolveChar(ioid2));
                });
            }
        }
    }
private:
    stdptr<eShepherdAction> mTptr;
    stdptr<eDomesticatedAnimal> mAptr;
};

class eSA_collectDecisionDeleteFail : public eCharActFunc {
public:
    eSA_collectDecisionDeleteFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionDeleteFail) {}
    eSA_collectDecisionDeleteFail(eGameBoard& board, eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionDeleteFail),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        const auto a = mAptr.get();
        a->setBusy(false);
        a->setVisible(true);
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mAptr = static_cast<eDomesticatedAnimal*>(c);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mAptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mAptr ? mAptr->ioID() : -1;
            ar.field("mAptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mAptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mAptr = static_cast<eDomesticatedAnimal*>(resolveChar(ioid));
                });
            }
        }
    }
private:
    stdptr<eDomesticatedAnimal> mAptr;
};

class eSA_groomDecisionFinish : public eCharActFunc {
public:
    eSA_groomDecisionFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionFinish) {}
    eSA_groomDecisionFinish(eGameBoard& board, eShepherdAction* const t,
                            eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionFinish),
        mTptr(t), mAptr(a) {}

    void call() override {
        if(mAptr) {
            const auto a = mAptr.get();
            a->groom();
            a->setBusy(false);
        }
        if(!mTptr) return;
        mTptr->mGroomed++;
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eShepherdAction*>(ca);
        });
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mAptr = static_cast<eDomesticatedAnimal*>(c);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        dst.writeCharacter(mAptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mTptr ? mTptr->ioID() : -1;
            ar.field("mTptr", ioid);
            int ioid2 = mAptr ? mAptr->ioID() : -1;
            ar.field("mAptr", ioid2);
        } else {
            int ioid = -1;
            ar.field("mTptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mTptr = static_cast<eShepherdAction*>(resolveCharAction(ioid));
                });
            }
            int ioid2 = -1;
            ar.field("mAptr", ioid2);
            if(ioid2 >= 0) {
                ar.addPostFunc([this, ioid2]() {
                    mAptr = static_cast<eDomesticatedAnimal*>(resolveChar(ioid2));
                });
            }
        }
    }
private:
    stdptr<eShepherdAction> mTptr;
    stdptr<eDomesticatedAnimal> mAptr;
};

class eSA_groomDecisionDeleteFail : public eCharActFunc {
public:
    eSA_groomDecisionDeleteFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionDeleteFail) {}
    eSA_groomDecisionDeleteFail(eGameBoard& board, eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionDeleteFail),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        const auto a = mAptr.get();
        a->setBusy(false);
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mAptr = static_cast<eDomesticatedAnimal*>(c);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mAptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mAptr ? mAptr->ioID() : -1;
            ar.field("mAptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mAptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mAptr = static_cast<eDomesticatedAnimal*>(resolveChar(ioid));
                });
            }
        }
    }
private:
    stdptr<eDomesticatedAnimal> mAptr;
};

#endif // ESHEPHERDACTION_H
