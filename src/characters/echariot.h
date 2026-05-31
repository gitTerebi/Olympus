#ifndef ECHARIOT_H
#define ECHARIOT_H

#include "echaracter.h"

class eChariot : public eCharacter {
public:
    eChariot(GameBoard& board);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;
};

#endif // ECHARIOT_H
