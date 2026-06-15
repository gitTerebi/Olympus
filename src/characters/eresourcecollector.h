#ifndef ERESOURCECOLLECTOR_H
#define ERESOURCECOLLECTOR_H

#include "eresourcecollectorbase.h"

#include "textures/character-textures.h"

class eResourceCollector : public eResourceCollectorBase {
public:
    using eCharTexs = ResourceCollectorTextures CharacterTextures::*;
    eResourceCollector(GameBoard& board, const eCharTexs charTexs,
                       const eCharacterType type);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
protected:
    void setCharTexs(const eCharTexs& texs);
private:
    const std::vector<CharacterTextures>& mTextures;
    eCharTexs mCharTexs;
};

#endif // ERESOURCECOLLECTOR_H
