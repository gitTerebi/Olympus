#include "fight-action.h"

#include "characters/echaracter.h"
#include "engine/etile.h"

#include "characters/ebasicpatroler.h"
#include "characters/efightingpatroler.h"
#include "characters/eresourcecollector.h"
#include "characters/animal.h"
#include "fileIO/esavearchive.h"
#include "enumbers.h"
#include "combat-timing.h"

#include <cstdlib>
#include <cstdio>

FightAction::FightAction(eCharacter *const c, eCharacter *const o) : eCharacterAction(c, eCharActionType::fightAction),
                                                                       mOpponent(o)
{
    c->setActionType(eCharacterActionType::fight);
}

void FightAction::increment(const int by)
{
    const auto c = character();
    if (!mOpponent)
    {
        setState(eCharacterActionState::finished);
        return;
    }
    const auto ct = c->tile();
    const auto ot = mOpponent->tile();
    if(!ct || !ot ||
       abs(ct->x() - ot->x()) > 1 ||
       abs(ct->y() - ot->y()) > 1) {
        setState(eCharacterActionState::finished);
        return;
    }
    mTime += by;
    const int cycleMs = CombatTiming::meleeCycleMs(*c);
    const int animMs = CombatTiming::meleeAnimationMs(*c);
    const auto wantedAction = mTime + animMs >= cycleMs ?
                              eCharacterActionType::fight :
                              eCharacterActionType::stand;
    if(c->actionType() != wantedAction) c->setActionType(wantedAction);
    if (mTime < cycleMs)
        return;
    mTime -= cycleMs;
    if(c->actionType() != eCharacterActionType::fight) {
        c->setActionType(eCharacterActionType::fight);
    }
    const double a = c->attack();
    const double arm = mOpponent->armor();
    const double dmg = a - arm;
    const double finalDmg = dmg > 0 ? dmg : 0.;
    const bool dead = mOpponent->takeMeleeDamage(finalDmg, c);
    if (dead || c->dead())
        setState(eCharacterActionState::finished);
}

void FightAction::serializeFields(eSaveArchive &ar)
{
    eCharacterAction::serializeFields(ar);
    ar.characterField("opponent", &board(), mOpponent);
    ar.field("time", mTime);
}

void FightAction::resumeFromSavedState()
{
    const auto c = character();
    if (!mOpponent || mOpponent->dead() || c->dead())
    {
        setState(eCharacterActionState::finished);
        return;
    }
    c->setActionType(eCharacterActionType::fight);
}
