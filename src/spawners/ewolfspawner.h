#ifndef EWOLFSPAWNER_H
#define EWOLFSPAWNER_H

#include "eanimalspawner.h"

class eWolfSpawner : public eAnimalSpawner {
public:
    eWolfSpawner(const int id, eTile* const tile,
                 GameBoard& board);

    stdsptr<eWildAnimal> create(GameBoard& board) override;
    void incTime(const int by) override;
    void spawnMax() override;
};

#endif // EWOLFSPAWNER_H
