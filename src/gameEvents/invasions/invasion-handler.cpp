#include "invasion-handler.h"

#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

#include "engine/eeventdata.h"
#include "engine/eevent.h"

#include "characters/actions/soldier-action.h"

#include "characters/egreekhoplite.h"
#include "characters/egreekhorseman.h"
#include "characters/egreekrockthrower.h"

#include "characters/etrojanhoplite.h"
#include "characters/etrojanspearthrower.h"
#include "characters/etrojanhorseman.h"

#include "characters/ecentaurhorseman.h"
#include "characters/ecentaurarcher.h"

#include "characters/epersianhoplite.h"
#include "characters/epersianhorseman.h"
#include "characters/epersianarcher.h"

#include "characters/eegyptianhoplite.h"
#include "characters/eegyptianchariot.h"
#include "characters/eegyptianarcher.h"

#include "characters/eatlanteanhoplite.h"
#include "characters/eatlanteanchariot.h"
#include "characters/eatlanteanarcher.h"

#include "characters/emayanhoplite.h"
#include "characters/emayanarcher.h"

#include "characters/eoceanidhoplite.h"
#include "characters/eoceanidspearthrower.h"

#include "characters/ephoenicianhorseman.h"
#include "characters/ephoenicianarcher.h"

#include "characters/eamazon.h"
#include "characters/eareswarrior.h"

#include "characters/eenemyboat.h"

#include "characters/actions/eattackcityaction.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/ekillcharacterfinishfail.h"

#include "enumbers.h"
#include "etilehelper.h"
#include "buildings/epalace.h"

#include "invasion-event.h"
#include "invasion-general.h"
#include "gameEvents/conquest/player-conquest-event.h"
#include "characters/soldier-banner.h"
#include "characters/formation-facing.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"
#include "engine/boardData/eheatmaptask.h"

#include "eiteratesquare.h"
#include "erand.h"

const int boatSpawnPeriod = 825;
const int spawnWaitDays = 14;

eInvasionHandler::eInvasionHandler(GameBoard& board,
                                   const eCityId targetCity,
                                   const stdsptr<WorldCity>& city,
                                   eInvasionEvent* const event) :
    mBoard(board), mTargetCity(targetCity), mCity(city), mEvent(event) {
    board.addInvasionHandler(targetCity, this);
    if(event) event->addInvasionHandler(this);
    mAttackType = static_cast<InvasionAttackType>(
            eRand::rand() % static_cast<int>(InvasionAttackType::count));
}

eInvasionHandler::~eInvasionHandler() {
    mBoard.removeInvasionHandler(mTargetCity, this);
    if(mEvent) mEvent->removeInvasionHandler(this);
}

void eInvasionHandler::disembark() {
    const auto cid = mCity->cityId();

    int mRem = mSoldiersPerBoat;
    eSs ss;
    for(auto& s : mForcesLeft) {
        int& rem = s.second;
        const int use = std::min(mRem, rem);
        if(use > 0) {
            rem -= use;
            mRem -= use;

            ss.push_back({s.first, use});
        }
    }
    if(!ss.empty()) {
        std::vector<SoldierBanner*> solds;
        generateSoldiersForCity(mTile, ss, cid, solds);
    }

    int infantry = 0;
    int cavalry = 0;
    int archers = 0;

    {
        const int use = std::min(mRem, mInfantryLeft);
        if(use > 0) {
            mRem -= use;
            mInfantryLeft -= use;
            infantry = use;
        }
    }
    {
        const int use = std::min(mRem, mCavalryLeft);
        if(use > 0) {
            mRem -= use;
            mCavalryLeft -= use;
            cavalry = use;
        }
    }
    {
        const int use = std::min(mRem, mArchersLeft);
        if(use > 0) {
            mRem -= use;
            mArchersLeft -= use;
            archers = use;
        }
    }
    {
        std::vector<SoldierBanner*> solds;
        const auto nat = mCity->nationality();
        generateSoldiersForCity(mTile,
                                infantry, cavalry, archers,
                                cid, nat, solds);
    }

    for(auto& f : mAllyForcesLeft) {
        int infantry = 0;
        int cavalry = 0;
        int archers = 0;

        {
            const int use = std::min(mRem, f.fInfantryLeft);
            if(use > 0) {
                mRem -= use;
                f.fInfantryLeft -= use;
                infantry = use;
            }
        }
        {
            const int use = std::min(mRem, f.fCavalryLeft);
            if(use > 0) {
                mRem -= use;
                f.fCavalryLeft -= use;
                cavalry = use;
            }
        }
        {
            const int use = std::min(mRem, f.fArchersLeft);
            if(use > 0) {
                mRem -= use;
                f.fArchersLeft -= use;
                archers = use;
            }
        }
        {
            std::vector<SoldierBanner*> solds;
            generateSoldiersForCity(mTile,
                                    infantry, cavalry, archers,
                                    f.fCid, f.fNat, solds);
        }
    }

    std::vector<SoldierBanner*> solds;
    for(const auto& b : mBanners) {
        solds.push_back(b.get());
    }

    const int tx = mTile->x();
    const int ty = mTile->y();
    int facing;
    int lineDX;
    int lineDY;
    spawnFacingTowardTarget(tx, ty, facing, lineDX, lineDY);
    SoldierBanner::sPlaceFacing(solds, tx, ty, mBoard, facing, lineDX, lineDY, 3, 3);
    if(eNumbers::sInvasionAppearAtPlaces) {
        for(const auto b : solds) {
            b->teleportSoldiersToPlaces();
        }
    }
    mGState.fCurrentTile = mTile;
    mGState.fSpawnWait = spawnWaitDays*eNumbers::sDayLength;
}

