#include "esoldier.h"

#include "engine/egameboard.h"
#include "esoldierbanner.h"
#include "actions/esoldieraction.h"

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
        brd.rockThrowerKilled(cid);
        break;
    case eCharacterType::hoplite:
        brd.hopliteKilled(cid);
        break;
    case eCharacterType::horseman:
        brd.horsemanKilled(cid);
        break;
    default:
        break;
    }
}

void eSoldier::read(eReadStream& src) {
    eFightingPatroler::read(src);
    eFightingCharacter::read(src);
    auto& board = getBoard();
    src.readSoldierBanner(&board, [this](const stdsptr<eSoldierBanner>& b) {
        mBanner = b;
    });
}

void eSoldier::write(eWriteStream& dst) const {
    eFightingPatroler::write(dst);
    eFightingCharacter::write(dst);
    dst.writeSoldierBanner(mBanner);
}

eSoldierAction *eSoldier::soldierAction() const {
    const auto a = action();
    return dynamic_cast<eSoldierAction*>(a);
}
