#include "etriremeaction.h"

#include "buildings/ebuilding.h"

eTriremeAction::eTriremeAction(eBuilding* const home,
                               eCharacter* const trireme) :
    eCharacterAction(trireme, eCharActionType::triremeAction),
    mHome(home) {}

void eTriremeAction::increment(const int by) {
    (void)by;
}

void eTriremeAction::read(eReadStream& src) {
    eCharacterAction::read(src);
    auto& board = eTriremeAction::board();
    src.readBuilding(&board, [this](eBuilding* const b) {
        mHome = b;
    });
}

void eTriremeAction::write(eWriteStream& dst) const {
    eCharacterAction::write(dst);
    dst.writeBuilding(mHome);
}
