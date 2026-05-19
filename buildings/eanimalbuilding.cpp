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

void eAnimalBuilding::serializeFields(eSaveArchive& ar) {
    eBuilding::serializeFields(ar);
    ar.characterField("animal", &getBoard(), mA);
}

void eAnimalBuilding::setAnimal(eCharacter* const a) {
    mA = a;
}
