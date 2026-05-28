#ifndef EWOLF_H
#define EWOLF_H

#include "ewildanimal.h"

class eWolf : public eWildAnimal {
public:
    eWolf(GameBoard& board);

    bool takeDamage(const double a, eCharacter* const attacker) override;
};

#endif // EWOLF_H
