#ifndef GOD_ACTION_H
#define GOD_ACTION_H

#include "characters/gods/actions/egodmonsteraction.h"

#include "characters/gods/egod.h"
#include "engine/game-board.h"
#include "audio/sounds.h"
#include "buildings/small-house.h"
#include "buildings/eresourcebuilding.h"
#include "fileIO/esavearchive.h"

class eDestructionTextures;
class eSaveArchive;
enum class eGodSound;

enum class eGodActType {
    lookForSoldierAttack,
    lookForBless,
    playMonsterBuildingAttackSound,
    playFightGodHitSoundGodAct,
    lookForAttack,

    provideResource,
    provideAgora,
    provideDrachmas,
    atlasHelp,
    apolloHelp,
    aphroditeHelp,

    lookForPlague,
    lookForEvict,
    lookForTargetedBless,
    lookForTargetedAttack,
    spawnImpactPuffs
};

class eGodAct {
public:
    eGodAct(GameBoard& board, const eGodActType type) :
        mType(type), mBoard(board) {}

    virtual eMissileTarget find(eTile* const t) = 0;
    virtual void act() = 0;

    eGodActType type() const { return mType; }
    GameBoard& board() { return mBoard; }

    void serialize(eSaveArchive& ar) { serializeFields(ar); }

    static stdsptr<eGodAct> sCreate(GameBoard& board, const eGodActType t);
protected:
    virtual void serializeFields(eSaveArchive& ar) { (void)ar; }
private:
    const eGodActType mType;
    GameBoard& mBoard;
};

class ePlayMonsterBuildingAttackSoundGodAct : public eGodAct {
public:
    ePlayMonsterBuildingAttackSoundGodAct(
            GameBoard& board, eBuilding* const b) :
        eGodAct(board, eGodActType::playMonsterBuildingAttackSound),
        mBuilding(b) {}
    ePlayMonsterBuildingAttackSoundGodAct(GameBoard& board) :
        ePlayMonsterBuildingAttackSoundGodAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(!mBuilding) return;
        const auto b = mBuilding.get();
        auto& board = b->getBoard();
        board.ifVisible(b->centerTile(), [&]() {
            eSounds::playFireballHitSound();
        });
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.buildingField("building", &board(), mBuilding);
    }
private:
    stdptr<eBuilding> mBuilding;
};

class ePlayFightGodHitSoundGodAct : public eGodAct {
public:
    ePlayFightGodHitSoundGodAct(GameBoard& board, eGod* const g) :
        eGodAct(board, eGodActType::playFightGodHitSoundGodAct),
        mG(g) {}
    ePlayFightGodHitSoundGodAct(GameBoard& board) :
        ePlayFightGodHitSoundGodAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(!mG) return;
        board().ifVisible(mG->tile(), [&]() {
            eSounds::playGodSound(mG->type(), eGodSound::hit);
        });
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.characterAsField("god", &board(), mG);
    }
private:
    stdptr<eGod> mG;
};

class eLookForPlagueGodAct : public eGodAct {
public:
    eLookForPlagueGodAct(GameBoard& board) :
        eGodAct(board, eGodActType::lookForPlague) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        const auto b = t->underBuilding();
        if(!b) return null;
        const auto type = b->type();
        if(type != eBuildingType::commonHouse) return null;
        const auto ch = static_cast<SmallHouse*>(b);
        const bool p = ch->plague();
        if(p) return null;
        const auto tile = b->centerTile();
        const int tx = tile->x();
        const int ty = tile->y();
        int dist;
        auto& board = this->board();
        const auto cid = tile->cityId();
        board.nearestPlague(cid, tx, ty, dist);
        if(dist < 10) return null;
        mTarget = static_cast<SmallHouse*>(b);
        return tile;
    }

    void act() override {
        if(mTarget) {
            auto& board = this->board();
            board.startPlague(mTarget);
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.buildingAsField("targetHouse", &board(), mTarget);
    }
private:
    stdptr<SmallHouse> mTarget;
};

class eLookForEvictGodAct : public eGodAct {
public:
    eLookForEvictGodAct(GameBoard& board) :
        eGodAct(board, eGodActType::lookForEvict) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        const auto b = t->underBuilding();
        if(!b) return null;
        const auto type = b->type();
        if(type != eBuildingType::commonHouse &&
           type != eBuildingType::eliteHousing) {
            return null;
        }
        const auto hb = static_cast<eHouseBase*>(b);
        if(hb->people() <= 0) return null;
        mTarget = hb;
        return b->centerTile();
    }

    void act() override {
        if(mTarget) {
            mTarget->leave();
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.buildingAsField("targetHouse", &board(), mTarget);
    }
private:
    stdptr<eHouseBase> mTarget;
};


