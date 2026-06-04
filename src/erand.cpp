#include "erand.h"

#include "elimits.h"

#include <algorithm>
#include <cmath>

std::random_device eRand::sDev;
std::mt19937 eRand::sRng(sDev());
std::uniform_int_distribution<int> eRand::sDist(0, __INT_MAX__);

int eRand::rand() {
    return sDist(sRng);
}

int eRand::rollDice(const int nDice, const int faces) {
    if(faces <= 0) return 0;
    int total = 0;
    for(int i = 0; i < nDice; i++) total += 1 + (std::abs(rand()) % faces);
    return total;
}

double eRand::combatChance(const int strA, const int strB, const int nRolls) {
    if(strA <= 0) return 0;
    if(strB <= 0) return 1;
    const double baseChance = std::min(1., double(strA)/strB);
    double loseChance = 1.;
    for(int i = 0; i < nRolls; i++) {
        loseChance *= 1. - baseChance;
    }
    return 1. - loseChance;
}

int eRand::combatChancePercent(const int strA, const int strB, const int nRolls) {
    return std::round(100*combatChance(strA, strB, nRolls));
}

bool eRand::combatRoll(const int strA, const int strB, const int nRolls) {
    if(strA <= 0) return false;
    if(strB <= 0) return true;
    const int chance = std::min(strA, strB);
    for(int i = 0; i < nRolls; i++) {
        if(std::abs(rand()) % strB < chance) return true;
    }
    return false;
}
