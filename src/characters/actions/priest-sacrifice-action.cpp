#include "priest-sacrifice-action.h"

#include "characters/echaracter.h"
#include "characters/edomesticatedanimal.h"
#include "characters/egoat.h"
#include "characters/esheep.h"
#include "characters/ecattle.h"
#include "characters/actions/efollowaction.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/ewaitaction.h"
#include "buildings/sanctuaries/etemplealtarbuilding.h"
#include "engine/game-board.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "walkable/walkable-object.h"

PriestSacrificeAction::PriestSacrificeAction(eCharacter* const c,
                                             eTempleAltarBuilding* const altar) :
    eActionWithComeback(c, eCharActionType::priestSacrificeAction),
    mAltar(altar) {}

PriestSacrificeAction::PriestSacrificeAction(eCharacter* const c) :
    PriestSacrificeAction(c, nullptr) {}

PriestSacrificeAction::~PriestSacrificeAction() {
    if(mTargetAnimal) {
        mTargetAnimal->setBusy(false);
        mTargetAnimal->resumeAction();
    }
    if(mGhost) mGhost->kill();
}

eDomesticatedAnimal* PriestSacrificeAction::findAnimal() const {
    if(!mAltar) return nullptr;
    auto& b = board();
    const auto origin = mAltar->centerTile();
    const int ox = origin->x();
    const int oy = origin->y();
    const int dist = 200;
    for(int dy = -dist; dy <= dist; dy++) {
        for(int dx = -dist; dx <= dist; dx++) {
            const auto t = b.tile(ox + dx, oy + dy);
            if(!t) continue;
            for(const auto& c : t->characters()) {
                const auto ct = c->type();
                if(ct != eCharacterType::goat &&
                   ct != eCharacterType::sheep &&
                   ct != eCharacterType::cattle1 &&
                   ct != eCharacterType::cattle2 &&
                   ct != eCharacterType::cattle3) continue;
                if(c->busy()) continue;
                return static_cast<eDomesticatedAnimal*>(c.get());
            }
        }
    }
    return nullptr;
}

bool PriestSacrificeAction::decide() {
    if(!mAltar) {
        setState(eCharacterActionState::finished);
        return true;
    }
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    switch(mStage) {
    case ePriestSacrificeStage::idle:
        toFindAnimal();
        break;
    case ePriestSacrificeStage::findingAnimal:
        toFindAnimal();
        break;
    case ePriestSacrificeStage::movingToAnimal:
        toAtAnimal();
        break;
    case ePriestSacrificeStage::atAnimal:
        toReturning();
        break;
    case ePriestSacrificeStage::returning:
        toIdle();
        break;
    }
    return true;
}

void PriestSacrificeAction::toFindAnimal() {
    const auto a = findAnimal();
    if(!a) {
        mStage = ePriestSacrificeStage::findingAnimal;
        wait(eNumbers::sDayLength * 4);
        return;
    }
    toMovingToAnimal(a);
}

void PriestSacrificeAction::toMovingToAnimal(eDomesticatedAnimal* const a) {
    mStage = ePriestSacrificeStage::movingToAnimal;
    mTargetAnimal = a;
    a->setBusy(true);
    a->pauseAction();

    const auto c = character();
    c->setActionType(eCharacterActionType::walk);

    const stdptr<PriestSacrificeAction> tptr(this);
    const stdptr<eDomesticatedAnimal> aptr(a);
    const auto move = e::make_shared<eMoveToAction>(c);
    move->setFindFailAction([tptr, aptr]() {
        if(aptr) {
            aptr->setBusy(false);
            aptr->resumeAction();
        }
        if(!tptr) return;
        tptr->mTargetAnimal = nullptr;
        tptr->mStage = ePriestSacrificeStage::findingAnimal;
    });
    move->start(a->tile(), WalkableObject::sCreateDefault());
    setCurrentAction(move);
}

void PriestSacrificeAction::toAtAnimal() {
    mStage = ePriestSacrificeStage::atAnimal;
    const auto c = character();
    c->setActionType(eCharacterActionType::stand);

    // spawn ghost animal following priest back
    if(mTargetAnimal && !mGhost) {
        auto& b = board();
        const auto animalType = mTargetAnimal->type();
        stdsptr<eCharacter> ghost;
        switch(animalType) {
        case eCharacterType::goat:
            ghost = e::make_shared<eGoat>(b);
            break;
        case eCharacterType::cattle1:
        case eCharacterType::cattle2:
        case eCharacterType::cattle3: {
            ghost = eCharacter::sCreate(animalType, b);
        } break;
        default:
            ghost = e::make_shared<eSheep>(b);
            break;
        }
        if(ghost) {
            ghost->setBusy(true);
            ghost->setBothCityIds(c->cityId());
            ghost->changeTile(mTargetAnimal->tile());
            const auto follow = e::make_shared<eFollowAction>(c, ghost.get());
            ghost->setAction(follow);
            mGhost = ghost.get();
        }
        mTargetAnimal->setBusy(false);
        mTargetAnimal->resumeAction();
        mTargetAnimal = nullptr;
    }

    const auto finish = std::make_shared<ePrSA_atAnimalFinish>(board(), this);
    const auto w = e::make_shared<eWaitAction>(c);
    w->setFinishAction(finish);
    w->setFailAction(finish);
    w->setTime(eNumbers::sDayLength * 2);
    setCurrentAction(w);
}

void PriestSacrificeAction::toReturning() {
    mStage = ePriestSacrificeStage::returning;
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    goBack(mAltar.get(), WalkableObject::sCreateDefault());
}

void PriestSacrificeAction::toIdle() {
    if(mGhost) {
        mGhost->kill();
        mGhost = nullptr;
    }
    if(mAltar) {
        const auto animalSacType = eSacrifice::sheep;
        mAltar->startSacrifice(animalSacType);
    }
    mStage = ePriestSacrificeStage::idle;
    wait(eNumbers::sDayLength * eNumbers::sPriestSacrificeRecurringSpawnDays);
}

void PriestSacrificeAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.buildingAsField("altar", &board(), mAltar);
    ar.characterAsField("targetAnimal", &board(), mTargetAnimal);
    ar.characterAsField("ghost", &board(), mGhost);
    ar.field("stage", mStage, ePriestSacrificeStage::idle);
    ar.field("spawnTimer", mSpawnTimer, 0);
}

void PriestSacrificeAction::resumeFromSavedState() {
    switch(mStage) {
    case ePriestSacrificeStage::idle:
    case ePriestSacrificeStage::findingAnimal:
        toFindAnimal();
        break;
    case ePriestSacrificeStage::movingToAnimal:
        if(mTargetAnimal) {
            toMovingToAnimal(mTargetAnimal.get());
        } else {
            toFindAnimal();
        }
        break;
    case ePriestSacrificeStage::atAnimal:
        toAtAnimal();
        break;
    case ePriestSacrificeStage::returning:
        toReturning();
        break;
    }
}
