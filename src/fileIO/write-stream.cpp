#include "write-stream.h"

#include "engine/game-board.h"
#include "save-archive.h"
#include "characters/actions/character-action.h"
#include "characters/actions/walkable/walkable-object.h"
#include "characters/actions/walkable/ehasresourceobject.h"
#include "characters/gods/actions/god-action.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "characters/actions/epatrolmoveaction.h"
#include "spawners/banner.h"
#include "gameEvents/invasions/invasion-handler.h"
#include "characters/soldier-banner.h"

WriteStream::WriteStream(const WriteTarget& dst) :
    mDst(dst) {}

void WriteStream::writeFormat(const std::string& format) {
    mFormat = format;
    *this << std::string(format);
    *this << 0;
}

void WriteStream::writeTile(eTile* const tile) {
    *this << bool(tile);
    if(tile) {
        *this << tile->x();
        *this << tile->y();
    }
}

void WriteStream::writeBuilding(eBuilding* const b) {
    const int bid = b ? b->ioID() : -1;
    *this << bid;
}

void WriteStream::writeCharacter(eCharacter* const c) {
    const int bid = c ? c->ioID() : -1;
    *this << bid;
}

void WriteStream::writeCharacterAction(eCharacterAction* const ca) {
    const int bid = ca ? ca->ioID() : -1;
    *this << bid;
}

void WriteStream::writeCharActFunc(eCharacterActionFunction* const caf) {
    const bool hasFinish = caf != nullptr;
    *this << hasFinish;
    if(hasFinish) {
        *this << caf->type();
        SaveArchive ar(*this);
        caf->serialize(ar);
    }
}

void WriteStream::writeGodAct(eGodAct* const ga) {
    const bool hasFinish = ga != nullptr;
    *this << hasFinish;
    if(hasFinish) {
        *this << ga->type();
        SaveArchive ar(*this);
        ga->serialize(ar);
    }
}

void WriteStream::writeDirectionTimes(eDirectionTimes* const d) {
    *this << d->size();
    for(const auto& dt : *d) {
        writeTile(dt.first);
        SaveArchive ar(*this);
        const_cast<eDirectionLastUseTime&>(dt.second).serialize(ar);
    }
}

void WriteStream::writeCity(WorldCity* const c) {
    const int cid = c ? c->ioID() : -1;
    *this << cid;
}

void WriteStream::writeBanner(Banner* const b) {
    const int bid = b ? b->ioID() : -1;
    *this << bid;
}

void WriteStream::writSoldierBanner(SoldierBanner* const b) {
    const int bid = b ? b->ioID() : -1;
    *this << bid;
}

void WriteStream::writeGameEvent(eGameEvent* const e) {
    const int eid = e ? e->ioID() : -1;
    *this << eid;
}

void WriteStream::writeInvasionHandler(eInvasionHandler* const i) {
    const int iid = i ? i->ioID() : -1;
    *this << iid;
}
