#include "esoldier.h"

#include "engine/e-game-board.h"
#include "soldier-banner.h"
#include "actions/soldier-action.h"
#include "fileIO/esavearchive.h"

eSoldier::eSoldier(eGameBoard& board,
                   const eCharTexs charTexs,
                   const eCharacterType type) :
    eFightingPatroler(board, charTexs, type),
    eFightingCharacter(this) {
    board.registerSoldier(this);
}

eSoldier::~eSoldier() {
    auto& brd = ownerBoard();
    brd.unregisterSoldier(this);
    setBanner(nullptr);
}
SoldierBanner* eSoldier::banner() const {
    return mBanner;
}

void eSoldier::setBanner(SoldierBanner* const b) {
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

void eSoldier::serializeFields(eSaveArchive& ar) {
    eFightingPatroler::serializeFields(ar);
    eFightingCharacter::serializeFields(ar);
    ar.soldierBannerField("banner", &getBoard(), mBanner);
}

eSoldierAction *eSoldier::soldierAction() const {
    const auto a = action();
    return dynamic_cast<eSoldierAction*>(a);
}
