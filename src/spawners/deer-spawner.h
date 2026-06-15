#ifndef DEER_SPAWNER_H
#define DEER_SPAWNER_H

#include "animal-spawner.h"

class DeerSpawner : public AnimalSpawner {
public:
    DeerSpawner(const int id, eTile* const tile,
                 GameBoard& board);

    stdsptr<eWildAnimal> create(GameBoard& board) override;
};

#endif // DEER_SPAWNER_H
