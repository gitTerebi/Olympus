#ifndef EELITEHOUSING_H
#define EELITEHOUSING_H

#include "ehousebase.h"

class TextureCollection;

class SaveArchive;

class EliteHousing : public eHouseBase {
public:
    EliteHousing(GameBoard& board, const eCityId cid);

    std::shared_ptr<Texture>
    getTexture(const eTileSize) const override { return nullptr; }

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    std::vector<Overlay>
    getOverlays(const eTileSize size) const override;

    std::shared_ptr<Texture>
        getLeftTexture(const eTileSize size) const;
    std::shared_ptr<Texture>
        getBottomTexture(const eTileSize size) const;
    std::shared_ptr<Texture>
        getTopTexture(const eTileSize size) const;
    std::shared_ptr<Texture>
        getRightTexture(const eTileSize size) const;
    std::vector<Overlay>
        getHorseOverlays(const eTileSize size) const;

    int provide(const eProvide p, const int n) override;

    void timeChanged(const int by) override;

    void nextMonth() override;

    int wine() const { return mWine; }
    int arms() const { return mArms; }

    int horses() const { return mHorses; }

    bool lowFood() const;
    bool lowFleece() const { return mFleece < 2; }
    bool lowOil() const { return mOil < 2; }
    bool lowWine() const { return mWine < 2; }
    bool lowArms() const { return !mArms; }
    bool lowHorses() const { return !mHorses; }

    void removeArmor();
    void removeHorse();

    eHouseMissing missing() const override;

    static std::string sName(const int level);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    const TextureCollection& getTextureCollection(
            const eTileSize size) const;

    void updateLevel();

    int mUpdateLevel = 0;

    int mWine = 0;
    int mArms = 0;
    int mHorses = 0;
};

#endif // EELITEHOUSING_H
