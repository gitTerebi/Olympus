#ifndef COMBAT_TIMING_H
#define COMBAT_TIMING_H

class eCharacter;

namespace CombatTiming {

int meleeCycleMs(const eCharacter& c);
int meleeAnimationMs(const eCharacter& c);

}

#endif