class eLookForBlessGodActBase : public eGodAct {
public:
    using eGodAct::eGodAct;

    double bless() const { return mBless; }

    void act() override {
        if(mTarget) {
            const auto type = mTarget->type();
            const bool batch = type == eBuildingType::oliveTree ||
                               type == eBuildingType::vine ||
                               type == eBuildingType::orangeTree;
            if(batch) {
                auto& board = this->board();
                const auto tile = mTarget->centerTile();
                const int tx = tile->x();
                const int ty = tile->y();
                for(int x = tx - 3; x <= tx + 3; x++) {
                    for(int y = ty - 3; y <= ty + 3; y++) {
                        const auto tile = board.tile(x, y);
                        if(!tile) continue;
                        const auto ub = tile->underBuilding();
                        if(!ub) continue;
                        const auto utype = ub->type();
                        if(type == utype) ub->setBlessed(mBless);
                    }
                }
            }
            mTarget->setBlessed(mBless);
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.buildingField("targetBuilding", &board(), mTarget);
        ar.field("blessAmount", mBless);
    }
protected:
    stdptr<eBuilding> mTarget;
    double mBless = 0;
};

class eLookForTargetedBlessGodAct : public eLookForBlessGodActBase {
public:
    eLookForTargetedBlessGodAct(GameBoard& board, const double bless,
                                const eGodType type) :
        eLookForBlessGodActBase(board, eGodActType::lookForTargetedBless),
        mType(type) {
        mBless = bless;
    }
    eLookForTargetedBlessGodAct(GameBoard& board) :
        eLookForTargetedBlessGodAct(board, 0, eGodType::zeus) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        const auto b = t->underBuilding();
        if(!b) {
            return null;
        }
        const auto type = b->type();
        if(!eBuilding::sBlessable(type)) {
            return null;
        }
        if(!eGod::sTarget(mType, type)) {
            return null;
        }
        if(b->blessed() || b->cursed()) {
            return null;
        }
        if(const auto rb = dynamic_cast<eResourceBuilding*>(b)) {
            if(rb->sanctuary()) return null;
        }
        mTarget = b;
        return b->centerTile();
    }

    void serializeFields(eSaveArchive& ar) override {
        eLookForBlessGodActBase::serializeFields(ar);
        ar.field("godType", mType);
    }
private:
    eGodType mType;
};

class eLookForBlessGodAct : public eLookForBlessGodActBase {
public:
    eLookForBlessGodAct(GameBoard& board, const double bless) :
        eLookForBlessGodActBase(board, eGodActType::lookForBless) {
        mBless = bless;
    }
    eLookForBlessGodAct(GameBoard& board) :
        eLookForBlessGodAct(board, 0) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        const auto b = t->underBuilding();
        if(!b) return null;
        if(!eBuilding::sBlessable(b->type())) return null;
        if(b->blessed() || b->cursed()) return null;
        mTarget = b;
        return b->centerTile();
    }
};

class eLookForSoldierAttackGodAct : public eGodAct {
public:
    eLookForSoldierAttackGodAct(GameBoard& board, const eTeamId team) :
        eGodAct(board, eGodActType::lookForSoldierAttack),
        mGodTeam(team) {}
    eLookForSoldierAttackGodAct(GameBoard& board) :
        eLookForSoldierAttackGodAct(board, eTeamId::neutralFriendly) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        const auto& chars = t->characters();
        if(chars.empty()) return null;
        for(const auto& cc : chars) {
            if(cc->dead()) continue;
            const bool is = cc->isSoldier();
            if(!is) continue;
            const auto cctid = cc->teamId();
            if(!eTeamIdHelpers::isEnemy(cctid, mGodTeam)) continue;
            mTarget = cc;
            return cc.get();
        }
        return null;
    }

    void act() override {
        if(mTarget && !mTarget->dead()) {
            mTarget->killWithCorpse();
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.field("godTeam", mGodTeam);
        ar.characterField("targetCharacter", &board(), mTarget);
    }
private:
    eTeamId mGodTeam;
    stdptr<eCharacter> mTarget;
};

