#include "e-board-player.h"

#include "gameEvents/requests/send-resources-to-city-event.h"
#include "gameEvents/requests/receive-tribute-event.h"

#include "game-board.h"
#include "eevent.h"
#include "eeventdata.h"
#include "vector-helpers.h"
#include "egifthelpers.h"
#include "fileIO/save-archive.h"

#include <algorithm>

eBoardPlayer::eBoardPlayer(const ePlayerId pid, GameBoard& board) :
    mBoard(board), mId(pid) {}

eTeamId eBoardPlayer::teamId() const {
    return mBoard.playerIdToTeamId(mId);
}

void eBoardPlayer::nextMonth() {
    const auto& date = mBoard.date();
    const bool nextYear = date.month() == eMonth::january;
    if(nextYear) {
        mFinances.nextYear();
    }
    if(mDrachmas < 0) {
        const bool sameMonth = date.month() == mInDebtSince.month();
        const bool oneYear = date.year() - mInDebtSince.year() == 1;
        if(sameMonth && oneYear) {
            eEventData ed(mId);
            mBoard.event(eEvent::debtAnniversary, ed);
        }
    } else if(!isPerson()) {
        giftAllies();
        bool built = false;
        const auto cids = mBoard.playerCitiesOnBoard(mId);
        for(const auto cid : cids) {
            const auto c = mBoard.boardCityWithId(cid);
            c->buildScheduled();
            built = built | c->buildNextDistrict(mDrachmas);
        }
        if(built) {
            mStuckFinanciallyMonths = 0;
        } else {
            if(mStuckFinanciallyMonths == -1) {
                mStuckFinanciallyMonths = 0;
                mLastMonthDrachmas = mDrachmas;
            } else if(mStuckFinanciallyMonths > 3) {
                const bool r = askForDrachmas();
                if(r) mStuckFinanciallyMonths = 0;
            } else if(mDrachmas - mLastMonthDrachmas < 50) {
                const auto cities = mBoard.playerCitiesOnBoard(mId);
                bool stuck = true;
                for(const auto cid : cities) {
                    const auto c = mBoard.boardCityWithId(cid);
                    const int cost = c->nextDistrictCost();
                    if(cost > 0 && cost < mDrachmas) stuck = false;
                }
                if(stuck) mStuckFinanciallyMonths++;
                else mStuckFinanciallyMonths = 0;
            } else {
                mStuckFinanciallyMonths = std::max(mStuckFinanciallyMonths - 1, 0);
            }
        }
        mLastMonthDrachmas = mDrachmas;
    }
}

void eBoardPlayer::incDrachmas(const int by, const eFinanceTarget t) {
    mFinances.add(by, t);
    const bool wasInDebt = mDrachmas < 0;
    mDrachmas += by;
    const bool isInDebt = mDrachmas < 0;
    if(!wasInDebt && isInDebt) {
        mInDebtSince = mBoard.date();
    }
    const bool isPp = isPerson();
    if(isPp) {
        mBoard.incProduced(eResourceType::drachmas, by);
    }
}

void eBoardPlayer::setDifficulty(const Difficulty d) {
    mDifficulty = d;
}

bool eBoardPlayer::isPerson() const {
    return mBoard.personPlayer() == mId;
}

void eBoardPlayer::incTime(const int by) {
    mGodAttackTimer += by;
}

void eBoardPlayer::addFulfilledQuest(const eGodQuest q) {
    mFulfilledQuests.push_back(q);
}

void eBoardPlayer::addSlayedMonster(const eMonsterType m) {
    mSlayedMonsters.push_back(m);
}

void eBoardPlayer::addGodQuest(eGodQuestEvent* const q) {
    VectorHelpers::remove(mGodQuests, q);
    mGodQuests.push_back(q);
}

void eBoardPlayer::removeGodQuest(eGodQuestEvent* const q) {
    VectorHelpers::remove(mGodQuests, q);
}

void eBoardPlayer::addCityRequest(SendResourcesToCityEvent* const q) {
    const auto sameRequest = [q](const SendResourcesToCityEvent* r) {
        return r->requestId() == q->requestId();
    };
    if(std::find_if(mCityRequests.begin(), mCityRequests.end(),
                    sameRequest) == mCityRequests.end()) {
        mCityRequests.push_back(q);
    }
}

void eBoardPlayer::removeCityRequest(SendResourcesToCityEvent* const q) {
    const auto sameRequest = [q](const SendResourcesToCityEvent* r) {
        return r->requestId() == q->requestId();
    };
    mCityRequests.erase(std::remove_if(mCityRequests.begin(),
                                       mCityRequests.end(),
                                       sameRequest),
                        mCityRequests.end());
}

