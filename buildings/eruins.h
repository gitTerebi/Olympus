#ifndef ERUINS_H
#define ERUINS_H

#include "ebuilding.h"
#include <vector>
#include <cstdint>

class eSaveArchive;

class eRuins : public eBuilding {
public:
    eRuins(eGameBoard& board, const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(class eJsonArchive& ar) override;

    void setWasType(const eBuildingType type) { mWasType = type; }
    eBuildingType wasType() const { return mWasType; }

    void setOrigin(int x, int y, int w, int h) { mOriginX=x; mOriginY=y; mOriginW=w; mOriginH=h; }
    int originX() const { return mOriginX; }
    int originY() const { return mOriginY; }
    int originW() const { return mOriginW; }
    int originH() const { return mOriginH; }

    void setSavedBuilding(std::vector<uint8_t> data) { mSavedBuilding = std::move(data); }
    const std::vector<uint8_t>& savedBuilding() const { return mSavedBuilding; }
    bool hasSavedBuilding() const { return !mSavedBuilding.empty(); }

    void setRestoreBundle(std::vector<uint8_t> data) { mRestoreBundle = std::move(data); }
    const std::vector<uint8_t>& restoreBundle() const { return mRestoreBundle; }
    bool hasRestoreBundle() const { return !mRestoreBundle.empty(); }

    void setSavedPier(std::vector<uint8_t> data, const SDL_Rect& rect) {
        mSavedPier = std::move(data);
        mSavedPierRect = rect;
    }
    const std::vector<uint8_t>& savedPier() const { return mSavedPier; }
    const SDL_Rect& savedPierRect() const { return mSavedPierRect; }
    bool hasSavedPier() const { return !mSavedPier.empty(); }

private:
    void serialize(eSaveArchive& ar);

    eBuildingType mWasType = eBuildingType::none;
    int mOriginX = -1;
    int mOriginY = -1;
    int mOriginW = 1;
    int mOriginH = 1;

    std::vector<uint8_t> mSavedBuilding;
    std::vector<uint8_t> mRestoreBundle;
    std::vector<uint8_t> mSavedPier;
    SDL_Rect mSavedPierRect = {-1, -1, 0, 0};
};

#endif // ERUINS_H
