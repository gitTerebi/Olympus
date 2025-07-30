#ifndef EWOLFSPAWNER_H
#define EWOLFSPAWNER_H

#include "eanimalspawner.h"

class eWolfSpawner : public eAnimalSpawner {
public:
    eWolfSpawner(const int id, eTile* const tile,
                 eGameBoard& board);

    stdsptr<eWildAnimal> create(eGameBoard& board) override;
};

#endif // EWOLFSPAWNER_H
