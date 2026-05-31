#include "combat-timing.h"

#include "characters/echaracter.h"

namespace {
int sMeleeAnimationMs(const eCharacterType type) {
    switch(type) {
    case eCharacterType::rockThrower:
    case eCharacterType::greekRockThrower:
    case eCharacterType::hoplite:
    case eCharacterType::hoplitePoseidon:
    case eCharacterType::greekHoplite:
    case eCharacterType::watchman:
        return 160;
    case eCharacterType::horseman:
    case eCharacterType::greekHorseman:
    case eCharacterType::chariot:
    case eCharacterType::chariotPoseidon:
        return 240;
    case eCharacterType::trireme:
        return 240;
    case eCharacterType::hydra:
        return 460;
    case eCharacterType::cerberus:
        return 260;
    case eCharacterType::cyclops:
        return 340;
    case eCharacterType::dragon:
        return 360;
    case eCharacterType::hector:
        return 320;
    case eCharacterType::kraken:
        return 720;
    case eCharacterType::maenads:
        return 420;
    case eCharacterType::minotaur:
        return 400;
    case eCharacterType::scylla:
        return 600;
    case eCharacterType::sphinx:
        return 520;
    case eCharacterType::talos:
        return 300;
    case eCharacterType::satyr:
        return 320;
    default:
        return 200;
    }
}
}

int CombatTiming::meleeCycleMs(const eCharacter& c) {
    const int freq = c.attackFreq();
    if(freq > 0) return freq * 10;
    return sMeleeAnimationMs(c.type());
}

int CombatTiming::meleeAnimationMs(const eCharacter& c) {
    return sMeleeAnimationMs(c.type());
}
