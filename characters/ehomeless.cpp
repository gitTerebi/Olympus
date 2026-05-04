#include "ehomeless.h"

#include "textures/egametextures.h"
#include "engine/ecityid.h"
#include "engine/etile.h"
#include "characters/actions/esettleraction.h"

eHomeless::eHomeless(eGameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fHomeless,
                   eCharacterType::homeless) {
    eGameTextures::loadHomeless();
}

void eHomeless::spawn(eGameBoard& board, eTile* tile,
                      const eCityId cid, const int spawnCount,
                      const int waitTime) {
    const auto c = e::make_shared<eHomeless>(board);
    c->setBothCityIds(cid);
    c->changeTile(tile);
    const auto a = e::make_shared<eSettlerAction>(c.get());
    a->setNumberPeople(spawnCount);
    a->setInitialWait(waitTime);
    c->setAction(a);
    c->setActionType(eCharacterActionType::walk);
}
