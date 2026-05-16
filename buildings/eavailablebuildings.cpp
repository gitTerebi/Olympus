#include "eavailablebuildings.h"

#include "fileIO/ereadstream.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ewritestream.h"

#include "buildings/ebuilding.h"

#include <algorithm>
#include <iterator>

void eAvailableBuildings::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eAvailableBuildings::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eAvailableBuildings*>(this)->serialize(ar);
}

void eAvailableBuildings::serialize(eSaveArchive& ar) {
    ar.field("eliteHousing", fEliteHousing);

    ar.field("wheatFarm", fWheatFarm);
    ar.field("carrotsFarm", fCarrotsFarm);
    ar.field("onionFarm", fOnionFarm);

    ar.field("vine", fVine);
    ar.field("oliveTree", fOliveTree);
    ar.field("orangeTree", fOrangeTree);

    ar.field("dairy", fDairy);
    ar.field("cardingShed", fCardingShed);

    ar.field("fishery", fFishery);
    ar.field("urchinQuay", fUrchinQuay);
    ar.field("huntingLodge", fHuntingLodge);

    ar.field("mint", fMint);
    ar.field("foundry", fFoundry);
    ar.field("timberMill", fTimberMill);
    ar.field("masonryShop", fMasonryShop);

    ar.field("refinery", fRefinery);
    ar.field("blackMarbleWorkshop", fBlackMarbleWorkshop);

    ar.field("winery", fWinery);
    ar.field("olivePress", fOlivePress);
    ar.field("sculptureStudio", fSculptureStudio);

    ar.field("armory", fArmory);

    ar.field("horseRanch", fHorseRanch);
    ar.field("chariotFactory", fChariotFactory);

    ar.field("triremeWharf", fTriremeWharf);
    ar.field("hippodrome", fHippodrome);

    ar.field("aphroditeSanctuary", fAphroditeSanctuary);
    ar.field("apolloSanctuary", fApolloSanctuary);
    ar.field("aresSanctuary", fAresSanctuary);
    ar.field("artemisSanctuary", fArtemisSanctuary);
    ar.field("athenaSanctuary", fAthenaSanctuary);
    ar.field("atlasSanctuary", fAtlasSanctuary);
    ar.field("demeterSanctuary", fDemeterSanctuary);
    ar.field("dionysusSanctuary", fDionysusSanctuary);
    ar.field("hadesSanctuary", fHadesSanctuary);
    ar.field("hephaestusSanctuary", fHephaestusSanctuary);
    ar.field("heraSanctuary", fHeraSanctuary);
    ar.field("hermesSanctuary", fHermesSanctuary);
    ar.field("poseidonSanctuary", fPoseidonSanctuary);
    ar.field("zeusSanctuary", fZeusSanctuary);

    ar.field("achillesHall", fAchillesHall);
    ar.field("atalantaHall", fAtalantaHall);
    ar.field("bellerophonHall", fBellerophonHall);
    ar.field("herculesHall", fHerculesHall);
    ar.field("jasonHall", fJasonHall);
    ar.field("odysseusHall", fOdysseusHall);
    ar.field("perseusHall", fPerseusHall);
    ar.field("theseusHall", fTheseusHall);

    ar.field("populationMonument", fPopulationMonument);
    ar.field("victoryMonument", fVictoryMonument);
    ar.field("colonyMonument", fColonyMonument);
    ar.field("athleteMonument", fAthleteMonument);
    ar.field("conquestMonument", fConquestMonument);
    ar.field("happinessMonument", fHappinessMonument);
    ar.field("heroicMonument", fHeroicMonument);
    ar.field("diplomacyMonument", fDiplomacyMonument);
    ar.field("scholarMonument", fScholarMonument);

    // godMonuments per-type
    const int godTypeMax = static_cast<int>(eGodType::zeus) + 1;
    for(int i = 0; i < godTypeMax; i++) {
        const auto godType = static_cast<eGodType>(i);
        ar.field(("godMonument." + std::to_string(i)).c_str(),
                 mGodMonuments[godType]);
    }

    // pyramids map<eBuildingType, ePyramidAvailable>
    {
        int pyramidCount = static_cast<int>(fPyramids.size());
        ar.field("pyramids.count", pyramidCount);
        if(ar.reading()) {
            fPyramids.clear();
            for(int i = 0; i < pyramidCount; i++) {
                eBuildingType buildingType;
                ePyramidAvailable pyramidData;
                ar.archiveField(("pyramids." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("buildingType", buildingType);
                        itemAr.field("availability", pyramidData.fA);
                        int levelCount = 0;
                        itemAr.field("levels.count", levelCount);
                        pyramidData.fLevels.clear();
                        for(int j = 0; j < levelCount; j++) {
                            bool level = false;
                            itemAr.field(("level." + std::to_string(j)).c_str(), level);
                            pyramidData.fLevels.push_back(level);
                        }
                    });
                fPyramids[buildingType] = pyramidData;
            }
        } else {
            int i = 0;
            for(auto& kv : fPyramids) {
                eBuildingType buildingType = kv.first;
                ePyramidAvailable& pyramidData = kv.second;
                ar.archiveField(("pyramids." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("buildingType", buildingType);
                        itemAr.field("availability", pyramidData.fA);
                        int levelCount = static_cast<int>(pyramidData.fLevels.size());
                        itemAr.field("levels.count", levelCount);
                        for(int j = 0; j < levelCount; j++) {
                            bool level = pyramidData.fLevels[j];
                            itemAr.field(("level." + std::to_string(j)).c_str(), level);
                        }
                    });
            }
        }
    }
}