void eInvasionHandler::initializeSeaInvasion(
        eTile* const waterTile,
        eTile* const disembarkTile,
        eTile* const shoreTile,
        const int infantry,
        const int cavalry,
        const int archers) {
    mDisembarkTile = disembarkTile;
    mTile = shoreTile;
    mGState.fCurrentTile = mTile;

    mInfantryLeft = infantry;
    mCavalryLeft = cavalry;
    mArchersLeft = archers;

    initializeBoats(waterTile, mInfantryLeft + mCavalryLeft + mArchersLeft);
}

void eInvasionHandler::initializeSeaInvasion(
        eTile* const waterTile,
        eTile* const disembarkTile,
        eTile* const shoreTile,
        const eEnlistedForces& forces,
        PlayerConquestEvent* const conquestEvent) {
    int troops = 0;

    mDisembarkTile = disembarkTile;
    mTile = shoreTile;

    mConquestEvent = conquestEvent;
    extractSSFromForces(forces, mForcesLeft);
    for(const auto& f : mForcesLeft) {
        troops += f.second;
    }
    mAresLeft = forces.fAres;
    for(const auto& h : forces.fHeroes) {
        mHeroesLeft.push_back(h.second);
    }

    for(const auto& f : forces.fAllies) {
        const auto cid = f->cityId();
        const auto nat = f->nationality();
        std::vector<SoldierBanner*> solds;
        int infantry;
        int cavalry;
        int archers;
        f->troopsByType(infantry, cavalry, archers);
        mAllyForcesLeft.push_back(eAllyForces{cid, nat, infantry, cavalry});
        troops += infantry + cavalry + archers;
    }

    initializeBoats(waterTile, troops);
}

template <typename T>
stdsptr<T> spawnSoldier(GameBoard& board,
                        const eCityId ocid,
                        const eCityId cid,
                        eTile* const tile) {
    const auto h = e::make_shared<T>(board);
    h->setCityId(cid);
    h->setOnCityId(ocid);
    const auto a = e::make_shared<SoldierAction>(h.get());
    a->setSpreadPeriod(true);
    h->setAction(a);
    h->changeTile(tile);
    h->setActionType(eCharacterActionType::walk);
    return h;
}

