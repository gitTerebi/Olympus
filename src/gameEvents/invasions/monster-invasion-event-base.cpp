#include "monster-invasion-event-base.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "characters/actions/monster-action.h"
#include "iterate-square.h"
#include "vector-helpers.h"
#include "messages.h"
#include "fileIO/save-archive.h"
#include "characters/soldier-banner.h"

eMonsterInvasionEventBase::eMonsterInvasionEventBase(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, type, branch, board),
    ePointEventValue(BannerTypeS::monsterPoint,
                    cid, board) {
    const auto e4 = Language::text("killed_trigger");
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

void eMonsterInvasionEventBase::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ePointEventValue::serialize(ar);
    eMonstersEventValue::serialize(ar);

    ar.field("chooseMonster", mChooseMonster, false);
    ar.field("aggressivness", mAggressivness, eMonsterAggressivness::passive);
    ar.field("valid", mValid, false);

    ar.arrayField("spawned", mSpawned, [](SaveArchive& ar, eMonsterType& s) {
        ar.field("s", s);
    });

    ar.arrayField("killed", mKilled, [](SaveArchive& ar, eMonsterType& k) {
        ar.field("k", k);
    });
}

bool eMonsterInvasionEventBase::finished() const {
    for(const auto s : mSpawned) {
        const bool c = VectorHelpers::contains(mKilled, s);
        if(!c) return false;
    }
    return eGameEvent::finished();
}

void eMonsterInvasionEventBase::killed(const eMonsterType monster) {
    const auto board = gameBoard();
    if(!board) return;
    mKilled.push_back(monster);
    const auto date = board->date();
    const auto& msgs = Messages::instance;
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

    const auto a = e::make_shared<MonsterAction>(monster.get());
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
            IterateSquare::iterateSquare(k++, placeMonster);
        }
    }
    a->increment(1);

    return monster.get();
}
