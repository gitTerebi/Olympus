#include "rand.h"

#include "game-limits.h"

#include <algorithm>
#include <cmath>

std::random_device Rand::sDev;
std::mt19937 Rand::sRng(sDev());
std::uniform_int_distribution<int> Rand::sDist(0, __INT_MAX__);

int Rand::rand() {
    return sDist(sRng);
}

int Rand::rollDice(const int nDice, const int faces) {
    if(faces <= 0) return 0;
    int total = 0;
    for(int i = 0; i < nDice; i++) total += 1 + (std::abs(rand()) % faces);
    return total;
}

double Rand::combatChance(const int strA, const int strB, const int nRolls) {
    if(strA <= 0) return 0;
    if(strB <= 0) return 1;
    const double baseChance = std::min(1., double(strA)/strB);
    double loseChance = 1.;
    for(int i = 0; i < nRolls; i++) {
        loseChance *= 1. - baseChance;
    }
    return 1. - loseChance;
}

int Rand::combatChancePercent(const int strA, const int strB, const int nRolls) {
    return std::round(100*combatChance(strA, strB, nRolls));
}

bool Rand::combatRoll(const int strA, const int strB, const int nRolls) {
    if(strA <= 0) return false;
    if(strB <= 0) return true;
    const int chance = std::min(strA, strB);
    for(int i = 0; i < nRolls; i++) {
        if(std::abs(rand()) % strB < chance) return true;
    }
    return false;
}
