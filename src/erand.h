#ifndef ERAND_H
#define ERAND_H

#include <random>

class eRand {
public:
    static int rand();
    static int rollDice(const int nDice, const int faces);
    static double combatChance(const int strA, const int strB, const int nRolls = 2);
    static int combatChancePercent(const int strA, const int strB, const int nRolls = 2);
    static bool combatRoll(const int strA, const int strB, const int nRolls = 2);
private:
    static std::random_device sDev;
    static std::mt19937 sRng;
    static std::uniform_int_distribution<int> sDist;
};

#endif // ERAND_H