void eAvailableBuildings::allowPyramid(
        const eBuildingType type,
        const std::vector<bool>& levels) {
    auto& a = fPyramids[type];
    if(a.fA == eAvailable::built) return;
    a.fA = eAvailable::available;
    a.fLevels = levels;
}

std::vector<bool> eAvailableBuildings::pyramidLevels(
        const eBuildingType type) const {
    const auto it = fPyramids.find(type);
    if(it == fPyramids.end()) return {};
    const auto& a = it->second;
    return a.fLevels;
}

bool eAvailableBuildings::available(
        const eBuildingType type,
        const int id) const {
    const bool pyramid = eBuilding::sPyramidBuilding(type);
    if(pyramid) {
        const auto it = fPyramids.find(type);
        if(it == fPyramids.end()) return false;
        const auto& a = it->second;
        return a.fA == eAvailable::available;
    }

    switch(type) {
    case eBuildingType::eliteHousing:
        return fEliteHousing;

    case eBuildingType::wheatFarm: {
        const bool s = fDemeterSanctuary == eAvailable::built;
        if(s) return true;
        return fWheatFarm;
    } break;
    case eBuildingType::carrotsFarm:
        return fCarrotsFarm;
    case eBuildingType::onionsFarm:
        return fOnionFarm;

    case eBuildingType::growersLodge: {
        const bool s = fAthenaSanctuary == eAvailable::built ||
                       fDionysusSanctuary == eAvailable::built;
        if(s) return true;
        return fVine || fOliveTree;
    } break;
    case eBuildingType::vine:
        return fVine;
    case eBuildingType::oliveTree:
        return fOliveTree;
    case eBuildingType::orangeTendersLodge: {
        const bool s = fHeraSanctuary == eAvailable::built;
        if(s) return true;
        return fOrangeTree;
    } break;
    case eBuildingType::orangeTree:
        return fOrangeTree;

    case eBuildingType::dairy:
    case eBuildingType::goat:
        return fDairy;
    case eBuildingType::cardingShed:
    case eBuildingType::sheep:
        return fCardingShed;

    case eBuildingType::fishery:
        return fFishery;
    case eBuildingType::urchinQuay:
        return fUrchinQuay;
    case eBuildingType::huntingLodge:
        return fHuntingLodge;
    case eBuildingType::corral:
    case eBuildingType::cattle:
        return fCorral;

    case eBuildingType::mint: {
        const bool s = fHadesSanctuary == eAvailable::built;
        if(s) return true;
        return fMint;
    } break;
    case eBuildingType::foundry: {
        const bool s = fHephaestusSanctuary == eAvailable::built;
        if(s) return true;
        return fFoundry;
    } break;
    case eBuildingType::timberMill:
        return fTimberMill;
    case eBuildingType::masonryShop:
        return fMasonryShop;
    case eBuildingType::refinery:
        return fRefinery;
    case eBuildingType::blackMarbleWorkshop:
        return fBlackMarbleWorkshop;

    case eBuildingType::winery: {
        const bool s = fDionysusSanctuary == eAvailable::built;
        if(s) return true;
        return fWinery;
    } break;
    case eBuildingType::olivePress: {
        const bool s = fAthenaSanctuary == eAvailable::built;
        if(s) return true;
        return fOlivePress;
    } break;
    case eBuildingType::sculptureStudio:
        return fSculptureStudio;

    case eBuildingType::armory:
        return fArmory;

    case eBuildingType::horseRanch:
        return fHorseRanch;
    case eBuildingType::chariotFactory:
        return fChariotFactory;

    case eBuildingType::triremeWharf:
        return fTriremeWharf;
    case eBuildingType::hippodromePiece:
    case eBuildingType::crosswalk:
        return fHippodrome;

    case eBuildingType::templeAphrodite:
        return fAphroditeSanctuary == eAvailable::available;
    case eBuildingType::templeApollo:
        return fApolloSanctuary == eAvailable::available;
    case eBuildingType::templeAres:
        return fAresSanctuary == eAvailable::available;
    case eBuildingType::templeArtemis:
        return fArtemisSanctuary == eAvailable::available;
    case eBuildingType::templeAthena:
        return fAthenaSanctuary == eAvailable::available;
    case eBuildingType::templeAtlas:
        return fAtlasSanctuary == eAvailable::available;
    case eBuildingType::templeDemeter:
        return fDemeterSanctuary == eAvailable::available;
    case eBuildingType::templeDionysus:
        return fDionysusSanctuary == eAvailable::available;
    case eBuildingType::templeHades:
        return fHadesSanctuary == eAvailable::available;
    case eBuildingType::templeHephaestus:
        return fHephaestusSanctuary == eAvailable::available;
    case eBuildingType::templeHera:
        return fHeraSanctuary == eAvailable::available;
    case eBuildingType::templeHermes:
        return fHermesSanctuary == eAvailable::available;
    case eBuildingType::templePoseidon:
        return fPoseidonSanctuary == eAvailable::available;
    case eBuildingType::templeZeus:
        return fZeusSanctuary == eAvailable::available;

    case eBuildingType::achillesHall:
        return fAchillesHall == eAvailable::available;
    case eBuildingType::atalantaHall:
        return fAtalantaHall == eAvailable::available;
    case eBuildingType::bellerophonHall:
        return fBellerophonHall == eAvailable::available;
    case eBuildingType::herculesHall:
        return fHerculesHall == eAvailable::available;
    case eBuildingType::jasonHall:
        return fJasonHall == eAvailable::available;
    case eBuildingType::odysseusHall:
        return fOdysseusHall == eAvailable::available;
    case eBuildingType::perseusHall:
        return fPerseusHall == eAvailable::available;
    case eBuildingType::theseusHall:
        return fTheseusHall == eAvailable::available;

    case eBuildingType::commemorative:
        if(id == 0) return fPopulationMonument;
        if(id == 1) return fVictoryMonument;
        if(id == 2) return fColonyMonument;
        if(id == 3) return fAthleteMonument;
        if(id == 4) return fConquestMonument;
        if(id == 5) return fHappinessMonument;
        if(id == 6) return fHeroicMonument;
        if(id == 7) return fDiplomacyMonument;
        if(id == 8) return fScholarMonument;
        break;
    case eBuildingType::godMonument: {
        const auto t = static_cast<eGodType>(id);
        return mGodMonuments.at(t);
    } break;
    default:
        return true;
    }
    return true;
}