void
eInvasionHandler::generateSoldiersForCity(
        eTile* const tile,
        const int infantry,
        const int cavalry,
        const int archers,
        const eCityId cid,
        const eNationality nat,
        std::vector<SoldierBanner*>& solds) {
    const auto ocid = tile->cityId();

    const int tx = tile->x();
    const int ty = tile->y();

    stdsptr<SoldierBanner> b;
    const auto requestBanner = [&](const eBannerFormationRole role) {
        if(!b || b->count() >= 8) {
            b = e::make_shared<SoldierBanner>(
                    eBannerType::enemy, mBoard);
            b->setFormationRole(role);
            b->setOnCityId(ocid);
            b->setCityId(cid);
            mBanners.push_back(b);
            solds.push_back(b.get());
            b->setCityId(cid);
            b->backFromHome();
            b->moveTo(tx, ty);
        }
    };

    for(int i = 0; i < infantry; i++) {
        stdsptr<eSoldier> s;
        switch(nat) {
        case eNationality::greek:
            s = spawnSoldier<eGreekHoplite>(mBoard, ocid, cid, tile);
            break;
        case eNationality::trojan:
            s = spawnSoldier<eTrojanHoplite>(mBoard, ocid, cid, tile);
            break;
        case eNationality::persian:
            s = spawnSoldier<ePersianHoplite>(mBoard, ocid, cid, tile);
            break;
        case eNationality::centaur:
            break;
        case eNationality::amazon: {
            const auto a = spawnSoldier<eAmazon>(mBoard, ocid, cid, tile);
            a->setIsArcher(false);
            s = a;
        } break;

        case eNationality::egyptian:
            s = spawnSoldier<eEgyptianHoplite>(mBoard, ocid, cid, tile);
            break;
        case eNationality::mayan:
            s = spawnSoldier<eMayanHoplite>(mBoard, ocid, cid, tile);
            break;
        case eNationality::phoenician:
            break;
        case eNationality::oceanid:
            s = spawnSoldier<eOceanidHoplite>(mBoard, ocid, cid, tile);
            break;
        case eNationality::atlantean:
            s = spawnSoldier<eAtlanteanHoplite>(mBoard, ocid, cid, tile);
            break;

        default:
            break;
        }
        if(s) {
            requestBanner(eBannerFormationRole::melee);
            b->incCount();
            s->setBanner(b.get());
        }
    }
    b.reset();
    for(int i = 0; i < cavalry; i++) {
        stdsptr<eSoldier> s;
        switch(nat) {
        case eNationality::greek:
            s = spawnSoldier<eGreekHorseman>(mBoard, ocid, cid, tile);
            break;
        case eNationality::trojan:
            s = spawnSoldier<eTrojanHorseman>(mBoard, ocid, cid, tile);
            break;
        case eNationality::persian:
            s = spawnSoldier<ePersianHorseman>(mBoard, ocid, cid, tile);
            break;
        case eNationality::centaur:
            s = spawnSoldier<eCentaurHorseman>(mBoard, ocid, cid, tile);
            break;
        case eNationality::amazon:
            break;

        case eNationality::egyptian:
            s = spawnSoldier<eEgyptianChariot>(mBoard, ocid, cid, tile);
            break;
        case eNationality::mayan:
            break;
        case eNationality::phoenician:
            s = spawnSoldier<ePhoenicianHorseman>(mBoard, ocid, cid, tile);
            break;
        case eNationality::oceanid:
            break;
        case eNationality::atlantean:
            s = spawnSoldier<eAtlanteanChariot>(mBoard, ocid, cid, tile);
            break;

        default:
            break;
        }
        if(s) {
            requestBanner(eBannerFormationRole::cavalry);
            b->incCount();
            s->setBanner(b.get());
        }
    }
    b.reset();
    for(int i = 0; i < archers; i++) {
        stdsptr<eSoldier> s;
        switch(nat) {
        case eNationality::greek:
            s = spawnSoldier<eGreekRockThrower>(mBoard, ocid, cid, tile);
            break;
        case eNationality::trojan:
            s = spawnSoldier<eTrojanSpearthrower>(mBoard, ocid, cid, tile);
            break;
        case eNationality::persian:
            s = spawnSoldier<ePersianArcher>(mBoard, ocid, cid, tile);
            break;
        case eNationality::centaur:
            s = spawnSoldier<eCentaurArcher>(mBoard, ocid, cid, tile);
            break;
        case eNationality::amazon: {
            const auto a = spawnSoldier<eAmazon>(mBoard, ocid, cid, tile);
            a->setIsArcher(true);
            s = a;
        } break;

        case eNationality::egyptian:
            s = spawnSoldier<eEgyptianArcher>(mBoard, ocid, cid, tile);
            break;
        case eNationality::mayan:
            s = spawnSoldier<eMayanArcher>(mBoard, ocid, cid, tile);
            break;
        case eNationality::phoenician:
            s = spawnSoldier<ePhoenicianArcher>(mBoard, ocid, cid, tile);
            break;
        case eNationality::oceanid:
            s = spawnSoldier<eOceanidSpearthrower>(mBoard, ocid, cid, tile);
            break;
        case eNationality::atlantean:
            s = spawnSoldier<eAtlanteanArcher>(mBoard, ocid, cid, tile);
            break;

        default:
            break;
        }
        if(s) {
            requestBanner(eBannerFormationRole::missile);
            b->incCount();
            s->setBanner(b.get());
        }
    }
}

void eInvasionHandler::initializeLandInvasion(
        eTile* const tile,
        const int infantry,
        const int cavalry,
        const int archers) {
    mStage = eInvasionStage::active;
    mGState.fPhase = eGeneralPhase::spread;
    mTile = tile;

    const auto cid = mCity->cityId();
    const auto nat = mCity->nationality();
    std::vector<SoldierBanner*> solds;
    generateSoldiersForCity(tile,
                            infantry, cavalry, archers,
                            cid, nat, solds);

    const int tx = tile->x();
    const int ty = tile->y();
    int facing;
    int lineDX;
    int lineDY;
    spawnFacingTowardTarget(tx, ty, facing, lineDX, lineDY);
    SoldierBanner::sPlaceFacing(solds, tx, ty, mBoard, facing, lineDX, lineDY, 3, 3);
    if(eNumbers::sInvasionAppearAtPlaces) {
        for(const auto b : solds) {
            b->teleportSoldiersToPlaces();
        }
    }
    mGState.fCurrentTile = tile;
    mGState.fSpawnWait = spawnWaitDays*eNumbers::sDayLength;
}

