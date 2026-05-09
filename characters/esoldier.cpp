#include "esoldier.h"

#include "engine/e-game-board.h"
#include "esoldierbanner.h"
#include "actions/esoldieraction.h"
#include "fileIO/esavearchive.h"

eSoldier::eSoldier(eGameBoard& board,
                   const eCharTexs charTexs,
                   const eCharacterType type) :
    eFightingPatroler(board, charTexs, type),
    eFightingCharacter(this) {
    board.registerSoldier(this);
}

eSoldier::~eSoldier() {
    auto& brd = getBoard();
    brd.unregisterSoldier(this);
    setBanner(nullptr);
}
eSoldierBanner* eSoldier::banner() const {
    return mBanner;
}

void eSoldier::setBanner(eSoldierBanner* const b) {
    if(mBanner) {
        mBanner->removeSoldier(this);
    }
    mBanner = b;
    if(mBanner) {
        mBanner->addSoldier(this);
    }
}

void eSoldier::beingKilled() {
    if(mBanner) mBanner->decCount();
    setBanner(nullptr);
    const auto cid = cityId();
    const auto ocid = onCityId();
    if(cid != ocid) return;
    auto& brd = getBoard();
    const auto ct = type();
    switch(ct) {
    case eCharacterType::rockThrower:
    case eCharacterType::archerPoseidon:
        brd.rockThrowerKilled(cid);
        break;
    case eCharacterType::hoplite:
    case eCharacterType::hoplitePoseidon:
        brd.hopliteKilled(cid);
        break;
    case eCharacterType::horseman:
    case eCharacterType::chariotPoseidon:
        brd.horsemanKilled(cid);
        break;
    default:
        break;
    }
}

void eSoldier::read(eReadStream& src) {
    eFightingPatroler::read(src);
    eFightingCharacter::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eSoldier::write(eWriteStream& dst) const {
    eFightingPatroler::write(dst);
    eFightingCharacter::write(dst);
    eSaveArchive ar(dst);
    const_cast<eSoldier*>(this)->serialize(ar);
}

void eSoldier::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        auto& board = getBoard();
        ar.readStream().readSoldierBanner(&board, [this](const stdsptr<eSoldierBanner>& b) {
            mBanner = b;
        });
    } else {
        ar.writeStream().writeSoldierBanner(mBanner);
    }
}

eSoldierAction *eSoldier::soldierAction() const {
    const auto a = action();
    return dynamic_cast<eSoldierAction*>(a);
}
