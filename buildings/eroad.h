#ifndef EROAD_H
#define EROAD_H

#include "ebuilding.h"

class eGatehouse;
class eAgoraBase;
class eHippodromePiece;

class eRoad : public eBuilding {
public:
    eRoad(eGameBoard& board, const eCityId cid);

    void erase();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    std::shared_ptr<eTexture> getHippodromeTexture(const eTileSize size) const;

    void setUnderAgora(eAgoraBase* const a);
    eAgoraBase* underAgora() const { return mUnderAgora; }

    void setUnderGatehouse(eGatehouse* const g);
    eGatehouse* underGatehouse() const { return mUnderGatehouse; }

    void setAboveHippodrome(eHippodromePiece* const h);
    eHippodromePiece* aboveHippodrome() const { return mAboveHippodrome; }

    bool isBridge() const;
    bool isRoadblock() const;
    void setRoadblock(const bool rb);

    void bridgeConnectedTiles(std::vector<eTile*>& tiles) const;
private:
    eGatehouse* mUnderGatehouse = nullptr;
    eAgoraBase* mUnderAgora = nullptr;
    eHippodromePiece* mAboveHippodrome = nullptr;
    bool mRoadblock = false;
};

#endif // EROAD_H