void eInvasionHandler::initializeLandInvasion(
        eTile* const tile,
        const eEnlistedForces& forces,
        PlayerConquestEvent* const conquestEvent) {
    mStage = eInvasionStage::active;
    mGState.fPhase = eGeneralPhase::spread;
    mConquestEvent = conquestEvent;
    mTile = tile;

    const auto cid = mCity->cityId();
    std::vector<SoldierBanner*> solds;
    {
        eSs ss;
        extractSSFromForces(forces, ss);
        generateSoldiersForCity(tile, ss, cid, solds);
    }

    for(const auto& f : forces.fAllies) {
        const auto cid = f->cityId();
        const auto nat = f->nationality();
        int infantry;
        int cavalry;
        int archers;
        f->troopsByType(infantry, cavalry, archers);
        generateSoldiersForCity(tile,
                                infantry, cavalry, archers,
                                cid, nat, solds);
    }

    std::vector<eHeroType> heroes;
    for(const auto& h : forces.fHeroes) {
        heroes.push_back(h.second);
    }
    generateImmortals(tile, cid, forces.fAres, heroes);

    const int tx = tile->x();
    const int ty = tile->y();
    int facing;
    int lineDX;
    int lineDY;
    spawnFacingTowardTarget(tx, ty, facing, lineDX, lineDY);
    SoldierBanner::sPlaceFacing(solds, tx, ty, mBoard, facing, lineDX, lineDY, 3, 3);
    if(eNumbers::sInvasionAppearAtPlaces) {
        for(const auto b : solds) {
            b->teleportSoldiersToPlaces();
        }
    }
    mGState.fCurrentTile = tile;
    mGState.fSpawnWait = spawnWaitDays*eNumbers::sDayLength;
}

void
eInvasionHandler::generateSoldiersForCity(
        eTile* const tile,
        const eSs& soldTypes,
        const eCityId cid,
        std::vector<SoldierBanner*>& solds) {
    const auto ocid = tile->cityId();

    const int tx = tile->x();
    const int ty = tile->y();

    stdsptr<SoldierBanner> b;
    const auto requestBanner = [&](const eBannerFormationRole role) {
        if(!b || b->count() >= 8) {
            b = e::make_shared<SoldierBanner>(
                    eBannerType::enemy, mBoard);
            b->setFormationRole(role);
            b->setOnCityId(ocid);
            b->setCityId(cid);
            mBanners.push_back(b);
            solds.push_back(b.get());
            b->backFromHome();
            b->moveTo(tx, ty);
        }
    };

    for(const auto& type : soldTypes) {
        for(int i = 0; i < type.second; i++) {
            stdsptr<eSoldier> s;
            switch(type.first) {
            case ePlayerSoldierType::greekHoplite:
                s = spawnSoldier<eGreekHoplite>(mBoard, ocid, cid, tile);
                break;
            case ePlayerSoldierType::greekHorseman:
                s = spawnSoldier<eGreekHorseman>(mBoard, ocid, cid, tile);
                break;
            case ePlayerSoldierType::greekRockthrower:
                s = spawnSoldier<eGreekRockThrower>(mBoard, ocid, cid, tile);
                break;

            case ePlayerSoldierType::atlanteanHoplite:
                s = spawnSoldier<eAtlanteanHoplite>(mBoard, ocid, cid, tile);
                break;
            case ePlayerSoldierType::atlanteanArcher:
                s = spawnSoldier<ePersianHoplite>(mBoard, ocid, cid, tile);
                break;
            case ePlayerSoldierType::atlanteanChariot:
                s = spawnSoldier<ePersianHoplite>(mBoard, ocid, cid, tile);
                break;

            case ePlayerSoldierType::amazon: {
                const auto a = spawnSoldier<eAmazon>(mBoard, ocid, cid, tile);
                a->setIsArcher(true);
                s = a;
            } break;
            case ePlayerSoldierType::aresWarrior:
                s = spawnSoldier<eAresWarrior>(mBoard, ocid, cid, tile);
                break;

            default:
                break;
            }
            if(s) {
                eBannerFormationRole role = eBannerFormationRole::melee;
                switch(type.first) {
                case ePlayerSoldierType::greekHorseman:
                case ePlayerSoldierType::atlanteanChariot:
                    role = eBannerFormationRole::cavalry;
                    break;
                case ePlayerSoldierType::greekRockthrower:
                case ePlayerSoldierType::atlanteanArcher:
                case ePlayerSoldierType::amazon:
                    role = eBannerFormationRole::missile;
                    break;
                default:
                    break;
                }
                requestBanner(role);
                b->incCount();
                s->setBanner(b.get());
            }
        }
        b.reset();
    }
}

