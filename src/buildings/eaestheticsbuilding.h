#ifndef EAESTHETICSBUILDING_H
#define EAESTHETICSBUILDING_H

#include "ebuilding.h"

#include "textures/building-textures.h"
#include "characters/gods/god.h"

class eAestheticsBuilding : public eBuilding {
public:
    using eBaseTex = std::shared_ptr<eTexture> BuildingTextures::*;
    eAestheticsBuilding(GameBoard& board,
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
    using eBaseTex = std::shared_ptr<eTexture> BuildingTextures::*;
    using eOverlays = eTextureCollection BuildingTextures::*;
    eOverlayAesthBuilding(GameBoard& board,
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
    eBench(GameBoard& board, const eCityId cid);
};

class eFlowerGarden : public eAestheticsBuilding {
public:
    eFlowerGarden(GameBoard& board, const eCityId cid);
};

class eGazebo : public eAestheticsBuilding {
public:
    eGazebo(GameBoard& board, const eCityId cid);
};

class eHedgeMaze : public eAestheticsBuilding {
public:
    eHedgeMaze(GameBoard& board, const eCityId cid);
};

class eFishPond : public eOverlayAesthBuilding {
public:
    eFishPond(GameBoard& board, const eCityId cid);
};

class eBirdBath : public eOverlayAesthBuilding {
public:
    eBirdBath(GameBoard& board, const eCityId cid);
};

class eShortObelisk : public eAestheticsBuilding {
public:
    eShortObelisk(GameBoard& board, const eCityId cid);
};

class eTallObelisk : public eAestheticsBuilding {
public:
    eTallObelisk(GameBoard& board, const eCityId cid);
};

class eOrrery : public eAestheticsBuilding {
public:
    eOrrery(GameBoard& board, const eCityId cid);
};

class eShellGarden : public eAestheticsBuilding {
public:
    eShellGarden(GameBoard& board, const eCityId cid);
};

class eSundial : public eAestheticsBuilding {
public:
    eSundial(GameBoard& board, const eCityId cid);
};

class eDolphinSculpture : public eAestheticsBuilding {
public:
    eDolphinSculpture(GameBoard& board, const eCityId cid);
};

class eSpring : public eOverlayAesthBuilding {
public:
    eSpring(GameBoard& board, const eCityId cid);
};

class eTopiary : public eAestheticsBuilding {
public:
    eTopiary(GameBoard& board, const eCityId cid);
};

class eBaths : public eOverlayAesthBuilding {
public:
    eBaths(GameBoard& board, const eCityId cid);
};

class eStoneCircle : public eOverlayAesthBuilding {
public:
    eStoneCircle(GameBoard& board, const eCityId cid);
};

class eWaterPark : public eBuilding {
public:
    eWaterPark(GameBoard& board, const eCityId cid);

    int id() const { return mId; }
    void setId(const int i);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mId = 0;
};

class eCommemorative : public eBuilding {
public:
    eCommemorative(const int id, GameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    int id() const { return mId; }
private:
    const int mId = 0;
};

class eGodMonumentTile;
class eSaveArchive;

class eGodMonument : public eBuilding {
public:
    eGodMonument(const GodType god,
                 const GodQuestId id,
                 GameBoard& board, const eCityId cid);

    void erase() override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    GodType god() const { return mGod; }
    GodQuestId id() const { return mId; }
    void addTile(eGodMonumentTile* const tile);
    const std::vector<eGodMonumentTile*>& tiles() const { return mMonumentTilesCache; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    const GodType mGod;
    const GodQuestId mId;
    std::vector<eGodMonumentTile*> mMonumentTilesCache;
};

class eGodMonumentTile : public eBuilding {
public:
    eGodMonumentTile(GameBoard& board, const eCityId cid);

    void erase() override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    void setMonument(eGodMonument* const mon);
    eGodMonument* monument() const { return mMonument; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eGodMonument* mMonument = nullptr;
};

#endif // EAESTHETICSBUILDING_H
