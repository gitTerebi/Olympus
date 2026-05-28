#include "eenlistedforces.h"

#include <algorithm>
#include <memory>

#include "engine/e-game-board.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

std::map<eCityId, eEnlistedForces>
eEnlistedForces::splitIntoCities() const {
    std::map<eCityId, eEnlistedForces> forces;
    for(const auto& s : fSoldiers) {
        const auto cid = s->cityId();
        forces[cid].fSoldiers.push_back(s);
    }
    for(const auto& s : fHeroes) {
        forces[s.first].fHeroes.push_back(s);
    }
    if(fAres) {
        const auto cid = fAresCity;
        forces[cid].fAres = true;
        forces[cid].fAresCity = cid;
    }
    if(!forces.empty()) {
        const auto cid = forces.begin()->first;
        forces[cid].fAllies = fAllies;
    }

    return forces;
}

void eEnlistedForces::serialize(eSaveArchive& ar, eGameBoard* board) {
    eWorldBoard* wboard = board ? &board->world() : nullptr;
    {
        if(ar.reading()) {
            const auto soldiers = std::make_shared<std::vector<stdsptr<SoldierBanner>>>();
            ar.arrayField("soldiers", *soldiers, [board](eSaveArchive& ar, auto& soldier) {
                ar.soldierBanner(board, soldier);
            });
            ar.addPostFunc([this, soldiers]() {
                fSoldiers = *soldiers;
                fSoldiers.erase(std::remove_if(fSoldiers.begin(), fSoldiers.end(),
                                               [](const auto& s) { return !s; }),
                                fSoldiers.end());
            }, "eEnlistedForces::soldiers");
        } else {
            ar.arrayField("soldiers", fSoldiers, [board](eSaveArchive& ar, auto& soldier) {
                ar.soldierBanner(board, soldier);
            });
        }
    }
    {
        ar.arrayField("heroes", fHeroes, [](eSaveArchive& ar, auto& h) {
            ar.field("h.first", h.first);
            ar.field("h.second", h.second);
        });
    }
    {
        if(ar.reading()) {
            const auto allies = std::make_shared<std::vector<stdsptr<eWorldCity>>>();
            ar.arrayField("allies", *allies, [wboard](eSaveArchive& ar, auto& ally) {
                ar.city(wboard, ally);
            });
            ar.addPostFunc([this, allies]() {
                fAllies = *allies;
                fAllies.erase(std::remove_if(fAllies.begin(), fAllies.end(),
                                             [](const auto& a) { return !a; }),
                              fAllies.end());
            }, "eEnlistedForces::allies");
        } else {
            ar.arrayField("allies", fAllies, [wboard](eSaveArchive& ar, auto& ally) {
                ar.city(wboard, ally);
            });
        }
    }

    ar.field("fAres", fAres);
    ar.field("fAresCity", fAresCity);
}

void eEnlistedForces::clear() {
    fSoldiers.clear();
    fHeroes.clear();
    fAllies.clear();
    fAres = false;
}

void eEnlistedForces::add(const eEnlistedForces& o) {
    for(const auto& s : o.fSoldiers) {
        fSoldiers.push_back(s);
    }
    for(const auto& h : o.fHeroes) {
        fHeroes.push_back(h);
    }
    for(const auto& a : o.fAllies) {
        fAllies.push_back(a);
    }
    if(o.fAres && !fAres) fAresCity = o.fAresCity;
    fAres = fAres || o.fAres;
}

const double gHelpFrac = 0.33;

int eEnlistedForces::strength() const {
    double str = 0;
    for(const auto& s : fSoldiers) {
        double mult = 1.;
        switch(s->type()) {
        case eBannerType::horseman:
            mult = eNumbers::sArmyStrengthHorsemanMult;
            break;
        default:
            break;
        }

        str += mult*s->count();
    }
    for(const auto& c : fAllies) {
        str += gHelpFrac*c->troops();
    }
    str += 8*fHeroes.size();
    if(fAres) str += 8;
    return std::round(str);
}

void eEnlistedForces::kill(const double killFrac) const {
    for(const auto& s : fSoldiers) {
        const auto cid = s->cityId();
        const int oC = s->count();
        int nC = std::round((1 - killFrac)*oC);
        nC = std::clamp(nC, 0, eNumbers::sSoldiersPerBanner);
        auto& board = s->getBoard();
        const auto type = s->type();
        for(int i = nC; i < oC; i++) {
            s->decCount();
            switch(type) {
            case eBannerType::hoplite:
                board.hopliteKilled(cid);
                break;
            case eBannerType::horseman:
                board.horsemanKilled(cid);
                break;
            default:
                break;
            }
        }
    }

    for(const auto& c : fAllies) {
        const int t = c->troops();
        c->setTroops((1 - gHelpFrac*killFrac)*t);
    }
}

int eEnlistedForces::count() const {
    int result = 0;
    for(const auto& b : fSoldiers) {
        result += b->count();
    }
    for(const auto& f : fAllies) {
        std::vector<SoldierBanner*> solds;
        int infantry;
        int cavalry;
        int archers;
        f->troopsByType(infantry, cavalry, archers);
        result += infantry;
        result += cavalry;
        result += archers;
    }
    return result;
}
