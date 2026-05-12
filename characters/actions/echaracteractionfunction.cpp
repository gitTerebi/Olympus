#include "echaracteractionfunction.h"

#include "characters/echaracter.h"
#include "engine/e-game-board.h"
#include "buildings/ebuilding.h"

eCharacterActionFunction::eCharacterActionFunction(
        eGameBoard& board, const eCharActFuncType t) :
    mBoard(board), mType(t) {}

eCharacterAction* eCharacterActionFunction::resolveCharAction(int ioid) {
    return mBoard.characterActionWithIOID(ioid);
}

eCharacter* eCharacterActionFunction::resolveChar(int ioid) {
    return mBoard.characterWithIOID(ioid);
}

eBuilding* eCharacterActionFunction::resolveBuilding(int ioid) {
    return mBoard.buildingWithIOID(ioid);
}
