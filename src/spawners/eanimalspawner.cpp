#include "eanimalspawner.h"

#include "characters/ewildanimal.h"
#include "characters/actions/eanimalaction.h"
#include "characters/actions/ewolfaction.h"
#include "erand.h"

void eAnimalSpawner::spawn(eTile* const tile) {
    const auto b = create(board());
    b->setActionType(eCharacterActionType::stand);
    b->setSpawner(this);
    b->changeTile(tile);
    const int oi = eRand::rand() % 8;
    const auto o = static_cast<eOrientation>(oi);
    b->setOrientation(o);
    const auto t = this->tile();
    const int tx = t->x();
    const int ty = t->y();
    if(b->type() == eCharacterType::wolf) {
        const auto a = e::make_shared<eWolfAction>(
            b.get(), tx, ty);
        b->setAction(a);
    } else {
        const auto a = e::make_shared<eAnimalAction>(
            b.get(), tx, ty);
        b->setAction(a);
    }
}
