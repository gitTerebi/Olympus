#ifndef EHIPPODROMEPIECE_H
#define EHIPPODROMEPIECE_H

#include "ebuilding.h"
#include "missiles/missile.h"

class eHippodrome;
class SaveArchive;

class eHippodromePiece : public eBuildingWithResource {
public:
    eHippodromePiece(GameBoard& board, const eCityId cid);
    ~eHippodromePiece();

    bool spawnsCartWalkers() const override { return true; }

    void timeChanged(const int by) override;

    int add(const eResourceType type, const int count) override;
    int take(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    struct eNeighbour {
        eHippodromePiece* fPtr = nullptr;
        eDiagonalOrientation fO;
    };

    using eN = eNeighbour;
    std::vector<eN> neighbours() const;

    eHippodrome* hippodrome() const { return mHippodrome; }
    void setHippodrome(eHippodrome* const h);

    void progressPath(std::vector<PathPoint>& path) const;

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const override;
    std::vector<Overlay>
    getOverlays(const eTileSize size) const override;

    int id() const { return mId; }
    void setId(const int i) { mId = i % 8; }

    void setPartId(const int i) { mPartId = i; }

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    void crossTile(eTile* const t,
                   std::vector<Overlay>& result,
                   const eWorldDirection dir,
                   const eTileSize size,
                   const SDL_Rect& rr,
                   const bool back) const;
    void horseTile(eTile* const t,
                   std::vector<Overlay>& result,
                   const eWorldDirection dir,
                   const eTileSize size,
                   const SDL_Rect& rr) const;

    int mPartId = 0;
    int mCleaningTimeShift = 0;

    int mId = 0;
    eHippodrome* mHippodrome = nullptr;
    stdptr<eCartTransporter> mCart;
};

#endif // EHIPPODROMEPIECE_H
