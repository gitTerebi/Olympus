#include "eobject.h"

#include "engine/game-board.h"

eObject::eObject(GameBoard& board) :
    mBoard(board) {

}

void eObject::deleteLater() {
    if(mDeleteLater) return;
    mDeleteLater = true;
    mBoard.addRubbish(ref<eObject>());
}
