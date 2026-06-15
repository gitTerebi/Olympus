#include "edomesticatedanimal.h"

#include "fileIO/save-archive.h"
#include "audio/sounds.h"
#include "rand.h"

eDomesticatedAnimal::eDomesticatedAnimal(
        GameBoard& board,
        const eCharTexs charTexs,
        const eCharacterType type,
        const int maxGroom) :
    Animal(board, charTexs, type),
    mMaxGroom(maxGroom) {
    mMonthsGrown = Rand::rand() % 3;
}

void eDomesticatedAnimal::groom() {
    if(mResource >= 1) return;
    mGroomed++;
    if(mGroomed >= mMaxGroom) mGroomed = 0;
}

void eDomesticatedAnimal::nextMonth() {
    if(mResource >= 1) return;
    mMonthsGrown++;
    if(mMonthsGrown >= 12) {
        mMonthsGrown = 0;
        mResource = 1;
        setFleecedTexture();
    }
}

int eDomesticatedAnimal::collect() {
    if(mResource <= 0) return 0;
    const int r = mResource;
    mResource = 0;
    mMonthsGrown = 0;
    setNakedTexture();
    if(type() == eCharacterType::sheep) {
        eSounds::playShearingSound();
    }
    return r;
}

void eDomesticatedAnimal::serializeFields(SaveArchive& ar) {
    Animal::serializeFields(ar);
    ar.field("mGroomed", mGroomed);
    ar.field("mResource", mResource);
    ar.field("mMonthsGrown", mMonthsGrown);
    if(ar.reading()) {
        setBusy(false);
        clearPausedActions();
        if(mResource == 0) {
            setNakedTexture();
        } else {
            setFleecedTexture();
        }
    }
}
