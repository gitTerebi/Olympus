#include "esettleraction.h"

#include "engine/etile.h"
#include "characters/echaracter.h"
#include "characters/esettler.h"
#include "engine/ethreadpool.h"
#include "emovetoaction.h"
#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "engine/e-game-board.h"
#include "ekillcharacterfinishfail.h"
#include "fileIO/esavearchive.h"

eSettlerAction::eSettlerAction(eCharacter* const c) :
    eActionWithComeback(c, eCharActionType::settlerAction) {
    setFinishOnComeback(true);
}

eSettlerAction::~eSettlerAction() {
    //setNumberPeople(0);
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

void eSettlerAction::read(eReadStream& src) {
    eActionWithComeback::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eSettlerAction::write(eWriteStream& dst) const {
    eActionWithComeback::write(dst);
    eSaveArchive ar(dst);
    const_cast<eSettlerAction*>(this)->serialize(ar);
}

void eSettlerAction::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        int nPeople;
        ar.field("nPeople", nPeople);
        setNumberPeople(nPeople);
    } else {
        ar.field("mNPeople", mNPeople);
    }
    ar.field("mNoHouses", mNoHouses);
    if(ar.reading()) mInitialWait = 0; // reset on load
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
    const auto c = character();

    const auto finalTile = [](eThreadTile* const t) {
        const auto ut = t->underBuildingType();
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
        mNoHouses = true;
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
    eActionWithComeback::goBack(eWalkableObject::sCreateDefault());
}

void eSettlerAction::leave() {
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
    c->setAction(a);
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
            const auto ch = static_cast<eSmallHouse*>(b);
            const int v = ch->vacancies();
            if(v <= 0) continue;
            const int nPeople = mNPeople - ch->moveIn(mNPeople);
            setNumberPeople(nPeople);
            return true;
        } else if(t == eBuildingType::eliteHousing) {
            const auto ch = static_cast<eEliteHousing*>(b);
            const int v = ch->vacancies();
            if(v <= 0) continue;
            const int nPeople = mNPeople - ch->moveIn(mNPeople);
            setNumberPeople(nPeople);
            return true;
        }
    }
    return false;
}