void eAvailableBuildings::built(
        const eBuildingType type, const int id) {
    const bool pyramid = eBuilding::sPyramidBuilding(type);
    if(pyramid) {
        const auto it = fPyramids.find(type);
        if(it == fPyramids.end()) return;
        auto& a = it->second;
        a.fA = eAvailable::built;
        return;
    }

    switch(type) {
    case eBuildingType::commemorative: {
        int* c = nullptr;
        if(id == 0) c = &fPopulationMonument;
        else if(id == 1) c = &fVictoryMonument;
        else if(id == 2) c = &fColonyMonument;
        else if(id == 3) c = &fAthleteMonument;
        else if(id == 4) c = &fConquestMonument;
        else if(id == 5) c = &fHappinessMonument;
        else if(id == 6) c = &fHeroicMonument;
        else if(id == 7) c = &fDiplomacyMonument;
        else if(id == 8) c = &fScholarMonument;
        if(!c) return;
        *c = std::max(0, *c - 1);
    } break;
    case eBuildingType::godMonument: {
        const auto t = static_cast<eGodType>(id);
        int& c = mGodMonuments[t];
        c = std::max(0, c - 1);
    } break;

    case eBuildingType::templeAphrodite:
        fAphroditeSanctuary = eAvailable::built; break;
    case eBuildingType::templeApollo:
        fApolloSanctuary = eAvailable::built; break;
    case eBuildingType::templeAres:
        fAresSanctuary = eAvailable::built; break;
    case eBuildingType::templeArtemis:
        fArtemisSanctuary = eAvailable::built; break;
    case eBuildingType::templeAthena:
        fAthenaSanctuary = eAvailable::built; break;
    case eBuildingType::templeAtlas:
        fAtlasSanctuary = eAvailable::built; break;
    case eBuildingType::templeDemeter:
        fDemeterSanctuary = eAvailable::built; break;
    case eBuildingType::templeDionysus:
        fDionysusSanctuary = eAvailable::built; break;
    case eBuildingType::templeHades:
        fHadesSanctuary = eAvailable::built; break;
    case eBuildingType::templeHephaestus:
        fHephaestusSanctuary = eAvailable::built; break;
    case eBuildingType::templeHera:
        fHeraSanctuary = eAvailable::built; break;
    case eBuildingType::templeHermes:
        fHermesSanctuary = eAvailable::built; break;
    case eBuildingType::templePoseidon:
        fPoseidonSanctuary = eAvailable::built; break;
    case eBuildingType::templeZeus:
        fZeusSanctuary = eAvailable::built; break;

    case eBuildingType::achillesHall:
        fAchillesHall = eAvailable::built; break;
    case eBuildingType::atalantaHall:
        fAtalantaHall = eAvailable::built; break;
    case eBuildingType::bellerophonHall:
        fBellerophonHall = eAvailable::built; break;
    case eBuildingType::herculesHall:
        fHerculesHall = eAvailable::built; break;
    case eBuildingType::jasonHall:
        fJasonHall = eAvailable::built; break;
    case eBuildingType::odysseusHall:
        fOdysseusHall = eAvailable::built; break;
    case eBuildingType::perseusHall:
        fPerseusHall = eAvailable::built; break;
    case eBuildingType::theseusHall:
        fTheseusHall = eAvailable::built; break;

    default:
        break;
    }
}

