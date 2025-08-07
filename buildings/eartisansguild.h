#ifndef EARTISANSGUILD_H
#define EARTISANSGUILD_H

#include "eemployingbuilding.h"
#include "characters/eartisan.h"
#include "enumbers.h"

class eArtisansGuild : public eEmployingBuilding {
public:
    eArtisansGuild(eGameBoard& board, const eCityId cid);
    ~eArtisansGuild();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    using eArtisanPtr = stdptr<eArtisan> eArtisansGuild::*;
    bool spawnArtisan(const eArtisanPtr artisan);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    stdptr<eArtisan> mArtisan;

    const int mWaitTime = eNumbers::sArtisanWaitTime;
    int mSpawnTime = mWaitTime;
};

#endif // EARTISANSGUILD_H
