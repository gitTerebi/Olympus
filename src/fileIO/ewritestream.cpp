#include "ewritestream.h"

#include "engine/game-board.h"
#include "esavearchive.h"
#include "characters/actions/character-action.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "characters/actions/walkable/ehasresourceobject.h"
#include "characters/gods/actions/god-action.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "characters/actions/epatrolmoveaction.h"
#include "spawners/ebanner.h"
#include "gameEvents/invasions/invasion-handler.h"
#include "characters/soldier-banner.h"

eWriteStream::eWriteStream(const eWriteTarget& dst) :
    mDst(dst) {}

void eWriteStream::writeFormat(const std::string& format) {
    mFormat = format;
    *this << std::string(format);
    *this << 0;
}

void eWriteStream::writeTile(eTile* const tile) {
    *this << bool(tile);
    if(tile) {
        *this << tile->x();
        *this << tile->y();
    }
}

void eWriteStream::writeBuilding(eBuilding* const b) {
    const int bid = b ? b->ioID() : -1;
    *this << bid;
}

void eWriteStream::writeCharacter(eCharacter* const c) {
    const int bid = c ? c->ioID() : -1;
    *this << bid;
}

void eWriteStream::writeCharacterAction(eCharacterAction* const ca) {
    const int bid = ca ? ca->ioID() : -1;
    *this << bid;
}

void eWriteStream::writeCharActFunc(eCharacterActionFunction* const caf) {
    const bool hasFinish = caf != nullptr;
    *this << hasFinish;
    if(hasFinish) {
        *this << caf->type();
        eSaveArchive ar(*this);
        caf->serialize(ar);
    }
}

void eWriteStream::writeGodAct(eGodAct* const ga) {
    const bool hasFinish = ga != nullptr;
    *this << hasFinish;
    if(hasFinish) {
        *this << ga->type();
        eSaveArchive ar(*this);
        ga->serialize(ar);
    }
}

void eWriteStream::writeDirectionTimes(eDirectionTimes* const d) {
    *this << d->size();
    for(const auto& dt : *d) {
        writeTile(dt.first);
        eSaveArchive ar(*this);
        const_cast<eDirectionLastUseTime&>(dt.second).serialize(ar);
    }
}

void eWriteStream::writeCity(eWorldCity* const c) {
    const int cid = c ? c->ioID() : -1;
    *this << cid;
}

void eWriteStream::writeBanner(eBanner* const b) {
    const int bid = b ? b->ioID() : -1;
    *this << bid;
}

void eWriteStream::writSoldierBanner(SoldierBanner* const b) {
    const int bid = b ? b->ioID() : -1;
    *this << bid;
}

void eWriteStream::writeGameEvent(eGameEvent* const e) {
    const int eid = e ? e->ioID() : -1;
    *this << eid;
}

void eWriteStream::writeInvasionHandler(eInvasionHandler* const i) {
    const int iid = i ? i->ioID() : -1;
    *this << iid;
}
