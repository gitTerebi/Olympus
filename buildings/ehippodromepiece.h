#ifndef EHIPPODROMEPIECE_H
#define EHIPPODROMEPIECE_H

#include "ebuilding.h"

class eHippodromePiece : public eBuilding {
public:
    eHippodromePiece(eGameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;
    std::vector<eOverlay>
    getOverlays(const eTileSize size) const override;

    int id() const { return mId; }
    void setId(const int i) { mId = i % 8; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
private:
    int mId = 0;
};

#endif // EHIPPODROMEPIECE_H
