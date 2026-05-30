#include "egodattackaction.h"

#include "buildings/ebuilding.h"
#include "engine/game-board.h"
#include "characters/actions/dionysus-follow-action.h"
#include "characters/monsters/ecalydonianboar.h"
#include "characters/actions/ewaitaction.h"
#include "buildings/epalace.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eGodAttackAction::eGodAttackAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::godAttackAction) {}

eGodAttackAction::~eGodAttackAction() {
    if(mSanctuary) mSanctuary->godComeback();
}

void eGodAttackAction::increment(const int by) {
    const auto c = character();
    const auto type = this->type();
    const auto at = c->actionType();
    if(at == eCharacterActionType::walk) {
        const int cursePeriod = eNumbers::sGodAttackCursePeriod;
        const int curseRange = eNumbers::sGodAttackCurseRange;
        const int lookForGodCheck = 1000;
        int attackPeriod;
        switch(type) {
        case eGodType::ares:
        case eGodType::artemis:
        case eGodType::athena:
        case eGodType::zeus:
            attackPeriod = eNumbers::sGodAttackAggressiveAttackPeriod;
            break;
        case eGodType::aphrodite:
        case eGodType::apollo:
        case eGodType::atlas:
        case eGodType::demeter:
        case eGodType::dionysus:
        case eGodType::hades:
        case eGodType::hephaestus:
        case eGodType::hera:
        case eGodType::hermes:
        case eGodType::poseidon:
        default:
            attackPeriod = eNumbers::sGodAttackAttackPeriod;
            break;
        }
        const int attackRange = eNumbers::sGodAttackAttackRange;

        const int targetedAttackPeriod = eNumbers::sGodAttackTargetedAttackPeriod;
        const int targetedAttackRange = eNumbers::sGodAttackTargetedAttackRange;
        const int targetedCursePeriod = eNumbers::sGodAttackTargetedCursePeriod;
        const int targetedCurseRange = eNumbers::sGodAttackTargetedCurseRange;
        const int godFightRange = eNumbers::sGodAttackGodFightRange;

        bool r = lookForBlessCurse(by, mLookForCurse, cursePeriod, curseRange, -1);
        if(!r) r = lookForAttack(by, mLookForAttack, attackPeriod, attackRange);
        if(!r) r = lookForTargetedAttack(by, mLookForTargetedAttack,
                                         targetedAttackPeriod, targetedAttackRange);
        if(!r) r = lookForTargetedBlessCurse(by, mLookForTargetedCurse,
                                             targetedCursePeriod, targetedCurseRange, -1);
        if(!r) lookForGodAttack(by, mLookForGod, lookForGodCheck, godFightRange);
        if(!r) {
            if(type == eGodType::apollo) {
                auto& board = this->board();
                using eLFPG = eLookForPlagueGodAct;
                const auto act = std::make_shared<eLFPG>(board);
                const auto at = eCharacterActionType::curse;
                const auto s = eGodSound::curse;
                const auto c = character();
                const auto chart = c->type();
                const int plaguePeriod = eNumbers::sGodAttackApolloPlaguePeriod;
                const int plagueRange = eNumbers::sGodAttackApolloPlagueRange;
                lookForRangeAction(by, mLookForSpecial,
                                   plaguePeriod, plagueRange,
                                   at, act, chart, s);
            } else if(type == eGodType::aphrodite) {
                auto& board = this->board();
                using eLFEG = eLookForEvictGodAct;
                const auto act = std::make_shared<eLFEG>(board);
                const auto at = eCharacterActionType::curse;
                const auto s = eGodSound::curse;
                const auto c = character();
                const auto chart = c->type();
                const int evictPeriod = eNumbers::sGodAttackAphroditeEvictPeriod;
                const int evictRange = eNumbers::sGodAttackAphroditeEvictRange;
                lookForRangeAction(by, mLookForSpecial,
                                   evictPeriod, evictRange,
                                   at, act, chart, s);
            }
        }
    }

    if(type == eGodType::atlas) {
        auto& board = this->board();
        const auto tile = c->tile();
        if(tile) {
            const auto& chars = tile->characters();
            for(const auto& cc : chars) {
                if(cc.get() == c) continue;
                const auto cType = cc->type();
                const bool r = DionysusFollowAction::sShouldFollow(cType);
                if(!r) continue;
                const auto ccaa = cc->actionType();
                if(ccaa == eCharacterActionType::die) continue;
                const auto wa = e::make_shared<eWaitAction>(cc.get());
                wa->setTime(20000);
                cc->setActionType(eCharacterActionType::stand);
                cc->setAction(wa);
                const auto killA = std::make_shared<eChar_killWithCorpseFinish>(
                                       board, cc.get(), true);
                wa->setFinishAction(killA);
                wa->setFailAction(killA);
            }
        }
    }

    eGodAction::increment(by);
}

