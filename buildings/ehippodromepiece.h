#ifndef EHIPPODROMEPIECE_H
#define EHIPPODROMEPIECE_H

#include "ebuilding.h"
#include "missiles/emissile.h"

class eHippodrome;

class eHippodromePiece : public eBuilding {
public:
    eHippodromePiece(eGameBoard& board, const eCityId cid);
    ~eHippodromePiece();

    struct eNeighbour {
        eHippodromePiece* fPtr = nullptr;
        eDiagonalOrientation fO;
    };

    using eN = eNeighbour;
    std::vector<eN> neighbours() const;

    eHippodrome* hippodrome() const { return mHippodrome; }
    void setHippodrome(eHippodrome* const h) { mHippodrome = h; }

    void progressPath(std::vector<ePathPoint>& path) const;

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;
    std::vector<eOverlay>
    getOverlays(const eTileSize size) const override;

    int id() const { return mId; }
    void setId(const int i) { mId = i % 8; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
private:
    void crossTile(eTile* const t,
                   std::vector<eOverlay>& result,
                   const eWorldDirection dir,
                   const eTileSize size,
                   const SDL_Rect& rr,
                   const bool back) const;
    void horseTile(eTile* const t,
                   std::vector<eOverlay>& result,
                   const eWorldDirection dir,
                   const eTileSize size,
                   const SDL_Rect& rr) const;

    int mId = 0;
    eHippodrome* mHippodrome = nullptr;
};

#endif // EHIPPODROMEPIECE_H
