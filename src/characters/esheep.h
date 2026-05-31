#ifndef ESHEEP_H
#define ESHEEP_H

#include "edomesticatedanimal.h"

class eSheep : public eDomesticatedAnimal {
public:
    eSheep(GameBoard& board);

private:
    void setNakedTexture();
    void setFleecedTexture();
};

#endif // ESHEEP_H
