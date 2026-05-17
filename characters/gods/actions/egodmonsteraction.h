#ifndef EGODMONSTERACTION_H
#define EGODMONSTERACTION_H

#include "characters/echaracter.h"
#include "characters/actions/ecomplexaction.h"

#include "buildings/eheatgetters.h"
#include "characters/actions/walkable/ewalkableobject.h"
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

    void read(eReadStream& src, eGameBoard& board) {
        mTile = src.readTile(board);
        src.readCharacter(&board, [this](eCharacter* const c) {
            mChar = c;
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeTile(mTile);
        dst.writeCharacter(mChar);
    }
private:
    eTile* mTile = nullptr;
    stdptr<eCharacter> mChar;
};

inline bool missileTargetField(eSaveArchive& ar, const char* name,
                               eGameBoard& board, eMissileTarget& val) {
    return ar.payloadField(name,
        [&val](eWriteStream& dst) { val.write(dst); },
        [&val, &board](eReadStream& src) { val.read(src, board); });
}

class eFindFailFunc {
public:
    eFindFailFunc(eGameBoard& board, const eFindFailFuncType type) :
        mBoard(board), mType(type) {}

    virtual void call(eTile* const tile) = 0;

    eFindFailFuncType type() const { return mType; }

    eGameBoard& board() { return mBoard; }

    virtual void read(eReadStream& src) = 0;
    virtual void write(eWriteStream& dst) const = 0;

    static stdsptr<eFindFailFunc> sCreate(eGameBoard& board,
                                          const eFindFailFuncType type);
private:
    eGameBoard& mBoard;
    const eFindFailFuncType mType;
};

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

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mWinnerPtr = static_cast<eGodMonsterAction*>(ca);
        });
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mLoserPtr = static_cast<eGodMonsterAction*>(ca);
        });
        eSaveArchive ar(src);
        ar.field("winnerGodType", mWt);
        ar.field("loserGodType", mLt);
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mWinnerPtr);
        dst.writeCharacterAction(mLoserPtr);
        eSaveArchive ar(dst);
        ar.field("winnerGodType", const_cast<eGodType&>(mWt));
        ar.field("loserGodType", const_cast<eGodType&>(mLt));
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

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodMonsterAction*>(ca);
        });
        mFinishAct = src.readCharActFunc(board());
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        dst.writeCharActFunc(mFinishAct.get());
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

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodMonsterAction*>(ca);
        });
        mFinishAct = src.readCharActFunc(board());
        eSaveArchive ar(src);
        ar.field("distance", mDist);
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        dst.writeCharActFunc(mFinishAct.get());
        eSaveArchive ar(dst);
        ar.field("distance", const_cast<int&>(mDist));
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

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodMonsterAction*>(ca);
        });
        mFinishAct = src.readCharActFunc(board());
        eSaveArchive ar(src);
        ar.field("distance", mDist);
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        dst.writeCharActFunc(mFinishAct.get());
        eSaveArchive ar(dst);
        ar.field("distance", const_cast<int&>(mDist));
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

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = c;
        });
        eSaveArchive ar(src);
        ar.field("actionType", mAt);
        ar.field("characterType", mChart);
        mTarget = src.readTile(board());

        mHitAct = src.readGodAct(board());
        mFinishAttackA = src.readCharActFunc(board());
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCptr);
        eSaveArchive ar(dst);
        ar.field("actionType", const_cast<eCharacterActionType&>(mAt));
        ar.field("characterType", const_cast<eCharacterType&>(mChart));
        dst.writeTile(mTarget);

        dst.writeGodAct(mHitAct.get());
        dst.writeCharActFunc(mFinishAttackA.get());
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

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eGodMonsterAction*>(ca);
        });
        eSaveArchive ar(src);
        ar.field("actionType", mAt);
        ar.field("characterType", mChart);
        ar.field("attackTime", mAttackTime);
        mTarget.read(src, board());
        mPlaySound = src.readCharActFunc(board());
        mPlayHitSound = src.readGodAct(board());
        mFinishA = src.readCharActFunc(board());
        ar.field("missileCount", mNMissiles);
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        eSaveArchive ar(dst);
        ar.field("actionType", const_cast<eCharacterActionType&>(mAt));
        ar.field("characterType", const_cast<eCharacterType&>(mChart));
        ar.field("attackTime", const_cast<int&>(mAttackTime));
        mTarget.write(dst);
        dst.writeCharActFunc(mPlaySound.get());
        dst.writeGodAct(mPlayHitSound.get());
        dst.writeCharActFunc(mFinishA.get());
        ar.field("missileCount", const_cast<int&>(mNMissiles));
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

    void read(eReadStream& src) override {
        mTile = src.readTile(board());
        bool hasFunc;
        eSaveArchive ar(src);
        ar.field("hasFindFailFunc", hasFunc);
        if(hasFunc) {
            eFindFailFuncType type;
            ar.field("findFailFuncType", type);
            mFunc = eFindFailFunc::sCreate(board(), type);
            mFunc->read(src);
        }
    }

    void write(eWriteStream& dst) const override {
        dst.writeTile(mTile);
        const bool hasFunc = mFunc != nullptr;
        eSaveArchive ar(dst);
        ar.field("hasFindFailFunc", const_cast<bool&>(hasFunc));
        if(hasFunc) {
            auto type = mFunc->type();
            ar.field("findFailFuncType", type);
            mFunc->write(dst);
        }
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
