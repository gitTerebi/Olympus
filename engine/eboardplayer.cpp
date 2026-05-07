#include "eboardplayer.h"

#include "egameboard.h"
#include "eevent.h"
#include "eeventdata.h"
#include "evectorhelpers.h"
#include "egifthelpers.h"
#include "fileIO/esavearchive.h"

eBoardPlayer::eBoardPlayer(const ePlayerId pid, eGameBoard& board) :
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

void eBoardPlayer::setDifficulty(const eDifficulty d) {
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
    eVectorHelpers::remove(mGodQuests, q);
    mGodQuests.push_back(q);
}

void eBoardPlayer::removeGodQuest(eGodQuestEvent* const q) {
    eVectorHelpers::remove(mGodQuests, q);
}

void eBoardPlayer::addCityRequest(eReceiveRequestEvent* const q) {
    mCityRequests.push_back(q);
}

void eBoardPlayer::removeCityRequest(eReceiveRequestEvent* const q) {
    eVectorHelpers::remove(mCityRequests, q);
}

void eBoardPlayer::addCityTroopsRequest(eTroopsRequestEvent* const q) {
    mCityTroopsRequests.push_back(q);
}

void eBoardPlayer::removeCityTroopsRequest(eTroopsRequestEvent* const q) {
    eVectorHelpers::remove(mCityTroopsRequests, q);
}

void eBoardPlayer::addConquest(ePlayerConquestEventBase* const q) {
    mConquests.push_back(q);
}

void eBoardPlayer::removeConquest(ePlayerConquestEventBase* const q) {
    eVectorHelpers::remove(mConquests, q);
}

void eBoardPlayer::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eBoardPlayer::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eBoardPlayer*>(this)->serialize(ar);
}

void eBoardPlayer::serialize(eSaveArchive& ar) {
    ar.value(mId);
    ar.value(mDifficulty);

    {
        int nq;
        if(ar.writing()) nq = mFulfilledQuests.size();
        ar.value(nq);
        if(ar.reading()) mFulfilledQuests.clear();
        for(int i = 0; i < nq; i++) {
            eGodQuest q;
            if(ar.writing()) q = mFulfilledQuests[i];
            if(ar.reading()) q.read(ar.readStream());
            else q.write(ar.writeStream());
            if(ar.reading()) mFulfilledQuests.push_back(q);
        }
    }

    {
        int nm;
        if(ar.writing()) nm = mSlayedMonsters.size();
        ar.value(nm);
        if(ar.reading()) mSlayedMonsters.clear();
        for(int i = 0; i < nm; i++) {
            eMonsterType m;
            if(ar.writing()) m = mSlayedMonsters[i];
            ar.value(m);
            if(ar.reading()) mSlayedMonsters.push_back(m);
        }
    }

    {
        int nq;
        if(ar.writing()) nq = mGodQuests.size();
        ar.value(nq);
        if(ar.reading()) mGodQuests.clear();
        for(int i = 0; i < nq; i++) {
            if(ar.reading()) {
                ar.readStream().readGameEvent(&mBoard, [this](eGameEvent* const e) {
                    const auto ge = static_cast<eGodQuestEvent*>(e);
                    mGodQuests.push_back(ge);
                });
            } else {
                ar.writeStream().writeGameEvent(mGodQuests[i]);
            }
        }
    }

    ar.value(mDrachmas);
    if(ar.reading()) mInDebtSince.read(ar.readStream());
    else mInDebtSince.write(ar.writeStream());

    ar.value(mGodAttackTimer);

    if(ar.reading()) mFinances.read(ar.readStream());
    else mFinances.write(ar.writeStream());
}

void eBoardPlayer::giftAllies() {
    const auto allyCids = mBoard.allyCidsNotOnBoard(mId);
    const auto& wboard = mBoard.world();
    stdsptr<eWorldCity> city;
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
    stdsptr<eWorldCity> city;
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