void eBoardPlayer::addTributeRequest(ReceiveTributeEvent* const q) {
    if(!VectorHelpers::contains(mTributeRequests, q)) {
        mTributeRequests.push_back(q);
    }
}

void eBoardPlayer::removeTributeRequest(ReceiveTributeEvent* const q) {
    VectorHelpers::remove(mTributeRequests, q);
}

void eBoardPlayer::addCityTroopsRequest(SendTroopsEvent* const q) {
    mCityTroopsRequests.push_back(q);
}

void eBoardPlayer::removeCityTroopsRequest(SendTroopsEvent* const q) {
    VectorHelpers::remove(mCityTroopsRequests, q);
}

void eBoardPlayer::addConquest(PlayerConquestEventBase* const q) {
    mConquests.push_back(q);
}

void eBoardPlayer::removeConquest(PlayerConquestEventBase* const q) {
    VectorHelpers::remove(mConquests, q);
}

void eBoardPlayer::serialize(SaveArchive& ar) {
    ar.field("playerId", mId);
    ar.field("difficulty", mDifficulty);

    ar.arrayField("fulfilledQuests", mFulfilledQuests,
        [](SaveArchive& itemAr, eGodQuest& q) {
            q.serialize(itemAr);
        });

    ar.arrayField("slayedMonsters", mSlayedMonsters,
        [](SaveArchive& itemAr, eMonsterType& m) {
            itemAr.field("monsterType", m);
        });

    ar.arrayField("godQuests", mGodQuests,
        [this](SaveArchive& itemAr, eGodQuestEvent*& q) {
            itemAr.gameEventField("event", &mBoard, q);
        });

    ar.field("drachmas", mDrachmas);
    ar.dateField("inDebtSince", mInDebtSince);
    ar.field("godAttackTimer", mGodAttackTimer);

    ar.archiveField("finances",
        [this](SaveArchive& itemAr) { mFinances.serialize(itemAr); });
}

void eBoardPlayer::giftAllies() {
    const auto allyCids = mBoard.allyCidsNotOnBoard(mId);
    const auto& wboard = mBoard.world();
    stdsptr<WorldCity> city;
    const auto pCities =  mBoard.playerCities(mId);
    for(const auto cid : pCities) {
        const auto c = mBoard.boardCityWithId(cid);
        for(const auto cid : allyCids) {
            const auto c = wboard.cityWithId(cid);
            const int att = c->attitude(mId);
            if(att < 50) {
                city = c;
                break;
            }
        }
        if(!city) break;
        const auto tryGift = [&](const eResourceType type) {
            const int giftSize = eGiftHelpers::giftCount(type);
            const int waiting = c->waitingCount(type);
            if(waiting < giftSize) return false;
            const int space = c->spaceForResource(type);
            if(space > waiting/2) return false;
            const int count = c->resourceCount(type);
            if(count < 2*giftSize) return false;
            mBoard.giftTo(city, type, giftSize, cid);
            return true;
        };
        const auto& buys = city->buys();
        bool r = false;
        for(const auto& b : buys) {
            r = tryGift(b.fType);
            if(r) break;
        }
        if(!r) r = tryGift(eResourceType::food);
        if(!r) r = tryGift(eResourceType::fleece);
        if(!r) r = tryGift(eResourceType::oliveOil);
        if(!r) r = tryGift(eResourceType::wine);
        if(!r) r = tryGift(eResourceType::armor);
        if(!r) r = tryGift(eResourceType::marble);
        if(!r) r = tryGift(eResourceType::wood);
        if(!r) r = tryGift(eResourceType::bronze);
        if(!r) r = tryGift(eResourceType::marble);
        if(!r) r = tryGift(eResourceType::sculpture);
    }
}

bool eBoardPlayer::askFor(const eResourceType type, const eCityId cid) {
    const auto allyCids = mBoard.allyCidsNotOnBoard(mId);
    const auto& wboard = mBoard.world();
    stdsptr<WorldCity> city;
    for(const auto cid : allyCids) {
        const auto c = wboard.cityWithId(cid);
        const int att = c->attitude(mId);
        if(att >= 50) {
            city = c;
            break;
        }
    }
    if(!city) return false;
    mBoard.request(city, type, cid);
    return true;
}

bool eBoardPlayer::askForDrachmas() {
    const auto pCities =  mBoard.playerCities(mId);
    if(pCities.empty()) return false;
    return askFor(eResourceType::drachmas, pCities[0]);
}
