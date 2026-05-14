#include "eshepherdaction.h"

#include "characters/esheep.h"
#include "characters/egoat.h"

#include "ewaitaction.h"
#include "emovetoaction.h"

#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "engine/e-game-board.h"
#include "erand.h"

#include <cstdlib>
#include <vector>

eShepherdAction::eShepherdAction(
        eShepherBuildingBase* const shed,
        eResourceCollectorBase* const c,
        const eCharacterType animalType) :
    eActionWithComeback(c, eCharActionType::shepherdAction),
    mAnimalType(animalType),
    mCharacter(c),
    mShed(shed) {}

eShepherdAction::eShepherdAction(eCharacter* const c) :
    eShepherdAction(nullptr, static_cast<eResourceCollectorBase*>(c),
                    eCharacterType::sheep) {}


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

bool eShepherdAction::decide() {
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

void eShepherdAction::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eShepherdAction::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eShepherdAction*>(this)->serialize(ar);
}

void eShepherdAction::serialize(eSaveArchive& ar) {
    eActionWithComeback::serialize(ar);
    ar.field("mAnimalType", mAnimalType);
    ar.characterAs(&board(), mCharacter);
    ar.buildingAs(&board(), mShed);
    ar.field("mFinishOnce", mFinishOnce);
    ar.field("mGroomed", mGroomed);
    ar.field("mNoResource", mNoResource);
    ar.characterAs(&board(), mTargetAnimal);
}

eDomesticatedAnimal* eShepherdAction::findAnimal(
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

void eShepherdAction::reserveAnimal(eDomesticatedAnimal* const a) {
    if(!a) return;
    a->setBusy(true);
    a->pauseAction();
    const auto w = e::make_shared<eWaitAction>(a);
    a->setAction(w);
    a->setActionType(eCharacterActionType::lay);
    mTargetAnimal = a;
}

void eShepherdAction::releaseAnimal(eDomesticatedAnimal* const a) {
    if(!a) return;
    a->setBusy(false);
    a->resumeAction();
    if(mTargetAnimal.get() == a) mTargetAnimal = nullptr;
}

bool eShepherdAction::findResourceDecision() {
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
    reserveAnimal(animal);
    const stdptr<eDomesticatedAnimal> aptr(animal);
    const stdptr<eShepherdAction> tptr(this);
    const auto a = makeFindAnimalMove();
    a->setFindFailAction([tptr, aptr]() {
        if(!tptr) return;
        tptr.get()->releaseAnimal(aptr.get());
        tptr.get()->mNoResource = true;
    });
    a->start(animal->tile());
    setCurrentAction(a);
    return true;
}

stdsptr<eMoveToAction> eShepherdAction::makeFindAnimalMove() {
    const stdptr<eShepherdAction> tptr(this);
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

void eShepherdAction::collectDecision(eDomesticatedAnimal* const a) {
    if(!mCharacter) return;
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
    wait->setTime(eNumbers::sShepherdGoatherdCollectTime);
    setCurrentAction(wait);
}

void eShepherdAction::groomDecision(eDomesticatedAnimal* const a) {
    if(!mCharacter) return;
    mLastAnimal = a;
    a->setBusy(true);
    mCharacter->setActionType(eCharacterActionType::fight);
    const auto finish = std::make_shared<eSA_groomDecisionFinish>(
                            board(), this, a);
    const auto wait = e::make_shared<eWaitAction>(mCharacter);
    wait->setFailAction(finish);
    wait->setFinishAction(finish);
    const auto deleteFail = std::make_shared<eSA_groomDecisionDeleteFail>(
                                board(), a);
    wait->setDeleteFailAction(deleteFail);
    wait->setTime(eNumbers::sShepherdGoatherdGroomTime);
    setCurrentAction(wait);
}

void eShepherdAction::goBackDecision() {
    if(!mCharacter || !mShed) return;
    if(mCharacter->collected()) {
        mCharacter->setActionType(eCharacterActionType::carry);
    } else {
        mCharacter->setActionType(eCharacterActionType::walk);
    }

    goBack(mShed, eWalkableObject::sCreateDefault());
}

void eShepherdAction::waitDecision() {
    if(!mCharacter) return;
    wait(eNumbers::sShepherdGoatherdWaitTime);
}
