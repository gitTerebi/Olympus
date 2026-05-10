#include "einvasionevent.h"

#include "engine/e-game-board.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "gameEvents/invasions/e-invasion-handler.h"
#include "elanguage.h"
#include "gameEvents/invasions/einvasionwarning.h"
#include "audio/emusic.h"
#include "evectorhelpers.h"
#include "gameEvents/eplayerconquestevent.h"
#include "engine/edifficulty.h"
#include "engine/epathfinder.h"
#include "eiteratesquare.h"
#include "fileIO/esavearchive.h"

#include <cstdio>

namespace {
// Configurable settings for invasion events
// Delay in months before the invasion actually starts after planning
constexpr int invasionDelayMonths = 2;
constexpr int invasionDelayDays = 62;

// Warning months before invasion for each warning type (initial, 24, 12, 6, 1 months)
// Order must match warnTypes in pointerCreated()
const std::array<int, 5> invasionWarningMonths = {2, 24, 12, 6, 1};

constexpr int kInvaderDefeatedAttitudeRestore = 15;
constexpr int kInvaderWonAttitudeRestore = 35;
}

eInvasionEvent::eInvasionEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::invasion,
               branch, board),
    ePointEventValue(eBannerTypeS::landInvasion,
                    cid, board),
    eCityEventValue(board, [this, cid](eWorldCity& c) {
        auto& board2 = *gameBoard();
        const auto ccid = c.cityId();
        const auto tid = board2.cityIdToTeamId(cid);
        const auto ctid = board2.cityIdToTeamId(ccid);
        const bool e = eTeamIdHelpers::isEnemy(tid, ctid);
        return e;
    }) {
    mDatePlusMonths = invasionDelayMonths;
}

eInvasionEvent::~eInvasionEvent() {
    const auto board = gameBoard();
    if(board) board->removeInvasion(this);
}

void eInvasionEvent::pointerCreated() {
    const std::array<eInvasionWarningType, 5> warnTypes = {
        eInvasionWarningType::warningInitial,
        eInvasionWarningType::warning24,
        eInvasionWarningType::warning12,
        eInvasionWarningType::warning6,
        eInvasionWarningType::warning1
    };
    const auto cid = cityId();
    auto& board = *gameBoard();
    for(size_t i = 0; i < warnTypes.size(); i++) {
        const auto w = warnTypes[i];
        const int months = invasionWarningMonths[i];
        const auto e = std::make_shared<eInvasionWarning>(
            months, *this, cid, board, w);
        if(w == eInvasionWarningType::warningInitial) {
            mInitialWarning = e.get();
        }
        addWarning(e);
    }
}

void eInvasionEvent::initialize(const stdsptr<eWorldCity>& city,
                                const int count, const ePlayerId sentBy) {
    setSingleCity(city);
    setMinCount(count);
    setMaxCount(count);
    mSentByPlayer = sentBy;
}

void eInvasionEvent::initialize(const stdsptr<eWorldCity>& city,
                                const eEnlistedForces& forces,
                                ePlayerConquestEvent* const conquestEvent) {
    mCity = city;
    if(city) {
        const auto cid = city->cityId();
        const int i = static_cast<int>(cid);
        setMinCityId(i);
        setMaxCityId(i);
    }
    mForces = forces;
    mConquestEvent = conquestEvent;
}

bool eInvasionEvent::tryCreateCityInvasion(eWorldCity& attacker,
                                           eGameBoard& board) {
    const auto ppid = board.personPlayer();
    const auto attitude = attacker.attitudeClass(ppid);
    if(!attacker.isRival() || !attacker.active() || !attacker.visible() ||
       attitude != eCityAttitude::hostile) {
        return false;
    }

    auto targetCid = board.personPlayerCapital();
    if(targetCid == eCityId::neutralFriendly ||
       targetCid == eCityId::neutralAggresive) {
        targetCid = board.world().currentCityId();
    }
    const auto targetCity = board.world().cityWithId(targetCid);
    const bool canInvade = targetCity &&
                           !board.hasActiveInvasions(targetCid) &&
                           board.date().year() > attacker.nextInvasionYear();
    if(!canInvade || eRand::rand() % 12 != 0) {
        return false;
    }

    const auto e = e::make_shared<eInvasionEvent>(
        targetCid, eGameEventBranch::root, board);
    const auto attackingCity = board.world().cityWithId(attacker.cityId());
    if(!attackingCity) return false;

    const int unitCount =
        static_cast<int>(attacker.militaryStrength() *
                         eDifficultyHelpers::costMultiplier(
                             board.difficulty(attackingCity->playerId()))) * 8;

    e->setSingleCity(attackingCity);
    e->setMinPointId(1);
    e->setMaxPointId(16);
    e->setMinCount(static_cast<int>(unitCount / 2));
    e->setMaxCount(unitCount);
    e->useGeneratedCityWarnings();
    auto date = board.date() + invasionDelayDays;
    e->initializeDate(date);
    board.addRootGameEvent(e);
    e->sendInitialAnnouncement();
    attacker.setNextInvasionYear(board.date().year() + 1 + eRand::rand() % 3);
    return true;
}

