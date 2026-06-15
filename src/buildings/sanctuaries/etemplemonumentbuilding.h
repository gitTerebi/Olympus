#ifndef ETEMPLEMONUMENTBUILDING_H
#define ETEMPLEMONUMENTBUILDING_H


#include "characters/gods/god.h"

#include "esanctbuilding.h"

class eTempleMonumentBuilding : public eSanctBuilding {
public:
    eTempleMonumentBuilding(const GodType god,
                            const int id, GameBoard& board,
                            const eCityId cid);

    static const TextureCollection* sGodMonumentTextureCollection(
            const eTileSize size, const GodType god);

    std::shared_ptr<Texture> getTexture(const eTileSize) const override { return nullptr; }

    GodType godType() const { return mGod; }
    int id() const { return mId; }
private:
    const GodType mGod;
    const int mId;
};

#endif // ETEMPLEMONUMENTBUILDING_H
