#ifndef INVASION_GENERAL_H
#define INVASION_GENERAL_H

#include <vector>

#include "engine/ecityid.h"
#include "invasion-targeting.h"

class GameBoard;
class SoldierBanner;
class eTile;

// Campaign phases owned by the general. State lives on the handler (so it gets
// serialized there); the general is stateless and just drives transitions on a
// state struct passed by reference.
enum class eGeneralPhase {
    spread = 0,
    wait = 1,
    march = 2,
    invade = 3,
    done = 4,
    defend = 5
};

struct eGeneralState {
    eGeneralPhase fPhase = eGeneralPhase::spread;
    eGeneralPhase fPhaseBeforeDefend = eGeneralPhase::spread;
    eTile* fTargetTile = nullptr;   // building the general is closing on
    eTile* fCurrentTile = nullptr;  // logical position used for pathing/repick
    eTile* fMoveFrom = nullptr;     // where the last order started (debug FROM)
    eTile* fMoveTo = nullptr;       // where the last order sends them (debug GOAL)
    int fWait = 0;                  // 3000ms cycle gate
    int fSpawnWait = 0;             // 14-day pre-invade countdown
    int fMoveWait = 0;              // 7-day pause after a repositioning move
    int fDefendHold = 0;            // minimum quiet time before leaving defense
    int fDefendEnemyWait = 0;       // hard cap on "enemy still near" stall after hold drains
    int fRepinWait = 0;             // countdown to re-issue pinOnTarget during invade hold
};

class InvasionGeneral {
public:
    InvasionGeneral(GameBoard& board,
                    const eCityId targetCity,
                    const eCityId invadingCity,
                    const InvasionAttackType attackType);

    // Drives spread -> wait/march -> invade across all objectives. Returns true
    // when the campaign is complete (no valid targets left), so the handler can
    // issue comeback. State is read/written through s; the general holds none.
    bool advance(eGeneralState& s,
                 eTile* const landingTile,
                 const std::vector<SoldierBanner*>& banners,
                 const int by) const;

private:
    eTile* chooseTargetTile(const int fromX, const int fromY) const;
    // The single target-picker ("one cook"): keeps a valid target, or picks a
    // fresh one from where the formation currently is. Stale targets are
    // dropped at the top of advance(), so a null here means "needs a target".
    eTile* ensureTarget(eGeneralState& s, eTile* const landingTile) const;
    eTile* moveHalfwayToTarget(eGeneralState& s,
                               eTile* const from,
                               eTile* const target,
                               const std::vector<SoldierBanner*>& banners) const;
    void moveToTarget(eGeneralState& s,
                      eTile* const from,
                      eTile* const target,
                      const std::vector<SoldierBanner*>& banners) const;
    // Shoves the banner nearest the target ON the building tile so its soldiers
    // bump and raze it. Returns true once a banner is moved.
    bool pinOnTarget(eGeneralState& s,
                     const std::vector<SoldierBanner*>& banners) const;
    // True while fTargetTile still holds an attackable building of the target
    // city. False once it is destroyed/disabled, so the general picks the next.
    bool generalTargetValid(const eGeneralState& s) const;

    GameBoard& mBoard;
    eCityId mTargetCity;
    eCityId mInvadingCity;
    InvasionAttackType mAttackType;
};

#endif // INVASION_GENERAL_H
