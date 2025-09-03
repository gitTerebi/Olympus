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
    void handleTile(eTile* const t,
                    std::vector<eOverlay>& result,
                    const eWorldDirection dir,
                    const eTileSize size,
                    const SDL_Rect& rr) const;

    int mId = 0;
};

#endif // EHIPPODROMEPIECE_H