void eAvailableBuildings::destroyed(
        const eBuildingType type, const int id) {
    const bool pyramid = eBuilding::sPyramidBuilding(type);
    if(pyramid) {
        const auto it = fPyramids.find(type);
        if(it == fPyramids.end()) return;
        auto& a = it->second;
        a.fA = eAvailable::available;
        return;
    }
    (void)id;
    const auto a = availablePtr(type);
    if(!a) return;
    if(*a == eAvailable::built) *a = eAvailable::available;
}

void eAvailableBuildings::allow(
        const eBuildingType type, const int id) {
    switch(type) {
    case eBuildingType::commemorative: {
        int* c = nullptr;
        if(id == 0) c = &fPopulationMonument;
        else if(id == 1) c = &fVictoryMonument;
        else if(id == 2) c = &fColonyMonument;
        else if(id == 3) c = &fAthleteMonument;
        else if(id == 4) c = &fConquestMonument;
        else if(id == 5) c = &fHappinessMonument;
        else if(id == 6) c = &fHeroicMonument;
        else if(id == 7) c = &fDiplomacyMonument;
        else if(id == 8) c = &fScholarMonument;
        if(!c) return;
        (*c)++;
    } break;

    case eBuildingType::godMonument: {
        const auto t = static_cast<eGodType>(id);
        int& c = mGodMonuments[t];
        c++;
    } break;

    default: {
        const auto a = availablePtr(type);
        if(a) {
            if(*a == eAvailable::built) return;
            *a = eAvailable::available;
            return;
        }
        const auto aa = allowedPtr(type);
        if(aa) {
            *aa = true;
            return;
        }
    } break;
    }
}

