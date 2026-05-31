#ifndef EARTISANSGUILD_H
#define EARTISANSGUILD_H

#include "eemployingbuilding.h"
#include "characters/eartisan.h"
#include "enumbers.h"

class eSaveArchive;

class eArtisansGuild : public eEmployingBuilding {
public:
    eArtisansGuild(GameBoard& board, const eCityId cid);
    ~eArtisansGuild();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    using eArtisanPtr = stdptr<eArtisan> eArtisansGuild::*;
    bool spawnArtisan(const eArtisanPtr artisan);

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdptr<eArtisan> mArtisan;

    const int mWaitTime = eNumbers::sArtisanWaitTime;
    double mSpawnTime = mWaitTime;
};

#endif // EARTISANSGUILD_H