eTile* nearestDisembarkTile(eTile* const tile, eGameBoard& board,
                            const eCityId cid) {
    eTile* final = nullptr;
    ePathFinder p([](eTileBase* const t) {
        return t->hasWater();
    }, [&](eTileBase* const t) {
        const auto tt = static_cast<eTile*>(t);
        const auto& bs = tt->banners();
        for(const auto& b : bs) {
            const auto type = b->type();
            const bool r = type == eBannerTypeS::disembarkPoint;
            if(r) {
                final = tt;
                return true;
            }
        }
        return false;
    });
    const int w = board.width();
    const int h = board.height();
    const auto rect = board.boardCityTileBRect(cid);
    const bool r = p.findPath(rect, tile, 1000, true, w, h);
    if(!r) return nullptr;
    return final;
}


eTile* nearestShoreTile(eTile* const tile) {
    if(!tile) return nullptr;
    eTile* result = nullptr;
    const auto prcs = [&](const int dx, const int dy) {
        const auto t = tile->tileRel<eTile>(dx, dy);
        if(!t) return false;
         if(!t->hasBridge() && t->walkable()) {
            result = t;
            return true;
        }
        return false;
    };
    for(int i = 0; i < 9; i++) {
        eIterateSquare::iterateSquare(i, prcs);
        if(result) return result;
    }
    return result;
}

void eInvasionEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    printf("invasion trigger: target=%i warned=%i attacker=%s point=%i next_date=%s\n",
           static_cast<int>(cityId()), mWarned,
           mCity ? mCity->name().c_str() : "null",
           pointId(), nextDate().shortString().c_str());
    if(!mWarned) {
        choosePointId();
        if(!mCity) chooseCity();
        updateDisembarkAndShoreTile();
    }
    chooseCount();
    board->removeInvasion(this);
    const auto tile = invasionTile();
    if(!mCity || !tile) {
        printf("invasion trigger aborted: attacker=%s tile=%p point=%i\n",
               mCity ? mCity->name().c_str() : "null",
               static_cast<void*>(tile), pointId());
        if(mConquestEvent) {
            mConquestEvent->planArmyReturn();
        }
        return;
    }
    const int c = count();
    const auto cid = cityId();
    mWarned = false;

    int infantry = 0;
    int cavalry = 0;
    int archers = 0;

    const auto city = mCity;

    if(mHardcoded) {
        city->troopsByType(c, infantry, cavalry, archers);
    } else {
        city->troopsByType(infantry, cavalry, archers);
    }
    const auto self = ref<eInvasionEvent>();
    const auto startInvasion = [self, board, tile, cid, city,
                                infantry, cavalry, archers]() {
        self->mWaitingForResponse = false;
        if(!tile) return;
        const auto invadingCid = self->mCity->cityId();
        const auto invadingC = board->boardCityWithId(invadingCid);

        eInvasionHandler* eh = nullptr;
        if(tile->hasWater()) {
            if(!self->mDisembarkTile || !self->mShoreTile) {
                printf("invasion start aborted: sea tile missing disembark=%p shore=%p\n",
                       static_cast<void*>(self->mDisembarkTile),
                       static_cast<void*>(self->mShoreTile));
                return;
            }
            eh = new eInvasionHandler(*board, cid, self->mCity, self.get());
            if(invadingC) {
                eh->initializeSeaInvasion(tile, self->mDisembarkTile, self->mShoreTile,
                                          self->mForces, self->mConquestEvent);
            } else {
                eh->initializeSeaInvasion(tile, self->mDisembarkTile, self->mShoreTile,
                                          infantry, cavalry, archers);
            }
            } else {
            eh = new eInvasionHandler(*board, cid, self->mCity, self.get());
            if(invadingC) {
                eh->initializeLandInvasion(tile, self->mForces, self->mConquestEvent);
            } else {
                eh->initializeLandInvasion(tile, infantry, cavalry, archers);
            }
        }

        const auto invadingPid = board->cityIdToPlayerId(invadingCid);
        const auto ppid = board->personPlayer();
        if(invadingPid == ppid && eh) {
            eEventData ed(invadingCid);
            ed.fTile = eh->currentTile();
            board->event(eEvent::playerInvasion, ed);
        }
        printf("invasion handler started: attacker=%s target=%i tile=%p water=%i infantry=%i cavalry=%i archers=%i\n",
               city ? city->name().c_str() : "null",
               static_cast<int>(cid), static_cast<void*>(tile),
               tile->hasWater(), infantry, cavalry, archers);
    };

    const auto pid = board->cityIdToPlayerId(cid);
    const int drachmas = board->drachmas(pid);
    const int bribe = bribeCost();
    const auto bribeFunc = [self, board, pid, bribe, city, cid]() {
        self->mWaitingForResponse = false;
        const auto invadingPid = self->mCity->playerId();
        board->incDrachmas(invadingPid, bribe, eFinanceTarget::tributeReceived);
        board->incDrachmas(pid, -bribe, eFinanceTarget::bribesTributePaid);
        eEventData ed(cid);
        ed.fCity = city;
        board->event(eEvent::invasionBribed, ed);
        board->updateMusic();
        if(self->mConquestEvent) {
            self->mConquestEvent->planArmyReturn();
        }
    };

    if(!isPersonPlayer()) {
        startInvasion();
    } else {
        eEventData ed(cid);
        ed.fCity = mCity;
        ed.fType = eMessageEventType::invasion;
        ed.fBribe = bribe;
        ed.fReason = reason();

        ed.fPrimaryAction = [self, board]() { // surrender
            self->mWaitingForResponse = false;
            board->updateMusic();
            self->invadersWon();
        };
        if(drachmas >= bribe) { // bribe
            ed.fSecondaryAction = bribeFunc;
        }

        ed.fTile = tile;
        ed.fTertiaryAction = startInvasion; // fight
        mWaitingForResponse = true;
        board->event(eEvent::invasion, ed);
        eMusic::playRandomBattleMusic();
    }
}

void eInvasionEvent::useGeneratedCityWarnings() {
    clearWarnings();
    mInitialWarning = nullptr;
    const auto cid = cityId();
    auto& board = *gameBoard();
    addWarning(std::make_shared<eInvasionWarning>(
        1, *this, cid, board, eInvasionWarningType::warning1));
}

void eInvasionEvent::sendInitialAnnouncement() {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    if(!mCity) chooseCity();
    if(!mCity) return;
    updateDisembarkAndShoreTile();

    eEventData ed(cityId());
    ed.fCity = mCity;
    ed.fTile = invasionTile();
    ed.fReason = reason();
    ed.fTime = invasionDelayMonths;
    board->event(eEvent::invasionInitial, ed);
}

std::string eInvasionEvent::longName() const {
    return eLanguage::zeusText(156, 2);
}

void eInvasionEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
    eCityEventValue::write(dst);
    eCountEventValue::write(dst);

    eSaveArchive ar(dst);
    const_cast<eInvasionEvent*>(this)->serialize(ar);
}

void eInvasionEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
    const auto board = gameBoard();
    eCityEventValue::read(src, *board);
    eCountEventValue::read(src);

    eSaveArchive ar(src);
    serialize(ar);
    if(mWarned) {
        board->addInvasion(this);
        updateDisembarkAndShoreTile();
    }
}

void eInvasionEvent::serialize(eSaveArchive& ar) {
    ar.field("mHardcoded", mHardcoded);
    ar.field("mSentByPlayer", mSentByPlayer);

    const auto board = gameBoard();
    if(ar.reading()) {
        ar.readStream().readGameEvent(board, [this](eGameEvent* const e) {
            mConquestEvent = static_cast<ePlayerConquestEvent*>(e);
        });
    } else {
        ar.writeStream().writeGameEvent(mConquestEvent);
    }
    mForces.serialize(ar, board);

    ar.field("mWarned", mWarned);
    if(ar.reading()) {
        mFirstWarning.read(ar.readStream());
    } else {
        mFirstWarning.write(ar.writeStream());
    }
}

