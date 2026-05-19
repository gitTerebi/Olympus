#include "eepisode.h"

#include "elanguage.h"
#include "evectorhelpers.h"
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
    // drachmas map<ePlayerId, int>
    {
        int drachmasCount = ar.writing() ? static_cast<int>(fDrachmas.size()) : 0;
        ar.field("drachmas.count", drachmasCount);
        if(ar.reading()) {
            fDrachmas.clear();
            for(int i = 0; i < drachmasCount; i++) {
                ePlayerId pid; int amount;
                ar.archiveField(("drachmas." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
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
                    [&](eSaveArchive& it) {
                        it.field("playerId", pid);
                        it.field("amount", amount);
                    });
            }
        }
    }

    ar.dateField("startDate", fStartDate);

    // friendlyGods map<eCityId, vector<eGodType>>
    {
        int cityCount = ar.writing() ? static_cast<int>(fFriendlyGods.size()) : 0;
        ar.field("friendlyGods.count", cityCount);
        if(ar.reading()) {
            fFriendlyGods.clear();
            for(int i = 0; i < cityCount; i++) {
                eCityId cid;
                std::vector<eGodType> gods;
                ar.archiveField(("friendlyGods." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int godCount = 0;
                        cityAr.field("gods.count", godCount);
                        for(int j = 0; j < godCount; j++) {
                            eGodType g;
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
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int godCount = static_cast<int>(gods.size());
                        cityAr.field("gods.count", godCount);
                        for(int j = 0; j < godCount; j++) {
                            eGodType g = gods[j];
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
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int eventCount = 0;
                        cityAr.field("events.count", eventCount);
                        for(int j = 0; j < eventCount; j++) {
                            eGameEventType type;
                            cityAr.archiveField(("event." + std::to_string(j)).c_str(),
                                [&](eSaveArchive& evtAr) {
                                    evtAr.field("type", type);
                                    evtAr.archiveField("eventData",
                                        [&](eSaveArchive& childAr) {
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
                eVectorHelpers::removeAll(events, stdsptr<eGameEvent>(nullptr));
                ar.archiveField(("events." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int eventCount = static_cast<int>(events.size());
                        cityAr.field("events.count", eventCount);
                        for(int j = 0; j < eventCount; j++) {
                            eGameEventType type = events[j]->type();
                            auto& evt = events[j];
                            cityAr.archiveField(("event." + std::to_string(j)).c_str(),
                                [&](eSaveArchive& evtAr) {
                                    evtAr.field("type", type);
                                    evtAr.archiveField("eventData",
                                        [&evt](eSaveArchive& childAr) { evt->serialize(childAr); });
                                });
                        }
                    });
            }
        }
    }

    ar.arrayField("goals", fGoals, [](eSaveArchive& ar, auto& g) {
        if(ar.reading()) g = std::make_shared<eEpisodeGoal>();
        ar.archiveField("state", [&g](eSaveArchive& childAr) {
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
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        cityAr.archiveField("buildings",
                            [&ab](eSaveArchive& buildingsAr) {
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
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        cityAr.archiveField("buildings",
                            [&ab](eSaveArchive& buildingsAr) {
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
                    [&](eSaveArchive& it) {
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
                    [&](eSaveArchive& it) {
                        it.field("cityId", cid);
                        it.field("max", maxValue);
                    });
            }
        }
    }

    ar.field("introId", fIntroId);
    ar.field("completeId", fCompleteId);
}

void eEpisode::clear() {
    fFriendlyGods.clear();
    fEvents.clear();
    fGoals.clear();
}
