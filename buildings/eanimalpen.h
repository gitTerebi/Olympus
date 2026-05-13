#ifndef EANIMALPEN_H
#define EANIMALPEN_H

// 1x1 home tile for a domesticated animal (sheep/goat/cattle).
// The animal roams freely but is owned by and returns to this tile.
// nextMonth() ticks the animal's resource timer.

#include "ebuilding.h"
#include "characters/echaracter.h"

class eAnimalPen : public eBuilding {
public:
    eAnimalPen(eGameBoard& board,
               eCharacter* const a,
               const eBuildingType type,
               const eCityId cid);
    ~eAnimalPen();

    static void sDimensions(int& sw, int& sh);
    static constexpr int sBuildW = 1;
    static constexpr int sBuildH = 1;

    std::shared_ptr<eTexture> getTexture(
            const eTileSize) const override { return nullptr; };

    void nextMonth() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(class eJsonArchive& ar) override;

    eCharacter* animal() const { return mA; }
    void setAnimal(eCharacter* const a);
private:
    stdptr<eCharacter> mA;
};

#endif // EANIMALPEN_H
