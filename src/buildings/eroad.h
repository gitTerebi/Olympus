#ifndef EROAD_H
#define EROAD_H

#include "ebuilding.h"

class eGatehouse;
class eAgoraBase;
class eHippodromePiece;
class SaveArchive;

class eRoad : public eBuilding {
public:
    eRoad(GameBoard& board, const eCityId cid);

    void erase() override;

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const override;

    int getHippodromeTextureId() const;

    void setUnderAgora(eAgoraBase* const a);
    eAgoraBase* underAgora() const { return mUnderAgora; }

    void setUnderGatehouse(eGatehouse* const g);
    eGatehouse* underGatehouse() const { return mUnderGatehouse; }

    void setAboveHippodrome(eHippodromePiece* const h);
    eHippodromePiece* aboveHippodrome() const { return mAboveHippodrome; }

    bool isBridge() const;
    bool isRoadblock() const;
    void setRoadblock(const bool rb);

    void setCharacterAltitude(const char a) { mCharacterAltitude = a; }
    char characterAltitude() const { return mCharacterAltitude; }

    void bridgeConnectedTiles(std::vector<eTile*>& tiles) const;

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    eGatehouse* mUnderGatehouse = nullptr;
    eAgoraBase* mUnderAgora = nullptr;
    eHippodromePiece* mAboveHippodrome = nullptr;
    bool mRoadblock = false;
    char mCharacterAltitude = 0;
};

#endif // EROAD_H
