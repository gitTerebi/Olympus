#ifndef ANIMAL_SPAWNER_H
#define ANIMAL_SPAWNER_H

#include "spawner.h"

#include "pointers/estdselfref.h"

class eWildAnimal;

class AnimalSpawner : public Spawner {
public:
    using Spawner::Spawner;

    virtual stdsptr<eWildAnimal> create(GameBoard& board) = 0;

    void spawn(eTile* const tile) override;
};

#endif // ANIMAL_SPAWNER_H
