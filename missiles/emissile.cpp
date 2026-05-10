#include "emissile.h"

#include "engine/e-game-board.h"
#include "engine/etile.h"
#include "characters/gods/actions/egodaction.h"

#include "erockmissile.h"
#include "egodmissile.h"
#include "earrowmissile.h"
#include "espearmissile.h"
#include "ewavemissile.h"
#include "elavamissile.h"
#include "edustmissile.h"
#include "fileIO/esavearchive.h"
#include "characters/eracinghorse.h"

eMissile::eMissile(eGameBoard& board, const eMissileType type,
                   const std::vector<ePathPoint>& path) :
    mType(type), mBoard(board), mPath(path) {
    mBoard.registerMissile(this);
}

eMissile::~eMissile() {
    mBoard.unregisterMissile(this);
}

void eMissile::incTime(const int by) {
    mTime += by;
    if(mPath.finished()) return;
    mPath.progress(0.025*mSpeed*by);
    const auto& pos = mPath.pos();
    const int tx = pos.fX;
    const int ty = pos.fY;
    const auto t = mBoard.tile(tx, ty);
    changeTile(t);
    const bool f = mPath.finished();
    if(f) {
        if(mFinish) mFinish->act();
        changeTile(nullptr);
    }
}

void eMissile::destroy() {
    changeTile(nullptr);
}

double eMissile::x() const {
    if(mTile) {
        return mPath.pos().fX - mTile->x();
    } else {
        return mPath.pos().fX;
    }
}

double eMissile::y() const {
    if(mTile) {
        return mPath.pos().fY - mTile->y();
    } else {
        return mPath.pos().fY;
    }
}

double eMissile::globalX() const {
    return mPath.pos().fX;
}

double eMissile::globalY() const {
    return mPath.pos().fY;
}

void eMissile::setFinishAction(const stdsptr<eGodAct>& act) {
    mFinish = act;
}

void eMissile::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eMissile::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eMissile*>(this)->serialize(ar);
}

void eMissile::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        mPath.read(ar.readStream());
    } else {
        mPath.write(ar.writeStream());
    }
    ar.field("mTime", mTime);
    ar.field("mSpeed", mSpeed);
    if(ar.reading()) {
        mFinish = ar.readStream().readGodAct(mBoard);
        const auto t = ar.readStream().readTile(mBoard);
        changeTile(t);
    } else {
        ar.writeStream().writeGodAct(mFinish.get());
        ar.writeStream().writeTile(mTile);
    }
}

stdsptr<eMissile> eMissile::sCreate(
        eGameBoard& brd, const eMissileType type) {
    switch(type) {
    case eMissileType::rock:
        return e::make_shared<eRockMissile>(brd);
    case eMissileType::god:
        return e::make_shared<eGodMissile>(brd);
    case eMissileType::arrow:
        return e::make_shared<eArrowMissile>(brd);
    case eMissileType::spear:
        return e::make_shared<eSpearMissile>(brd);
    case eMissileType::wave:
        return e::make_shared<eWaveMissile>(brd);
    case eMissileType::lava:
        return e::make_shared<eLavaMissile>(brd);
    case eMissileType::dust:
        return e::make_shared<eDustMissile>(brd);
    case eMissileType::racingHorse:
        return e::make_shared<eRacingHorse>(brd);
    }
    return nullptr;
}

void eMissile::changeTile(eTile* const t) {
    if(t != mTile) {
        const auto r = ref<eMissile>();
        if(mTile) {
            mTile->removeMissile(r);
        }
        mTile = t;
        if(mTile) {
            mTile->addMissile(r);
        }
    }
}