bool eGodAttackAction::lookForRangeAction(const int dtime,
                                          int& time, const int freq,
                                          const int range,
                                          const eCharacterActionType at,
                                          const stdsptr<eGodAct>& act,
                                          const eCharacterType chart,
                                          const eGodSound missileSound,
                                          const int nMissiles) {
    (void)chart;
    const auto c = character();
    const auto cat = c->actionType();
    const bool walking = cat == eCharacterActionType::walk;
    if(!walking) return false;
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return false;
    const int tx = ct->x();
    const int ty = ct->y();

    time += dtime;
    if(time > freq) {
        time = 0;
        std::vector<eTile*> tiles;
        const int rr = 2*range + 1;
        tiles.reserve(rr*rr);
        for(int i = -range; i <= range; i++) {
            for(int j = -range; j <= range; j++) {
                const int ttx = tx + i;
                const int tty = ty + j;
                const auto t = brd.tile(ttx, tty);
                if(!t) continue;
                tiles.push_back(t);
            }
        }
        std::random_shuffle(tiles.begin(), tiles.end());
        for(const auto t : tiles) {
            const auto tt = act->find(t);
            if(!tt.target()) continue;
            pauseAction();
            double bless = 0.0;
            const auto kind = act->type();
            if(kind == eGodActType::lookForBless ||
               kind == eGodActType::lookForTargetedBless) {
                const auto bb = static_cast<eLookForBlessGodActBase*>(act.get());
                bless = bb->bless();
            }
            beginAttacking(tt, kind, at, missileSound, nMissiles, bless, mStage);
            spawnAttackMissile();
            return true;
        }
        time += freq/2;
    }
    return false;
}

bool eGodAttackAction::lookForAttack(const int dtime,
                                     int& time, const int freq,
                                     const int range) {
    const auto c = character();
    const auto act = std::make_shared<eLookForAttackGodAct>(board(), c);

    const auto at = eCharacterActionType::fight2;
    const auto s = eGodSound::attack;
    const auto chart = c->type();

    return lookForRangeAction(dtime, time, freq, range,
                              at, act, chart, s);
}

bool eGodAttackAction::lookForTargetedAttack(const int dtime,
                                             int& time, const int freq,
                                             const int range) {
    const auto c = character();
    const auto act = std::make_shared<eLookForTargetedAttackGodAct>(
                         board(), type());

    const auto at = eCharacterActionType::fight2;
    const auto s = eGodSound::attack;
    const auto chart = c->type();

    return lookForRangeAction(dtime, time, freq, range,
                              at, act, chart, s);
}

stdsptr<eObsticleHandler> eGodAttackAction::obsticleHandler() {
    return std::make_shared<eGodObsticleHandler>(board(), this);
}

void eGodAttackAction::destroyBuilding(eBuilding* const b) {
    pauseAction();
    mPreAttackStage = mStage;
    mAttackBuilding = b;
    mStage = eGodAttackStage::destroyingBuilding;
    const auto finishAttackA = std::make_shared<eGAA_destroyBuildingFinish>(
                                   board(), this, b);
    const auto playHitSound = std::make_shared<ePlayMonsterBuildingAttackSoundGodAct>(
                                  board(), b);
    const auto at = eCharacterActionType::fight2;
    const auto s = eGodSound::attack;
    const auto c = character();
    const auto chart = c->type();
    spawnGodMultipleMissiles(at, chart, b->centerTile(),
                             s, playHitSound, finishAttackA, 3);
}