void eAvailableBuildings::disallow(
        const eBuildingType type, const int id) {
    const bool pyramid = eBuilding::sPyramidBuilding(type);
    if(pyramid) {
        const auto it = fPyramids.find(type);
        if(it == fPyramids.end()) return;
        auto& a = it->second;
        a.fA = eAvailable::notAvailable;
        return;
    }
    (void)id;
    const auto a = availablePtr(type);
    if(a) {
        *a = eAvailable::notAvailable;
        return;
    }
    const auto aa = allowedPtr(type);
    if(aa) {
        *aa = false;
        return;
    }
}

void eAvailableBuildings::startEpisode(const eAvailableBuildings& o) {
    for(auto& op : o.fPyramids) {
        auto& oa = op.second;
        auto& a = fPyramids[op.first];
        if(a.fA == eAvailable::built) continue;
        a.fA = oa.fA;
        a.fLevels = oa.fLevels;
    }

    startEpisode(o, &eAvailableBuildings::fEliteHousing);

    startEpisode(o, &eAvailableBuildings::fWheatFarm);
    startEpisode(o, &eAvailableBuildings::fCarrotsFarm);
    startEpisode(o, &eAvailableBuildings::fOnionFarm);

    startEpisode(o, &eAvailableBuildings::fVine);
    startEpisode(o, &eAvailableBuildings::fOliveTree);
    startEpisode(o, &eAvailableBuildings::fOrangeTree);

    startEpisode(o, &eAvailableBuildings::fDairy);
    startEpisode(o, &eAvailableBuildings::fCardingShed);

    startEpisode(o, &eAvailableBuildings::fFishery);
    startEpisode(o, &eAvailableBuildings::fUrchinQuay);
    startEpisode(o, &eAvailableBuildings::fHuntingLodge);
    startEpisode(o, &eAvailableBuildings::fCorral);

    startEpisode(o, &eAvailableBuildings::fMint);
    startEpisode(o, &eAvailableBuildings::fFoundry);
    startEpisode(o, &eAvailableBuildings::fTimberMill);
    startEpisode(o, &eAvailableBuildings::fMasonryShop);
    startEpisode(o, &eAvailableBuildings::fRefinery);
    startEpisode(o, &eAvailableBuildings::fBlackMarbleWorkshop);

    startEpisode(o, &eAvailableBuildings::fWinery);
    startEpisode(o, &eAvailableBuildings::fOlivePress);
    startEpisode(o, &eAvailableBuildings::fSculptureStudio);

    startEpisode(o, &eAvailableBuildings::fArmory);

    startEpisode(o, &eAvailableBuildings::fHorseRanch);
    startEpisode(o, &eAvailableBuildings::fChariotFactory);

    startEpisode(o, &eAvailableBuildings::fTriremeWharf);
    startEpisode(o, &eAvailableBuildings::fHippodrome);

    startEpisode(o, &eAvailableBuildings::fAphroditeSanctuary);
    startEpisode(o, &eAvailableBuildings::fApolloSanctuary);
    startEpisode(o, &eAvailableBuildings::fAresSanctuary);
    startEpisode(o, &eAvailableBuildings::fArtemisSanctuary);
    startEpisode(o, &eAvailableBuildings::fAthenaSanctuary);
    startEpisode(o, &eAvailableBuildings::fAtlasSanctuary);
    startEpisode(o, &eAvailableBuildings::fDemeterSanctuary);
    startEpisode(o, &eAvailableBuildings::fDionysusSanctuary);
    startEpisode(o, &eAvailableBuildings::fHadesSanctuary);
    startEpisode(o, &eAvailableBuildings::fHephaestusSanctuary);
    startEpisode(o, &eAvailableBuildings::fHeraSanctuary);
    startEpisode(o, &eAvailableBuildings::fHermesSanctuary);
    startEpisode(o, &eAvailableBuildings::fPoseidonSanctuary);
    startEpisode(o, &eAvailableBuildings::fZeusSanctuary);
}

void eAvailableBuildings::startEpisode(const eAvailableBuildings& o,
                               bool eAvailableBuildings::*ptr) {
    this->*ptr = o.*ptr;
}