bool eInvasionEvent::finished() const {
    return !mWaitingForResponse &&
           mHandlers.empty() &&
           eGameEvent::finished();
}

void eInvasionEvent::setWarningMonths(const int ms) {
    eGameEvent::setWarningMonths(ms);
    mInitialWarning->setWarningMonths(ms);
}

eTile* eInvasionEvent::invasionTile() const {
    const auto cid = cityId();
    const auto board = gameBoard();
    const int ptId = pointId();
    const auto tile = board->invasionTile(cid, ptId);
    return tile;
}

eTile* eInvasionEvent::landInvasionTile() const {
    if(mShoreTile) return mShoreTile;
    return invasionTile();
}

void eInvasionEvent::setFirstWarning(const eDate& w) {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    if(!mCity) chooseCity();
    if(!mCity) {
        printf("invasion warning aborted: no attacker target=%i point=%i\n",
               static_cast<int>(cityId()), pointId());
        return;
    }
    updateDisembarkAndShoreTile();
    board->addInvasion(this);
    mFirstWarning = w;
    mWarned = true;
    printf("invasion warning set: attacker=%s target=%i point=%i tile=%p first_warning_year=%i\n",
           mCity->name().c_str(), static_cast<int>(cityId()), pointId(),
           static_cast<void*>(invasionTile()), w.year());
}

bool eInvasionEvent::activeInvasions() const {
    return !mHandlers.empty();
}

void eInvasionEvent::addInvasionHandler(eInvasionHandler* const i) {
    mHandlers.push_back(i);
}

void eInvasionEvent::removeInvasionHandler(eInvasionHandler* const i) {
    eVectorHelpers::remove(mHandlers, i);
}

bool eInvasionEvent::nearestSoldier(const int fromX, const int fromY,
                                    int& toX, int& toY) const {
    bool found = false;
    int minDist = 99999;
    for(const auto i : mHandlers) {
        int toXX;
        int toYY;
        const bool r = i->nearestSoldier(fromX, fromY, toXX, toYY);
        if(!r) continue;
        const int dx = fromX - toXX;
        const int dy = fromY - toYY;
        const int dist = sqrt(dx*dx + dy*dy);
        if(dist > minDist) continue;
        found = true;
        toX = toXX;
        toY = toYY;
        minDist = dist;
    }
    return found;
}

ePlayerConquestEvent* eInvasionEvent::conquestEvent() const
{ return mConquestEvent; }

int eInvasionEvent::bribeCost() const {
    const auto board = gameBoard();
    if(!board) return 0;
    const auto cid = cityId();
    const auto pid = board->cityIdToPlayerId(cid);
    const auto diff = board->difficulty(pid);
    const int rt = eDifficultyHelpers::soliderBribe(
                       diff, eCharacterType::rockThrower);
    const int ht = eDifficultyHelpers::soliderBribe(
                       diff, eCharacterType::hoplite);
    const int hm = eDifficultyHelpers::soliderBribe(
                       diff, eCharacterType::horseman);

    int infantry = 0;
    int cavalry = 0;
    int archers = 0;

    if(mConquestEvent) {
        for(const auto& b : mForces.fSoldiers) {
            const auto type = b->type();
            switch(type) {
            case eBannerType::hoplite:
            case eBannerType::aresWarrior:
            case eBannerType::amazon:
                infantry += b->count();
                break;
            case eBannerType::horseman:
                cavalry += b->count();
                break;
            case eBannerType::rockThrower:
                archers += b->count();
                break;
            case eBannerType::enemy:
            case eBannerType::trireme:
                break;
            }
        }
        for(const auto& a : mForces.fAllies) {
            int aInfantry;
            int aCavalry;
            int aArchers;
            a->troopsByType(aInfantry, aCavalry, aArchers);
            infantry += aInfantry;
            cavalry += aCavalry;
            archers += aArchers;
        }
    } else {
        soldiersByType(infantry, cavalry, archers);
    }

    const int bribe = rt*archers + ht*infantry + hm*cavalry;
    return bribe;
}

void eInvasionEvent::updateDisembarkAndShoreTile() {
    mDisembarkTile = nullptr;
    const auto tile = invasionTile();
    if(!tile) return;
    const auto terr = tile->terrain();
    if(terr != eTerrain::water) return;
    auto& board = *gameBoard();
    const auto cid = cityId();
    mDisembarkTile = nearestDisembarkTile(tile, board, cid);
    mShoreTile = nearestShoreTile(mDisembarkTile);
}

