#include "ereadstream.h"

#include "engine/e-game-board.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "characters/actions/walkable/ehasresourceobject.h"
#include "characters/actions/character-action.h"
#include "characters/gods/actions/god-action.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "characters/actions/epatrolaction.h"
#include "characters/actions/epatrolmoveaction.h"
#include "esavearchive.h"

eReadStream::eReadStream(const eReadSource& src) :
    mSrc(src) {}

void eReadStream::readFormat() {
    *this >> mFormat;
    int ignoredLegacyVersion;
    *this >> ignoredLegacyVersion;
}

eTile* eReadStream::readTile(GameBoard& board) {
    bool valid;
    *this >> valid;
    if(valid) {
        int x;
        *this >> x;
        int y;
        *this >> y;
        return board.tile(x, y);
    } else {
        return nullptr;
    }
}

void eReadStream::readBuilding(GameBoard* board,
                               const eBuildingFunc& func,
                               const char* tag) {
    int bid;
    *this >> bid;
#ifdef SAVE_DEBUG
    printf("[saveLoad] readBuilding tag='%s' ioid=%d\n",
           tag ? tag : "?", bid);
#endif
    addPostFunc([board, func, bid, tag]() {
        if(bid < 0) return;
        const auto b = board->buildingWithIOID(bid);
        if(!b) {
#ifdef SAVE_DEBUG
            printf("[saveLoad] readBuilding tag='%s' ioid=%d UNRESOLVED\n",
                   tag ? tag : "?", bid);
#endif
            return;
        }
        func(b);
    }, tag);
}

void eReadStream::readCharacter(GameBoard* board,
                                const eCharFunc& func,
                                const char* tag) {
    int cid;
    *this >> cid;
#ifdef SAVE_DEBUG
    printf("[saveLoad] readCharacter tag='%s' ioid=%d\n",
           tag ? tag : "?", cid);
#endif
    addPostFunc([board, func, cid, tag]() {
        if(cid < 0) return;
        const auto b = board->characterWithIOID(cid);
        if(!b) {
#ifdef SAVE_DEBUG
            printf("[saveLoad] readCharacter tag='%s' ioid=%d UNRESOLVED\n",
                   tag ? tag : "?", cid);
#endif
            return;
        }
        func(b);
    }, tag);
}

void eReadStream::readCharacterAction(GameBoard* board,
                                      const eCharActFunc& func,
                                      const char* tag) {
    int caid;
    *this >> caid;
#ifdef SAVE_DEBUG
    printf("[saveLoad] readCharacterAction tag='%s' ioid=%d\n",
           tag ? tag : "?", caid);
#endif
    addPostFunc([board, func, caid, tag]() {
        if(caid < 0) return;
        const auto b = board->characterActionWithIOID(caid);
        if(!b) {
#ifdef SAVE_DEBUG
            printf("[saveLoad] readCharacterAction tag='%s' ioid=%d UNRESOLVED\n",
                   tag ? tag : "?", caid);
#endif
            return;
        }
        func(b);
    }, tag);
}

stdsptr<eCharacterActionFunction> eReadStream::readCharActFunc(
        GameBoard& board) {
    bool hasFinish;
    *this >> hasFinish;
    if(hasFinish) {
        eCharActFuncType type;
        *this >> type;
        const auto f = eCharacterActionFunction::sCreate(board, type);
        eSaveArchive ar(*this);
        f->serialize(ar);
        return f;
    }
    return nullptr;
}

stdsptr<eGodAct> eReadStream::readGodAct(GameBoard& board) {
    bool hasFinish;
    *this >> hasFinish;
    if(hasFinish) {
        eGodActType type;
        *this >> type;
        const auto f = eGodAct::sCreate(board, type);
        eSaveArchive ar(*this);
        f->serialize(ar);
        return f;
    }
    return nullptr;
}

stdsptr<eDirectionTimes> eReadStream::readDirectionTimes(
        GameBoard& board) {
    const auto r = std::make_shared<eDirectionTimes>();
    int n;
    *this >> n;
    for(int i = 0; i < n; i++) {
        const auto tile = readTile(board);
        eDirectionLastUseTime u;
        eSaveArchive ar(*this);
        u.serialize(ar);
        (*r)[tile] = u;
    }
    return r;
}

void eReadStream::readCity(GameBoard* board, const eCityFunc& func) {
    return readCity(&board->world(), func);
}

void eReadStream::readCity(eWorldBoard* board, const eCityFunc& func) {
    int cid;
    *this >> cid;
    addPostFunc([board, func, cid]() {
        if(cid < 0) return;
        const auto c = board->cityWithIOID(cid);
        if(!c) return;
        func(c);
    }, "city");
}

void eReadStream::readBanner(GameBoard* board, const eBannerFunc& func) {
    int bid;
    *this >> bid;
    addPostFunc([board, func, bid]() {
        if(bid < 0) return;
        const auto b = board->bannerWithIOID(bid);
        if(!b) return;
        func(b);
    }, "banner");
}

void eReadStream::readSoldierBanner(GameBoard* board, const SoldierBannerFunc& func) {
    int bid;
    *this >> bid;
    addPostFunc([board, func, bid]() {
        if(bid < 0) return;
        const auto b = board->soldierBannerWithIOID(bid);
        if(!b) return;
        func(b ? b->ref<SoldierBanner>() : nullptr);
    }, "soldierBanner");
}

void eReadStream::readGameEvent(GameBoard* board, const eEventFunc& func) {
    int eid;
    *this >> eid;
    addPostFunc([board, func, eid]() {
        if(eid < 0) return;
        const auto b = board->eventWithIOID(eid);
        if(!b) return;
        func(b);
    }, "gameEvent");
}

void eReadStream::readInvasionHandler(GameBoard* board, const eeInvasionHandlerFunc& func) {
    int iid;
    *this >> iid;
    addPostFunc([board, func, iid]() {
        if(iid < 0) return;
        const auto b = board->invasionHandlerWithIOID(iid);
        if(!b) return;
        func(b);
    }, "invasionHandler");
}

void eReadStream::addPostFunc(const eFunc& func, const char* tag) {
    mPostFuncs.push_back({func, tag});
}

void eReadStream::transferPostFuncsTo(eReadStream& dst) {
    for(const auto& f : mPostFuncs) {
        dst.mPostFuncs.push_back(f);
    }
    mPostFuncs.clear();
}

void eReadStream::handlePostFuncs() {
    const int n = (int)mPostFuncs.size();
    for(int i = 0; i < n; i++) {
        mPostFuncs[i].first();
    }
    mPostFuncs.clear();
}
