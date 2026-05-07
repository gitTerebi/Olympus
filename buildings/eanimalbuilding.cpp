#include "eanimalbuilding.h"

#include "fileIO/esavearchive.h"

eAnimalBuilding::eAnimalBuilding(
         eGameBoard& board,
         eCharacter* const a,
         const eBuildingType type,
         const eCityId cid) :
    eBuilding(board, type, 1, 2, cid),
    mA(a) {

}

eAnimalBuilding::~eAnimalBuilding() {
    if(mA) mA->kill();
}

void eAnimalBuilding::nextMonth() {
    const bool isCattle = type() == eBuildingType::cattle;
    if(!mA && !isCattle) erase();
}

void eAnimalBuilding::read(eReadStream& src) {
    eBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eAnimalBuilding::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eAnimalBuilding*>(this)->serialize(ar);
}

void eAnimalBuilding::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        auto& board = getBoard();
        ar.readStream().readCharacter(&board, [this](eCharacter* const c) {
            mA = c;
        });
    } else {
        ar.writeStream().writeCharacter(mA);
    }
}

void eAnimalBuilding::setAnimal(eCharacter* const a) {
    mA = a;
}
