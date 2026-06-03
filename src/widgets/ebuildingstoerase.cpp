#include "ebuildingstoerase.h"

#include "buildings/ebuilding.h"
#include "buildings/eagoraspace.h"
#include "buildings/evendor.h"
#include "buildings/eroad.h"
#include "buildings/eagorabase.h"
#include "buildings/egatehouse.h"
#include "buildings/epalacetile.h"
#include "buildings/epalace.h"
#include "buildings/epier.h"
#include "buildings/sanctuaries/esanctbuilding.h"
#include "buildings/sanctuaries/sanctuary.h"
#include "buildings/eaestheticsbuilding.h"
#include "buildings/ehippodromepiece.h"
#include "engine/etile.h"

#include "characters/ehomeless.h"
#include "characters/echaracter.h"
#include "characters/actions/animal-action.h"
#include "characters/actions/esettleraction.h"
#include "characters/actions/ekillcharacterfinishfail.h"
#include "buildings/ehousebase.h"

#include <algorithm>

#include "erand.h"
#include "evectorhelpers.h"

bool isImportant(eBuilding* const b) {
    const auto t = b->type();
    if(t == eBuildingType::palace) return true;
    const int min = static_cast<int>(eBuildingType::templeAphrodite);
    const int max = static_cast<int>(eBuildingType::templeZeus);
    const int bi = static_cast<int>(t);
    if(bi >= min && bi <= max) return true;
    return false;
}

bool isNonEmptyAgora(eBuilding* const b) {
    const auto t = b->type();
    const auto isa = t == eBuildingType::commonAgora ||
                     t == eBuildingType::grandAgora;
    if(!isa) return false;
    const auto a = static_cast<eAgoraBase*>(b);
    return a->hasVendors();
}

bool isAnimalPen(const eBuildingType t) {
    return t == eBuildingType::sheep ||
           t == eBuildingType::goat ||
           t == eBuildingType::cattle;
}

bool isAnimalForPen(const eCharacterType ct, const eBuildingType bt) {
    return (bt == eBuildingType::sheep && ct == eCharacterType::sheep) ||
           (bt == eBuildingType::goat && ct == eCharacterType::goat) ||
           (bt == eBuildingType::cattle &&
            (ct == eCharacterType::cattle1 ||
             ct == eCharacterType::cattle2 ||
             ct == eCharacterType::cattle3 ||
             ct == eCharacterType::bull));
}

void killAnimalPenAnimals(eBuilding* const b) {
    const auto bt = b->type();
    if(!isAnimalPen(bt)) return;
    const auto rect = b->tileRect();
    const auto chars = b->getBoard().characters();
    for(const auto c : chars) {
        if(!c) continue;
        if(!isAnimalForPen(c->type(), bt)) continue;
        const auto aa = dynamic_cast<AnimalAction*>(c->action());
        if(aa) {
            if(aa->spawnerX() != rect.x || aa->spawnerY() != rect.y) continue;
        } else {
            const auto tile = c->tile();
            if(!tile || tile->x() != rect.x || tile->y() != rect.y) continue;
        }
        c->kill();
    }
}

void eBuildingsToErase::addBuilding(eBuilding* const b) {
    eBuilding* bb = b;
    const auto t = b->type();
    if(t == eBuildingType::agoraSpace) {
        const auto as = static_cast<eAgoraSpace*>(b);
        bb = as->agora();
    } else if(t == eBuildingType::road) {
        const auto r = static_cast<eRoad*>(b);
        const auto a = r->underAgora();
        if(a) bb = a;
        const auto g = r->underGatehouse();
        if(g) bb = g;
        const auto h = r->aboveHippodrome();
        if(h) bb = h;
    } else if(t == eBuildingType::palaceTile) {
        const auto pt = static_cast<ePalaceTile*>(b);
        const auto p = pt->palace();
        if(p) bb = p;
    } else if(t == eBuildingType::godMonumentTile) {
        const auto pt = static_cast<eGodMonumentTile*>(b);
        const auto p = pt->monument();
        if(p) bb = p;
    } else if(t == eBuildingType::pier) {
        const auto pt = static_cast<ePier*>(b);
        const auto p = pt->tradePost();
        if(p) bb = p;
    } else if(const auto sb = dynamic_cast<eSanctBuilding*>(b)) {
        const auto s = sb->monument();
        if(s) bb = s;
    }
    if(isNonEmptyAgora(bb)) {
        mAgoBs.insert(bb);
    } else if(isImportant(bb)) {
        mImpBs.insert(bb);
    } else {
        mBs.insert(bb);
    }
}

void eBuildingsToErase::addCharacter(eCharacter* const c) {
    if(!c) return;
    mCs.insert(c);
}

void eBuildingsToErase::erase(eBuilding* const b) {
    killAnimalPenAnimals(b);
    switch(b->type()) {
    case eBuildingType::eliteHousing:
    case eBuildingType::commonHouse: {
        const auto hb = static_cast<eHouseBase*>(b);
        const int totalPeople = hb->people();
        if(totalPeople > 0) {
            auto& board = b->getBoard();
            const auto tile = b->centerTile();
            int remainingPeople = totalPeople;
            int waitTime = 0;
            while(remainingPeople > 0) {
                const int spawnCount = std::min(30, remainingPeople);
                eHomeless::spawn(board, tile, tile->cityId(), spawnCount, waitTime);
                remainingPeople -= spawnCount;
                waitTime += 10 + eRand::rand() % 25;
            }
        }
        b->erase();
    } break;
    default:
        b->erase();
    }
}

int eBuildingsToErase::erase(const bool important) {
    int total = 0;
    for(const auto c : mCs) {
        c->kill();
    }
    total += mCs.size();
    mCs.clear();
    for(const auto b : mBs) {
        erase(b);
    }
    total += mBs.size();
    mBs.clear();
    if(important) {
        for(const auto b : mImpBs) {
            erase(b);
        }
        total += mImpBs.size();
        mImpBs.clear();
        for(const auto b : mAgoBs) {
            erase(b);
        }
        total += mAgoBs.size();
        mAgoBs.clear();
    }
    return total;
}

bool eBuildingsToErase::hasImportantBuildings() const {
    return !mImpBs.empty();
}

bool eBuildingsToErase::hasNonEmptyAgoras() const {
    return !mAgoBs.empty();
}
