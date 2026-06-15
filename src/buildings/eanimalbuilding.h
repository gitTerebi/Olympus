#ifndef EANIMALBUILDING_H
#define EANIMALBUILDING_H

#include "ebuilding.h"
#include "characters/echaracter.h"

class SaveArchive;

class eAnimalBuilding : public eBuilding {
public:
    eAnimalBuilding(GameBoard& board,
                    eCharacter* const a,
                    const eBuildingType type,
                    const eCityId cid);
    ~eAnimalBuilding();

    std::shared_ptr<Texture> getTexture(
            const eTileSize) const override { return nullptr; };

    void nextMonth() override;

    eCharacter* animal() const { return mA; }
    void setAnimal(eCharacter* const a);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<eCharacter> mA;
};

#endif // EANIMALBUILDING_H
