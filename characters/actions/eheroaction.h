#ifndef EHEROACTION_H
#define EHEROACTION_H

#include "eactionwithcomeback.h"

#include "characters/monsters/emonster.h"
#include "characters/heroes/ehero.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

class eSaveArchive;

enum class eHeroActionStage {
    none, patrol, hunt, fight, goBack, defend
};

class eHeroAction : public eActionWithComeback {
public:
    eHeroAction(eCharacter* const c);

    bool decide() override;
    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    void lookForMonster();
    void sendOnQuest();
    void goBackToHall();
    void waitAndGoBackToHall(const int w);
private:
    void serialize(eSaveArchive& ar);
    void defendCity();
    void lookForMonsterFight();
    bool fightMonster(eMonster* const m);
    void huntMonster(eMonster* const m, const bool second);

    stdsptr<eWalkableObject> defaultWalkable() const;

    eHeroType heroType() const;
    bool rangedHero() const;

    bool mQuestWaiting = false;

    eHeroActionStage mStage = eHeroActionStage::none;
    int mLookForMonster = 0;
    int mLookForCityDefense = 0;
};

class eHA_patrolFail : public eCharActFunc {
public:
    eHA_patrolFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_patrolFail) {}
    eHA_patrolFail(eGameBoard& board, eHeroAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HA_patrolFail),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        const auto c = t->character();
        c->setActionType(eCharacterActionType::walk);
        t->goBack(eWalkableObject::sCreateDefault());
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eHeroAction*>(ca);
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
                    mTptr = static_cast<eHeroAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eHeroAction> mTptr;
};

class eHA_huntMonsterFinish : public eCharActFunc {
public:
    eHA_huntMonsterFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_huntMonsterFinish) {}
    eHA_huntMonsterFinish(eGameBoard& board, eHeroAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HA_huntMonsterFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        mTptr->lookForMonster();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eHeroAction*>(ca);
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
                    mTptr = static_cast<eHeroAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eHeroAction> mTptr;
};

class eHA_fightMonsterDie : public eCharActFunc {
public:
    eHA_fightMonsterDie(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_fightMonsterDie) {}
    eHA_fightMonsterDie(eGameBoard& board, eMonster* const m) :
        eCharActFunc(board, eCharActFuncType::HA_fightMonsterDie),
        mMptr(m) {}

    void call() override {
        if(!mMptr || mMptr->dead()) return;
        auto& board = eCharActFunc::board();
        const auto cid = mMptr->onCityId();
        eEventData ed(cid);
        const auto ct = mMptr->type();
        const auto mt = eMonster::sCharacterToMonsterType(ct);
        ed.fMonster = mt;
        ed.fTile = mMptr->tile();
        board.event(eEvent::monsterSlain, ed);
        mMptr->killWithCorpse();
        board.addSlayedMonster(cid, mt);
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mMptr = static_cast<eMonster*>(c);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mMptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mMptr ? mMptr->ioID() : -1;
            ar.field("mMptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mMptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mMptr = static_cast<eMonster*>(resolveChar(ioid));
                });
            }
        }
    }
private:
    stdptr<eMonster> mMptr;
};

class eHA_waitAndGoToHallFinish : public eCharActFunc {
public:
    eHA_waitAndGoToHallFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_waitAndGoToHallFinish) {}
    eHA_waitAndGoToHallFinish(eGameBoard& board, eHeroAction* const a) :
        eCharActFunc(board, eCharActFuncType::HA_waitAndGoToHallFinish),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        mAptr->goBackToHall();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const a) {
            mAptr = static_cast<eHeroAction*>(a);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mAptr);
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
                    mAptr = static_cast<eHeroAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eHeroAction> mAptr;
};

#endif // EHEROACTION_H
