#include "shepherd-action.h"

#include "characters/esheep.h"
#include "characters/egoat.h"

#include "ewaitaction.h"
#include "emovetoaction.h"

#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "engine/game-board.h"
#include "erand.h"

#include <cstdlib>
#include <vector>

ShepherdAction::ShepherdAction(
        eShepherBuildingBase* const shed,
        eResourceCollectorBase* const c,
        const eCharacterType animalType) :
    eActionWithComeback(c, eCharActionType::shepherdAction),
    mAnimalType(animalType),
    mCharacter(c),
    mShed(shed) {}

ShepherdAction::ShepherdAction(eCharacter* const c) :
    ShepherdAction(nullptr, static_cast<eResourceCollectorBase*>(c),
                    eCharacterType::sheep) {}

ShepherdAction::~ShepherdAction() {
    releaseAnimal(mTargetAnimal.get());
}

enum class eCollectType {
    groom,
    collect
};

eDomesticatedAnimal* tryToCollect(eTile* const tile,
                                  const eCharacterType type,
                                  eCollectType& collType,
                                  eDomesticatedAnimal* const target) {
    if(target && target->tile()) {
        const auto tt = target->tile();
        if(std::abs(tt->x() - tile->x()) <= 1 &&
           std::abs(tt->y() - tile->y()) <= 1) {
            collType = target->canCollect() ?
                       eCollectType::collect :
                       eCollectType::groom;
            return target;
        }
    }
    std::vector<eTile*> tiles{tile};
    for(const auto o : {eOrientation::topRight, eOrientation::right,
                        eOrientation::bottomRight, eOrientation::bottom,
                        eOrientation::bottomLeft, eOrientation::left,
                        eOrientation::topLeft, eOrientation::top}) {
        if(const auto t = tile->neighbour<eTile>(o)) tiles.push_back(t);
    }
    for(const auto tt : tiles) {
        const auto cs = tt->characters();
        for(const auto& c : cs) {
            if(c->type() != type) continue;
            const auto s = static_cast<eDomesticatedAnimal*>(c.get());
            if(s->busy() || !s->canCollect()) continue;
            collType = eCollectType::collect;
            return s;
        }
    }
    for(const auto tt : tiles) {
        const auto cs = tt->characters();
        for(const auto& c : cs) {
            if(c->type() != type) continue;
            const auto s = static_cast<eDomesticatedAnimal*>(c.get());
            if(s->busy()) continue;
            collType = eCollectType::groom;
            return s;
        }
    }
    return nullptr;
}

bool ShepherdAction::decide() {
    if(!mCharacter || !mShed) {
        setState(eCharacterActionState::finished);
        return true;
    }
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    const auto t = mCharacter->tile();

    const int coll = mCharacter->collected();

    const SDL_Point p{t->x(), t->y()};
    const auto rect = mShed->tileRect();
    const bool inShed = SDL_PointInRect(&p, &rect);

    if(coll > 0) {
        if(inShed) {
            const auto rType = mShed->resourceType();
            mShed->shepherdDelivered(rType, coll);
            mCharacter->incCollected(-coll);

            if(mFinishOnce) {
                setState(eCharacterActionState::finished);
                return true;
            } else {
                waitDecision();
            }
        } else {
            goBackDecision();
        }
    } else {
        eCollectType collType;
        if(const auto a = tryToCollect(t, mAnimalType, collType,
                                       mTargetAnimal.get())) {
            switch(collType) {
            case eCollectType::collect:
                collectDecision(a);
                break;
            case eCollectType::groom:
                groomDecision(a);
                break;
            }
        } else if(inShed) {
            const int res = mShed->resource();
            if(res >= mShed->maxResource() ||
               !mShed->enabled()) {
                waitDecision();
            } else {
                if(mNoResource) {
                    mNoResource = false;
                    waitDecision();
                } else {
                    findResourceDecision();
                }
            }
        } else if(mNoResource) {
            mNoResource = false;
            goBackDecision();
        } else {
            findResourceDecision();
        }
    }
    return true;
}

void ShepherdAction::increment(const int by) {
    if((mStage == eShepherdActionStage::collecting ||
        mStage == eShepherdActionStage::grooming ||
        mStage == eShepherdActionStage::waiting) &&
       mWaitRemaining > 0) {
        mWaitRemaining -= by;
        if(mWaitRemaining < 0) mWaitRemaining = 0;
    }
    eActionWithComeback::increment(by);
}

void ShepherdAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.field("animalType", mAnimalType);
    ar.characterAsField("shepherd", &board(), mCharacter);
    const auto shepherd = static_cast<eResourceCollectorBase*>(character());
    int carriedResourceCount = shepherd ? shepherd->collected() : 0;
    ar.field("carriedResourceCount", carriedResourceCount, 0);
    ar.buildingAsField("shed", &board(), mShed);
    ar.field("finishOnce", mFinishOnce);
    ar.field("groomed", mGroomed);
    ar.field("noResource", mNoResource);
    ar.field("stage", mStage, eShepherdActionStage::idle);
    ar.field("waitRemaining", mWaitRemaining, 0);
    ar.characterAsField("lastAnimal", &board(), mLastAnimal);
    ar.characterAsField("targetAnimal", &board(), mTargetAnimal);
    if(ar.reading()) {
        const stdptr<ShepherdAction> tptr(this);
        ar.addPostFunc([tptr, carriedResourceCount]() {
            if(!tptr) return;
            const auto a = tptr.get();
            const auto shepherd = static_cast<eResourceCollectorBase*>(
                                      a->character());
            if(!shepherd) return;
            a->mCharacter = shepherd;
            shepherd->incCollected(carriedResourceCount -
                                   shepherd->collected());
        }, "shepherdCarriedResourceCount");
    }
}

void ShepherdAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void ShepherdAction::rebuildCurrentStage() {
    if(!mCharacter || !mShed) {
        setState(eCharacterActionState::finished);
        return;
    }
    switch(mStage) {
    case eShepherdActionStage::findingAnimal:
        rebuildFindAnimal();
        return;
    case eShepherdActionStage::collecting:
        if(mTargetAnimal) return collectDecision(mTargetAnimal.get());
        mStage = eShepherdActionStage::idle;
        decide();
        return;
    case eShepherdActionStage::grooming:
        if(mTargetAnimal) return groomDecision(mTargetAnimal.get());
        mStage = eShepherdActionStage::idle;
        decide();
        return;
    case eShepherdActionStage::goingBack:
        goBackDecision();
        return;
    case eShepherdActionStage::waiting:
        if(mWaitRemaining > 0) {
            wait(mWaitRemaining);
        } else {
            mStage = eShepherdActionStage::idle;
            decide();
        }
        return;
    case eShepherdActionStage::idle:
        eActionWithComeback::resumeFromSavedState();
        return;
    }
}

void ShepherdAction::rebuildFindAnimal() {
    const auto a = mTargetAnimal.get();
    if(!a || !a->tile()) {
        mTargetAnimal = nullptr;
        mNoResource = true;
        mStage = eShepherdActionStage::idle;
        decide();
        return;
    }
    reserveAnimal(a);
    const stdptr<eDomesticatedAnimal> aptr(a);
    const stdptr<ShepherdAction> tptr(this);
    const auto move = makeFindAnimalMove();
    move->setFindFailAction([tptr, aptr]() {
        if(!tptr) return;
        tptr.get()->releaseAnimal(aptr.get());
        tptr.get()->mNoResource = true;
    });
    const auto deleteFail = std::make_shared<eSA_groomDecisionDeleteFail>(
                                board(), a);
    move->setDeleteFailAction(deleteFail);
    move->start(a->tile());
    setCurrentAction(move);
}

eDomesticatedAnimal* ShepherdAction::findAnimal(
        const bool wantShearable) {
    if(!mShed) return nullptr;
    auto& b = board();
    const auto origin = mShed->centerTile();
    const int ox = origin->x();
    const int oy = origin->y();
    const int dist = eNumbers::sShepherdGoatherdMaxDistance;
    std::vector<eDomesticatedAnimal*> candidates;
    eDomesticatedAnimal* lastCandidate = nullptr;
    for(int dy = -dist; dy <= dist; dy++) {
        for(int dx = -dist; dx <= dist; dx++) {
            const int x = ox + dx;
            const int y = oy + dy;
            const auto t = b.tile(x, y);
            if(!t) continue;
            const auto cs = t->characters();
            for(const auto& c : cs) {
                if(c->type() != mAnimalType || c->busy()) continue;
                const auto a = static_cast<eDomesticatedAnimal*>(c.get());
                if(a->canCollect() != wantShearable) continue;
                if(a == mLastAnimal.get()) {
                    lastCandidate = a;
                } else {
                    candidates.push_back(a);
                }
            }
        }
    }
    if(candidates.empty()) return lastCandidate;
    return candidates[eRand::rand() % candidates.size()];
}

void ShepherdAction::reserveAnimal(eDomesticatedAnimal* const a) {
    if(!a) return;
    a->setBusy(true);
    a->pauseAction();
    const auto w = e::make_shared<eWaitAction>(a);
    a->setAction(w);
    a->setActionType(eCharacterActionType::lay);
    mTargetAnimal = a;
}

