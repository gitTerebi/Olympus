#include "ewolfaction.h"

#include "erand.h"
#include "enumbers.h"
#include "emovetoaction.h"
#include "engine/egameboard.h"

eWolfAction::eWolfAction(eCharacter* const c,
                         const int spawnerX, const int spawnerY) :
    eAnimalAction(c, spawnerX, spawnerY,
                  eWalkableObject::sCreateDefault(),
                  eCharActionType::wolfAction) {}

eWolfAction::eWolfAction(eCharacter* const c) :
    eWolfAction(c, 0, 0) {}


bool killedPrey(eTile* const tile) {
    const auto cs = tile->characters();
    for(const auto& c : cs) {
        const auto t = c->type();
        if(t == eCharacterType::sheep ||
           t == eCharacterType::goat ||
           t == eCharacterType::cattle1 ||
           t == eCharacterType::cattle2 ||
           t == eCharacterType::cattle3) {
            if(c->dead()) return true;
        }
    }
    return false;
}

void eWolfAction::increment(const int by) {
    const auto c = character();
    const auto t = c->tile();
    if(t && killedPrey(t)) {
        goBack();
        return;
    }
    eAnimalAction::increment(by);
}

bool eWolfAction::decide() {
    if(mHunting) {
        findPrey();
        return true;
    }
    const int wait = eNumbers::sWolfHuntWait;
    if(wait <= 0) return eAnimalAction::decide();
    const bool hunt = (eRand::rand() % wait) == 0;
    if(hunt) {
        mHunting = true;
        findPrey();
        return true;
    }
    return eAnimalAction::decide();
}

void eWolfAction::write(eWriteStream &dst) const {
    eAnimalAction::write(dst);
    dst << mHunting;
}

void eWolfAction::read(eReadStream &src) {
    eAnimalAction::read(src);
    src >> mHunting;
}

void eWolfAction::goBack() {
    mHunting = false;

    const auto c = character();

    const auto& board = eWolfAction::board();
    const auto tile = board.tile(mSpawnerX, mSpawnerY);
    if(!tile) {
        c->kill();
        return;
    }

    const stdptr<eCharacter> cptr(c);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([cptr, c]() {
        if(!cptr) return;
        c->setActionType(eCharacterActionType::walk);
    });
    const auto findFailFunc = [cptr, c]() {
        if(!cptr) return;
        c->kill();
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(2*eNumbers::sWolfHuntDistance);
    a->start(tile);
    setCurrentAction(a);
}

void eWolfAction::findPrey() {
    const auto c = character();

    const stdptr<eCharacter> cptr(c);
    const stdptr<eWolfAction> tptr(this);

    const auto hha = [](eTileBase* const tile) {
        return tile->hasCharacter([](const eCharacterBase& c) {
            const auto type = c.type();
            return type == eCharacterType::sheep ||
                   type == eCharacterType::goat ||
                   type == eCharacterType::cattle1 ||
                   type == eCharacterType::cattle2 ||
                   type == eCharacterType::cattle3;
        });
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([cptr, c]() {
        if(!cptr) return;
        c->setActionType(eCharacterActionType::walk);
    });
    const auto findFailFunc = [tptr, this]() {
        if(tptr) goBack();
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(eNumbers::sWolfHuntDistance);
    a->start(hha);
    setCurrentAction(a);
}