class eLookForTargetedAttackGodAct : public eGodAct {
public:
    eLookForTargetedAttackGodAct(GameBoard& board,
                                 const eGodType type) :
        eGodAct(board, eGodActType::lookForTargetedAttack),
        mType(type) {}
    eLookForTargetedAttackGodAct(GameBoard& board) :
        eLookForTargetedAttackGodAct(board, eGodType::zeus) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        const auto b = t->underBuilding();
        if(!b) return null;
        const auto type = b->type();
        if(!eBuilding::sAttackable(type)) return null;
        const bool target = eGod::sTarget(mType, type);
        if(!target) return null;
        return b->centerTile();
    }

    void act() override {
        if(mBTarget) {
            mBTarget->collapse();
            eSounds::playCollapseSound();
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.field("godType", mType);
        ar.buildingField("targetBuilding", &board(), mBTarget);
    }
private:
    eGodType mType;
    stdptr<eBuilding> mBTarget;
};

class eLookForAttackGodAct : public eGodAct {
public:
    eLookForAttackGodAct(GameBoard& board) :
        eGodAct(board, eGodActType::lookForAttack) {}
    eLookForAttackGodAct(GameBoard& board, eCharacter* const c) :
        eGodAct(board, eGodActType::lookForAttack),
        mCptr(c) {}

    eMissileTarget find(eTile* const t) override {
        const auto null = static_cast<eTile*>(nullptr);
        if(!mCptr) return null;
        if(mCptr->tile() == t) return null;
        const auto srcTid = mCptr->teamId();
        const auto b = t->underBuilding();
        if(b && eBuilding::sAttackable(b->type())) {
            if(!eTeamIdHelpers::isEnemy(b->teamId(), srcTid)) return null;
            mBTarget = b;
            return b->centerTile();
        } else {
            const auto& chars = t->characters();
            if(chars.empty()) return null;
            for(const auto& cc : chars) {
                if(mCptr == cc.get()) continue;
                bool isGod = false;
                eGod::sCharacterToGodType(cc->type(), &isGod);
                if(isGod) continue;
                bool isMonster = false;
                eMonster::sCharacterToMonsterType(cc->type(), &isMonster);
                if(isMonster) continue;
                bool isHero = false;
                eHero::sCharacterToHeroType(cc->type(), &isHero);
                if(isHero) continue;
                if(!eTeamIdHelpers::isEnemy(cc->teamId(), srcTid)) continue;
                mCTarget = cc;
                return cc.get();
            }
            return null;
        }
    }

    void act() override {
        if(!mCptr) return;
        if(mBTarget) {
            const auto type = mCptr->type();
            if(type == eCharacterType::hephaestus) {
                mBTarget->setOnFire(true);
            } else {
                mBTarget->collapse();
                eSounds::playCollapseSound();
            }
        } else if(mCTarget && !mCTarget->dead()) {
            mCTarget->killWithCorpse();
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("sourceCharacter", &board(), mCptr);
        ar.characterField("targetCharacter", &board(), mCTarget);
        ar.buildingField("targetBuilding", &board(), mBTarget);
    }
private:
    stdptr<eCharacter> mCptr;
    stdptr<eCharacter> mCTarget;
    stdptr<eBuilding> mBTarget;
};

class eGodAction : public eGodMonsterAction {
public:
    eGodAction(eCharacter* const c, const eCharActionType type);

    using eTexPtr = eTextureCollection eDestructionTextures::*;
    virtual bool lookForRangeAction(const int dtime,
                            int& time, const int freq,
                            const int range,
                            const eCharacterActionType at,
                            const stdsptr<eGodAct>& act,
                            const eCharacterType chart,
                            const eGodSound missileSound,
                            const int nMissiles = 1);
    bool lookForBlessCurse(const int dtime, int& time,
                           const int freq, const int range,
                           const double bless);
    bool lookForTargetedBlessCurse(const int dtime, int& time,
                                   const int freq, const int range,
                                   const double bless);

    bool lookForSoldierAttack(const int dtime, int& time,
                              const int freq, const int range);

    void spawnGodMissile(const eCharacterActionType at,
                         const eCharacterType chart,
                         const eMissileTarget& target,
                         const eGodSound sound,
                         const stdsptr<eGodAct>& act,
                         const stdsptr<eCharActFunc>& finishAttackA = nullptr);

    void goBackToSanctuary();
    void goToTarget();

