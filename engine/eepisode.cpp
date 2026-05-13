#include "eepisode.h"

#include "elanguage.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"
#include "fileIO/eblob.h"

#include <cstdio>
#include <iterator>

void eEpisode::read(eReadStream& src) {
    (void)src;
    printf("Deprecated binary eEpisode::read called; JSON serializeJson should be used\n");
}

void eEpisode::write(eWriteStream& dst) const {
    (void)dst;
    printf("Deprecated binary eEpisode::write called; JSON serializeJson should be used\n");
}

void eEpisode::serialize(eSaveArchive& ar) {
    {
        int nc;
        if(ar.writing()) nc = fDrachmas.size();
        ar.field("nc", nc);
        if(ar.reading()) fDrachmas.clear();
        for(int i = 0; i < nc; i++) {
            ePlayerId pid;
            int d;
            if(ar.writing()) {
                auto it = fDrachmas.begin();
                std::advance(it, i);
                pid = it->first;
                d = it->second;
            }
            ar.field("pid", pid);
            ar.field("d", d);
            if(ar.reading()) fDrachmas[pid] = d;
        }
    }
    if(ar.reading()) fStartDate.read(ar.readStream());
    else fStartDate.write(ar.writeStream());
    {
        int nc;
        if(ar.writing()) nc = fFriendlyGods.size();
        ar.field("nc", nc);
        if(ar.reading()) fFriendlyGods.clear();
        for(int j = 0; j < nc; j++) {
            eCityId cid;
            std::vector<eGodType> fgs;
            if(ar.writing()) {
                auto it = fFriendlyGods.begin();
                std::advance(it, j);
                cid = it->first;
                fgs = it->second;
            }
            ar.field("cid", cid);
            int nfg;
            if(ar.writing()) nfg = fgs.size();
            ar.field("nfg", nfg);
            for(int i = 0; i < nfg; i++) {
                eGodType g;
                if(ar.writing()) g = fgs[i];
                ar.field("g", g);
                if(ar.reading()) fgs.push_back(g);
            }
            if(ar.reading()) fFriendlyGods[cid] = fgs;
        }
    }
    {
        int ncs;
        if(ar.writing()) ncs = fEvents.size();
        ar.field("ncs", ncs);
        if(ar.reading()) fEvents.clear();
        for(int i = 0; i < ncs; i++) {
            eCityId cid;
            std::vector<stdsptr<eGameEvent>> events;
            if(ar.writing()) {
                auto it = fEvents.begin();
                std::advance(it, i);
                cid = it->first;
                events = it->second;
            }
            ar.field("cid", cid);
            int ne;
            if(ar.writing()) ne = events.size();
            ar.field("ne", ne);
            for(int j = 0; j < ne; j++) {
                eGameEventType type;
                if(ar.writing()) type = events[j]->type();
                ar.field("type", type);
                if(ar.reading()) {
                    const auto branch = eGameEventBranch::root;
                    const auto e = eGameEvent::sCreate(cid, type, branch, *fBoard);
                    printf("Deprecated binary episode event read skipped; JSON serializeJson should be used\n");
                    fEvents[cid].push_back(e);
                } else {
                    printf("Deprecated binary episode event write skipped; JSON serializeJson should be used\n");
                }
            }
        }
    }
    {
        int ng;
        if(ar.writing()) ng = fGoals.size();
        ar.field("ng", ng);
        if(ar.reading()) fGoals.clear();
        for(int i = 0; i < ng; i++) {
            if(ar.reading()) {
                const auto g = std::make_shared<eEpisodeGoal>();
                printf("Deprecated binary episode goal read skipped; JSON serializeJson should be used\n");
                fGoals.push_back(g);
            } else {
                printf("Deprecated binary episode goal write skipped; JSON serializeJson should be used\n");
            }
        }
    }

    {
        int nc;
        if(ar.writing()) nc = fAvailableBuildings.size();
        ar.field("nc", nc);
        if(ar.reading()) fAvailableBuildings.clear();
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            eAvailableBuildings ab;
            if(ar.writing()) {
                auto it = fAvailableBuildings.begin();
                std::advance(it, i);
                cid = it->first;
                ab = it->second;
            }
            ar.field("cid", cid);
            if(ar.reading()) ab.read(ar.readStream());
            else ab.write(ar.writeStream());
            if(ar.reading()) fAvailableBuildings[cid] = ab;
        }
    }

    {
        int nc;
        if(ar.writing()) nc = fMaxSanctuaries.size();
        ar.field("nc", nc);
        if(ar.reading()) fMaxSanctuaries.clear();
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            int m;
            if(ar.writing()) {
                auto it = fMaxSanctuaries.begin();
                std::advance(it, i);
                cid = it->first;
                m = it->second;
            }
            ar.field("cid", cid);
            ar.field("m", m);
            if(ar.reading()) fMaxSanctuaries[cid] = m;
        }
    }

    ar.field("fIntroId", fIntroId);
    ar.field("fCompleteId", fCompleteId);
}