void eInvasionHandler::tellHeroesAndGodsToGoBack() const {
    for(const auto& c : mHeroesAndGods) {
        if(!c) continue;
        const auto a = c->action();
        if(const auto aa = dynamic_cast<eAttackCityAction*>(a)) {
            aa->invasionFinished();
        }
    }
}

void eInvasionHandler::extractSSFromForces(
        const eEnlistedForces& forces, eSs& ss) const {
    auto& world = mBoard.world();
    int greekInfantry = 0;
    int greekCavalry = 0;
    int greekArchers = 0;
    int atlanteanInfantry = 0;
    int atlanteanCavalry = 0;
    int atlanteanArchers = 0;
    int aresWarriors = 0;
    int amazons = 0;
    for(const auto& s : forces.fSoldiers) {
        const auto cid = s->cityId();
        const auto c = world.cityWithId(cid);
        const auto nat = c->nationality();
        const bool atlantean = nat == eNationality::atlantean;
        const auto type = s->type();
        if(atlantean) {
            if(type == eBannerType::hoplite) {
                atlanteanInfantry += s->count();
            } else if(type == eBannerType::horseman) {
                atlanteanCavalry += s->count();
            } else if(type == eBannerType::rockThrower) {
                atlanteanArchers += s->count();
            } else if(type == eBannerType::aresWarrior) {
                aresWarriors += s->count();
            } else if(type == eBannerType::amazon) {
                amazons += s->count();
            }
        } else {
            if(type == eBannerType::hoplite) {
                greekInfantry += s->count();
            } else if(type == eBannerType::horseman) {
                greekCavalry += s->count();
            } else if(type == eBannerType::rockThrower) {
                greekArchers += s->count();
            } else if(type == eBannerType::aresWarrior) {
                aresWarriors += s->count();
            } else if(type == eBannerType::amazon) {
                amazons += s->count();
            }
        }
    }

    if(greekInfantry > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
            ePlayerSoldierType::greekHoplite, greekInfantry));
    }
    if(atlanteanInfantry > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
            ePlayerSoldierType::atlanteanHoplite, greekInfantry));
    }
    if(greekCavalry > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
            ePlayerSoldierType::greekHorseman, greekCavalry));
    }
    if(atlanteanCavalry > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
            ePlayerSoldierType::atlanteanChariot, greekCavalry));
    }
    if(greekArchers > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
            ePlayerSoldierType::greekRockthrower, greekArchers));
    }
    if(atlanteanArchers > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
            ePlayerSoldierType::atlanteanArcher, greekArchers));
    }
    if(amazons > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
                            ePlayerSoldierType::amazon, amazons));
    }
    if(aresWarriors > 0) {
        ss.push_back(std::pair<ePlayerSoldierType, int>(
                            ePlayerSoldierType::aresWarrior, aresWarriors));
    }
}

