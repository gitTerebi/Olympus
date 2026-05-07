#include "eenlistedforces.h"

#include <algorithm>

#include "engine/egameboard.h"
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

void eEnlistedForces::read(eGameBoard& board,
                           eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar, &board);
}

void eEnlistedForces::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eEnlistedForces*>(this)->serialize(ar, nullptr);
}

void eEnlistedForces::serialize(eSaveArchive& ar, eGameBoard* board) {
    eWorldBoard* wboard = board ? &board->world() : nullptr;
    {
        int ns = fSoldiers.size();
        ar.value(ns);
        if(ar.reading()) fSoldiers.clear();
        for(int i = 0; i < ns; i++) {
            if(ar.reading()) {
                const auto func = [this](const stdsptr<eSoldierBanner>& b) {
                    if(b) fSoldiers.push_back(b);
                };
                ar.readStream().readSoldierBanner(board, func);
            } else {
                ar.writeStream().writeSoldierBanner(fSoldiers[i].get());
            }
        }
    }
    {
        int nh = fHeroes.size();
        ar.value(nh);
        if(ar.reading()) fHeroes.clear();
        for(int i = 0; i < nh; i++) {
            std::pair<eCityId, eHeroType> h;
            if(ar.writing()) h = fHeroes[i];
            ar.value(h.first);
            ar.value(h.second);
            if(ar.reading()) fHeroes.push_back(h);
        }
    }
    {
        int nc = fAllies.size();
        ar.value(nc);
        if(ar.reading()) fAllies.clear();
        for(int i = 0; i < nc; i++) {
            if(ar.reading()) {
                const auto func = [this](const stdsptr<eWorldCity>& c) {
                    fAllies.push_back(c);
                };
                ar.readStream().readCity(wboard, func);
            } else {
                ar.writeStream().writeCity(fAllies[i].get());
            }
        }
    }

    ar.value(fAres);
    ar.value(fAresCity);
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
        std::vector<eSoldierBanner*> solds;
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