void ShepherdAction::releaseAnimal(eDomesticatedAnimal* const a) {
    if(!a) return;
    a->setBusy(false);
    a->resumeAction();
    if(mTargetAnimal.get() == a) mTargetAnimal = nullptr;
}

bool ShepherdAction::findResourceDecision() {
    if(!mCharacter) {
        return true;
    }

    auto* animal = findAnimal(true);
    bool shear = true;
    if(!animal) {
        shear = false;
        animal = findAnimal(false);
    }
    if(!animal) {
        mNoResource = true;
        return true;
    }
    mStage = eShepherdActionStage::findingAnimal;
    reserveAnimal(animal);
    const stdptr<eDomesticatedAnimal> aptr(animal);
    const stdptr<ShepherdAction> tptr(this);
    const auto a = makeFindAnimalMove();
    a->setFindFailAction([tptr, aptr]() {
        if(!tptr) return;
        tptr.get()->releaseAnimal(aptr.get());
        tptr.get()->mNoResource = true;
    });
    const auto deleteFail = std::make_shared<eSA_groomDecisionDeleteFail>(
                                board(), animal);
    a->setDeleteFailAction(deleteFail);
    a->start(animal->tile());
    setCurrentAction(a);
    return true;
}

stdsptr<eMoveToAction> ShepherdAction::makeFindAnimalMove() {
    const stdptr<ShepherdAction> tptr(this);
    const auto m = e::make_shared<eMoveToAction>(mCharacter);
    m->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    m->setFoundAction([tptr]() {
        if(!tptr) return;
        const auto action = tptr.get();
        if(!action->mCharacter) return;
        action->mCharacter->setActionType(eCharacterActionType::walk);
    });
    m->setMaxFindDistance(eNumbers::sShepherdGoatherdMaxDistance);
    return m;
}

void ShepherdAction::collectDecision(eDomesticatedAnimal* const a) {
    if(!mCharacter) return;
    mStage = eShepherdActionStage::collecting;
    mTargetAnimal = a;
    if(mWaitRemaining <= 0) {
        mWaitRemaining = eNumbers::sShepherdGoatherdCollectTime;
    }
    mLastAnimal = a;
    a->setBusy(true);
    a->setVisible(false);
    mCharacter->setActionType(eCharacterActionType::collect);
    const auto finish = std::make_shared<eSA_collectDecisionFinish>(
                            board(), this, a);;
    const auto wait = e::make_shared<eWaitAction>(mCharacter);
    wait->setFailAction(finish);
    wait->setFinishAction(finish);
    const auto deleteFail = std::make_shared<eSA_collectDecisionDeleteFail>(
                                board(), a);
    wait->setDeleteFailAction(deleteFail);
    wait->setTime(mWaitRemaining);
    setCurrentAction(wait);
}

void ShepherdAction::groomDecision(eDomesticatedAnimal* const a) {
    if(!mCharacter) return;
    mStage = eShepherdActionStage::grooming;
    mTargetAnimal = a;
    if(mWaitRemaining <= 0) {
        mWaitRemaining = eNumbers::sShepherdGoatherdGroomTime;
    }
    mLastAnimal = a;
    a->setBusy(true);
    mCharacter->setActionType(eCharacterActionType::groom);
    const auto finish = std::make_shared<eSA_groomDecisionFinish>(
                            board(), this, a);
    const auto wait = e::make_shared<eWaitAction>(mCharacter);
    wait->setFailAction(finish);
    wait->setFinishAction(finish);
    const auto deleteFail = std::make_shared<eSA_groomDecisionDeleteFail>(
                                board(), a);
    wait->setDeleteFailAction(deleteFail);
    wait->setTime(mWaitRemaining);
    setCurrentAction(wait);
}

void ShepherdAction::goBackDecision() {
    if(!mCharacter || !mShed) return;
    mStage = eShepherdActionStage::goingBack;
    mWaitRemaining = 0;
    if(mCharacter->collected()) {
        mCharacter->setActionType(eCharacterActionType::carry);
    } else {
        mCharacter->setActionType(eCharacterActionType::walk);
    }

    goBack(mShed, eWalkableObject::sCreateDefault());
}

void ShepherdAction::waitDecision() {
    if(!mCharacter) return;
    mStage = eShepherdActionStage::waiting;
    if(mWaitRemaining <= 0) {
        mWaitRemaining = eNumbers::sShepherdGoatherdWaitTime;
    }
    wait(mWaitRemaining);
}