void eInvasionHandler::incTime(const int by) {
    // Retaliation brain: enemy banners only chase defenders after being hit.
    // Strategic building movement stays with InvasionGeneral.
    if(mStage != eInvasionStage::arrive &&
       mStage != eInvasionStage::comeback &&
       mStage != eInvasionStage::walkOff) {
        for(const auto& b : mBanners) {
            if(b->count() <= 0) continue;
            b->tickCombat(by);
        }
    }

    const auto invasionDefeated = [&]() {
        tellHeroesAndGodsToGoBack();
        if(mEvent) mEvent->invadersDefeated();
    };
    if(mStage == eInvasionStage::arrive) {
        if(mBoatsLeft > 0) {
            mWait += by;
            if(mWait < boatSpawnPeriod) return;
            mWait -= boatSpawnPeriod;
            spawnBoat();
            mBoatsLeft--;
            return;
        }
        mWait = 0;
        bool found = false;
        for(const auto& b : mBoats) {
            if(b) {
                found = true;
                break;
            }
        }
        if(!found) {
            if(mBanners.empty() && !mAresLeft && mHeroesLeft.empty()) {
                invasionDefeated();
                mStage = eInvasionStage::comeback;
            } else {
                generateImmortals(mTile, mCity->cityId(),
                                  mAresLeft, mHeroesLeft);
                mStage = eInvasionStage::active;
                mGState.fPhase = eGeneralPhase::spread;
                mGState.fSpawnWait = spawnWaitDays*eNumbers::sDayLength;
            }
        }
        return;
    }
    // Active campaign. Gather live banners + total strength for the wipe check.
    int ss = 0;
    std::vector<SoldierBanner*> solds;
    for(const auto& b : mBanners) {
        const int c = b->count();
        if(c <= 0) continue;
        solds.push_back(b.get());
        ss += c;
    }

    const auto goBack = [&]() {
        const int tx = mTile->x();
        const int ty = mTile->y();
        for(const auto& b : solds) {
            b->cancelSoldiersAttack();
            b->moveTo(tx, ty);
        }
        mGState.fMoveFrom = mGState.fCurrentTile;
        mGState.fMoveTo = mTile;
        mGState.fCurrentTile = mTile;
        mGState.fTargetTile = nullptr;
        tellHeroesAndGodsToGoBack();
    };

    const auto walkOff = [&]() {
        const auto exit = mBoard.exitPoint(mTargetCity);
        const auto from = mGState.fCurrentTile ? mGState.fCurrentTile :
                          (solds.empty() ? mTile : solds.front()->tile());
        for(const auto& b : solds) {
            b->cancelSoldierActions();
            if(b->count() > 0) b->goAbroad();
        }
        mGState.fMoveFrom = from;
        mGState.fMoveTo = exit;
        mGState.fCurrentTile = exit;
        mGState.fTargetTile = nullptr;
        mWait = 0;
        tellHeroesAndGodsToGoBack();
        mStage = eInvasionStage::walkOff;
    };

    if(mStage == eInvasionStage::comeback) {
        const int tx = mTile->x();
        const int ty = mTile->y();
        bool allArrived = true;
        mWait += by;
        mComebackTimeout += by;
        for(const auto& b : mBanners) {
            if(!b->soldiersOnMap()) continue;
            if(!b->stationary()) {
                if(mWait >= 2000) {
                    b->cancelSoldiersAttack();
                    b->moveTo(tx, ty);
                }
                allArrived = false;
            }
        }
        if(mWait >= 2000) mWait = 0;
        if(!allArrived && mComebackTimeout < stallMaxWait) return;
        if(!allArrived) {
            for(const auto& b : mBanners) b->teleportSoldiersToPlaces();
        }
        for(const auto& b : mBanners) {
            if(b->count() > 0) b->goAbroad();
        }
        mGState.fMoveFrom = mTile;
        mGState.fMoveTo = mBoard.exitPoint(mTargetCity);
        mWait = 0;
        mStage = eInvasionStage::walkOff;
        return;
    }

    if(mStage == eInvasionStage::walkOff) {
        bool anyOnMap = false;
        for(const auto& b : mBanners) {
            if(b->soldiersOnMap()) { anyOnMap = true; break; }
        }
        if(anyOnMap) {
            mWait += by;
            if(mWait < stallMaxWait) return;
            mWait = 0;
            for(const auto& b : mBanners) {
                b->teleportSoldiersToPlaces();
                if(b->count() > 0) b->goAbroad();
            }
            return;
        }
        if(mFireRaidOverOnExit) {
            eEventData ed(mTargetCity);
            ed.fCity = mCity;
            mBoard.event(eEvent::invasionRaidOver, ed);
        }
        delete this;
        return;
    }

    // Force wiped: defenders won the field. Handler owns defeat reporting.
    if(ss == 0) {
        if(!immortalsFighting()) {
            invasionDefeated();
            delete this;
        }
        return;
    }

    // Stall detection: ss unchanged means no soldiers died — troops are stuck.
    // Every 2 min: teleport stuck soldiers to their banner tile so combat can resolve.
    if(ss == mLastSs) {
        mStallTime += by;
        if(mStallTime >= 120000) {
            mStallTime = 0;
for(const auto& b : mBanners) {
                if(b->count() <= 0) continue;
                const auto bt = b->tile();
                if(!bt) continue;
                for(const auto s : b->soldiers()) {
                    if(!s) continue;
                    const auto slot = b->place(s);
                    s->changeTile(slot ? slot : bt);
                    const auto a = s->soldierAction();
                    if(a) a->setCurrentAction(nullptr);
                }
            }
        }
    } else {
        mLastSs = ss;
        mStallTime = 0;
    }

    // Friendly team (e.g. ally turned non-hostile): break off, go home.
    const auto invadingPid = mBoard.cityIdToPlayerId(mCity->cityId());
    const auto invadingTid = mBoard.playerIdToTeamId(invadingPid);
    const auto invadedTid = mBoard.cityIdToTeamId(mTargetCity);
    if(invadingTid == invadedTid) {
        goBack();
        mStage = eInvasionStage::comeback;
        return;
    }

    // Palace destroyed mid-campaign: city is taken immediately.
    if(!mBoard.palace(mTargetCity)) {
        if(mConquestEvent) {
            const auto& forces = mConquestEvent->forces();
            const int iniCount = forces.count();
            if(iniCount > 0) {
                int count = 0;
                for(const auto& b : mBanners) {
                    count += b->count();
                }
                forces.kill(1 - double(count)/iniCount);
            }
        }
        walkOff();
        const auto targetWCity = mBoard.world().cityWithId(mTargetCity);
        mBoard.conqueredBy(mCity->cityId(), targetWCity);
        assert(mEvent);
        mEvent->invadersWon();
        return;
    }

    // Drive the campaign. General is stateless; it reads/writes mGState.
    const InvasionGeneral general(mBoard, mTargetCity,
                                  mCity->cityId(), mAttackType);
    const bool done = general.advance(mGState, mTile, solds, by);
    if(!done) return;

    // Campaign complete: no attackable targets of the attack type remain.
    if(!mBoard.palace(mTargetCity)) {
        // Palace gone: the city is taken.
        if(mConquestEvent) {
            const auto& forces = mConquestEvent->forces();
            const int iniCount = forces.count();
            if(iniCount > 0) {
                int count = 0;
                for(const auto& b : mBanners) {
                    count += b->count();
                }
                forces.kill(1 - double(count)/iniCount);
            }
        }
        walkOff();
        const auto targetWCity = mBoard.world().cityWithId(mTargetCity);
        mBoard.conqueredBy(mCity->cityId(), targetWCity);
        assert(mEvent);
        mEvent->invadersWon();
    } else {
        // Palace stands: raid over, troops retreat. City NOT conquered.
        goBack();
        mFireRaidOverOnExit = true;
        mStage = eInvasionStage::comeback;
    }
}

