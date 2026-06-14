#include "god-action.h"

#include "engine/game-board.h"
#include "audio/sounds.h"
#include "etilehelper.h"

#include "characters/actions/ewaitaction.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/ekillcharacterfinishfail.h"
#include "destruction-puff.h"
#include "erand.h"
#include "enumbers.h"

#include <cmath>

eGodAction::eGodAction(eCharacter* const c,
                       const eCharActionType type) :
    GodMonsterAction(c, type),
    mType(God::sCharacterToGodType(c->type())) {}

bool eGodAction::lookForBlessCurse(
        const int dtime,
        int& time, const int freq,
        const int range,
        const double bless) {
    using eLFBG = eLookForBlessGodAct;
    const auto act = std::make_shared<eLFBG>(
                         board(), bless);
    eCharacterActionType at;
    eGodSound s;
    if(bless > 0) {
        at = eCharacterActionType::bless;
        s = eGodSound::santcify;
    } else {
        at = eCharacterActionType::curse;
        s = eGodSound::curse;
    }
    const auto c = character();
    const auto chart = c->type();
    return lookForRangeAction(dtime, time, freq, range,
                              at, act, chart, s);
}

bool eGodAction::lookForTargetedBlessCurse(
        const int dtime, int& time,
        const int freq, const int range,
        const double bless) {
    using eLFBG = eLookForTargetedBlessGodAct;
    const auto act = std::make_shared<eLFBG>(
                         board(), bless, type());
    eCharacterActionType at;
    eGodSound s;
    if(bless > 0) {
        at = eCharacterActionType::bless;
        s = eGodSound::santcify;
    } else {
        at = eCharacterActionType::curse;
        s = eGodSound::curse;
    }
    const auto c = character();
    const auto chart = c->type();
    return lookForRangeAction(dtime, time, freq, range,
                              at, act, chart, s);
}

bool eGodAction::lookForSoldierAttack(
        const int dtime, int& time,
        const int freq, const int range) {
    const auto c = character();
    const auto team = c->teamId();

    using eLFSAGA = eLookForSoldierAttackGodAct;
    const auto act = std::make_shared<eLFSAGA>(board(), team);

    const auto at = eCharacterActionType::fight2;
    const auto s = eGodSound::attack;
    const auto chart = c->type();
    return lookForRangeAction(dtime, time, freq, range,
                              at, act, chart, s, 4);
}

bool eGodAction::lookForRangeAction(
        const int dtime,
        int& time, const int freq,
        const int range,
        const eCharacterActionType at,
        const stdsptr<eGodAct>& act,
        const eCharacterType chart,
        const eGodSound missileSound,
        const int nMissiles) {
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

            using eGA_LFRAF = eGA_lookForRangeActionFinish;
            const auto finishAttackA =
                    std::make_shared<eGA_LFRAF>(
                        board(), this);

            pauseAction();
            if(nMissiles == 1) {
                spawnGodMissile(at, chart, tt, missileSound,
                                act, finishAttackA);
            } else {
                spawnGodMultipleMissiles(at, chart, tt, missileSound,
                                         act, finishAttackA, nMissiles);
            }
            return true;
        }
        time += freq/2;
    }
    return false;
}

void eGodAction::spawnGodMissile(
        const eCharacterActionType at,
        const eCharacterType chart,
        const eMissileTarget& target,
        const eGodSound sound,
        const stdsptr<eGodAct>& act,
        const stdsptr<eCharActFunc>& finishAttackA) {
    const int time = at == eCharacterActionType::bless ?
                         God::sGodBlessTime(type()) :
                         God::sGodAttackTime(type());
    const auto c = character();
    using eGA_SGMPS = eGA_spawnGodMissilePlaySound;
    const auto playSound = std::make_shared<eGA_SGMPS>(
                               board(), sound, c);
    spawnMissile(at, chart, time, target,
                 playSound, act, finishAttackA);
}

void eGodAction::goBackToSanctuary() {
    auto& board = eGodAction::board();
    const auto s = board.sanctuary(cityId(), type());
    if(!s) return;
    const auto c = character();
    const auto god = static_cast<God*>(c);
    const stdptr<God> cptr(god);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
                          board, god);
    const auto finish = std::make_shared<eKillCharacterFinishFail>(
                            board, god);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setFindFailAction([cptr]() {
        if(cptr) cptr->kill();
    });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);
    a->start(s);
}

void eGodAction::goToTarget() {
    const auto gt = type();
    const auto hg = eHeatGetters::godLeaning(gt);
    using eGTTT = eGoToTargetTeleport;
    const auto tele = std::make_shared<eGTTT>(board(), this);
    GodMonsterAction::goToTarget(hg, tele);
}

void eSpawnImpactPuffsGodAct::act() {
    auto& brd = board();
    const int groupCounts[4] = {
        2 + (std::abs(eRand::rand()) % 3),
        2 + (std::abs(eRand::rand()) % 3),
        4 + (std::abs(eRand::rand()) % 5),
        4 + (std::abs(eRand::rand()) % 5)
    };
    // spawn groups 3+4 first (draw underneath), then 1+2 on top
    const int spawnOrder[4] = {2, 3, 0, 1};
    for(const int group : spawnOrder) {
        for(int j = 0; j < groupCounts[group]; j++) {
            const double angle = (std::abs(eRand::rand()) % 360) * (M_PI / 180.0);
            const int stepCount = std::abs(eRand::rand()) % 11;
            const double dirX = std::cos(angle);
            const double dirY = std::sin(angle);
            new DestructionPuff(brd, mImpactX, mImpactY, dirX, dirY, stepCount, group);
        }
    }
    if(mWrappedAct) mWrappedAct->act();
}
