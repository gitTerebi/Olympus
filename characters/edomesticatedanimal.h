#ifndef EDOMESTICATEDANIMAL_H
#define EDOMESTICATEDANIMAL_H

#include "eanimal.h"

class eSaveArchive;

class eDomesticatedAnimal : public eAnimal {
public:
    using eCharTexs = eAnimalTextures eCharacterTextures::*;
    eDomesticatedAnimal(eGameBoard& board,
                        const eCharTexs charTexs,
                        const eCharacterType type,
                        const int maxGroom);

    void groom();
    int collect();
    void nextMonth();
    bool canCollect() const { return mResource; }
    int monthsGrown() const { return mMonthsGrown; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

private:
    void serialize(eSaveArchive& ar);
    void resetGrowthProgress();

    virtual void setNakedTexture() {}
    virtual void setFleecedTexture() {}

    const int mMaxGroom;
    int mGroomed = 0;
    int mResource = 0;
    int mMonthsGrown = 0;
};

#endif // EDOMESTICATEDANIMAL_H
