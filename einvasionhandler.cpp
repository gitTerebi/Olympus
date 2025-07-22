#include "einvasionhandler.h"

#include "engine/egameboard.h"

#include "engine/eeventdata.h"

#include "characters/actions/esoldieraction.h"

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

#include "characters/actions/eattackcityaction.h"

#include "engine/eevent.h"

#include "etilehelper.h"
#include "buildings/epalace.h"

#include "gameEvents/einvasionevent.h"
#include "gameEvents/eplayerconquestevent.h"

#include "eiteratesquare.h"

eInvasionHandler::eInvasionHandler(eGameBoard& board,
                                   const eCityId targetCity,
                                   const stdsptr<eWorldCity>& city,
                                   eInvasionEvent* const event) :
    mBoard(board), mTargetCity(targetCity), mCity(city), mEvent(event) {
    board.addInvasionHandler(targetCity, this);
    if(event) event->addInvasionHandler(this);
}

eInvasionHandler::~eInvasionHandler() {
    mBoard.removeInvasionHandler(mTargetCity, this);
    if(mEvent) mEvent->removeInvasionHandler(this);
}

template <typename T>
stdsptr<T> spawnSoldier(eGameBoard& board,
                        const eCityId ocid,
                        const eCityId cid,
                        eTile* const tile) {
    const auto h = e::make_shared<T>(board);
    h->setCityId(cid);
    h->setOnCityId(ocid);
    const auto a = e::make_shared<eSoldierAction>(h.get());
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
        std::vector<eSoldierBanner*>& solds) {
    const auto ocid = tile->cityId();

    const int tx = tile->x();
    const int ty = tile->y();

    stdsptr<eSoldierBanner> b;
    const auto requestBanner = [&]() {
        if(!b || b->count() >= 8) {
            b = e::make_shared<eSoldierBanner>(
                    eBannerType::enemy, mBoard);
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
            requestBanner();
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
            requestBanner();
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
            requestBanner();
            b->incCount();
            s->setBanner(b.get());
        }
    }
}

void eInvasionHandler::initialize(eTile* const tile,
                                  const int infantry,
                                  const int cavalry,
                                  const int archers) {
    mTile = tile;

    const auto cid = mCity->cityId();
    const auto nat = mCity->nationality();
    std::vector<eSoldierBanner*> solds;
    generateSoldiersForCity(tile,
                            infantry, cavalry, archers,
                            cid, nat, solds);

    const int tx = tile->x();
    const int ty = tile->y();
    eSoldierBanner::sPlace(solds, tx, ty, mBoard, 3, 3);
}

