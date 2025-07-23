#include "eboardplayer.h"

#include "egameboard.h"
#include "eevent.h"
#include "eeventdata.h"
#include "evectorhelpers.h"
#include "egifthelpers.h"

eBoardPlayer::eBoardPlayer(const ePlayerId pid, eGameBoard& board) :
    mBoard(board), mId(pid) {}

eTeamId eBoardPlayer::teamId() const {
    return mBoard.playerIdToTeamId(mId);
}

void eBoardPlayer::nextMonth() {
    if(mDrachmas < 0) {
        const auto date = mBoard.date();
        const bool sameMonth = date.month() == mInDebtSince.month();
        const bool oneYear = date.year() - mInDebtSince.year() == 1;
        if(sameMonth && oneYear) {
            eEventData ed(mId);
            mBoard.event(eEvent::debtAnniversary, ed);
        }
    } else if(!isPerson()) {
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
                const auto wboard = mBoard.getWorldBoard();
                const auto& cities = wboard->cities();
                std::vector<stdsptr<eWorldCity>> allyCities;
                stdsptr<eWorldCity> city;
                const auto tid = teamId();
                for(const auto& c : cities) {
                    const auto cid = c->cityId();
                    const auto cpid = mBoard.cityIdToPlayerId(cid);
                    if(cpid == mId) continue;
                    const auto ctid = mBoard.playerIdToTeamId(cpid);
                    if(tid != ctid) continue;
                    allyCities.push_back(c);
                    const int att = c->attitude(mId);
                    if(att >= 50) {
                        city = c;
                        break;
                    }
                }
                const auto pCities =  mBoard.playerCities(mId);
                if(city && !pCities.empty()) {
                    mBoard.request(city, eResourceType::drachmas,
                                   pCities[0]);
                    mStuckFinanciallyMonths = 0;
                }
                if(!city && !allyCities.empty()) {
                    for(const auto cid : pCities) {
                        const auto c = mBoard.boardCityWithId(cid);
                        for(const auto& c : allyCities) {
                            const int att = c->attitude(mId);
                            if(att < 50) {
                                city = c;
                                break;
                            }
                        }
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

void eBoardPlayer::incDrachmas(const int by) {
    const bool wasInDebt = mDrachmas < 0;
    mDrachmas += by;
    const bool isInDebt = mDrachmas < 0;
    if(!wasInDebt && isInDebt) {
        mInDebtSince = mBoard.date();
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
    src >> mId;
    src >> mDifficulty;

    {
        int nq;
        src >> nq;
        for(int i = 0; i < nq; i++) {
            auto& q = mFulfilledQuests.emplace_back();
            q.read(src);
        }
    }

    {
        int nm;
        src >> nm;
        for(int i = 0; i < nm; i++) {
            auto& m = mSlayedMonsters.emplace_back();
            src >> m;
        }
    }

    {
        int nq;
        src >> nq;
        for(int i = 0; i < nq; i++) {
            src.readGameEvent(&mBoard, [this](eGameEvent* const e) {
                const auto ge = static_cast<eGodQuestEvent*>(e);
                mGodQuests.push_back(ge);
            });
        }
    }

    src >> mDrachmas;
    mInDebtSince.read(src);

    src >> mGodAttackTimer;
}

void eBoardPlayer::write(eWriteStream& dst) const {
    dst << mId;
    dst << mDifficulty;

    {
        const int nq = mFulfilledQuests.size();
        dst << nq;
        for(const auto& q : mFulfilledQuests) {
            q.write(dst);
        }
    }

    {
        const int nm = mSlayedMonsters.size();
        dst << nm;
        for(const auto m : mSlayedMonsters) {
            dst << m;
        }
    }

    {
        const int nq = mGodQuests.size();
        dst << nq;
        for(const auto& q : mGodQuests) {
            dst.writeGameEvent(q);
        }
    }

    dst << mDrachmas;
    mInDebtSince.write(dst);

    dst << mGodAttackTimer;
}
