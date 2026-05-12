#include "ejsonarchive.h"
#include "engine/e-game-board.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"
#include "characters/echaracter.h"
#include "characters/actions/echaracteractionfunction.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "gameEvents/egameevent.h"

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

void eJsonArchive::characterRef(const char* key, eCharacter*& c, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { c = nullptr; return; }
        addPostFunc([&c, &board, ioid]() {
            c = board.characterWithIOID(ioid);
        });
    } else {
        int ioid = c ? c->ioID() : -1;
        field(key, ioid, -1);
    }
}

void eJsonArchive::characterRef(const char* key, const std::function<void(eCharacter*)>& cb, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { cb(nullptr); return; }
        addPostFunc([&board, ioid, cb]() {
            cb(board.characterWithIOID(ioid));
        });
    }
}

void eJsonArchive::walkableRef(const char* key, std::shared_ptr<eWalkableObject>& w) {
    if(mReading) {
        const njson* n = get(key);
        if(!n || n->is_null() || !n->contains("valid") || !(*n)["valid"].get<bool>()) {
            w = nullptr;
            return;
        }
        const auto t = static_cast<eWalkableObjectType>((*n)["type"].get<int32_t>());
        w = eWalkableObject::sCreate(t);
    } else {
        njson& n = set(key);
        if(w) {
            n["valid"] = true;
            n["type"] = static_cast<int32_t>(w->type());
        } else {
            n["valid"] = false;
        }
    }
}

void eJsonArchive::obsticleHandlerRef(const char* key, std::shared_ptr<eObsticleHandler>& oh, eGameBoard& board) {
    if(mReading) {
        const njson* n = get(key);
        if(!n || n->is_null() || !n->contains("valid") || !(*n)["valid"].get<bool>()) {
            oh = nullptr;
            return;
        }
        const auto t = static_cast<eObsticleHandlerType>((*n)["type"].get<int32_t>());
        oh = eObsticleHandler::sCreate(board, t);
    } else {
        njson& n = set(key);
        if(oh) {
            n["valid"] = true;
            n["type"] = static_cast<int32_t>(oh->type());
        } else {
            n["valid"] = false;
        }
    }
}

void eJsonArchive::gameEventRef(const char* key, eGameEvent*& e, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { e = nullptr; return; }
        addPostFunc([&e, &board, ioid]() {
            e = board.eventWithIOID(ioid);
        });
    } else {
        int ioid = e ? e->ioID() : -1;
        field(key, ioid, -1);
    }
}

void eJsonArchive::gameEventRef(const char* key, const std::function<void(eGameEvent*)>& cb, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { cb(nullptr); return; }
        addPostFunc([&board, ioid, cb]() {
            cb(board.eventWithIOID(ioid));
        });
    }
}

void eJsonArchive::charActFuncRef(const char* key,
                                   std::shared_ptr<eCharacterActionFunction>& f,
                                   eGameBoard& board) {
    if(mReading) {
        int32_t t = -1;
        field((std::string(key) + ".type").c_str(), t, int32_t(-1));
        if(t < 0) { f = nullptr; return; }
        f = eCharActFunc::sCreate(board, static_cast<eCharActFuncType>(t));
        if(f) {
            auto sub = child(key);
            f->serializeJson(sub);
        }
    } else {
        int32_t t = f ? static_cast<int32_t>(f->type()) : -1;
        field((std::string(key) + ".type").c_str(), t, int32_t(-1));
        if(f) {
            auto sub = child(key);
            f->serializeJson(sub);
        }
    }
}
