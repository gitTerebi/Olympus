#ifndef ESHEPHERDBASE_H
#define ESHEPHERDBASE_H

#include "eresourcecollectorbase.h"

#include "textures/character-textures.h"

class eShepherdBase : public eResourceCollectorBase {
public:
    using eCharTexs = ShepherdTextures CharacterTextures::*;
    eShepherdBase(GameBoard& board, const eCharTexs charTexs,
                  const eCharacterType type);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    const std::vector<CharacterTextures>& mTextures;
    eCharTexs mCharTexs;
};

#endif // ESHEPHERDBASE_H
