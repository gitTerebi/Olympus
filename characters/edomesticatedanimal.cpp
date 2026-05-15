#include "edomesticatedanimal.h"

#include "fileIO/esavearchive.h"
#include "audio/sounds.h"
#include "erand.h"

eDomesticatedAnimal::eDomesticatedAnimal(
        eGameBoard& board,
        const eCharTexs charTexs,
        const eCharacterType type,
        const int maxGroom) :
    eAnimal(board, charTexs, type),
    mMaxGroom(maxGroom) {
    resetGrowthProgress();
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
    resetGrowthProgress();
    setNakedTexture();
    if(type() == eCharacterType::sheep) {
        eSounds::playShearingSound();
    }
    return r;
}

void eDomesticatedAnimal::read(eReadStream& src) {
    eAnimal::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eDomesticatedAnimal::write(eWriteStream& dst) const {
    eAnimal::write(dst);
    eSaveArchive ar(dst);
    const_cast<eDomesticatedAnimal*>(this)->serialize(ar);
}

void eDomesticatedAnimal::serialize(eSaveArchive& ar) {
    ar.field("mGroomed", mGroomed);
    ar.field("mResource", mResource);
    ar.field("mMonthsGrown", mMonthsGrown);
    if(ar.reading()) {
        setBusy(false);
        if(mResource == 0) {
            setNakedTexture();
        } else {
            setFleecedTexture();
        }
    }
}

void eDomesticatedAnimal::resetGrowthProgress() {
    mMonthsGrown = eRand::rand() % 3;
}
