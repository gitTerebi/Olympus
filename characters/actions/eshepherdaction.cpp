#include "eshepherdaction.h"

#include "characters/esheep.h"
#include "characters/egoat.h"

#include "ewaitaction.h"
#include "emovetoaction.h"

#include "enumbers.h"
#include "erand.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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

void eShepherdAction::serializeJson(eJsonArchive& ar) {
    eActionWithComeback::serializeJson(ar);
    ar.field("mAnimalType", mAnimalType);
    if(ar.writing()) {
        eCharacter* rawCharacter = mCharacter;
        ar.characterRef("mCharacter", rawCharacter, board());
        eBuilding* rawShed = mShed;
        ar.buildingRef("mShed", rawShed, board());
    } else {
        ar.characterRef("mCharacter", [this](eCharacter* c) {
            mCharacter = dynamic_cast<eResourceCollectorBase*>(c);
        }, board());
        ar.buildingRef("mShed", [this](eBuilding* b) {
            mShed = dynamic_cast<eShepherBuildingBase*>(b);
        }, board());
    }
    ar.field("mFinishOnce", mFinishOnce);
    ar.field("mGroomed", mGroomed);
    ar.field("mNoResource", mNoResource);
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
    if(!mCharacter) return true;
    const stdptr<eShepherdAction> tptr(this);

    const auto aType = mAnimalType;
    const stdptr<eDomesticatedAnimal> lastAnimal = mLastAnimal;
    // THREADING: predicates run on pathfinder worker thread — never capture 'this' or mutate shared state here
    const auto readyRand = [aType, lastAnimal](eTileBase* const tile) {
        return tile->hasCharacter([&](const eCharacterBase& c) {
            if(c.type() != aType || c.busy()) return false;
            if(static_cast<const eDomesticatedAnimal*>(&c) == lastAnimal.get()) return false;
            if(!static_cast<const eDomesticatedAnimal*>(&c)->canCollect()) return false;
            return eRand::rand() % 3 != 0;
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
            return eRand::rand() % 2 == 0;
        });
    };

    const auto makeMove = [tptr, this]() {
        const auto m = e::make_shared<eMoveToAction>(mCharacter);
        m->setStateRelevance(eStateRelevance::domesticatedAnimals |
                             eStateRelevance::buildings |
                             eStateRelevance::terrain);
        m->setFoundAction([tptr, this]() {
            if(!tptr || !mCharacter) return;
            mCharacter->setActionType(eCharacterActionType::walk);
        });
        m->setMaxFindDistance(eNumbers::sShepherdGoatherdMaxDistance);
        return m;
    };

    const auto a = makeMove();
    const auto findFailFunc = [tptr, this, readyAny, hha, makeMove]() {
        if(!tptr) return;
        const auto a2 = makeMove();
        a2->setFindFailAction([tptr, this, hha, makeMove]() {
            if(!tptr) return;
            const auto a3 = makeMove();
            a3->setFindFailAction([tptr, this]() {
                if(tptr) mNoResource = true;
            });
            a3->start(hha);
            setCurrentAction(a3);
        });
        a2->start(readyAny);
        setCurrentAction(a2);
    };
    a->setFindFailAction(findFailFunc);
    a->start(readyRand);
    setCurrentAction(a);
    return true;
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
