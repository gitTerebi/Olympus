#include "eanimalbuilding.h"

#include "fileIO/ejsonarchive.h"

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
}

void eAnimalBuilding::write(eWriteStream& dst) const {
    eBuilding::write(dst);
}

void eAnimalBuilding::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
}

void eAnimalBuilding::setAnimal(eCharacter* const a) {
    mA = a;
}
