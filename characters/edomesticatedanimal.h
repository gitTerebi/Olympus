#ifndef EDOMESTICATEDANIMAL_H
#define EDOMESTICATEDANIMAL_H

#include "animal.h"

class eSaveArchive;

class eDomesticatedAnimal : public Animal {
public:
    using eCharTexs = eAnimalTextures eCharacterTextures::*;
    eDomesticatedAnimal(GameBoard& board,
                        const eCharTexs charTexs,
                        const eCharacterType type,
                        const int maxGroom);

    void groom();
    int collect();
    void nextMonth();
    bool canCollect() const { return mResource; }
    int monthsGrown() const { return mMonthsGrown; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    virtual void setNakedTexture() {}
    virtual void setFleecedTexture() {}

    const int mMaxGroom;
    int mGroomed = 0;
    int mResource = 0;
    int mMonthsGrown = 0;
};

#endif // EDOMESTICATEDANIMAL_H
