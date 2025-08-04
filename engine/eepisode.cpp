#include "eepisode.h"

#include "elanguage.h"
#include "ecampaign.h"

void eEpisode::read(eReadStream& src) {
    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            ePlayerId pid;
            src >> pid;
            src >> fDrachmas[pid];
        }
    }
    fStartDate.read(src);
    {
        int nc;
        src >> nc;
        for(int j = 0; j < nc; j++) {
            eCityId cid;
            src >> cid;
            auto& fgs = fFriendlyGods[cid];
            int nfg;
            src >> nfg;
            for(int i = 0; i < nfg; i++) {
                eGodType g;
                src >> g;
                fgs.push_back(g);
            }
        }
    }
    {
        int ncs;
        src >> ncs;
        for(int i = 0; i < ncs; i++) {
            eCityId cid;
            src >> cid;
            int ne;
            src >> ne;
            for(int j = 0; j < ne; j++) {
                eGameEventType type;
                src >> type;
                const auto branch = eGameEventBranch::root;
                const auto e = eGameEvent::sCreate(cid, type, branch, *fBoard);
                e->read(src);
                fEvents[cid].push_back(e);
            }
        }
    }
    {
        int ng;
        src >> ng;
        for(int i = 0; i < ng; i++) {
            const auto g = std::make_shared<eEpisodeGoal>();
            g->read(fWorldBoard, src);
            fGoals.push_back(g);
        }
    }

    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            src >> cid;
            eAvailableBuildings& ab = fAvailableBuildings[cid];
            ab.read(src);
        }
    }

    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            src >> cid;
            int& m = fMaxSanctuaries[cid];
            src >> m;
        }
    }

    src >> fIntroId;
    src >> fCompleteId;

    if(fIntroId != 0 && fCompleteId != 0) {
        const auto intro = eLanguage::zeusMM(fIntroId);
        fTitle = intro.first;
        fIntroduction = intro.second;
        eCampaign::sReplaceSpecial(fIntroduction);
        const auto complete = eLanguage::zeusMM(fCompleteId);
        fComplete = complete.second;
        eCampaign::sReplaceSpecial(fComplete);
    }
}

void eEpisode::write(eWriteStream& dst) const {
    {
        dst << fDrachmas.size();
        for(const auto& d : fDrachmas) {
            dst << d.first;
            dst << d.second;
        }
    }
    fStartDate.write(dst);
    dst << fFriendlyGods.size();
    for(const auto& fg : fFriendlyGods) {
        dst << fg.first;
        dst << fg.second.size();
        for(const auto g : fg.second) {
            dst << g;
        }
    }
    dst << fEvents.size();
    for(const auto& ce : fEvents) {
        dst << ce.first;
        dst << ce.second.size();
        for(const auto& e : ce.second) {
            dst << e->type();
            e->write(dst);
        }
    }
    dst << fGoals.size();
    for(const auto& g : fGoals) {
        g->write(dst);
    }

    dst << fAvailableBuildings.size();
    for(const auto& ab : fAvailableBuildings) {
        dst << ab.first;
        ab.second.write(dst);
    }

    dst << fMaxSanctuaries.size();
    for(const auto& m : fMaxSanctuaries) {
        dst << m.first;
        dst << m.second;
    }

    dst << fIntroId;
    dst << fCompleteId;
}

void eEpisode::clear() {
    fFriendlyGods.clear();
    fEvents.clear();
    fGoals.clear();
}
