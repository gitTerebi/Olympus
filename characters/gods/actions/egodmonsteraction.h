#ifndef EGODMONSTERACTION_H
#define EGODMONSTERACTION_H

#include "characters/echaracter.h"
#include "characters/actions/ecomplexaction.h"

#include "buildings/eheatgetters.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "fileIO/esavearchive.h"

#include "textures/edestructiontextures.h"

class eGod;
class eGodAct;
enum class eGodSound;

enum class eFindFailFuncType {
    teleport,
    teleport2,
    tryAgain
};

class eMissileTarget {
public:
    eMissileTarget() {}
    eMissileTarget(eTile* const tile) :
        mTile(tile) {}
    eMissileTarget(eCharacter* const c) :
        mChar(c) {}

    bool operator!() const {
        return !target();
    }

    eTile* target() const {
        if(mChar) return mChar->tile();
        return mTile;
    }

    void serialize(eSaveArchive& ar, eGameBoard& board) {
        ar.tileField("tile", board, mTile);
        ar.characterField("character", &board, mChar);
    }
private:
    eTile* mTile = nullptr;
    stdptr<eCharacter> mChar;
};

inline bool missileTargetField(eSaveArchive& ar, const char* name,
                               eGameBoard& board, eMissileTarget& val) {
    const bool ok = ar.archiveField(name, [&](eSaveArchive& childAr) {
        val.serialize(childAr, board);
    });
    if(!ok) printf("[saveLoad] missileTargetField '%s' missing data.\n", name);
    return ok;
}

class eFindFailFunc {
public:
    eFindFailFunc(eGameBoard& board, const eFindFailFuncType type) :
        mBoard(board), mType(type) {}
    virtual ~eFindFailFunc() = default;

    virtual void call(eTile* const tile) = 0;

    eFindFailFuncType type() const { return mType; }

    eGameBoard& board() { return mBoard; }

    void serialize(eSaveArchive& ar) { serializeFields(ar); }

    static stdsptr<eFindFailFunc> sCreate(eGameBoard& board,
                                          const eFindFailFuncType type);
protected:
    virtual void serializeFields(eSaveArchive& ar) { (void)ar; }
private:
    eGameBoard& mBoard;
    const eFindFailFuncType mType;
};

inline bool findFailFuncField(eSaveArchive& ar, const char* name,
                              eGameBoard& board, stdsptr<eFindFailFunc>& val) {
    bool hasValue = val != nullptr;
    const std::string hasName = std::string(name) + ".has";
    ar.field(hasName.c_str(), hasValue, false);
    if(!hasValue) {
        if(ar.reading()) val = nullptr;
        return true;
    }
    eFindFailFuncType type = ar.writing() ? val->type() : eFindFailFuncType::teleport;
    const std::string typeName = std::string(name) + ".type";
    ar.field(typeName.c_str(), type);
    if(ar.reading()) val = eFindFailFunc::sCreate(board, type);
    if(!val) {
        printf("[saveLoad] findFailFuncField '%s' unknown type %d.\n",
               name, static_cast<int>(type));
        return false;
    }
    const bool ok = ar.archiveField(name, [&](eSaveArchive& childAr) {
        val->serialize(childAr);
    });
    if(!ok) printf("[saveLoad] findFailFuncField '%s' missing data.\n", name);
    return ok;
}

class eGodMonsterAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void moveAround(const stdsptr<eCharActFunc>& finishAct = nullptr,
                    const int time = 5000,
                    const stdsptr<eWalkableObject>& walkable =
                            eWalkableObject::sCreateDefault());
    using eTileDistance = std::function<int(eTileBase* const)>;
    void goToTarget(const eHeatGetters::eHeatGetter hg,
                    const stdsptr<eFindFailFunc>& findFailFunc,
                    const stdsptr<eObsticleHandler>& oh = nullptr,
                    const eTileDistance& tileDistance = nullptr,
                    const stdsptr<eWalkableObject>& pathFindWalkable =
                        eWalkableObject::sCreateDefault(),
                    const stdsptr<eWalkableObject>& moveWalkable = nullptr);
    void goToTile(eTile* const tile,
                  const stdsptr<eFindFailFunc>& findFailFunc,
                  const stdsptr<eObsticleHandler>& oh = nullptr,
                  const eTileDistance& tileDistance = nullptr,
                  const stdsptr<eWalkableObject>& pathFindWalkable =
                      eWalkableObject::sCreateDefault(),
                  const stdsptr<eWalkableObject>& moveWalkable = nullptr);


    using eTexPtr = eTextureCollection eDestructionTextures::*;
    using eFunc = std::function<void()>;
    void spawnMissile(const eCharacterActionType at,
                      const eCharacterType chart,
                      const int attackTime,
                      const eMissileTarget& target,
                      const stdsptr<eCharActFunc>& playSound,
                      const stdsptr<eGodAct>& hitAct,
                      const stdsptr<eCharActFunc>& finishAttackA = nullptr);
    void spawnMultipleMissiles(const eCharacterActionType at,
                               const eCharacterType ct,
                               const int attackTime,
                               const eMissileTarget& target,
                               const stdsptr<eCharActFunc>& playSound,
                               const stdsptr<eGodAct>& playHitSound,
                               const stdsptr<eCharActFunc>& finishA,
                               const int nMissiles);
    void spawnTimedMissiles(const eCharacterActionType at,
                            const eCharacterType ct,
                            const int attackTime,
                            const eMissileTarget& target,
                            const stdsptr<eCharActFunc>& playSound,
                            const stdsptr<eGodAct>& playHitSound,
                            const stdsptr<eCharActFunc>& finishA,
                            const int time);

    bool lookForGodAttack(const int dtime, int& time,
                          const int freq, const int range);
    void fightGod(eGod* const g, const stdsptr<eCharActFunc>& finishAttackA);

    void spawnGodMultipleMissiles(
            const eCharacterActionType at,
            const eCharacterType chart,
            const eMissileTarget& target,
            const eGodSound sound,
            const stdsptr<eGodAct>& playHitSound,
            const stdsptr<eCharActFunc>& finishA,
            const int nMissiles);
    void spawnGodTimedMissiles(const eCharacterActionType at,
            const eCharacterType chart,
            eTile* const target,
            const eGodSound sound,
            const stdsptr<eGodAct>& playHitSound,
            const stdsptr<eCharActFunc>& finishA,
            const int time);

    void patrol(const stdsptr<eCharActFunc>& finishAct = nullptr,
                const int dist = 100);
    void goToNearestRoad(const stdsptr<eCharActFunc>& finishAct = nullptr,
                         const int dist = 100);

    void pauseAction();
    void resumeAction();

    void appear();
    void disappear(const bool die = false,
                   const stdsptr<eCharActFunc>& finish = nullptr);
    void teleport(eTile* const tile);

    void randomPlaceOnBoard();

    void playAppearSound();
    void playDisappearSound();
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    void hermesRun(const bool appear);

    std::vector<ePausedAction> mPausedActions;
};

class eGAA_fightFinish : public eCharActFunc {
public:
    eGAA_fightFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_fightFinish) {}
    eGAA_fightFinish(eGameBoard& board, eGodMonsterAction* const winnerA,
                     eGodMonsterAction* const loserA, const eGodType wt,
                     const eGodType lt) :
        eCharActFunc(board, eCharActFuncType::GAA_fightFinish),
        mWinnerPtr(winnerA), mLoserPtr(loserA),
        mWt(wt), mLt(lt) {}

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("winner", &board(), mWinnerPtr);
        ar.characterActionAsField("loser", &board(), mLoserPtr);
        ar.field("winnerGodType", mWt);
        ar.field("loserGodType", mLt);
    }
private:
    stdptr<eGodMonsterAction> mWinnerPtr;
    stdptr<eGodMonsterAction> mLoserPtr;
    eGodType mWt;
    eGodType mLt;
};

class eGMA_patrolFailFail : public eCharActFunc {
public:
    eGMA_patrolFailFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMA_patrolFailFail) {}
    eGMA_patrolFailFail(eGameBoard& board, eGodMonsterAction* const ca,
                        const stdsptr<eCharActFunc>& finishAct) :
        eCharActFunc(board, eCharActFuncType::GMA_patrolFailFail),
        mTptr(ca), mFinishAct(finishAct) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->moveAround(mFinishAct, 15000);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.charActFuncField("finishAction", board(), mFinishAct);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
    stdsptr<eCharActFunc> mFinishAct;
};

class eGMA_patrolFailFinish : public eCharActFunc {
public:
    eGMA_patrolFailFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMA_patrolFailFinish) {}
    eGMA_patrolFailFinish(eGameBoard& board, eGodMonsterAction* const ca,
                          const stdsptr<eCharActFunc>& finishAct,
                          const int dist) :
        eCharActFunc(board, eCharActFuncType::GMA_patrolFailFinish),
        mTptr(ca), mFinishAct(finishAct), mDist(dist) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->patrol(mFinishAct, mDist);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.charActFuncField("finishAction", board(), mFinishAct);
        ar.field("distance", mDist);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
    stdsptr<eCharActFunc> mFinishAct;
    int mDist = 100;
};