void eInvasionEvent::soldiersByType(int& infantry,
                                    int& cavalry,
                                    int& archers) const {
    if(mCity) {
        const int c = count();
        mCity->troopsByType(c, infantry, cavalry, archers);
    } else {
        infantry = 0;
        cavalry = 0;
        archers = 0;
    }
}

void eInvasionEvent::restoreAttitudeAfterInvasion(
        const eInvasionResult result) {
    if(!mCity) return;
    if(mSentByPlayer != ePlayerId::neutralFriendly) return;

    auto& board = *gameBoard();
    const int amount = result == eInvasionResult::invaderWonOrPlayerSurrendered ?
                       kInvaderWonAttitudeRestore :
                       kInvaderDefeatedAttitudeRestore;
    mCity->incAttitude(amount, board.personPlayer());
}

void eInvasionEvent::invadersWon() {
    auto& board = *gameBoard();
    const auto targetCity = cityId();
    board.conqueredBy(targetCity, mCity);
    eEventData ed(targetCity);
    ed.fCity = mCity;
    board.event(eEvent::invasionDefeat, ed);
    if(mSentByPlayer != ePlayerId::neutralFriendly) {
        const auto ppid = board.personPlayer();
        const auto& wboard = board.world();
        const auto targetWCity = wboard.cityWithId(targetCity);
        eEventData ed(mSentByPlayer);
        ed.fCity = mCity;
        ed.fRivalCity = targetWCity;
        board.event(eEvent::strikeSuccessful, ed);
        if(mSentByPlayer == ppid) {
            targetWCity->setRelationship(eForeignCityRelationship::vassal);
        }
        const auto ppc = board.currentCityId();
        if(targetCity != ppc) {
            board.moveCityToPlayer(targetCity, mSentByPlayer);
        }
    } else {
        const auto invadingCid = mCity->cityId();
        restoreAttitudeAfterInvasion(eInvasionResult::invaderWonOrPlayerSurrendered);
        const auto invadingPid = board.cityIdToPlayerId(invadingCid);
        const auto invadingC = board.boardCityWithId(invadingCid);
        const auto ppid = board.personPlayer();
        const auto& wboard = board.world();
        const auto targetWCity = wboard.cityWithId(targetCity);
        if(invadingC) {
            eEventData ed(invadingPid);
            ed.fCity = targetWCity;
            board.event(eEvent::cityConquered, ed);
            board.allow(invadingCid, eBuildingType::commemorative, 4);
            if(invadingPid == ppid) {
                targetWCity->setRelationship(eForeignCityRelationship::vassal);
            }
            const auto ppc = board.currentCityId();
            if(targetCity != ppc) {
                board.moveCityToPlayer(targetCity, invadingPid);
            }
            if(mConquestEvent) mConquestEvent->planArmyReturn();
        }
        mCity->setAttitude(60., ppid);
    }
}

void eInvasionEvent::invadersDefeated() {
    auto& board = *gameBoard();
    const auto targetCity = cityId();
    eEventData ed(targetCity);
    ed.fCity = mCity;

    const bool monn = eRand::rand() % 2;
    if(monn) {
        board.allow(targetCity, eBuildingType::commemorative, 1);
        board.event(eEvent::invasionVictoryMonn, ed);
    } else {
        board.event(eEvent::invasionVictory, ed);
    }

    const auto& wboard = board.world();
    const auto targetWCity = wboard.cityWithId(targetCity);
    if(mSentByPlayer != ePlayerId::neutralFriendly) {
        eEventData ed(mSentByPlayer);
        ed.fCity = mCity;
        ed.fRivalCity = targetWCity;
        board.event(eEvent::strikeUnsuccessful, ed);
    } else {
        const auto invadingCid = mCity->cityId();
        restoreAttitudeAfterInvasion(eInvasionResult::invaderDefeated);
        const auto invadingC = board.boardCityWithId(invadingCid);
        if(invadingC) {
            eEventData ied(invadingCid);
            ied.fCity = targetWCity;

            board.event(eEvent::cityConquerFailed, ied);
            if(mConquestEvent) {
                const auto& forces = mConquestEvent->forces();
                forces.kill(1.);
                mConquestEvent->planArmyReturn();
            }
        }
    }
}