void eInvasionHandler::spawnFacingTowardTarget(
        const int fromX, const int fromY,
        int& facing, int& lineDX, int& lineDY) const {
    const auto rect = mBoard.boardCityTileBRect(mTargetCity);
    const int cx = rect.x + rect.w/2;
    const int cy = rect.y + rect.h/2;
    eFormationFacing::facingAndLineToward(cx - fromX, cy - fromY,
                                          facing, lineDX, lineDY);
}

void eInvasionHandler::serialize(eSaveArchive& ar) {
    ar.field("ioId", mIOID);
    ar.worldCityField("city", &mBoard, mCity);
    ar.tileField("tile", mBoard, mTile);
    ar.field("stage", mStage);
    ar.field("attackType", mAttackType, InvasionAttackType::food);

    // General campaign state (general is stateless; handler owns + persists it).
    ar.field("gPhase", mGState.fPhase, eGeneralPhase::spread);
    ar.field("gPhaseBeforeDefend", mGState.fPhaseBeforeDefend,
             eGeneralPhase::spread);
    ar.tileField("gTargetTile", mBoard, mGState.fTargetTile);
    ar.tileField("gCurrentTile", mBoard, mGState.fCurrentTile);
    ar.tileField("gMoveFrom", mBoard, mGState.fMoveFrom);
    ar.tileField("gMoveTo", mBoard, mGState.fMoveTo);
    ar.field("gWait", mGState.fWait, 0);
    ar.field("gSpawnWait", mGState.fSpawnWait, 0);
    ar.field("gMoveWait", mGState.fMoveWait, 0);
    ar.field("gDefendHold", mGState.fDefendHold, 0);
    ar.field("gDefendEnemyWait", mGState.fDefendEnemyWait, 0);
    ar.field("gRepinWait", mGState.fRepinWait, 0);


    ar.arrayField("banners", mBanners,
        [this](eSaveArchive& itemAr, stdsptr<SoldierBanner>& b) {
            eBannerType type = b ? b->type() : eBannerType::hoplite;
            itemAr.field("type", type);
            if(itemAr.reading()) {
                b = e::make_shared<SoldierBanner>(type, mBoard);
            }
            b->serialize(itemAr);
        });

    ar.field("wait", mWait);
    ar.field("stallTime", mStallTime, 0);
    ar.field("lastSs", mLastSs, -1);
    ar.gameEventField("event", &mBoard, mEvent);
    if(ar.reading()) {
        ar.addPostFunc([this]() {
            if(mEvent) mEvent->addInvasionHandler(this);
        }, "eInvasionHandler::event.addHandler");
    }
    ar.gameEventField("conquestEvent", &mBoard, mConquestEvent);

    ar.arrayField("heroesAndGods", mHeroesAndGods,
        [this](eSaveArchive& itemAr, stdptr<eCharacter>& c) {
            itemAr.characterField("c", &mBoard, c);
        });

    ar.field("infantryLeft", mInfantryLeft);
    ar.field("cavalryLeft", mCavalryLeft);
    ar.field("archersLeft", mArchersLeft);

    ar.arrayField("forcesLeft", mForcesLeft,
        [](eSaveArchive& itemAr, std::pair<ePlayerSoldierType, int>& s) {
            itemAr.field("type", s.first);
            itemAr.field("count", s.second);
        });

    ar.field("aresLeft", mAresLeft);

    ar.arrayField("heroesLeft", mHeroesLeft,
        [](eSaveArchive& itemAr, eHeroType& h) {
            itemAr.field("hero", h);
        });

    ar.tileField("boatsTile", mBoard, mBoatsTile);
    ar.tileField("disembarkTile", mBoard, mDisembarkTile);
    ar.field("boatsLeft", mBoatsLeft);

    ar.arrayField("boats", mBoats,
        [this](eSaveArchive& itemAr, stdptr<eCharacter>& b) {
            itemAr.characterField("c", &mBoard, b);
        });
}

