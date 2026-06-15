#include "missile.h"

#include "engine/game-board.h"
#include "engine/etile.h"
#include "characters/gods/actions/god-action.h"

#include "rock-missile.h"
#include "god-missile.h"
#include "arrow-missile.h"
#include "spear-missile.h"
#include "wave-missile.h"
#include "lava-missile.h"
#include "dust-missile.h"
#include "fileIO/save-archive.h"
#include "characters/eracinghorse.h"

Missile::Missile(GameBoard& board, const MissileType type,
                   const std::vector<PathPoint>& path) :
    mType(type), mBoard(board), mPath(path) {
    mBoard.registerMissile(this);
}

Missile::~Missile() {
    board().unregisterMissile(this);
}

GameBoard& Missile::board() const {
    if(mTile) return mTile->board();
    return mBoard;
}

void Missile::incTime(const int by) {
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

void Missile::destroy() {
    changeTile(nullptr);
}

double Missile::x() const {
    if(mTile) {
        return mPath.pos().fX - mTile->x();
    } else {
        return mPath.pos().fX;
    }
}

double Missile::y() const {
    if(mTile) {
        return mPath.pos().fY - mTile->y();
    } else {
        return mPath.pos().fY;
    }
}

double Missile::globalX() const {
    return mPath.pos().fX;
}

double Missile::globalY() const {
    return mPath.pos().fY;
}

void Missile::setFinishAction(const stdsptr<eGodAct>& act) {
    mFinish = act;
}

void Missile::serialize(SaveArchive& ar) {
    serializeFields(ar);
}

void Missile::serializeFields(SaveArchive& ar) {
    ar.objectField("path", mPath);
    ar.field("time", mTime);
    ar.field("speed", mSpeed);
    ar.godActField("finishAction", mBoard, mFinish);
    if(ar.reading()) {
        eTile* t = nullptr;
        ar.tileField("tile", mBoard, t);
        changeTile(t);
    } else {
        ar.tileField("tile", mBoard, mTile);
    }
}

stdsptr<Missile> Missile::sCreate(
        GameBoard& brd, const MissileType type) {
    switch(type) {
    case MissileType::rock:
        return e::make_shared<RockMissile>(brd);
    case MissileType::god:
        return e::make_shared<GodMissile>(brd);
    case MissileType::arrow:
        return e::make_shared<ArrowMissile>(brd);
    case MissileType::spear:
        return e::make_shared<SpearMissile>(brd);
    case MissileType::wave:
        return e::make_shared<WaveMissile>(brd);
    case MissileType::lava:
        return e::make_shared<LavaMissile>(brd);
    case MissileType::dust:
        return e::make_shared<DustMissile>(brd);
    case MissileType::racingHorse:
        return e::make_shared<eRacingHorse>(brd);
    }
    return nullptr;
}

void Missile::changeTile(eTile* const t) {
    if(t != mTile) {
        const auto r = ref<Missile>();
        if(mTile) {
            mTile->removeMissile(r);
        }
        mTile = t;
        if(mTile) {
            mTile->addMissile(r);
        }
    }
}
