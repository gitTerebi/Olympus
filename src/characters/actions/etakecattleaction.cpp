#include "etakecattleaction.h"

#include "buildings/ecorral.h"

#include "characters/echaracter.h"
#include "characters/ecattle.h"
#include "efollowaction.h"
#include "emovetoaction.h"
#include "fileIO/esavearchive.h"

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
            goBack(WalkableObject::sCreateDefault());
        }
    }
    eActionWithComeback::increment(by);
}

void eTakeCattleAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.field("stage", mStage);
    ar.buildingAsField("corral", &board(), mCorral);
    ar.field("noCattle", mNoCattle);
}

void eTakeCattleAction::resumeFromSavedState() {
    switch(mStage) {
    case eTakeCattleActionStage::none:
        eActionWithComeback::resumeFromSavedState();
        break;
    case eTakeCattleActionStage::get:
        goGetCattle();
        break;
    case eTakeCattleActionStage::goBack:
        goBack(WalkableObject::sCreateDefault());
        break;
    }
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
            goBack(WalkableObject::sCreateDefault());
        }
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(40);
    a->start(hha);
    setCurrentAction(a);
}
