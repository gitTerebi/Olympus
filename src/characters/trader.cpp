#include "trader.h"

#include "edonkey.h"
#include "characters/actions/efollowaction.h"

#include "textures/game-textures.h"

Trader::Trader(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fTrader,
                   eCharacterType::trader) {
    GameTextures::loadTrader();
}

void Trader::createFollowers() {
    const auto t = tile();
    auto& board = getBoard();
    mFollowers.clear();
    for(int i = 0; i < 2; i++) {
        const auto d = e::make_shared<eDonkey>(board);
        eCharacter* follow = i == 0 ? this : mFollowers.back().get();
        const auto aox = e::make_shared<eFollowAction>(follow, d.get());
        d->setAction(aox);
        d->changeTile(t);
        d->setOnCityId(onCityId());
        d->setCityId(cityId());
        mFollowers.push_back(d);
    }
}

void Trader::setActionType(const eCharacterActionType t) {
    eBasicPatroler::setActionType(t);
    for(const auto& f : mFollowers) {
        f->setActionType(t);
    }
}
