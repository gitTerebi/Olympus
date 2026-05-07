#include "eepisode.h"

#include "elanguage.h"
#include "fileIO/esavearchive.h"

#include <iterator>

void eEpisode::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);

    if(fIntroId != 0 && fCompleteId != 0) {
        const auto intro = eLanguage::zeusMM(fIntroId);
        fTitle = intro.fTitle;
        fIntroduction = intro.fContent;
        if(const auto cep = dynamic_cast<eColonyEpisode*>(this)) {
            cep->fSelection = intro.fSubtitle;
        }
        const auto complete = eLanguage::zeusMM(fCompleteId);
        fComplete = complete.fContent;
    }
}

void eEpisode::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eEpisode*>(this)->serialize(ar);
}

void eEpisode::serialize(eSaveArchive& ar) {
    {
        int nc;
        if(ar.writing()) nc = fDrachmas.size();
        ar.value(nc);
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
            ar.value(pid);
            ar.value(d);
            if(ar.reading()) fDrachmas[pid] = d;
        }
    }
    if(ar.reading()) fStartDate.read(ar.readStream());
    else fStartDate.write(ar.writeStream());
    {
        int nc;
        if(ar.writing()) nc = fFriendlyGods.size();
        ar.value(nc);
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
            ar.value(cid);
            int nfg;
            if(ar.writing()) nfg = fgs.size();
            ar.value(nfg);
            for(int i = 0; i < nfg; i++) {
                eGodType g;
                if(ar.writing()) g = fgs[i];
                ar.value(g);
                if(ar.reading()) fgs.push_back(g);
            }
            if(ar.reading()) fFriendlyGods[cid] = fgs;
        }
    }
    {
        int ncs;
        if(ar.writing()) ncs = fEvents.size();
        ar.value(ncs);
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
            ar.value(cid);
            int ne;
            if(ar.writing()) ne = events.size();
            ar.value(ne);
            for(int j = 0; j < ne; j++) {
                eGameEventType type;
                if(ar.writing()) type = events[j]->type();
                ar.value(type);
                if(ar.reading()) {
                    const auto branch = eGameEventBranch::root;
                    const auto e = eGameEvent::sCreate(cid, type, branch, *fBoard);
                    e->read(ar.readStream());
                    fEvents[cid].push_back(e);
                } else {
                    events[j]->write(ar.writeStream());
                }
            }
        }
    }
    {
        int ng;
        if(ar.writing()) ng = fGoals.size();
        ar.value(ng);
        if(ar.reading()) fGoals.clear();
        for(int i = 0; i < ng; i++) {
            if(ar.reading()) {
                const auto g = std::make_shared<eEpisodeGoal>();
                g->read(ar.readStream());
                fGoals.push_back(g);
            } else {
                fGoals[i]->write(ar.writeStream());
            }
        }
    }

    {
        int nc;
        if(ar.writing()) nc = fAvailableBuildings.size();
        ar.value(nc);
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
            ar.value(cid);
            if(ar.reading()) ab.read(ar.readStream());
            else ab.write(ar.writeStream());
            if(ar.reading()) fAvailableBuildings[cid] = ab;
        }
    }

    {
        int nc;
        if(ar.writing()) nc = fMaxSanctuaries.size();
        ar.value(nc);
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
            ar.value(cid);
            ar.value(m);
            if(ar.reading()) fMaxSanctuaries[cid] = m;
        }
    }

    ar.value(fIntroId);
    ar.value(fCompleteId);
}

void eEpisode::clear() {
    fFriendlyGods.clear();
    fEvents.clear();
    fGoals.clear();
}