void eEpisode::serializeJson(eJsonArchive& ar) {
    // drachmas
    {
        int nc = 0;
        if(ar.writing()) nc = static_cast<int>(fDrachmas.size());
        ar.field("drachmasCount", nc);
        if(ar.reading()) fDrachmas.clear();
        for(int i = 0; i < nc; i++) {
            ePlayerId pid{};
            int d = 0;
            if(ar.writing()) {
                auto it = fDrachmas.begin();
                std::advance(it, i);
                pid = it->first;
                d   = it->second;
            }
            const auto k = std::to_string(i);
            ar.field(("drachmas." + k + ".pid").c_str(), pid);
            ar.field(("drachmas." + k + ".d").c_str(),   d);
            if(ar.reading()) fDrachmas[pid] = d;
        }
    }

    // start date
    { auto a = ar.child("fStartDate"); fStartDate.serializeJson(a); }

    // friendly gods
    {
        int nc = 0;
        if(ar.writing()) nc = static_cast<int>(fFriendlyGods.size());
        ar.field("friendlyGodsCount", nc);
        if(ar.reading()) fFriendlyGods.clear();
        for(int j = 0; j < nc; j++) {
            eCityId cid{};
            std::vector<eGodType> fgs;
            if(ar.writing()) {
                auto it = fFriendlyGods.begin();
                std::advance(it, j);
                cid = it->first;
                fgs = it->second;
            }
            const auto jk = std::to_string(j);
            ar.field(("fg." + jk + ".cid").c_str(), cid);
            int nfg = 0;
            if(ar.writing()) nfg = static_cast<int>(fgs.size());
            ar.field(("fg." + jk + ".n").c_str(), nfg);
            for(int i = 0; i < nfg; i++) {
                eGodType g{};
                if(ar.writing()) g = fgs[i];
                ar.field(("fg." + jk + ".g." + std::to_string(i)).c_str(), g);
                if(ar.reading()) fgs.push_back(g);
            }
            if(ar.reading()) fFriendlyGods[cid] = fgs;
        }
    }

    // events — each event is a JSON sub-object via serializeJson
    {
        int ncs = 0;
        if(ar.writing()) ncs = static_cast<int>(fEvents.size());
        ar.field("eventsCount", ncs);
        if(ar.reading()) fEvents.clear();
        for(int i = 0; i < ncs; i++) {
            eCityId cid{};
            std::vector<stdsptr<eGameEvent>> events;
            if(ar.writing()) {
                auto it = fEvents.begin();
                std::advance(it, i);
                cid    = it->first;
                events = it->second;
            }
            const auto ik = std::to_string(i);
            ar.field(("ev." + ik + ".cid").c_str(), cid);
            int ne = 0;
            if(ar.writing()) ne = static_cast<int>(events.size());
            ar.field(("ev." + ik + ".n").c_str(), ne);
            for(int j = 0; j < ne; j++) {
                eGameEventType type{};
                if(ar.writing()) type = events[j]->type();
                const auto jk = ik + "." + std::to_string(j);
                ar.field(("ev." + jk + ".type").c_str(), type);
                if(ar.writing()) {
                    auto eAr = ar.childAt(("ev." + ik + ".events").c_str(), j);
                    events[j]->serializeJson(eAr);
                } else {
                    const auto branch = eGameEventBranch::root;
                    const auto e = eGameEvent::sCreate(cid, type, branch, *fBoard);
                    auto eAr = ar.childAt(("ev." + ik + ".events").c_str(), j);
                    e->serializeJson(eAr);
                    fEvents[cid].push_back(e);
                }
            }
        }
    }

    // goals — JSON sub-objects
    {
        int ng = 0;
        if(ar.writing()) ng = static_cast<int>(fGoals.size());
        ar.field("goalsCount", ng);
        if(ar.reading()) fGoals.clear();
        for(int i = 0; i < ng; i++) {
            if(ar.reading()) {
                const auto g = std::make_shared<eEpisodeGoal>();
                auto gAr = ar.childAt("goals", i);
                g->serializeJson(gAr);
                fGoals.push_back(g);
            } else {
                auto gAr = ar.childAt("goals", i);
                fGoals[i]->serializeJson(gAr);
            }
        }
    }

    // available buildings — JSON sub-objects
    {
        int nc = 0;
        if(ar.writing()) nc = static_cast<int>(fAvailableBuildings.size());
        ar.field("availBuildingsCount", nc);
        if(ar.reading()) fAvailableBuildings.clear();
        for(int i = 0; i < nc; i++) {
            eCityId cid{};
            eAvailableBuildings ab;
            if(ar.writing()) {
                auto it = fAvailableBuildings.begin();
                std::advance(it, i);
                cid = it->first;
                ab  = it->second;
            }
            const auto k = std::to_string(i);
            ar.field(("ab." + k + ".cid").c_str(), cid);
            auto abAr = ar.childAt("ab", i);
            ab.serializeJson(abAr);
            if(ar.reading()) fAvailableBuildings[cid] = ab;
        }
    }

    // max sanctuaries
    {
        int nc = 0;
        if(ar.writing()) nc = static_cast<int>(fMaxSanctuaries.size());
        ar.field("maxSanctuariesCount", nc);
        if(ar.reading()) fMaxSanctuaries.clear();
        for(int i = 0; i < nc; i++) {
            eCityId cid{};
            int m = 0;
            if(ar.writing()) {
                auto it = fMaxSanctuaries.begin();
                std::advance(it, i);
                cid = it->first;
                m   = it->second;
            }
            const auto k = std::to_string(i);
            ar.field(("ms." + k + ".cid").c_str(), cid);
            ar.field(("ms." + k + ".m").c_str(),   m);
            if(ar.reading()) fMaxSanctuaries[cid] = m;
        }
    }

    ar.field("fIntroId",   fIntroId);
    ar.field("fCompleteId", fCompleteId);
}

void eParentCityEpisode::serializeJson(eJsonArchive& ar) {
    eEpisode::serializeJson(ar);
    ar.field("fNextEpisode", fNextEpisode);
}

void eColonyEpisode::serializeJson(eJsonArchive& ar) {
    eEpisode::serializeJson(ar);
    int cityIOID = -1;
    if(ar.writing() && fCity) cityIOID = fCity->ioID();
    ar.field("fCityIOID", cityIOID);
    if(ar.reading() && cityIOID >= 0 && fWorldBoard) {
        fCity = fWorldBoard->cityWithIOID(cityIOID);
    }
}

void eEpisode::clear() {
    fFriendlyGods.clear();
    fEvents.clear();
    fGoals.clear();
}