void eInvasionHandler::killAllWithCorpse() {
    mWait = 0;
    for(const auto& b : mBanners) {
        b->killAllWithCorpse();
    }
    for(const auto& c : mHeroesAndGods) {
        if(!c) continue;
        bool isGod = false;
        const auto cType = c->type();
        God::sCharacterToGodType(cType, &isGod);
        if(isGod) {
            const auto a = c->action();
            if(const auto aa = dynamic_cast<GodMonsterAction*>(a)) {
                aa->disappear(true);
            } else {
                c->killWithCorpse();
            }
        } else {
            c->killWithCorpse();
        }
    }
}

bool eInvasionHandler::nearestSoldier(const int fromX, const int fromY,
                                      int& toX, int& toY) const {
    bool found = false;
    int minDist = 99999;
    for(const auto& b : mBanners) {
        int toXX;
        int toYY;
        const bool r = b->nearestSoldier(fromX, fromY, toXX, toYY);
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

void eInvasionHandler::initializeBoats(eTile* const tile, const int troops) {
    mBoatsTile = tile;
    const int nBoats = std::ceil(double(troops)/mSoldiersPerBoat);
    mBoatsLeft = nBoats;
    mWait = boatSpawnPeriod;
}

void eInvasionHandler::spawnBoat() {
    const auto cid = mCity->cityId();
    const auto ocid = mBoatsTile->cityId();
    const auto b = e::make_shared<eEnemyBoat>(mBoard);
    b->setCityId(cid);
    b->setOnCityId(ocid);
    b->changeTile(mBoatsTile);
    const auto a = e::make_shared<eMoveToAction>(b.get());
    a->setStateRelevance(eStateRelevance::terrain);
    b->setActionType(eCharacterActionType::walk);
    b->setAction(a);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
                          mBoard, b.get());
    const auto finish = std::make_shared<eEnemyBoatFinish>(
                            mBoard, b.get(), this);
    a->setFinishAction(finish);
    a->setFailAction(fail);
    const stdptr<eEnemyBoat> bptr(b.get());
    a->setFindFailAction([bptr]() {
        if(bptr) bptr->kill();
    });
    mBoats.push_back(b.get());
    a->start(mDisembarkTile, WalkableObject::sCreateDeepWater());
}

void eInvasionHandler::generateImmortals(
        eTile* const tile, const eCityId cid,
        const bool ares, const std::vector<eHeroType>& heroes) {
    const auto tileCid = tile->cityId();
    const int tx = tile->x();
    const int ty = tile->y();

    const auto prcsAttack = [&](const stdsptr<eCharacter>& c) {
        c->setOnCityId(tileCid);
        c->setCityId(cid);
        bool found = false;
        const auto prcsTile = [&](const int dx, const int dy) {
            const int x = tx + dx;
            const int y = ty + dy;
            const auto t = mBoard.tile(x, y);
            if(!t) return false;
            const auto tCid = t->cityId();
            if(tCid != tileCid) return false;
            const auto& chars = t->characters();
            if(!chars.empty()) return false;
            c->changeTile(t);
            found = true;
            return true;
        };
        for(int k = 0; !found; k++) {
            (void)found;
            eIterateSquare::iterateSquare(k, prcsTile);
        }
        const auto a = e::make_shared<eAttackCityAction>(c.get());
        c->setAction(a);
        mHeroesAndGods.push_back(c);
    };

    if(ares) {
        const auto god = God::sCreateGod(GodType::ares, mBoard);
        god->setAttitude(GodAttitude::hostile);
        prcsAttack(god);
    }

    for(const auto h : heroes) {
        const auto hero = eHero::sCreateHero(h, mBoard);
        prcsAttack(hero);
    }
}

bool eInvasionHandler::immortalsFighting() const {
    for(const auto& i : mHeroesAndGods) {
        if(i) return true;
    }
    return false;
}
