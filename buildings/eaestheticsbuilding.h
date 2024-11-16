#ifndef EAESTHETICSBUILDING_H
#define EAESTHETICSBUILDING_H

#include "ebuilding.h"

#include "textures/ebuildingtextures.h"
#include "characters/gods/egod.h"

class eAestheticsBuilding : public eBuilding {
public:
    using eBaseTex = std::shared_ptr<eTexture> eBuildingTextures::*;
    eAestheticsBuilding(eGameBoard& board,
                        const eBaseTex baseTex,
                        const eBuildingType type,
                        const int sw, const int sh,
                        const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    const eBaseTex mTexture;
};

class eOverlayAesthBuilding : public eAestheticsBuilding {
public:
    using eBaseTex = std::shared_ptr<eTexture> eBuildingTextures::*;
    using eOverlays = eTextureCollection eBuildingTextures::*;
    eOverlayAesthBuilding(eGameBoard& board,
                          const eBaseTex baseTex,
                          const double overlayX,
                          const double overlayY,
                          const eOverlays overlays,
                          const eBuildingType type,
                          const int sw, const int sh,
                          const eCityId cid);

    std::vector<eOverlay> getOverlays(const eTileSize size) const;
private:
    const eOverlays mOverlays;

    const double mOverlayX;
    const double mOverlayY;
};

class eBench : public eAestheticsBuilding {
public:
    eBench(eGameBoard& board, const eCityId cid);
};

class eFlowerGarden : public eAestheticsBuilding {
public:
    eFlowerGarden(eGameBoard& board, const eCityId cid);
};

class eGazebo : public eAestheticsBuilding {
public:
    eGazebo(eGameBoard& board, const eCityId cid);
};

class eHedgeMaze : public eAestheticsBuilding {
public:
    eHedgeMaze(eGameBoard& board, const eCityId cid);
};

class eFishPond : public eOverlayAesthBuilding {
public:
    eFishPond(eGameBoard& board, const eCityId cid);
};

class eBirdBath : public eOverlayAesthBuilding {
public:
    eBirdBath(eGameBoard& board, const eCityId cid);
};

class eShortObelisk : public eAestheticsBuilding {
public:
    eShortObelisk(eGameBoard& board, const eCityId cid);
};

class eTallObelisk : public eAestheticsBuilding {
public:
    eTallObelisk(eGameBoard& board, const eCityId cid);
};

class eOrrery : public eAestheticsBuilding {
public:
    eOrrery(eGameBoard& board, const eCityId cid);
};

class eShellGarden : public eAestheticsBuilding {
public:
    eShellGarden(eGameBoard& board, const eCityId cid);
};

class eSundial : public eAestheticsBuilding {
public:
    eSundial(eGameBoard& board, const eCityId cid);
};

class eDolphinSculpture : public eAestheticsBuilding {
public:
    eDolphinSculpture(eGameBoard& board, const eCityId cid);
};

class eSpring : public eOverlayAesthBuilding {
public:
    eSpring(eGameBoard& board, const eCityId cid);
};

class eTopiary : public eAestheticsBuilding {
public:
    eTopiary(eGameBoard& board, const eCityId cid);
};

class eBaths : public eOverlayAesthBuilding {
public:
    eBaths(eGameBoard& board, const eCityId cid);
};

class eStoneCircle : public eOverlayAesthBuilding {
public:
    eStoneCircle(eGameBoard& board, const eCityId cid);
};

class eWaterPark : public eBuilding {
public:
    eWaterPark(eGameBoard& board, const eCityId cid);

    int id() const { return mId; }
    void setId(const int i);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
    std::vector<eOverlay> getOverlays(const eTileSize size) const;
private:
    int mId = 0;
};

class eCommemorative : public eBuilding {
public:
    eCommemorative(const int id, eGameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    int id() const { return mId; }
private:
    const int mId = 0;
};

class eGodMonumentTile;

class eGodMonument : public eBuilding {
public:
    eGodMonument(const eGodType god,
                 const eGodQuestId id,
                 eGameBoard& board, const eCityId cid);

    void erase() override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    eGodType god() const { return mGod; }
    eGodQuestId id() const { return mId; }
    void addTile(eGodMonumentTile* const tile);
    const std::vector<eGodMonumentTile*>& tiles() const { return mTiles; }
private:
    const eGodType mGod;
    const eGodQuestId mId;
    std::vector<eGodMonumentTile*> mTiles;
};

class eGodMonumentTile : public eBuilding {
public:
    eGodMonumentTile(eGameBoard& board, const eCityId cid);

    void erase();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    void setMonument(eGodMonument* const mon);
    eGodMonument* monument() const { return mMonument; }
private:
    eGodMonument* mMonument = nullptr;
};

#endif // EAESTHETICSBUILDING_H