void eInvasionHandler::initialize(eTile* const tile,
                                  const eEnlistedForces& forces,
                                  ePlayerConquestEvent* const conquestEvent) {
    mConquestEvent = conquestEvent;
    mTile = tile;

    const auto cid = mCity->cityId();
    const auto nat = mCity->nationality();
    std::vector<eSoldierBanner*> solds;
    {
        int infantry = 0;
        int cavalry = 0;
        int archers = 0;
        int aresWarriors = 0;
        int amazons = 0;
        for(const auto& s : forces.fSoldiers) {
            const auto type = s->type();
            if(type == eBannerType::hoplite) {
                infantry += s->count();
            } else if(type == eBannerType::horseman) {
                cavalry += s->count();
            } else if(type == eBannerType::rockThrower) {
                archers += s->count();
            } else if(type == eBannerType::aresWarrior) {
                aresWarriors += s->count();
            } else if(type == eBannerType::amazon) {
                amazons += s->count();
            }
        }

        eSs ss;
        if(infantry > 0) {
            if(nat == eNationality::greek) {
                ss.push_back(std::pair<ePlayerSoldierType, int>(
                                    ePlayerSoldierType::greekHoplite, infantry));
            } else {
                ss.push_back(std::pair<ePlayerSoldierType, int>(
                                    ePlayerSoldierType::atlanteanHoplite, infantry));
            }
        }
        if(cavalry > 0) {
            if(nat == eNationality::greek) {
                ss.push_back(std::pair<ePlayerSoldierType, int>(
                                    ePlayerSoldierType::greekHorseman, infantry));
            } else {
                ss.push_back(std::pair<ePlayerSoldierType, int>(
                                    ePlayerSoldierType::atlanteanChariot, infantry));
            }
        }
        if(archers > 0) {
            if(nat == eNationality::greek) {
                ss.push_back(std::pair<ePlayerSoldierType, int>(
                                    ePlayerSoldierType::greekRockthrower, infantry));
            } else {
                ss.push_back(std::pair<ePlayerSoldierType, int>(
                                    ePlayerSoldierType::atlanteanArcher, infantry));
            }
        }
        if(amazons > 0) {
            ss.push_back(std::pair<ePlayerSoldierType, int>(
                                ePlayerSoldierType::amazon, amazons));
        }
        if(aresWarriors > 0) {
            ss.push_back(std::pair<ePlayerSoldierType, int>(
                                ePlayerSoldierType::aresWarrior, aresWarriors));
        }

        generateSoldiersForCity(tile, ss, cid, solds);
    }

    for(const auto& f : forces.fAllies) {
        const auto cid = f->cityId();
        const auto nat = f->nationality();
        std::vector<eSoldierBanner*> solds;
        int infantry;
        int cavalry;
        int archers;
        f->troopsByType(infantry, cavalry, archers);
        generateSoldiersForCity(tile,
                                infantry, cavalry, archers,
                                cid, nat, solds);
    }

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

    if(forces.fAres) {
        const auto god = eGod::sCreateGod(eGodType::ares, mBoard);
        god->setAttitude(eGodAttitude::hostile);
        prcsAttack(god);
    }

    for(const auto h : forces.fHeroes) {
        const auto hero = eHero::sCreateHero(h, mBoard);
        prcsAttack(hero);
    }

    eSoldierBanner::sPlace(solds, tx, ty, mBoard, 3, 3);
}

