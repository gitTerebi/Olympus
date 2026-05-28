#ifndef EARESWARRIOR_H
#define EARESWARRIOR_H

#include "esoldier.h"

class eAresWarrior : public eSoldier {
public:
    eAresWarrior(GameBoard& board);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
};

#endif // EARESWARRIOR_H
