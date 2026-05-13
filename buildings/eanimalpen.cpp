#include "eanimalpen.h"

#include "fileIO/ejsonarchive.h"
#include "characters/edomesticatedanimal.h"

void eAnimalPen::sDimensions(int& sw, int& sh) {
    sw = sBuildW;
    sh = sBuildH;
}

eAnimalPen::eAnimalPen(
         eGameBoard& board,
         eCharacter* const a,
         const eBuildingType type,
         const eCityId cid) :
    eBuilding(board, type, sBuildW, sBuildH, cid),
    mA(a) {
}

eAnimalPen::~eAnimalPen() {
    if(mA) mA->kill();
}

void eAnimalPen::nextMonth() {
    const bool isCattle = type() == eBuildingType::cattle;
    if(!mA && !isCattle) erase();
    if(const auto da = dynamic_cast<eDomesticatedAnimal*>(mA.get())) {
        da->nextMonth();
    }
}

void eAnimalPen::read(eReadStream& src) {
    eBuilding::read(src);
}

void eAnimalPen::write(eWriteStream& dst) const {
    eBuilding::write(dst);
}

void eAnimalPen::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
}

void eAnimalPen::setAnimal(eCharacter* const a) {
    mA = a;
}