void
eInvasionHandler::generateSoldiersForCity(
        eTile* const tile,
        const eSs& soldTypes,
        const eCityId cid,
        std::vector<eSoldierBanner*>& solds) {
    const auto ocid = mTile->cityId();

    const int tx = tile->x();
    const int ty = tile->y();

    stdsptr<eSoldierBanner> b;
    const auto requestBanner = [&]() {
        if(!b || b->count() >= 8) {
            b = e::make_shared<eSoldierBanner>(
                    eBannerType::enemy, mBoard);
            b->setOnCityId(ocid);
            b->setCityId(cid);
            mBanners.push_back(b);
            solds.push_back(b.get());
            b->setCityId(cid);
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
                requestBanner();
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

void eInvasionHandler::incTime(const int by) {
    std::vector<eSoldierBanner*> solds;
    for(const auto& b : mBanners) {
        solds.push_back(b.get());
        const bool r = b->stationary();
        if(!r) return;
    }
    const int wait = 10000;
    mWait += by;
    if(mWait < wait) {
        if(mStage == eInvasionStage::spread) {
            mStage = eInvasionStage::wait;
        }
        return;
    }
    mWait -= wait;
    switch(mStage) {
    case eInvasionStage::spread:
    case eInvasionStage::wait: {
        mStage = eInvasionStage::invade;
        const auto p = mBoard.palace(mTargetCity);
        if(p) {
            const auto t = p->centerTile();
            for(const auto& b : mBanners) {
                b->moveTo(t->x(), t->y());
            }
        } else {
            const int dtx = mBoard.width()/2;
            const int dty = mBoard.height()/2;
            int tx;
            int ty;
            eTileHelper::dtileIdToTileId(dtx, dty, tx, ty);
            eSoldierBanner::sPlace(solds, tx, ty, mBoard, 3, 3);
        }
    } break;
    case eInvasionStage::invade: {
        mStage = eInvasionStage::comeback;
        const auto p = mBoard.palace(mTargetCity);
        int ss = 0;
        for(const auto& b : mBanners) {
            ss += b->count();
        }
        eEventData ed(mTargetCity);
        ed.fCity = mCity;

        const auto invadingCid = mCity->cityId();
        const auto ppid = mBoard.personPlayer();
        const auto invadingPid = mBoard.cityIdToPlayerId(invadingCid);
        const auto invadingC = mBoard.boardCityWithId(invadingCid);
        const auto wboard = mBoard.getWorldBoard();
        eEventData ied(invadingPid);
        const auto targetWCity = wboard->cityWithId(mTargetCity);
        ied.fCity = targetWCity;
        if(ss == 0) {
            tellHeroesAndGodsToGoBack();
            const bool monn = eRand::rand() % 2;
            if(monn) {
                mBoard.allow(mTargetCity, eBuildingType::commemorative, 1);
                mBoard.event(eEvent::invasionVictoryMonn, ed);
            } else {
                mBoard.event(eEvent::invasionVictory, ed);
            }
            if(invadingC) {
                mBoard.event(eEvent::cityConquerFailed, ied);
                if(mConquestEvent) {
                    const auto& forces = mConquestEvent->forces();
                    forces.kill(1.);
//                    mConquestEvent->planArmyReturn();
                }
            }
        } else if(p) {
            mStage = eInvasionStage::spread;
        } else {
            mBoard.defeatedBy(mTargetCity, mCity);
            mBoard.event(eEvent::invasionDefeat, ed);
            const int tx = mTile->x();
            const int ty = mTile->y();
            eSoldierBanner::sPlace(solds, tx, ty, mBoard, 3, 0);
            tellHeroesAndGodsToGoBack();
            if(invadingC) {
                mBoard.event(eEvent::cityConquered, ied);
                mBoard.allow(invadingCid, eBuildingType::commemorative, 4);
                if(invadingPid == ppid) {
                    targetWCity->setRelationship(eForeignCityRelationship::vassal);
                }
                const auto ppc = mBoard.personPlayerCapital();
                if(mTargetCity != ppc) {
                    mBoard.moveCityToPlayer(mTargetCity, invadingPid);
                }
                if(mConquestEvent) {
                    const auto& forces = mConquestEvent->forces();
                    const int iniCount = forces.count();
                    int count = 0;
                    for(const auto& b : mBanners) {
                        count += b->count();
                    }
                    forces.kill(1 - double(count)/iniCount);
                    mConquestEvent->planArmyReturn();
                }
            }
        }
    } break;
    case eInvasionStage::comeback: {
        for(const auto& b : mBanners) {
            b->killAll();
        }
        delete this;
    } break;
    }
}

void eInvasionHandler::read(eReadStream& src) {
    src.readCity(&mBoard, [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });
    mTile = src.readTile(mBoard);
    src >> mStage;

    {
        int nb;
        src >> nb;

        for(int i = 0; i < nb; i++) {
            eBannerType type;
            src >> type;
            const auto b = e::make_shared<eSoldierBanner>(type, mBoard);
            b->read(src);
            mBanners.push_back(b);
        }
    }

    src >> mWait;
    src.readGameEvent(&mBoard, [this](eGameEvent* const e) {
        mEvent = static_cast<eInvasionEvent*>(e);
        if(mEvent) mEvent->addInvasionHandler(this);
    });
    src.readGameEvent(&mBoard, [this](eGameEvent* const e) {
        mConquestEvent = static_cast<ePlayerConquestEvent*>(e);
    });

    int nhg;
    src >> nhg;
    for(int i = 0; i < nhg; i++) {
        src.readCharacter(&mBoard, [this](eCharacter* const c) {
            mHeroesAndGods.push_back(c);
        });
    }
}

void eInvasionHandler::write(eWriteStream& dst) const {
    dst.writeCity(mCity.get());
    dst.writeTile(mTile);
    dst << mStage;

    {
        const int nb = mBanners.size();
        dst << nb;
        for(const auto& b : mBanners) {
            dst << b->type();
            b->write(dst);
        }
    }

    dst << mWait;
    dst.writeGameEvent(mEvent);
    dst.writeGameEvent(mConquestEvent);

    dst << mHeroesAndGods.size();
    for(const auto& c : mHeroesAndGods) {
        dst.writeCharacter(c.get());
    }
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
        eGod::sCharacterToGodType(cType, &isGod);
        if(isGod) {
            const auto a = c->action();
            if(const auto aa = dynamic_cast<eGodMonsterAction*>(a)) {
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
