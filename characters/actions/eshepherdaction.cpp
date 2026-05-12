#include "eshepherdaction.h"

#include "characters/esheep.h"
#include "characters/egoat.h"

#include "ewaitaction.h"
#include "emovetoaction.h"

#include "enumbers.h"
#include "fileIO/esavearchive.h"

namespace {
bool sUseTileChance(eTileBase* const tile, const int div, const int pass) {
    const unsigned long long x = static_cast<unsigned long long>(tile->x());
    const unsigned long long y = static_cast<unsigned long long>(tile->y());
    return ((x*73856093) ^ (y*19349663)) % div ==
           static_cast<unsigned long long>(pass);
}
}

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
                                  eCollectType& collType) {
    const auto cs = tile->characters();
    for(const auto& c : cs) {
        const auto t = c->type();
        if(t != type) continue;
        const auto s = static_cast<eDomesticatedAnimal*>(c.get());
        if(s->busy()) return nullptr;
        if(s->canCollect()) {
            collType = eCollectType::collect;
        } else {
            collType = eCollectType::groom;
        }
        return s;
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
        if(const auto a = tryToCollect(t, mAnimalType, collType)) {
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
    if(ar.reading()) {
        ar.readStream().readCharacter(&board(), [this](eCharacter* const c) {
            mCharacter = static_cast<eResourceCollectorBase*>(c);
        });
        ar.readStream().readBuilding(&board(), [this](eBuilding* const b) {
            mShed = static_cast<eShepherBuildingBase*>(b);
        });
    } else {
        ar.writeStream().writeCharacter(mCharacter);
        ar.writeStream().writeBuilding(mShed);
    }
    ar.field("mFinishOnce", mFinishOnce);
    ar.field("mGroomed", mGroomed);
    ar.field("mNoResource", mNoResource);
}

bool eShepherdAction::findResourceDecision() {
    if(!mCharacter) {
        mGroomedThisTrip = 0;
        return true;
    }
    const stdptr<eShepherdAction> tptr(this);

    const auto aType = mAnimalType;
    const stdptr<eDomesticatedAnimal> lastAnimal = mLastAnimal;
    // Predicates run on pathfinder worker thread; keep them stateless.
    const auto readyRand = [aType, lastAnimal](eTileBase* const tile) {
        return tile->hasCharacter([&](const eCharacterBase& c) {
            if(c.type() != aType || c.busy()) return false;
            if(static_cast<const eDomesticatedAnimal*>(&c) == lastAnimal.get()) return false;
            if(!static_cast<const eDomesticatedAnimal*>(&c)->canCollect()) return false;
            return !sUseTileChance(tile, 3, 0);
        });
    };
    const auto readyAny = [aType, lastAnimal](eTileBase* const tile) {
        return tile->hasCharacter([&](const eCharacterBase& c) {
            if(c.type() != aType || c.busy()) return false;
            if(static_cast<const eDomesticatedAnimal*>(&c) == lastAnimal.get()) return false;
            return static_cast<const eDomesticatedAnimal*>(&c)->canCollect();
        });
    };
    const auto hha = [aType, lastAnimal](eTileBase* const tile) {
        return tile->hasCharacter([&](const eCharacterBase& c) {
            if(c.type() != aType || c.busy()) return false;
            if(static_cast<const eDomesticatedAnimal*>(&c) == lastAnimal.get()) return false;
            return sUseTileChance(tile, 2, 0);
        });
    };

    const auto a = makeFindAnimalMove();
    const auto findFailFunc = [tptr, readyAny, hha]() {
        if(!tptr) return;
        const auto action = tptr.get();
        if(!action->mCharacter) return;
        const auto a2 = action->makeFindAnimalMove();
        a2->setFindFailAction([tptr, hha]() {
            if(!tptr) return;
            const auto action = tptr.get();
            if(!action->mCharacter) return;
            const auto a3 = action->makeFindAnimalMove();
            a3->setFindFailAction([tptr]() {
                if(tptr) tptr.get()->mNoResource = true;
            });
            a3->start(hha);
            action->setCurrentAction(a3);
        });
        a2->start(readyAny);
        action->setCurrentAction(a2);
    };
    a->setFindFailAction(findFailFunc);
    a->start(readyRand);
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
    const stdptr<eCharacterAction> tptr(this);
    const stdptr<eDomesticatedAnimal> aa(a);
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
    const stdptr<eCharacterAction> tptr(this);
    const stdptr<eDomesticatedAnimal> aa(a);
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
    mGroomedThisTrip = 0;
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
