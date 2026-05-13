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
    bool canCollect() const { return mResource; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override {
        eAnimal::serializeJson(ar);
        ar.field("mGroomed", mGroomed);
        ar.field("mResource", mResource);
        if(ar.reading()) {
            if(mResource == 0) setNakedTexture();
            else setFleecedTexture();
        }
    }

private:
    void serialize(eSaveArchive& ar);

    virtual void setNakedTexture() {}
    virtual void setFleecedTexture() {}

    const int mMaxGroom;
    int mGroomed = 0;
    int mResource = 0;
};

#endif // EDOMESTICATEDANIMAL_H
