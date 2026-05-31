#ifndef EGAMEUNDO_H
#define EGAMEUNDO_H

#include <vector>

class eBuilding;
enum class eBuildingType;
class GameBoard;

struct eUndoTile {
    int tx, ty;
    eBuilding* prev; // nullptr if was empty
    eUndoTile(int ttx, int tty, eBuilding* p) :
        tx(ttx), ty(tty), prev(p) {}
};

struct eUndoState {
    bool valid = false;
    int cost = 0;
    int timeoutTicks = 0;
    std::vector<eBuilding*> placed;
    std::vector<eUndoTile> tiles;
};

class eGameUndo {
public:
    eGameUndo(GameBoard& board);

    void startBuild(eBuildingType type);
    void finishBuild();
    void undoLastAction();
    void incTime(const int by);

    std::vector<eBuilding*>& placed() { return mState.placed; }
    int& cost() { return mState.cost; }
    std::vector<eUndoTile>& tiles() { return mState.tiles; }
    bool& valid() { return mState.valid; }
    bool valid() const { return mState.valid; }
    int& timeoutTicks() { return mState.timeoutTicks; }

private:
    GameBoard& mBoard;
    eUndoState mState;
};

#endif // EGAMEUNDO_H