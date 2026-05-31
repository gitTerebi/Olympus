#ifndef ETEMPLEALTARBUILDING_H
#define ETEMPLEALTARBUILDING_H

#include "esanctbuilding.h"

class eSaveArchive;

enum class eSacrifice {
    none,
    sheep,
    goods,
    bull
};

class eTempleAltarBuilding : public eSanctBuilding {
public:
    eTempleAltarBuilding(GameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

    std::vector<eOverlay>
    getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void startSacrifice(const eSacrifice s);
    bool sacrificing() const { return mSacrifice != eSacrifice::none; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eSacrifice mSacrifice = eSacrifice::none;
    int mSacrificeTime = 600000;
};

#endif // ETEMPLEALTARBUILDING_H