void eAvailableBuildings::startEpisode(const eAvailableBuildings& o,
                                       eAvailable eAvailableBuildings::*ptr) {
    if(this->*ptr == eAvailable::built) return;
    this->*ptr = o.*ptr;
}

bool* eAvailableBuildings::allowedPtr(const eBuildingType type) {
    switch(type) {
    case eBuildingType::eliteHousing:
        return &fEliteHousing;

    case eBuildingType::wheatFarm:
        return &fWheatFarm;
    case eBuildingType::carrotsFarm:
        return &fCarrotsFarm;
    case eBuildingType::onionsFarm:
        return &fOnionFarm;

    case eBuildingType::vine:
        return &fVine;
    case eBuildingType::oliveTree:
        return &fOliveTree;
    case eBuildingType::orangeTree:
        return &fOrangeTree;

    case eBuildingType::dairy:
        return &fDairy;
    case eBuildingType::cardingShed:
        return &fCardingShed;

    case eBuildingType::fishery:
        return &fFishery;
    case eBuildingType::urchinQuay:
        return &fUrchinQuay;
    case eBuildingType::huntingLodge:
        return &fHuntingLodge;
    case eBuildingType::corral:
        return &fCorral;

    case eBuildingType::mint:
        return &fMint;
    case eBuildingType::foundry:
        return &fFoundry;
    case eBuildingType::timberMill:
        return &fTimberMill;
    case eBuildingType::masonryShop:
        return &fMasonryShop;
    case eBuildingType::refinery:
        return &fRefinery;
    case eBuildingType::blackMarbleWorkshop:
        return &fBlackMarbleWorkshop;

    case eBuildingType::winery:
        return &fWinery;
    case eBuildingType::olivePress:
        return &fOlivePress;
    case eBuildingType::sculptureStudio:
        return &fSculptureStudio;

    case eBuildingType::armory:
        return &fArmory;

    case eBuildingType::horseRanch:
        return &fHorseRanch;
    case eBuildingType::chariotFactory:
        return &fChariotFactory;

    case eBuildingType::triremeWharf:
        return &fTriremeWharf;
    case eBuildingType::hippodromePiece:
    case eBuildingType::crosswalk:
        return &fHippodrome;

    default:
        return nullptr;
    }
    return nullptr;
}

eAvailable* eAvailableBuildings::availablePtr(const eBuildingType type) {
    eAvailable* a = nullptr;

    switch(type) {
    case eBuildingType::templeAphrodite:
        a = &fAphroditeSanctuary; break;
    case eBuildingType::templeApollo:
        a = &fApolloSanctuary; break;
    case eBuildingType::templeAres:
        a = &fAresSanctuary; break;
    case eBuildingType::templeArtemis:
        a = &fArtemisSanctuary; break;
    case eBuildingType::templeAthena:
        a = &fAthenaSanctuary; break;
    case eBuildingType::templeAtlas:
        a = &fAtlasSanctuary; break;
    case eBuildingType::templeDemeter:
        a = &fDemeterSanctuary; break;
    case eBuildingType::templeDionysus:
        a = &fDionysusSanctuary; break;
    case eBuildingType::templeHades:
        a = &fHadesSanctuary; break;
    case eBuildingType::templeHephaestus:
        a = &fHephaestusSanctuary; break;
    case eBuildingType::templeHera:
        a = &fHeraSanctuary; break;
    case eBuildingType::templeHermes:
        a = &fHermesSanctuary; break;
    case eBuildingType::templePoseidon:
        a = &fPoseidonSanctuary; break;
    case eBuildingType::templeZeus:
        a = &fZeusSanctuary; break;

    case eBuildingType::achillesHall:
        a = &fAchillesHall; break;
    case eBuildingType::atalantaHall:
        a = &fAtalantaHall; break;
    case eBuildingType::bellerophonHall:
        a = &fBellerophonHall; break;
    case eBuildingType::herculesHall:
        a = &fHerculesHall; break;
    case eBuildingType::jasonHall:
        a = &fJasonHall; break;
    case eBuildingType::odysseusHall:
        a = &fOdysseusHall; break;
    case eBuildingType::perseusHall:
        a = &fPerseusHall; break;
    case eBuildingType::theseusHall:
        a = &fTheseusHall; break;

    default:
        break;
    }
    return a;
}
