#ifndef WOLF_SPAWNER_H
#define WOLF_SPAWNER_H

#include "animal-spawner.h"

class WolfSpawner : public AnimalSpawner {
public:
    WolfSpawner(const int id, eTile* const tile,
                 GameBoard& board);

    stdsptr<eWildAnimal> create(GameBoard& board) override;
    void incTime(const int by) override;
    void spawnMax() override;
};

#endif // WOLF_SPAWNER_H
