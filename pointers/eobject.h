#ifndef EOBJECT_H
#define EOBJECT_H

#include "estdselfref.h"

class GameBoard;

class eObject : public eStdSelfRef {
public:
    eObject(GameBoard& board);

    GameBoard& getBoard() const { return mBoard; }

    void deleteLater();
    bool deleteScheduled() const { return mDeleteLater; }
private:
    GameBoard& mBoard;
    bool mDeleteLater = false;
};

#endif // EOBJECT_H
