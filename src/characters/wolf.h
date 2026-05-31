#ifndef WOLF_H
#define WOLF_H

#include "ewildanimal.h"

class Wolf : public eWildAnimal {
public:
    Wolf(GameBoard& board);

    bool takeDamage(const double a, eCharacter* const attacker) override;
};

#endif // WOLF_H
