#include "eobsticlehandler.h"

#include "characters/actions/monster-action.h"
#include "characters/gods/actions/god-attack-action.h"
#include "characters/actions/soldier-action.h"

stdsptr<eObsticleHandler> eObsticleHandler::sCreate(
        GameBoard& board,
        const eObsticleHandlerType type) {
    switch(type) {
    case eObsticleHandlerType::monster:
        return std::make_shared<eMonsterObsticleHandler>(board);
    case eObsticleHandlerType::god:
        return std::make_shared<eGodObsticleHandler>(board);
    case eObsticleHandlerType::soldier:
        return std::make_shared<SoldierObsticleHandler>(board);
    }
    return nullptr;
}
