#include "eanimalpen.h"

#include "fileIO/ejsonarchive.h"
#include "fileIO/esavearchive.h"
#include "characters/edomesticatedanimal.h"
#include "characters/actions/eanimalaction.h"
#include "engine/e-game-board.h"

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
    killAnimal();
}

void eAnimalPen::nextMonth() {
    const bool isCattle = type() == eBuildingType::cattle;
    if(!mA && !isCattle) erase();
    if(const auto da = dynamic_cast<eDomesticatedAnimal*>(mA.get())) {
        da->nextMonth();
    }
}

void eAnimalPen::erase() {
    killAnimal();
    eBuilding::erase();
}

void eAnimalPen::read(eReadStream& src) {
    eBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eAnimalPen::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eAnimalPen*>(this)->serialize(ar);
}

void eAnimalPen::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
    if(ar.writing()) {
        eCharacter* raw = mA.get();
        ar.characterRef("mA", raw, getBoard());
    } else {
        ar.characterRef("mA", [this](eCharacter* c) {
            mA = c;
        }, getBoard());
    }
}

void eAnimalPen::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter* const c) {
            mA = c;
        });
    } else {
        ar.writeStream().writeCharacter(mA.get());
    }
}

void eAnimalPen::setAnimal(eCharacter* const a) {
    mA = a;
}

void eAnimalPen::killAnimal() {
    auto a = mA.get();
    if(!a) {
        const auto rect = tileRect();
        const auto penType = type();
        for(const auto c : getBoard().characters()) {
            if(!c) continue;
            const auto ct = c->type();
            const bool matchingAnimal =
                (penType == eBuildingType::sheep &&
                 ct == eCharacterType::sheep) ||
                (penType == eBuildingType::goat &&
                 ct == eCharacterType::goat) ||
                (penType == eBuildingType::cattle &&
                 (ct == eCharacterType::cattle1 ||
                  ct == eCharacterType::cattle2 ||
                  ct == eCharacterType::cattle3 ||
                  ct == eCharacterType::bull));
            if(!matchingAnimal) continue;
            const auto aa = dynamic_cast<eAnimalAction*>(c->action());
            if(!aa) continue;
            if(aa->spawnerX() != rect.x || aa->spawnerY() != rect.y) continue;
            a = c;
            break;
        }
    }
    if(a) a->kill();
    mA = nullptr;
}
