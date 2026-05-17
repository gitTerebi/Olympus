#include "ehuntaction.h"

#include "engine/epathfinder.h"

#include "emovetoaction.h"
#include "ewaitaction.h"

#include "characters/eboar.h"
#include "characters/edeer.h"

#include "buildings/ehuntinglodge.h"
#include "fileIO/esavearchive.h"

eHuntAction::eHuntAction(eHuntingLodge* const b, eCharacter* const c) :
    eActionWithComeback(c, eCharActionType::huntAction),
    mLodge(b), mHunter(static_cast<eHunter*>(c)) {}

eHuntAction::eHuntAction(eCharacter* const c) :
    eHuntAction(nullptr, c) {}

bool tryToCollect(eTile* const tile) {
    const auto cs = tile->characters();
    for(const auto& c : cs) {
        const auto t = c->type();
        if(t == eCharacterType::boar) {
            const auto b = static_cast<eBoar*>(c.get());
            if(b->dead()) {
                b->kill();
                return true;
            }
        } else if(t == eCharacterType::deer) {
            const auto d = static_cast<eDeer*>(c.get());
            if(d->dead()) {
                d->kill();
                return true;
            }
        }
    }
    return false;
}

void eHuntAction::increment(const int by) {
    if(mStage == eHuntActionStage::waiting && mWaitRemaining > 0) {
        mWaitRemaining -= by;
        if(mWaitRemaining < 0) mWaitRemaining = 0;
    }
    if(mHunter) {
        const auto t = mHunter->tile();
        if(t && tryToCollect(t)) {
            mHunter->incCollected(1);
            setCurrentAction(nullptr);
            return;
        }
    }
    eActionWithComeback::increment(by);
}

bool hasAnimal(eTileBase* const tile, eCharacterType& type) {
    return tile->hasCharacter([&type](const eCharacterBase& c) {
        const bool b = c.type() == eCharacterType::boar && !c.fighting();
        if(b) {
            type = eCharacterType::boar;
            return true;
        }
        const bool d = c.type() == eCharacterType::deer && !c.fighting();
        if(d) {
            type = eCharacterType::deer;
            return true;
        }
        return false;
    });
}

bool eHuntAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    const auto t = mHunter->tile();
    if(!t) return false;

    if(tryToCollect(t)) mHunter->incCollected(1);
    const int coll = mHunter->collected();

    const SDL_Point p{t->x(), t->y()};
    const auto rect = mLodge->tileRect();
    const bool inLodge = SDL_PointInRect(&p, &rect);

    if(coll > 0) {
        if(inLodge) {
            mLodge->hunterDelivered(eResourceType::meat, coll);
            mHunter->incCollected(-coll);
            setState(eCharacterActionState::finished);
        } else {
            goBackDecision();
        }
    } else {
        if(inLodge) {
            const int res = mLodge->resource();
            if(res >= mLodge->maxResource() ||
               !mLodge->enabled()) {
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

void eHuntAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.buildingAsField("lodge", &board(), mLodge);
    ar.characterAsField("hunter", &board(), mHunter);
    const auto hunter = static_cast<eHunter*>(character());
    int carriedResourceCount = hunter ? hunter->collected() : 0;
    ar.field("carriedResourceCount", carriedResourceCount, 0);
    ar.field("noResource", mNoResource);
    ar.field("stage", mStage, eHuntActionStage::idle);
    ar.field("waitRemaining", mWaitRemaining, 0);
    if(ar.reading()) {
        const stdptr<eHuntAction> tptr(this);
        ar.addPostFunc([tptr, carriedResourceCount]() {
            if(!tptr) return;
            const auto a = tptr.get();
            const auto hunter = static_cast<eHunter*>(a->character());
            if(!hunter) return;
            a->mHunter = hunter;
            hunter->incCollected(carriedResourceCount - hunter->collected());
        }, "hunterCarriedResourceCount");
    }
}

void eHuntAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eHuntAction::rebuildCurrentStage() {
    switch(mStage) {
    case eHuntActionStage::findingResource:
        return findResourceDecision();
    case eHuntActionStage::goingBack:
        return goBackDecision();
    case eHuntActionStage::waiting:
        if(mWaitRemaining > 0) {
            const auto w = e::make_shared<eWaitAction>(mHunter);
            w->setTime(mWaitRemaining);
            setCurrentAction(w);
        } else {
            mStage = eHuntActionStage::idle;
            decide();
        }
        return;
    case eHuntActionStage::idle:
        return eActionWithComeback::resumeFromSavedState();
    }
}

void eHuntAction::findResourceDecision() {
    mStage = eHuntActionStage::findingResource;
    const auto c = character();

    const stdptr<eHuntAction> tptr(this);

    auto aType = std::make_shared<eCharacterType>();

    const auto hha = [aType](eTileBase* const tile) {
        eCharacterType type;
        const bool r = hasAnimal(tile, type);
        if(r) *aType = type;
        return r;
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::huntingGrounds |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([tptr, this, c, aType]() {
        if(tptr) {
            if(*aType == eCharacterType::deer) {
                mHunter->setDeerHunter(true);
            }
            mLodge->setNoTarget(false);
        }
        c->setActionType(eCharacterActionType::walk);
    });
    const auto findFailFunc = [tptr, this]() {
        if(tptr) {
            mNoResource = true;
            mLodge->setNoTarget(true);
        }
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(60);
    a->start(hha);
    setCurrentAction(a);
}

void eHuntAction::goBackDecision() {
    mStage = eHuntActionStage::goingBack;
    mHunter->setActionType(eCharacterActionType::carry);
    const auto rect = mLodge->tileRect();
    eActionWithComeback::goBack(rect, eWalkableObject::sCreateDefault());
}

void eHuntAction::waitDecision() {
    mStage = eHuntActionStage::waiting;
    if(mWaitRemaining <= 0) {
        mWaitRemaining = 5000;
    }
    const auto w = e::make_shared<eWaitAction>(mHunter);
    w->setTime(mWaitRemaining);
    setCurrentAction(w);
}
