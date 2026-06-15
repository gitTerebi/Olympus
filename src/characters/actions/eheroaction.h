#ifndef EHEROACTION_H
#define EHEROACTION_H

#include "eactionwithcomeback.h"

#include "characters/monsters/emonster.h"
#include "characters/heroes/ehero.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

class SaveArchive;

enum class eHeroActionStage {
    none, patrol, hunt, fight, goBack, defend, quest, waitToHall, goBackToHall
};

class eHeroAction : public eActionWithComeback {
public:
    eHeroAction(eCharacter* const c);

    bool decide() override;
    void increment(const int by) override;

    void lookForMonster();
    void sendOnQuest();
    void goBackToHall();
    void waitAndGoBackToHall(const int w);
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void startPatrol();
    void rebuildCurrentStage();
    void rebuildWaitToHall();
    void defendCity();
    void lookForMonsterFight();
    bool fightMonster(eMonster* const m);
    void huntMonster(eMonster* const m, const bool second);

    stdsptr<WalkableObject> defaultWalkable() const;

    eHeroType heroType() const;
    bool rangedHero() const;

    bool mQuestWaiting = false;

    eHeroActionStage mStage = eHeroActionStage::none;
    int mLookForMonster = 0;
    int mLookForCityDefense = 0;
    int mWaitToHallRemaining = 0;
    stdptr<eMonster> mTargetMonster;
};

class eHA_patrolFail : public eCharActFunc {
public:
    eHA_patrolFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_patrolFail) {}
    eHA_patrolFail(GameBoard& board, eHeroAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HA_patrolFail),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        const auto c = t->character();
        c->setActionType(eCharacterActionType::walk);
        t->goBack(WalkableObject::sCreateDefault());
    }
protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eHeroAction> mTptr;
};

class eHA_huntMonsterFinish : public eCharActFunc {
public:
    eHA_huntMonsterFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_huntMonsterFinish) {}
    eHA_huntMonsterFinish(GameBoard& board, eHeroAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HA_huntMonsterFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        mTptr->lookForMonster();
    }
protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eHeroAction> mTptr;
};

class eHA_fightMonsterDie : public eCharActFunc {
public:
    eHA_fightMonsterDie(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_fightMonsterDie) {}
    eHA_fightMonsterDie(GameBoard& board, eMonster* const m) :
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

protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterAsField("monster", &board(), mMptr);
    }
private:
    stdptr<eMonster> mMptr;
};

class eHA_waitAndGoToHallFinish : public eCharActFunc {
public:
    eHA_waitAndGoToHallFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HA_waitAndGoToHallFinish) {}
    eHA_waitAndGoToHallFinish(GameBoard& board, eHeroAction* const a) :
        eCharActFunc(board, eCharActFuncType::HA_waitAndGoToHallFinish),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        mAptr->goBackToHall();
    }
protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mAptr);
    }
private:
    stdptr<eHeroAction> mAptr;
};

#endif // EHEROACTION_H
