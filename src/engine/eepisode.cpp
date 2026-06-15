#include "eepisode.h"

#include "language.h"
#include "vector-helpers.h"
#include "fileIO/save-archive.h"

#include <iterator>

void eEpisode::serialize(SaveArchive& ar) {
    // drachmas map<ePlayerId, int>
    {
        int drachmasCount = ar.writing() ? static_cast<int>(fDrachmas.size()) : 0;
        ar.field("drachmas.count", drachmasCount);
        if(ar.reading()) {
            fDrachmas.clear();
            for(int i = 0; i < drachmasCount; i++) {
                ePlayerId pid; int amount;
                ar.archiveField(("drachmas." + std::to_string(i)).c_str(),
                    [&](SaveArchive& it) {
                        it.field("playerId", pid);
                        it.field("amount", amount);
                    });
                fDrachmas[pid] = amount;
            }
        } else {
            int i = 0;
            for(auto& kv : fDrachmas) {
                ePlayerId pid = kv.first; int amount = kv.second;
                ar.archiveField(("drachmas." + std::to_string(i++)).c_str(),
                    [&](SaveArchive& it) {
                        it.field("playerId", pid);
                        it.field("amount", amount);
                    });
            }
        }
    }

    ar.dateField("startDate", fStartDate);

    // friendlyGods map<eCityId, vector<GodType>>
    {
        int cityCount = ar.writing() ? static_cast<int>(fFriendlyGods.size()) : 0;
        ar.field("friendlyGods.count", cityCount);
        if(ar.reading()) {
            fFriendlyGods.clear();
            for(int i = 0; i < cityCount; i++) {
                eCityId cid;
                std::vector<GodType> gods;
                ar.archiveField(("friendlyGods." + std::to_string(i)).c_str(),
                    [&](SaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int godCount = 0;
                        cityAr.field("gods.count", godCount);
                        for(int j = 0; j < godCount; j++) {
                            GodType g;
                            cityAr.field(("god." + std::to_string(j)).c_str(), g);
                            gods.push_back(g);
                        }
                    });
                fFriendlyGods[cid] = gods;
            }
        } else {
            int i = 0;
            for(auto& kv : fFriendlyGods) {
                eCityId cid = kv.first;
                auto& gods = kv.second;
                ar.archiveField(("friendlyGods." + std::to_string(i++)).c_str(),
                    [&](SaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int godCount = static_cast<int>(gods.size());
                        cityAr.field("gods.count", godCount);
                        for(int j = 0; j < godCount; j++) {
                            GodType g = gods[j];
                            cityAr.field(("god." + std::to_string(j)).c_str(), g);
                        }
                    });
            }
        }
    }

    // events map<eCityId, vector<eGameEvent>>
    {
        int cityCount = ar.writing() ? static_cast<int>(fEvents.size()) : 0;
        ar.field("events.count", cityCount);
        if(ar.reading()) {
            fEvents.clear();
            for(int i = 0; i < cityCount; i++) {
                eCityId cid;
                ar.archiveField(("events." + std::to_string(i)).c_str(),
                    [&](SaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int eventCount = 0;
                        cityAr.field("events.count", eventCount);
                        for(int j = 0; j < eventCount; j++) {
                            eGameEventType type;
                            cityAr.archiveField(("event." + std::to_string(j)).c_str(),
                                [&](SaveArchive& evtAr) {
                                    evtAr.field("type", type);
                                    evtAr.archiveField("eventData",
                                        [&](SaveArchive& childAr) {
                                            const auto branch = eGameEventBranch::root;
                                            const auto e = eGameEvent::sCreate(cid, type, branch, *fBoard);
                                            e->serialize(childAr);
                                            fEvents[cid].push_back(e);
                                        });
                                });
                        }
                    });
            }
        } else {
            int i = 0;
            for(auto& kv : fEvents) {
                eCityId cid = kv.first;
                auto events = kv.second;
                VectorHelpers::removeAll(events, stdsptr<eGameEvent>(nullptr));
                ar.archiveField(("events." + std::to_string(i++)).c_str(),
                    [&](SaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int eventCount = static_cast<int>(events.size());
                        cityAr.field("events.count", eventCount);
                        for(int j = 0; j < eventCount; j++) {
                            eGameEventType type = events[j]->type();
                            auto& evt = events[j];
                            cityAr.archiveField(("event." + std::to_string(j)).c_str(),
                                [&](SaveArchive& evtAr) {
                                    evtAr.field("type", type);
                                    evtAr.archiveField("eventData",
                                        [&evt](SaveArchive& childAr) { evt->serialize(childAr); });
                                });
                        }
                    });
            }
        }
    }

    ar.arrayField("goals", fGoals, [](SaveArchive& ar, auto& g) {
        if(ar.reading()) g = std::make_shared<eEpisodeGoal>();
        ar.archiveField("state", [&g](SaveArchive& childAr) {
            g->serialize(childAr);
        });
    });

    // availableBuildings map<eCityId, eAvailableBuildings>
    {
        int cityCount = ar.writing() ? static_cast<int>(fAvailableBuildings.size()) : 0;
        ar.field("availableBuildings.count", cityCount);
        if(ar.reading()) {
            fAvailableBuildings.clear();
            for(int i = 0; i < cityCount; i++) {
                eCityId cid;
                eAvailableBuildings ab;
                ar.archiveField(("availableBuildings." + std::to_string(i)).c_str(),
                    [&](SaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        cityAr.archiveField("buildings",
                            [&ab](SaveArchive& buildingsAr) {
                                ab.serialize(buildingsAr);
                            });
                    });
                fAvailableBuildings[cid] = ab;
            }
        } else {
            int i = 0;
            for(auto& kv : fAvailableBuildings) {
                eCityId cid = kv.first;
                eAvailableBuildings& ab = kv.second;
                ar.archiveField(("availableBuildings." + std::to_string(i++)).c_str(),
                    [&](SaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        cityAr.archiveField("buildings",
                            [&ab](SaveArchive& buildingsAr) {
                                ab.serialize(buildingsAr);
                            });
                    });
            }
        }
    }

    // maxSanctuaries map<eCityId, int>
    {
        int cityCount = ar.writing() ? static_cast<int>(fMaxSanctuaries.size()) : 0;
        ar.field("maxSanctuaries.count", cityCount);
        if(ar.reading()) {
            fMaxSanctuaries.clear();
            for(int i = 0; i < cityCount; i++) {
                eCityId cid; int maxValue;
                ar.archiveField(("maxSanctuaries." + std::to_string(i)).c_str(),
                    [&](SaveArchive& it) {
                        it.field("cityId", cid);
                        it.field("max", maxValue);
                    });
                fMaxSanctuaries[cid] = maxValue;
            }
        } else {
            int i = 0;
            for(auto& kv : fMaxSanctuaries) {
                eCityId cid = kv.first; int maxValue = kv.second;
                ar.archiveField(("maxSanctuaries." + std::to_string(i++)).c_str(),
                    [&](SaveArchive& it) {
                        it.field("cityId", cid);
                        it.field("max", maxValue);
                    });
            }
        }
    }

    ar.field("introId", fIntroId);
    ar.field("completeId", fCompleteId);

    if(ar.reading() && fIntroId != 0 && fCompleteId != 0) {
        const auto intro = Language::zeusMM(fIntroId);
        fTitle = intro.fTitle;
        fIntroduction = intro.fContent;
        if(const auto cep = dynamic_cast<eColonyEpisode*>(this)) {
            cep->fSelection = intro.fSubtitle;
        }
        const auto complete = Language::zeusMM(fCompleteId);
        fComplete = complete.fContent;
    }
}

void eEpisode::clear() {
    fFriendlyGods.clear();
    fEvents.clear();
    fGoals.clear();
}
