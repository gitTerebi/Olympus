#include "eboardplayer.h"

#include "egameboard.h"
#include "eevent.h"
#include "eeventdata.h"
#include "evectorhelpers.h"

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
        const auto cids = mBoard.playerCitiesOnBoard(mId);
        for(const auto cid : cids) {
            const auto c = mBoard.boardCityWithId(cid);
            c->buildScheduled();
            c->buildNextDistrict(mDrachmas);
        }
        if(mStuckFinanciallyMonths == -1) {
            mStuckFinanciallyMonths = 0;
            mStuckDrachmas = mDrachmas;
        } else if(mStuckFinanciallyMonths > 3) {
            const auto wboard = mBoard.getWorldBoard();
            const auto& cities = wboard->cities();
            stdsptr<eWorldCity> city;
            const auto tid = teamId();
            for(const auto& c : cities) {
                const auto cid = c->cityId();
                const auto ctid = mBoard.cityIdToTeamId(cid);
                if(tid != ctid) continue;
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
            }
        } else if(mDrachmas - mStuckDrachmas < 50) {
            mStuckFinanciallyMonths++;
        } else {
            mStuckFinanciallyMonths = 0;
        }
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
