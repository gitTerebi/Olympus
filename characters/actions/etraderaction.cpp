#include "etraderaction.h"

#include "emovetoaction.h"
#include "ewaitaction.h"
#include "fileIO/esavearchive.h"

#include <vector>

eTraderAction::eTraderAction(eCharacter* const c) :
    eActionWithComeback(c, eCharActionType::traderAction) {}

bool eTraderAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    if(mFinishedTrade || !mTradePost || mNotFound) {
        const auto c = character();
        c->setActionType(eCharacterActionType::walk);
        goBack(mWalkable);
    } else if(mAtTradePost) {
        trade();
    } else {
        goToTradePost();
    }
    return true;
}

void eTraderAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.walkableField("walkable", mWalkable);
    ar.field("cash", mCash);
    ar.field("items", mItems);
    ar.buildingAsField("tradePost", &board(), mTradePost);
    ar.buildingField("unpackBuilding", &board(), mUnpackBuilding);
    ar.field("atTradePost", mAtTradePost);
    ar.field("finishedTrade", mFinishedTrade);
    ar.field("notFound", mNotFound);
    struct TradeEntry {
        eResourceType resource;
        int count;
    };
    auto tradeMapField = [&ar](const char* const name,
                               std::map<eResourceType, int>& values) {
        std::vector<TradeEntry> entries;
        if(ar.writing()) {
            for(const auto& v : values) {
                entries.push_back({v.first, v.second});
            }
        }
        ar.arrayField(name, entries, [](eSaveArchive& itemAr,
                                        TradeEntry& entry) {
            itemAr.field("resource", entry.resource);
            itemAr.field("count", entry.count, 0);
        });
        if(ar.reading()) {
            values.clear();
            for(const auto& entry : entries) {
                values[entry.resource] = entry.count;
            }
        }
    };
    tradeMapField("bought", mBought);
    tradeMapField("sold", mSold);
}

void eTraderAction::setTradePost(eTradePost* const tp) {
    mTradePost = tp;
}

void eTraderAction::setUnpackBuilding(eBuilding* const b) {
    mUnpackBuilding = b;
}

void eTraderAction::setWalkable(const stdsptr<eWalkableObject>& w) {
    mWalkable = w;
}

void eTraderAction::goToTradePost() {
    const auto c = character();

    const stdptr<eTraderAction> tptr(this);
    const auto fail = std::make_shared<eTA_tradeFail>(
                          board(), this);
    stdptr<eCharacter> cptr(c);
    const auto finish = std::make_shared<eTA_goToTradePostFinish>(
                            board(), this);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setRemoveLastTurn(true);
    a->setFindFailAction([tptr, cptr, this]() {
        if(!tptr) return;
        mNotFound = true;
    });
    a->start(mUnpackBuilding, mWalkable);
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);
}

void eTraderAction::trade() {
    const auto c = character();

    const stdptr<eTraderAction> tptr(this);
    const auto fail = std::make_shared<eTA_tradeFail>(
                          board(), this);
    const auto finish = std::make_shared<eTA_tradeFinish>(
                            board(), this);
    const auto a = e::make_shared<eWaitAction>(c);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setTime(500);
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::stand);
}

void eTraderAction::tradeIncrement() {
    if(!mTradePost) return;
    const int bought = mTradePost->buy(mCash, mBought);
    const int sold = mTradePost->sell(mItems, mSold);
    if(bought == 0 && sold == 0) {
        mFinishedTrade = true;
    } else {
        mCash -= bought;
        mItems -= sold;
    }
}
