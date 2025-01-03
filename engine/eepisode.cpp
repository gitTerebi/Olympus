#include "eepisode.h"

void eEpisode::read(eReadStream& src) {
    src >> fAtlantean;
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
        int nevs;
        src >> nevs;
        for(int i = 0; i < nevs; i++) {
            eGameEventType type;
            src >> type;
            const auto branch = eGameEventBranch::root;
            const auto e = eGameEvent::sCreate(type, branch, fBoard);
            e->setGameBoard(fBoard);
            e->setWorldBoard(fWorldBoard);
            e->read(src);
            fEvents.push_back(e);
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
}

void eEpisode::write(eWriteStream& dst) const {
    dst << fAtlantean;
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
    for(const auto& e : fEvents) {
        dst << e->type();
        e->write(dst);
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
}

bool eEpisode::availableBuilding(const eBuildingType type, const int id) const {
    switch(type) {
    case eBuildingType::chariotVendor:
    case eBuildingType::chariotFactory: {
        if(!fAtlantean) return false;
    } break;
    case eBuildingType::horseTrainer: {
        if(fAtlantean) return false;
    } break;
    default:
        break;
    }

    return fAvailableBuildingsDelete.available(type, id);
}

void eEpisode::clear() {
    fFriendlyGodsDelete.clear();
    fEvents.clear();
    fGoals.clear();
    fAvailableBuildingsDelete = eAvailableBuildings();
}
