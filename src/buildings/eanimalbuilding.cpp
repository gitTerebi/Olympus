#include "eanimalbuilding.h"

#include "fileIO/save-archive.h"

eAnimalBuilding::eAnimalBuilding(
         GameBoard& board,
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

void eAnimalBuilding::serializeFields(SaveArchive& ar) {
    eBuilding::serializeFields(ar);
    ar.characterField("animal", &getBoard(), mA);
}

void eAnimalBuilding::setAnimal(eCharacter* const a) {
    mA = a;
}
