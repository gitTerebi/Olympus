#include "esettleraction.h"

#include "engine/etile.h"
#include "characters/echaracter.h"
#include "characters/esettler.h"
#include "engine/ethreadpool.h"
#include "emovetoaction.h"
#include "buildings/small-house.h"
#include "buildings/elite-housing.h"
#include "engine/game-board.h"
#include "ekillcharacterfinishfail.h"
#include "fileIO/esavearchive.h"

eSettlerAction::eSettlerAction(eCharacter* const c) :
    eActionWithComeback(c, eCharActionType::settlerAction) {
    setFinishOnComeback(true);
}

bool eSettlerAction::decide() {
    if(mInitialWait > 0) {
        --mInitialWait;
        return true;
    }
    const auto c = character();
    const auto ct = c->type();
    if(ct == eCharacterType::settler) {
        const auto s = static_cast<eSettler*>(c);
        if(s->emigrant()) {
            leave();
            return true;
        }
    }
    if(mNoHouses) {
        if(ct == eCharacterType::settler) {
            goBack2();
        } else {
            leave();
        }
    } else {
        findHouse();
    }
    return true;
}

void eSettlerAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    int nPeople = mNPeople;
    ar.field("nPeople", nPeople);
    if(ar.reading()) setNumberPeople(nPeople);
    ar.field("noHouses", mNoHouses);
    ar.field("initialWait", mInitialWait, 0);
    ar.field("stage", mStage);
}

void eSettlerAction::resumeFromSavedState() {
    switch(mStage) {
    case eSettlerActionStage::idle:
        eActionWithComeback::resumeFromSavedState();
        break;
    case eSettlerActionStage::findingHouse:
        findHouse();
        break;
    case eSettlerActionStage::goingBack:
        goBack2();
        break;
    case eSettlerActionStage::leaving:
        leave();
        break;
    }
}

void eSettlerAction::setNumberPeople(const int p) {
    const auto c = character();
    const bool homeless = c && c->type() == eCharacterType::homeless;
    if(!homeless) {
        auto& board = this->board();
        const auto popData = board.populationData(cityId());
        if(popData) {
            popData->incSettlers(p - mNPeople);
        }
    }
    mNPeople = p;
}

void eSettlerAction::setInitialWait(const int w) {
    mInitialWait = w;
}

void eSettlerAction::findHouse() {
    if(mNoHouses) return;
    mStage = eSettlerActionStage::findingHouse;
    const auto c = character();

    const bool eliteOnly = mEliteOnly;
    const auto finalTile = [eliteOnly](eThreadTile* const t) {
        const auto ut = t->underBuildingType();
        if(eliteOnly) return ut == eBuildingType::eliteHousing && t->houseVacancies() > 0;
        const bool h = ut == eBuildingType::commonHouse ||
                       ut == eBuildingType::eliteHousing;
        return h && t->houseVacancies() > 0;
    };
    const stdptr<eCharacterAction> tptr(this);
    const auto failFunc = std::make_shared<eSA_findHouseFail>(
                              board(), this);
    const auto finishAction = std::make_shared<eSA_findHouseFinish>(
                                  board(), this);

    c->setActionType(eCharacterActionType::walk);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::houseVacancies |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(failFunc);
    a->setFinishAction(finishAction);
    a->setFindFailAction([tptr, this]() {
        if(!tptr) return;
        if(mEliteOnly) {
            mEliteOnly = false;
            findHouse();
        } else {
            mNoHouses = true;
        }
    });
    a->setFoundAction([tptr, this]() {
        if(!tptr) return;
        const auto c = character();
        if(!c) return;
        c->setVisible(true);
    });
    a->setRemoveLastTurn(true);
    a->start(finalTile);
    setCurrentAction(a);
}

void eSettlerAction::goBack2() {
    mStage = eSettlerActionStage::goingBack;
    eActionWithComeback::goBack(eWalkableObject::sCreateDefault());
}

void eSettlerAction::leave() {
    mStage = eSettlerActionStage::leaving;
    auto& board = eSettlerAction::board();
    const auto c = character();
    const stdptr<eCharacter> cptr(c);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
                          board, c);
    const auto finish = std::make_shared<eKillCharacterFinishFail>(
                            board, c);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setFindFailAction([cptr]() {
        if(cptr) cptr->kill();
    });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);
    const auto edgeTile = [](eTileBase* const tile) {
        return tile->isCityEdge();
    };
    a->start(edgeTile);
}

bool eSettlerAction::enterHouse() {
    const auto c = character();
    const auto t = c->tile();
    if(!t) {
        setState(eCharacterActionState::failed);
        return false;
    }
    for(const auto n : {eOrientation::topRight,
                        eOrientation::right,
                        eOrientation::bottomRight,
                        eOrientation::bottom,
                        eOrientation::bottomLeft,
                        eOrientation::left,
                        eOrientation::topLeft,
                        eOrientation::top}) {
        const auto nn = t->neighbour<eTile>(n);
        if(!nn) continue;
        const auto b = nn->underBuilding();
        if(!b) continue;
        const auto t = b->type();
        if(t == eBuildingType::commonHouse) {
            const auto ch = static_cast<SmallHouse*>(b);
            const int v = ch->vacancies();
            if(v <= 0) continue;
            const int nPeople = mNPeople - ch->moveIn(mNPeople);
            setNumberPeople(nPeople);
            return true;
        } else if(t == eBuildingType::eliteHousing) {
            const auto ch = static_cast<EliteHousing*>(b);
            const int v = ch->vacancies();
            if(v <= 0) continue;
            const int nPeople = mNPeople - ch->moveIn(mNPeople);
            setNumberPeople(nPeople);
            return true;
        }
    }
    return false;
}
