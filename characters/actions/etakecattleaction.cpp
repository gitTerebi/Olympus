#include "etakecattleaction.h"

#include "buildings/ecorral.h"

#include "characters/echaracter.h"
#include "characters/ecattle.h"
#include "efollowaction.h"
#include "emovetoaction.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

eTakeCattleAction::eTakeCattleAction(
        eCharacter* const c, eCorral* const cc) :
    eActionWithComeback(c, eCharActionType::takeCattleAction),
    mCorral(cc) {}

bool eTakeCattleAction::decide() {
    switch(mStage) {
    case eTakeCattleActionStage::none:
    case eTakeCattleActionStage::get: {
        mStage = eTakeCattleActionStage::get;
        goGetCattle();
    } break;
    case eTakeCattleActionStage::goBack: {
        if(mCorral && !mNoCattle) mCorral->addCattle();
        setState(eCharacterActionState::finished);
    } break;
    }
    return true;
}

void eTakeCattleAction::increment(const int by) {
    if(mStage == eTakeCattleActionStage::get) {
        const auto c = character();
        const auto tile = c->tile();
        const auto& chars = tile->characters();
        for(const auto& cc : chars) {
            const auto cct = cc->type();
            if(cct != eCharacterType::cattle3) continue;
            const auto fa = e::make_shared<eFollowAction>(c, cc.get());
            cc->setAction(fa);

            mStage = eTakeCattleActionStage::goBack;
            goBack(eWalkableObject::sCreateDefault());
        }
    }
    eActionWithComeback::increment(by);
}

void eTakeCattleAction::read(eReadStream& src) {
    eActionWithComeback::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eTakeCattleAction::write(eWriteStream& dst) const {
    eActionWithComeback::write(dst);
    eSaveArchive ar(dst);
    const_cast<eTakeCattleAction*>(this)->serialize(ar);
}

void eTakeCattleAction::serialize(eSaveArchive& ar) {
    ar.field("mStage", mStage);
    if(ar.reading()) {
        auto& board = this->board();
        ar.readStream().readBuilding(&board, [this](eBuilding* const b) {
            mCorral = static_cast<eCorral*>(b);
        });
    } else {
        ar.writeStream().writeBuilding(mCorral);
    }
    ar.field("mNoCattle", mNoCattle);
}

void eTakeCattleAction::serializeJson(eJsonArchive& ar) {
    eActionWithComeback::serializeJson(ar);
    ar.field("mStage", mStage);
    if(ar.writing()) {
        eBuilding* raw = mCorral.get();
        ar.buildingRef("mCorral", raw, board());
    } else {
        ar.buildingRef("mCorral", [this](eBuilding* b) {
            mCorral = static_cast<eCorral*>(b);
        }, board());
    }
    ar.field("mNoCattle", mNoCattle);
}

bool hasCattle(eTileBase* const tile, const eCharacterType t) {
    return tile->hasCharacter([t](const eCharacterBase& c) {
        return c.type() == t;
    });
}

void eTakeCattleAction::goGetCattle() {
    const auto c = character();

    auto aType = std::make_shared<eCharacterType>();

    const auto foundAny = std::make_shared<bool>(false);

    const auto hha = [aType, foundAny](eTileBase* const tile) {
        const bool c3 = hasCattle(tile, eCharacterType::cattle3);
        if(c3) {
            *foundAny = true;
            return true;
        }
        const bool c2 = hasCattle(tile, eCharacterType::cattle2);
        if(c2) {
            *foundAny = true;
        } else {
            const bool c1 = hasCattle(tile, eCharacterType::cattle1);
            if(c1) {
                *foundAny = true;
            }
        }
        return false;
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    const stdptr<eTakeCattleAction> tptr(this);
    const stdptr<eCharacter> cptr(c);
    a->setFoundAction([tptr, this, cptr, c]() {
        if(tptr) {
            mNoCattle = false;
            mCorral->setNoCattle(false);
        }
        if(cptr) c->setActionType(eCharacterActionType::walk);
    });
    const auto findFailFunc = [tptr, this, foundAny]() {
        if(tptr) {
            mNoCattle = true;
            mCorral->setNoCattle(!*foundAny);
            goBack(eWalkableObject::sCreateDefault());
        }
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(40);
    a->start(hha);
    setCurrentAction(a);
}
