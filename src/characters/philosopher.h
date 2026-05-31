#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include "ebasicpatroler.h"

class Philosopher : public eBasicPatroler {
public:
    Philosopher(GameBoard& board);
};

#endif // PHILOSOPHER_H
