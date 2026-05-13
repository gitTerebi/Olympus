#include "ejsonarchive.h"
#include "engine/e-game-board.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"
#include "characters/echaracter.h"
#include "characters/esoldierbanner.h"
#include "characters/actions/echaracteractionfunction.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "characters/actions/walkable/ehasresourceobject.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "characters/actions/epatrolmoveaction.h"
#include "gameEvents/egameevent.h"
#include "gameEvents/invasions/invasion-handler.h"
#include "engine/e-worldcity.h"
#include "engine/eworldboard.h"

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
        if(w) {
            auto sub = child(key);
            w->serializeJson(sub);
        }
    } else {
        njson& n = set(key);
        if(w) {
            n["valid"] = true;
            n["type"] = static_cast<int32_t>(w->type());
            auto sub = child(key);
            w->serializeJson(sub);
        } else {
            n["valid"] = false;
        }
    }
}

void eJsonArchive::hasResourceRef(const char* key, std::shared_ptr<eHasResourceObject>& h) {
    if(mReading) {
        const njson* n = get(key);
        if(!n || n->is_null() || !n->contains("valid") || !(*n)["valid"].get<bool>()) {
            h = nullptr;
            return;
        }
        const auto t = static_cast<eHasResourceObjectType>((*n)["type"].get<int32_t>());
        h = eHasResourceObject::sCreate(t);
        if(h) {
            auto sub = child(key);
            h->serializeJson(sub);
        }
    } else {
        njson& n = set(key);
        if(h) {
            n["valid"] = true;
            n["type"] = static_cast<int32_t>(h->type());
            auto sub = child(key);
            h->serializeJson(sub);
        } else {
            n["valid"] = false;
        }
    }
}

void eJsonArchive::directionTimesRef(const char* key, std::shared_ptr<eDirectionTimes>& d,
                                      eGameBoard& board) {
    auto da = child(key);
    if(da.writing()) {
        int n = d ? static_cast<int>(d->size()) : 0;
        da.field("count", n);
        if(!d) return;
        int i = 0;
        for(auto& kv : *d) {
            auto item = da.child(("items." + std::to_string(i++)).c_str());
            eTile* tile = kv.first;
            item.tile("tile", tile, board);
            auto times = item.child("times");
            kv.second.serializeJson(times);
        }
    } else {
        if(!d) d = std::make_shared<eDirectionTimes>();
        d->clear();
        int n = 0;
        da.field("count", n);
        for(int i = 0; i < n; i++) {
            auto item = da.child(("items." + std::to_string(i)).c_str());
            eTile* tile = nullptr;
            item.tile("tile", tile, board);
            if(!tile) continue;
            auto times = item.child("times");
            (*d)[tile].serializeJson(times);
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

void eJsonArchive::invasionHandlerRef(const char* key, eInvasionHandler*& h, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { h = nullptr; return; }
        addPostFunc([&h, &board, ioid]() {
            h = board.invasionHandlerWithIOID(ioid);
        });
    } else {
        int ioid = h ? h->ioID() : -1;
        field(key, ioid, -1);
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

void eJsonArchive::cityRef(const char* key,
                           const std::function<void(const std::shared_ptr<eWorldCity>&)>& cb,
                           eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { cb(nullptr); return; }
        addPostFunc([&board, ioid, cb]() {
            cb(board.world().cityWithIOID(ioid));
        });
    }
}

void eJsonArchive::cityRef(const char* key, std::shared_ptr<eWorldCity>& c, eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { c = nullptr; return; }
        addPostFunc([&c, &board, ioid]() {
            c = board.world().cityWithIOID(ioid);
        });
    } else {
        int ioid = c ? c->ioID() : -1;
        field(key, ioid, -1);
    }
}

void eJsonArchive::cityRef(const char* key,
                           const std::function<void(const std::shared_ptr<eWorldCity>&)>& cb,
                           eWorldBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { cb(nullptr); return; }
        addPostFunc([&board, ioid, cb]() {
            cb(board.cityWithIOID(ioid));
        });
    }
}

void eJsonArchive::cityRef(const char* key, std::shared_ptr<eWorldCity>& c, eWorldBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { c = nullptr; return; }
        addPostFunc([&c, &board, ioid]() {
            c = board.cityWithIOID(ioid);
        });
    } else {
        int ioid = c ? c->ioID() : -1;
        field(key, ioid, -1);
    }
}

void eJsonArchive::soldierBannerRef(const char* key,
                                     const std::function<void(const std::shared_ptr<eSoldierBanner>&)>& cb,
                                     eGameBoard& board) {
    if(mReading) {
        int ioid = -1;
        field(key, ioid, -1);
        if(ioid < 0) { cb(nullptr); return; }
        addPostFunc([&board, ioid, cb]() {
            auto* b = board.soldierBannerWithIOID(ioid);
            cb(b ? b->ref<eSoldierBanner>() : nullptr);
        });
    } else {
        // write-side: used only via callback form, no-op
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
