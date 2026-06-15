#ifndef BOAR_SPAWNER_H
#define BOAR_SPAWNER_H

#include "animal-spawner.h"

class BoarSpawner : public AnimalSpawner {
public:
    BoarSpawner(const int id,
                 eTile* const tile,
                 GameBoard& board);

    stdsptr<eWildAnimal> create(GameBoard& board) override;
};

#endif // BOAR_SPAWNER_H
