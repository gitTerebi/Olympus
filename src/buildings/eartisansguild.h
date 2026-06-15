#ifndef EARTISANSGUILD_H
#define EARTISANSGUILD_H

#include "eemployingbuilding.h"
#include "characters/eartisan.h"
#include "numbers.h"

class SaveArchive;

class eArtisansGuild : public eEmployingBuilding {
public:
    eArtisansGuild(GameBoard& board, const eCityId cid);
    ~eArtisansGuild();

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    using eArtisanPtr = stdptr<eArtisan> eArtisansGuild::*;
    bool spawnArtisan(const eArtisanPtr artisan);

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<eArtisan> mArtisan;

    const int mWaitTime = Numbers::sArtisanWaitTime;
    double mSpawnTime = mWaitTime;
};

#endif // EARTISANSGUILD_H
