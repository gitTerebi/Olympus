#include "ejsonarchive.h"
#include "engine/e-game-board.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"

void eJsonArchive::tile(const char* key, eTile*& t, eGameBoard& board) {
    if(mReading) {
        const njson* n = get(key);
        if(!n || n->is_null() || !n->contains("valid") || !(*n)["valid"].get<bool>()) {
            t = nullptr;
            return;
        }
        const int x = (*n)["x"].get<int>();
        const int y = (*n)["y"].get<int>();
        t = board.tile(x, y);
    } else {
        njson& n = set(key);
        if(t) {
            n["valid"] = true;
            n["x"] = t->x();
            n["y"] = t->y();
        } else {
            n["valid"] = false;
        }
    }
}

void eJsonArchive::buildingRef(const char* key, eBuilding*& b, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { b = nullptr; return; }
        addPostFunc([&b, &board, ioid]() {
            b = board.buildingWithIOID(ioid);
        });
    } else {
        int ioid = b ? b->ioID() : -1;
        field(key, ioid, -1);
    }
}

void eJsonArchive::buildingRef(const char* key, const std::function<void(eBuilding*)>& cb, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { cb(nullptr); return; }
        addPostFunc([&board, ioid, cb]() {
            cb(board.buildingWithIOID(ioid));
        });
    }
}