void eGodAttackAction::goToTarget() {
    const auto gt = type();
    const auto hg = eHeatGetters::godLeaning(gt);
    const stdptr<eGodAction> tptr(this);
    const auto tele = std::make_shared<eTeleportFindFailFunc>(board(), this);
    eGodMonsterAction::goToTarget(hg, tele, obsticleHandler());
}

bool eGodAttackAction::decide() {
    const auto c = character();
    switch(mStage) {
    case eGodAttackStage::none:
        mStage = eGodAttackStage::appear;
        randomPlaceOnBoard();
        if(!c->tile()) {
            c->kill();
        } else {
            appear();
        }
        break;
    case eGodAttackStage::appear:
        mStage = eGodAttackStage::goTo1;
        initialize();
        goToTarget();
        break;
    case eGodAttackStage::goTo1: {
        mStage = eGodAttackStage::patrol1;
        goToNearestRoad();
    }   break;
    case eGodAttackStage::patrol1:
        mStage = eGodAttackStage::goTo2;
        goToTarget();
        break;
    case eGodAttackStage::goTo2: {
        mStage = eGodAttackStage::patrol2;
        goToNearestRoad();
    }   break;
    case eGodAttackStage::patrol2:
        mStage = eGodAttackStage::disappear;
        disappear();
        break;
    case eGodAttackStage::attacking:
        if(!mAttackTarget.target()) {
            finishAttacking();
            return decide();
        }
        spawnAttackMissile();
        break;
    case eGodAttackStage::destroyingBuilding:
        if(!mAttackBuilding) {
            finishBuildingAttack();
            return decide();
        }
        spawnDestroyBuildingMissile(mAttackBuilding.get());
        break;
    case eGodAttackStage::disappear:
        c->kill();
        break;
    }
    return true;
}

void eGodAttackAction::beginAttacking(const eMissileTarget& target,
                                      const eGodActType kind,
                                      const eCharacterActionType at,
                                      const eGodSound sound,
                                      const int nMissiles,
                                      const double bless,
                                      const eGodAttackStage prevStage) {
    mPreAttackStage = prevStage;
    mAttackTarget = target;
    mAttackKind = kind;
    mAttackActionType = at;
    mAttackSoundInt = static_cast<int>(sound);
    mAttackNMissiles = nMissiles;
    mAttackBless = bless;
    mStage = eGodAttackStage::attacking;
}

void eGodAttackAction::finishAttacking() {
    mStage = mPreAttackStage;
    mPreAttackStage = eGodAttackStage::none;
    mAttackTarget = eMissileTarget();
}

void eGodAttackAction::finishBuildingAttack() {
    mStage = mPreAttackStage;
    mPreAttackStage = eGodAttackStage::none;
    mAttackBuilding = nullptr;
}

stdsptr<eGodAct> eGodAttackAction::rebuildAttackAct() {
    const auto c = character();
    const auto team = c->teamId();
    const auto gt = type();
    auto& brd = board();
    switch(mAttackKind) {
    case eGodActType::lookForAttack:
        return std::make_shared<eLookForAttackGodAct>(brd, c);
    case eGodActType::lookForTargetedAttack:
        return std::make_shared<eLookForTargetedAttackGodAct>(brd, gt);
    case eGodActType::lookForBless:
        return std::make_shared<eLookForBlessGodAct>(brd, mAttackBless);
    case eGodActType::lookForTargetedBless:
        return std::make_shared<eLookForTargetedBlessGodAct>(brd, mAttackBless, gt);
    case eGodActType::lookForSoldierAttack:
        return std::make_shared<eLookForSoldierAttackGodAct>(brd, team);
    case eGodActType::lookForPlague:
        return std::make_shared<eLookForPlagueGodAct>(brd);
    case eGodActType::lookForEvict:
        return std::make_shared<eLookForEvictGodAct>(brd);
    default:
        return nullptr;
    }
}