    eGodType type() const { return mType; }
private:
    const eGodType mType;
};

class eGA_lookForSoldierAttackFinish : public eCharActFunc {
public:
    eGA_lookForSoldierAttackFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GA_lookForSoldierAttackFinish) {}
    eGA_lookForSoldierAttackFinish(GameBoard& board, eGodAction* const ca,
                                   eTile* const tile) :
        eCharActFunc(board, eCharActFuncType::GA_lookForSoldierAttackFinish),
        mTptr(ca), mTile(tile) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        const auto c = t->character();
        c->changeTile(mTile);
        t->appear();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.tileField("tile", board(), mTile);
    }
private:
    stdptr<eGodAction> mTptr;
    eTile* mTile = nullptr;
};

class eGA_lookForRangeActionFinish : public eCharActFunc {
public:
    eGA_lookForRangeActionFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GA_lookForRangeActionFinish) {}
    eGA_lookForRangeActionFinish(GameBoard& board, eGodAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GA_lookForRangeActionFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        mTptr->resumeAction();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eGodAction> mTptr;
};

class eGA_teleportFinish : public eCharActFunc {
public:
    eGA_teleportFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GA_teleportFinish) {}
    eGA_teleportFinish(GameBoard& board, eGodMonsterAction* const ca,
                       eTile* const tile) :
        eCharActFunc(board, eCharActFuncType::GA_teleportFinish),
        mTptr(ca), mTile(tile) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        const auto c = t->character();
        c->changeTile(mTile);
        t->appear();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.tileField("tile", board(), mTile);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
    eTile* mTile = nullptr;
};

class eGA_hermesRunFinish : public eCharActFunc {
public:
    eGA_hermesRunFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GA_hermesRunFinish) {}
    eGA_hermesRunFinish(GameBoard& board, eGodMonsterAction* const ca,
                        eCharacter* const c, const bool appear) :
        eCharActFunc(board, eCharActFuncType::GA_hermesRunFinish),
        mTptr(ca), mCptr(c), mAppear(appear) {}

    void call() override {
        if(!mTptr || !mCptr) return;
        const auto t = mTptr.get();
        mCptr->setSpeed(52.5);
        if(!mAppear) {
            t->playDisappearSound();
            mCptr->kill();
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.characterField("character", &board(), mCptr);
        ar.field("appear", mAppear);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
    stdptr<eCharacter> mCptr;
    bool mAppear;
};

class eGA_spawnGodMissilePlaySound : public eCharActFunc {
public:
    eGA_spawnGodMissilePlaySound(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GA_spawnGodMissilePlaySound) {}
    eGA_spawnGodMissilePlaySound(GameBoard& board, const eGodSound sound,
                                 eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::GA_spawnGodMissilePlaySound),
        mCptr(c), mSound(sound) {}

    void call() override {
        if(!mCptr) return;
        const auto c = mCptr.get();
        board().ifVisible(c->tile(), [&]() {
            const auto ct = c->type();
            const auto gt = eGod::sCharacterToGodType(ct);
            eSounds::playGodSound(gt, mSound);
        });
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
        ar.field("sound", mSound);
    }
private:
    stdptr<eCharacter> mCptr;
    eGodSound mSound;
};

class eGoToTargetTeleport : public eFindFailFunc {
public:
    eGoToTargetTeleport(GameBoard& board) :
        eFindFailFunc(board, eFindFailFuncType::teleport2) {}
    eGoToTargetTeleport(GameBoard& board, eGodMonsterAction* const ca) :
        eFindFailFunc(board, eFindFailFuncType::teleport2),
        mTptr(ca) {}

    void call(eTile* const tile) override {
        if(!mTptr) return;
        mTptr->teleport(tile);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eGodMonsterAction> mTptr;
};

class eSpawnImpactPuffsGodAct : public eGodAct {
public:
    eSpawnImpactPuffsGodAct(GameBoard& board) :
        eGodAct(board, eGodActType::spawnImpactPuffs) {}
    eSpawnImpactPuffsGodAct(GameBoard& board,
                             const double impactX, const double impactY,
                             const stdsptr<eGodAct>& wrappedAct) :
        eGodAct(board, eGodActType::spawnImpactPuffs),
        mImpactX(impactX), mImpactY(impactY), mWrappedAct(wrappedAct) {}

    eMissileTarget find(eTile* const) override { return static_cast<eTile*>(nullptr); }

    void act() override;

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.field("impactX", mImpactX);
        ar.field("impactY", mImpactY);
        ar.godActField("wrappedAct", board(), mWrappedAct);
    }
private:
    double mImpactX = 0;
    double mImpactY = 0;
    stdsptr<eGodAct> mWrappedAct;
};

#endif // GOD_ACTION_H
