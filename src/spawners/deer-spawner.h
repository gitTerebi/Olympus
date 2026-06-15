#ifndef EDEERSPAWNER_H
#define EDEERSPAWNER_H

#include "eanimalspawner.h"

class eDeerSpawner : public eAnimalSpawner {
public:
    eDeerSpawner(const int id, eTile* const tile,
                 GameBoard& board);

    stdsptr<eWildAnimal> create(GameBoard& board) override;
};

#endif // EDEERSPAWNER_H