void eGodAttackAction::spawnAttackMissile() {
    const auto act = rebuildAttackAct();
    if(!act) {
        finishAttacking();
        decide();
        return;
    }
    const auto c = character();
    const auto chart = c->type();
    const auto sound = static_cast<eGodSound>(mAttackSoundInt);
    const auto finishCb = std::make_shared<eGAA_rangeAttackFinish>(board(), this);
    if(mAttackNMissiles == 1) {
        spawnGodMissile(mAttackActionType, chart, mAttackTarget,
                        sound, act, finishCb);
    } else {
        spawnGodMultipleMissiles(mAttackActionType, chart, mAttackTarget,
                                 sound, act, finishCb, mAttackNMissiles);
    }
}

void eGodAttackAction::spawnDestroyBuildingMissile(eBuilding* const b) {
    const auto c = character();
    const auto chart = c->type();
    const auto finishCb = std::make_shared<eGAA_destroyBuildingFinish>(
                              board(), this, b);
    const auto playHitSound = std::make_shared<ePlayMonsterBuildingAttackSoundGodAct>(
                                  board(), b);
    spawnGodMultipleMissiles(eCharacterActionType::fight2,
                             chart, b->centerTile(),
                             eGodSound::attack, playHitSound,
                             finishCb, 3);
}

void eGodAttackAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eGodAttackAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eGodAttackStage::attacking:
        if(!mAttackTarget.target()) {
            finishAttacking();
            eGodAction::resumeFromSavedState();
            return;
        }
        spawnAttackMissile();
        return;
    case eGodAttackStage::destroyingBuilding:
        if(!mAttackBuilding) {
            finishBuildingAttack();
            eGodAction::resumeFromSavedState();
            return;
        }
        destroyBuilding(mAttackBuilding.get());
        return;
    case eGodAttackStage::none:
    case eGodAttackStage::appear:
    case eGodAttackStage::goTo1:
    case eGodAttackStage::patrol1:
    case eGodAttackStage::goTo2:
    case eGodAttackStage::patrol2:
    case eGodAttackStage::disappear:
        eGodAction::resumeFromSavedState();
        return;
    }
}

void eGodAttackAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("lookForCurse", mLookForCurse);
    ar.field("lookForTargetedCurse", mLookForTargetedCurse);
    ar.field("lookForAttack", mLookForAttack);
    ar.field("lookForTargetedAttack", mLookForTargetedAttack);
    ar.field("lookForGod", mLookForGod);
    ar.field("lookForSpecial", mLookForSpecial);
    ar.buildingAsField("sanctuary", &board(), mSanctuary);
    ar.field("preAttackStage", mPreAttackStage);
    ar.field("attackKind", mAttackKind);
    missileTargetField(ar, "attackTarget", board(), mAttackTarget);
    ar.field("attackActionType", mAttackActionType);
    ar.field("attackSoundInt", mAttackSoundInt);
    ar.field("attackNMissiles", mAttackNMissiles);
    ar.field("attackBless", mAttackBless);
    ar.buildingField("attackBuilding", &board(), mAttackBuilding);
}

void eGodAttackAction::setSanctuary(const stdptr<eSanctuary>& s) {
    mSanctuary = s;
}

void eGodAttackAction::initialize() {
    auto& board = this->board();
    const auto cid = cityId();
    const auto c = character();
    const auto tile = c->tile();
    if(!tile) return;
    const auto type = this->type();
    if(type == eGodType::dionysus) {
        eCharacter* f = c;
        DionysusFollowAction* fa = nullptr;
        for(int i = 0; i < 3; i++) {
            const auto s = e::make_shared<eSatyr>(board);
            s->changeTile(tile);
            const auto a = e::make_shared<DionysusFollowAction>(
                               f, s.get());
            s->setAction(a);
            f = s.get();
            if(fa) fa->setFollower(s.get());
            fa = a.get();
        }
    } else if(type == eGodType::hades) {
        const auto p = board.palace(cid);
        if(p) p->setBlessed(-1.);
        const auto& chars = board.characters();
        for(const auto c : chars) {
            const auto cType = c->type();
            const bool r = DionysusFollowAction::sShouldFollow(cType);
            if(!r) continue;
            if(c->dead()) continue;
            c->killWithCorpse();
        }
    }
}
