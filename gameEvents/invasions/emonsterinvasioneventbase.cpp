#include "emonsterinvasioneventbase.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "characters/actions/emonsteraction.h"
#include "eiteratesquare.h"
#include "evectorhelpers.h"
#include "emessages.h"
#include "fileIO/esavearchive.h"

eMonsterInvasionEventBase::eMonsterInvasionEventBase(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, type, branch, board),
    ePointEventValue(eBannerTypeS::monsterPoint,
                    cid, board) {
    const auto e4 = eLanguage::text("killed_trigger");
    mKilledTrigger = e::make_shared<eEventTrigger>(cid, e4, board);
    addTrigger(mKilledTrigger);
}

eMonsterInvasionEventBase::~eMonsterInvasionEventBase() {
    const auto board = gameBoard();
    if(board) board->removeMonsterEvent(this);
}

void eMonsterInvasionEventBase::chooseMonster() {
    if(mChooseMonster) return;
    mChooseMonster = true;
    mValid = eMonstersEventValue::chooseMonster(mSpawned);
}

void eMonsterInvasionEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
    eMonstersEventValue::write(dst);
    eSaveArchive ar(dst);
    const_cast<eMonsterInvasionEventBase*>(this)->serialize(ar);
}

void eMonsterInvasionEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
    eMonstersEventValue::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eMonsterInvasionEventBase::serialize(eSaveArchive& ar) {
    ar.field("mChooseMonster", mChooseMonster);
    ar.field("mAggressivness", mAggressivness);
    ar.field("mValid", mValid);

    int ns = mSpawned.size();
    ar.field("ns", ns);
    if(ar.reading()) mSpawned.clear();
    for(int i = 0; i < ns; i++) {
        eMonsterType s;
        if(ar.writing()) s = mSpawned[i];
        ar.field("s", s);
        if(ar.reading()) mSpawned.push_back(s);
    }

    int nk = mKilled.size();
    ar.field("nk", nk);
    if(ar.reading()) mKilled.clear();
    for(int i = 0; i < nk; i++) {
        eMonsterType k;
        if(ar.writing()) k = mKilled[i];
        ar.field("k", k);
        if(ar.reading()) mKilled.push_back(k);
    }
}

bool eMonsterInvasionEventBase::finished() const {
    for(const auto s : mSpawned) {
        const bool c = eVectorHelpers::contains(mKilled, s);
        if(!c) return false;
    }
    return eGameEvent::finished();
}

void eMonsterInvasionEventBase::killed(const eMonsterType monster) {
    const auto board = gameBoard();
    if(!board) return;
    mKilled.push_back(monster);
    const auto date = board->date();
    const auto& msgs = eMessages::instance;
    const auto monsterMsgs = msgs.monsterMessages(monster);
    const auto rFull = monsterMsgs->fSlainReason;
    mKilledTrigger->trigger(*this, date, rFull);
}

eMonster* eMonsterInvasionEventBase::triggerBase() {
    const auto board = gameBoard();
    if(!board) return nullptr;
    if(!mChooseMonster) chooseMonster();
    if(!mValid) return nullptr;
    mSpawned.push_back(mMonster);
    board->addMonsterEvent(mMonster, this);
    mChooseMonster = false;
    choosePointId();
    const auto cid = cityId();
    const auto monster = eMonster::sCreateMonster(mMonster, *board);
    board->registerMonster(cid, monster.get());
    monster->setOnCityId(cid);
    monster->setCityId(eCityId::neutralAggresive);

    const auto a = e::make_shared<eMonsterAction>(monster.get());
    a->setAggressivness(mAggressivness);
    monster->setAction(a);
    const int ptId = pointId();
    const auto tile = board->monsterTile(cid, ptId);
    if(tile) {
        const int tx = tile->x();
        const int ty = tile->y();
        const auto placeMonster = [board, monster, tx, ty, cid](
                                  const int dx, const int dy) {
            const int ttx = tx + dx;
            const int tty = ty + dy;
            const auto tile = board->tile(ttx, tty);
            if(!tile) return false;
            const auto tcid = tile->cityId();
            if(cid != tcid) return false;
            if(const auto ub = tile->underBuilding()) {
                const auto type = ub->type();
                const bool w = eBuilding::sWalkableBuilding(type);
                if(!w) {
                    const bool a = eBuilding::sAttackable(type);
                    if(a) {
                        ub->collapse();
                    } else {
                        return false;
                    }
                }
            }
            monster->changeTile(tile);
            return true;
        };
        int k = 0;
        while(!monster->tile()) {
            eIterateSquare::iterateSquare(k++, placeMonster);
        }
    }
    a->increment(1);

    return monster.get();
}
