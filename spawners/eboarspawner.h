#ifndef EBOARSPAWNER_H
#define EBOARSPAWNER_H

#include "eanimalspawner.h"

class eBoarSpawner : public eAnimalSpawner {
public:
    eBoarSpawner(const int id,
                 eTile* const tile,
                 GameBoard& board);

    stdsptr<eWildAnimal> create(GameBoard& board) override;
};

#endif // EBOARSPAWNER_H
