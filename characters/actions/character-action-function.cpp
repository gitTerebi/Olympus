#include "character-action-function.h"

#include "characters/echaracter.h"

eCharacterActionFunction::eCharacterActionFunction(
        GameBoard& board, const eCharActFuncType t) :
    mBoard(board), mType(t) {}