class eGMA_patrolFail : public eCharActFunc {
public:
    eGMA_patrolFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMA_patrolFail) {}
    eGMA_patrolFail(eGameBoard& board,
                    eGodMonsterAction* const ca,
                    const stdsptr<eCharActFunc>& finishAct,
                    const int dist) :
        eCharActFunc(board, eCharActFuncType::GMA_patrolFail),
        mTptr(ca), mFinishAct(finishAct), mDist(dist) {}

    void call() override {
        if(!mTptr) return;
        mTptr->goToNearestRoad(mFinishAct, mDist);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.charActFuncField("finishAction", board(), mFinishAct);
        ar.field("distance", mDist);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
    stdsptr<eCharActFunc> mFinishAct;
    int mDist = 100;
};

class eGMA_spawnMissileFinish : public eCharActFunc {
public:
    eGMA_spawnMissileFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMA_spawnMissileFinish) {}
    eGMA_spawnMissileFinish(eGameBoard& board,
                            eCharacter* const c,
                            const eCharacterActionType at,
                            const eCharacterType chart,
                            eTile* const target,
                            const stdsptr<eGodAct>& hitAct,
                            const stdsptr<eCharActFunc>& finishAttackA) :
        eCharActFunc(board, eCharActFuncType::GMA_spawnMissileFinish),
        mCptr(c), mAt(at), mChart(chart),
        mTarget(target),
        mHitAct(hitAct),
        mFinishAttackA(finishAttackA) {}

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
        ar.field("actionType", mAt);
        ar.field("characterType", mChart);
        ar.tileField("targetTile", board(), mTarget);
        ar.godActField("hitAction", board(), mHitAct);
        ar.charActFuncField("finishAttackAction", board(), mFinishAttackA);
    }
private:
    stdptr<eCharacter> mCptr;
    eCharacterActionType mAt;
    eCharacterType mChart;
    eTile* mTarget = nullptr;
    stdsptr<eGodAct> mHitAct;
    stdsptr<eCharActFunc> mFinishAttackA;
};

class eGMA_spawnMultipleMissilesFinish : public eCharActFunc {
public:
    eGMA_spawnMultipleMissilesFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMA_spawnMultipleMissilesFinish) {}
    eGMA_spawnMultipleMissilesFinish(eGameBoard& board,
                                     eGodMonsterAction* const ca,
                                     const eCharacterActionType at,
                                     const eCharacterType chart,
                                     const int attackTime,
                                     const eMissileTarget& target,
                                     const stdsptr<eCharActFunc>& playSound,
                                     const stdsptr<eGodAct>& playHitSound,
                                     const stdsptr<eCharActFunc>& finishA,
                                     const int nMissiles) :
        eCharActFunc(board, eCharActFuncType::GMA_spawnMultipleMissilesFinish),
        mTptr(ca), mAt(at), mChart(chart), mAttackTime(attackTime),
        mTarget(target), mPlaySound(playSound),
        mPlayHitSound(playHitSound), mFinishA(finishA),
        mNMissiles(nMissiles) {}

    void call() override {
        if(!mTptr) return;
        mTptr->spawnMultipleMissiles(mAt, mChart, mAttackTime,
                                     mTarget, mPlaySound, mPlayHitSound,
                                     mFinishA, mNMissiles - 1);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.field("actionType", mAt);
        ar.field("characterType", mChart);
        ar.field("attackTime", mAttackTime);
        missileTargetField(ar, "missileTarget", board(), mTarget);
        ar.charActFuncField("playSoundAction", board(), mPlaySound);
        ar.godActField("playHitSoundAction", board(), mPlayHitSound);
        ar.charActFuncField("finishAction", board(), mFinishA);
        ar.field("missileCount", mNMissiles);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
    eCharacterActionType mAt;
    eCharacterType mChart;
    int mAttackTime = 0;
    eMissileTarget mTarget;
    stdsptr<eCharActFunc> mPlaySound;
    stdsptr<eGodAct> mPlayHitSound;
    stdsptr<eCharActFunc> mFinishA;
    int mNMissiles = 1;
};

class eGMA_goToTargetFail : public eCharActFunc {
public:
    eGMA_goToTargetFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMA_goToTargetFail) {}
    eGMA_goToTargetFail(eGameBoard& board, eTile* const tile,
                        const stdsptr<eFindFailFunc>& func) :
        eCharActFunc(board, eCharActFuncType::GMA_goToTargetFail),
        mTile(tile), mFunc(func) {}

    void call() override {
        if(mFunc) mFunc->call(mTile);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.tileField("tile", board(), mTile);
        findFailFuncField(ar, "findFailFunc", board(), mFunc);
    }
private:
    eTile* mTile = nullptr;
    stdsptr<eFindFailFunc> mFunc;
};

class eGodMonsterActionInd : public eGodMonsterAction {
public:
    eGodMonsterActionInd(eCharacter* const c) :
        eGodMonsterAction(c, eCharActionType::godMonsterActionInd) {}

    bool decide() override {
        setState(eCharacterActionState::finished);
        return false;
    }
};

#endif // EGODMONSTERACTION_H
