#ifndef ETHREADTILE_H
#define ETHREADTILE_H

#include "ethreadbuilding.h"
#include "ethreadcharacter.h"
#include "spawners/ebanner.h"

#include "../etilebase.h"

class eThreadTile : public eTileBase {
public:
    void load(eTile* const src);

    bool hasRoad() const override;
    bool hasCharacter(const eHasChar& func) const override;
    eBuildingType underBuildingType() const override;
    const eThreadBuilding& underBuilding() const
    { return mUnderBuilding; }
    bool isUnderBuilding() const;

    int houseVacancies() const;

    bool onFire() const override { return mOnFire; }
    void setOnFire(const bool f) override { mOnFire = f; }

    eBannerTypeS bannerType() const override { return mBannerType; }
private:
    bool mOnFire = false;
    eBannerTypeS mBannerType = eBannerTypeS::none;
    std::vector<eThreadCharacter> mCharacters;
    eThreadBuilding mUnderBuilding;
};

#endif // ETHREADTILE_H
