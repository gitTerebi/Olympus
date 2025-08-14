#include "ecampaign.h"

#include "elanguage.h"
#include "pak/zeusfile.h"
#include "pak/epakhelpers.h"

#include "buildings/pyramids/epyramid.h"

#include "gameEvents/ereceiverequestevent.h"

eResourceType pakCityResourceByteToType(
        const uint8_t byte, const bool newVersion) {
    if(byte == 0) {
        return eResourceType::none;
    } else if(byte == 1) {
        return eResourceType::urchin;
    } else if(byte == 2) {
        return eResourceType::fish;
    } else if(byte == 3) {
        return eResourceType::meat;
    } else if(byte == 4) {
        return eResourceType::cheese;
    } else if(byte == 5) {
        return eResourceType::carrots;
    } else if(byte == 6) {
        return eResourceType::onions;
    } else if(byte == 7) {
        return eResourceType::wheat;
    } else if(newVersion && byte == 8) {
        return eResourceType::oranges;
    } else if(byte == 9 + (newVersion ? 0 : -1)) {
        return eResourceType::wood;
    } else if(byte == 10 + (newVersion ? 0 : -1)) {
        return eResourceType::bronze;
    } else if(byte == 11 + (newVersion ? 0 : -1)) {
        return eResourceType::marble;
    } else if(byte == 12 + (newVersion ? 0 : -1)) {
        return eResourceType::grapes;
    } else if(byte == 13 + (newVersion ? 0 : -1)) {
        return eResourceType::olives;
    } else if(byte == 14 + (newVersion ? 0 : -1)) {
        return eResourceType::fleece;
    } else if(newVersion && byte == 16) {
        return eResourceType::blackMarble;
    } else if(newVersion && byte == 17) {
        return eResourceType::orichalc;
    }

    printf("Invalid city resource byte %i\n", byte);
    return eResourceType::none;
}

void readEpisodeAllowedBuildings(eEpisode& ep, ZeusFile& file,
                                 const bool atlantean, const eCityId cid) {
    auto& av = ep.fAvailableBuildings[cid];
    for(const auto type : {eBuildingType::mint,
                           eBuildingType::huntingLodge,
                           eBuildingType::olivePress,
                           eBuildingType::grandAgora,
                           eBuildingType::corral,
                           eBuildingType::granary,
                           eBuildingType::commonAgora,
                           eBuildingType::warehouse,
                           eBuildingType::tradePost, // trade buildings
                           atlantean ? eBuildingType::museum : eBuildingType::stadium,
                           atlantean ? eBuildingType::laboratory : eBuildingType::theater,
                           atlantean ? eBuildingType::bibliotheke : eBuildingType::gymnasium,
                           eBuildingType::winery,
                           eBuildingType::sculptureStudio,
                           atlantean ? eBuildingType::observatory : eBuildingType::podium,
                           atlantean ? eBuildingType::university : eBuildingType::college,
                           eBuildingType::fountain,
                           eBuildingType::horseRanch,
                           atlantean ? eBuildingType::inventorsWorkshop : eBuildingType::dramaSchool,
                           eBuildingType::maintenanceOffice,
                           eBuildingType::hospital,
                           eBuildingType::taxOffice,
                           eBuildingType::watchPost,
                           eBuildingType::palace,
                           eBuildingType::none, // hero's hall
                           eBuildingType::none, // road block
                           eBuildingType::bridge,
                           eBuildingType::none, // columns
                           eBuildingType::park,
                           eBuildingType::avenue,
                           eBuildingType::none, // boulevards
                           eBuildingType::wall,
                           eBuildingType::tower,
                           eBuildingType::gatehouse,
                           eBuildingType::bench,
                           eBuildingType::gazebo,
                           eBuildingType::flowerGarden,
                           eBuildingType::hedgeMaze,
                           eBuildingType::fishPond,
                           eBuildingType::armory,
                           eBuildingType::triremeWharf,
                           eBuildingType::eliteHousing,
                           eBuildingType::none, // hippodrome
                           eBuildingType::chariotFactory,
                           eBuildingType::tallObelisk,
                           eBuildingType::sundial,
                           eBuildingType::topiary,
                           eBuildingType::spring,
                           eBuildingType::stoneCircle,
                           eBuildingType::shortObelisk,
                           eBuildingType::waterPark,
                           eBuildingType::dolphinSculpture,
                           eBuildingType::orrery,
                           eBuildingType::shellGarden,
                           eBuildingType::baths,
                           eBuildingType::birdBath}) {
        const auto allowedByte = file.readUByte();
        file.skipBytes(1);
        if(type == eBuildingType::grandAgora) file.skipBytes(2);
        if(allowedByte != 1) {
            printf("%s %i\n", eBuilding::sNameForBuilding(type).c_str(), allowedByte);
        }
        const bool allowed = allowedByte == 1;
        if(allowed) av.allow(type);
        else av.disallow(type);
    }
    printf("\n");
}

void readEpisodeResources(eEpisode& ep, ZeusFile& file,
                          const eCityId cid) {
    const bool newVersion = file.isNewVersion();
    auto& av = ep.fAvailableBuildings[cid];
    const int jMax = newVersion ? 12 : 10;
    for(int j = 0; j < jMax; j++) {
        const auto resourceByte = file.readUByte();
        const auto type = pakCityResourceByteToType(
                              resourceByte, newVersion);
        if(type != eResourceType::none) {
            printf("%s\n", eResourceTypeHelpers::typeName(type).c_str());
        } else {
            printf("none\n");
        }
        switch(type) {
        case eResourceType::urchin:
            av.allow(eBuildingType::urchinQuay);
            break;
        case eResourceType::fish:
            av.allow(eBuildingType::fishery);
            break;
        case eResourceType::meat:
            av.allow(eBuildingType::none);
            break;
        case eResourceType::cheese:
            av.allow(eBuildingType::dairy);
            av.allow(eBuildingType::goat);
            break;
        case eResourceType::carrots:
            av.allow(eBuildingType::carrotsFarm);
            break;
        case eResourceType::onions:
            av.allow(eBuildingType::onionsFarm);
            break;
        case eResourceType::wheat:
            av.allow(eBuildingType::wheatFarm);
            break;
        case eResourceType::oranges:
            av.allow(eBuildingType::orangeTendersLodge);
            av.allow(eBuildingType::orangeTree);
            break;
        case eResourceType::wood:
            av.allow(eBuildingType::timberMill);
            break;
        case eResourceType::bronze:
            av.allow(eBuildingType::foundry);
            break;
        case eResourceType::marble:
            av.allow(eBuildingType::masonryShop);
            break;
        case eResourceType::grapes:
            av.allow(eBuildingType::growersLodge);
            av.allow(eBuildingType::vine);
            break;
        case eResourceType::olives:
            av.allow(eBuildingType::growersLodge);
            av.allow(eBuildingType::oliveTree);
            break;
        case eResourceType::fleece:
            av.allow(eBuildingType::cardingShed);
            break;
        case eResourceType::blackMarble:
            av.allow(eBuildingType::blackMarbleWorkshop);
            break;
        case eResourceType::orichalc:
            av.allow(eBuildingType::refinery);
            break;
        default:
            break;
        }
    }
    printf("\n");
}

struct ePakGod {
    bool fValid = false;
    eGodType fType = eGodType::zeus;
};

struct ePakPyramid {
    bool fAllowed = false;
    eBuildingType fType = eBuildingType::modestPyramid;
    std::vector<bool> fLevels;
};

void readEpisodeAllowedSanctuaries(eEpisode& ep, ZeusFile& file,
                                   const std::vector<ePakGod>& friendlyGods,
                                   const eCityId cid) {
    for(int i = 0; i < 6; i++) {
        const uint8_t sanctuaryByte = file.readUByte();
        const auto& v = friendlyGods[i];
        const bool sanctuary = sanctuaryByte;
        if(v.fValid && sanctuary) {
            ep.fFriendlyGods[cid].push_back(v.fType);
        }
        if(v.fValid) {
            printf("%s %i\n", eGod::sGodName(v.fType).c_str(),
                   sanctuaryByte);
        }
    }
    printf("\n");
    file.skipBytes(6);

    const uint8_t maxSanct = file.readUByte();
    ep.fMaxSanctuaries[cid] = maxSanct;
    printf("max sanctuaries %i\n", maxSanct);
    printf("\n");
}

eGameEventType pakIdToEventType(const uint8_t id, bool& valid) {
    if(id == 0x1A) {
        valid = false;
        return eGameEventType::receiveRequest;
    }
    valid = true;
    if(id == 1) return eGameEventType::receiveRequest;

    valid = false;
    printf("Invalid event type id %i\n", id);
    return eGameEventType::receiveRequest;
}

eReceiveRequestType pakIdToReceiveRequestType(const uint16_t id) {
    if(id == 0) return eReceiveRequestType::general;
    else if(id == 3) return eReceiveRequestType::festival;
    else if(id == 4) return eReceiveRequestType::project;
    else if(id == 5) return eReceiveRequestType::famine;
    else if(id == 6) return eReceiveRequestType::financialWoes;
    printf("Invalid receive request type %i\n", id);
    return eReceiveRequestType::general;
}

eGodType pakIdToGodType(const uint8_t id, bool& valid) {
    valid = true;
    if(id == 0) return eGodType::zeus;
    else if(id == 1) return eGodType::poseidon;
    else if(id == 2) return eGodType::demeter;
    else if(id == 3) return eGodType::apollo;
    else if(id == 4) return eGodType::artemis;
    else if(id == 5) return eGodType::ares;
    else if(id == 6) return eGodType::aphrodite;
    else if(id == 7) return eGodType::hermes;
    else if(id == 8) return eGodType::athena;
    else if(id == 9) return eGodType::hephaestus;
    else if(id == 10) return eGodType::dionysus;
    else if(id == 11) return eGodType::hades;
    else if(id == 12) return eGodType::hera;
    else if(id == 13) return eGodType::atlas;
    printf("Invalid god id %i\n", id);
    valid = false;
    return eGodType::zeus;
}

void readEpisodeEvents(eEpisode& ep, ZeusFile& file,
                       const uint8_t nEvents, const eCityId cid) {
    const bool newVersion = file.isNewVersion();
    auto& events = ep.fEvents[cid];
    for(int i = 0; i < nEvents; i++) {
        const uint8_t eventTypeId = file.readUByte();
        bool valid = false;
        const auto type = pakIdToEventType(eventTypeId, valid);
        if(!valid) {
            file.skipBytes(123);
            continue;
        }

        const uint8_t pMonths = file.readUByte();
        file.skipBytes(2);
        const uint16_t value1 = file.readUShort();
        const uint16_t value2 = file.readUShort();
        const uint16_t value3 = file.readUShort();
        file.skipBytes(4);
        const uint16_t value4 = file.readUShort();
        const uint16_t value5 = file.readUShort();
        file.skipBytes(2);
        const uint16_t years0 = file.readUShort();
        const uint16_t years1 = file.readUShort();
        const uint16_t years2 = file.readUShort();
        file.skipBytes(2);
        const uint16_t city0 = file.readUShort();
        const uint16_t city1 = file.readUShort();
        const uint16_t city2 = file.readUShort();
        file.skipBytes(4);
        const uint16_t occuranceType = file.readUShort();
        file.skipBytes(12);
        const uint16_t godType = file.readUShort();
        file.skipBytes(40);
        const uint16_t requestType = file.readUShort();
        file.skipBytes(28);

        uint16_t city;
        if(city0 == 0xFF) {
            const bool first = eRand::rand() % 1;
            city = first ? city1 : city2;
        } else {
            city = city0;
        }

        stdsptr<eGameEvent> e;
        switch(type) {
        case eGameEventType::receiveRequest: {
            const auto type = pakIdToReceiveRequestType(requestType);
            const auto ee = e::make_shared<eReceiveRequestEvent>(
                    cid, eGameEventBranch::root, *ep.fBoard);
            if(value1 == 0xFFFF) {
                ee->setResourceType(0, eResourceType::none);
            } else {
                ee->setResourceType(0, ePakHelpers::pakResourceByteToType(value1, newVersion));
            }
            if(value2 == 0xFFFF) {
                ee->setResourceType(1, eResourceType::none);
            } else {
                ee->setResourceType(1, ePakHelpers::pakResourceByteToType(value2, newVersion));
            }
            if(value3 == 0xFFFF) {
                ee->setResourceType(2, eResourceType::none);
            } else {
                ee->setResourceType(2, ePakHelpers::pakResourceByteToType(value3, newVersion));
            }
            bool valid = false;
            const auto god = pakIdToGodType(godType, valid);
            ee->setRequestType(type);
            ee->setGod(god);
            ee->setMinResourceCount(value4);
            ee->setMaxResourceCount(value5);
            e = ee;
        } break;
        }
        events.push_back(e);
        e->setDatePlusMonths(pMonths);
        uint16_t year;
        if(years0 == 0xFFFF) {
            const uint16_t yearsDiff = years2 - years1;
            const uint16_t randDiff = yearsDiff == 0 ? 0 : (eRand::rand() % yearsDiff);
            year = years1 + randDiff;
        } else {
            year = years0;
        }
        e->setDatePlusYears(year);

        if(occuranceType == 0) { // one time event
        } else if(occuranceType == 2) { // recurring event
        } else {
            printf("Invalid occurance type %id\n", occuranceType);
        }

        printf("%s\n", e->longDatedName().c_str());
    }
    printf("\n");
}

eBuildingType pakIdToPyramidType(const uint8_t id) {
    if(id == 100) return eBuildingType::modestPyramid;
    else if(id == 101) return eBuildingType::pyramid;
    else if(id == 102) return eBuildingType::greatPyramid;
    else if(id == 103) return eBuildingType::majesticPyramid;

    else if(id == 104) return eBuildingType::smallMonumentToTheSky;
    else if(id == 105) return eBuildingType::monumentToTheSky;
    else if(id == 106) return eBuildingType::grandMonumentToTheSky;

    else if(id == 107) return eBuildingType::minorShrineAphrodite;
    else if(id == 108) return eBuildingType::shrineAphrodite;
    else if(id == 109) return eBuildingType::majorShrineAphrodite;

    else if(id == 110) return eBuildingType::pyramidOfThePantheon;
    else if(id == 111) return eBuildingType::altarOfOlympus;
    else if(id == 112) return eBuildingType::templeOfOlympus;
    else if(id == 113) return eBuildingType::observatoryKosmika;
    else if(id == 114) return eBuildingType::museumAtlantika;
    printf("Invalid pyramid type id %i\n", id);
    return eBuildingType::modestPyramid;
}

void readEpisodeAllowedPyramids(
        ZeusFile& file, std::vector<ePakPyramid>& pyramids) {
    const uint8_t n = file.readUByte();
    file.skipBytes(3);
    for(int i = 0; i < n; i++) {
        const uint8_t typeId = file.readUByte();
        std::vector<bool> levels;
        file.skipBytes(3);
        auto type = pakIdToPyramidType(typeId);
        if(type == eBuildingType::minorShrineAphrodite ||
           type == eBuildingType::shrineAphrodite ||
           type == eBuildingType::majorShrineAphrodite) {
            const uint8_t godId = file.readUByte();
            bool valid;
            const auto godType = pakIdToGodType(godId, valid);
            type = ePyramid::sSwitchGod(type, godType);
        } else {
            file.skipBytes(1);
        }
        file.skipBytes(7);
        pyramids.push_back({false, type, levels});
    }
}

void readEpisodeEnabledPyramids(
        ZeusFile& file, std::vector<ePakPyramid>& pyramids) {
    for(auto& p : pyramids) {
        const uint8_t id = file.readUByte();
        uint8_t levelsId;
        bool enabled = false;
        if(id >= 0xC0) {
            enabled = true;
            levelsId = id - 0xC0;
        } else if(id >= 0x40) {
            enabled = true;
            levelsId = id - 0x40;
        } else {
            levelsId = id;
        }
        const int levels = ePyramid::sLevels(p.fType);
        p.fLevels.clear();
        for(int i = 0; i < levels; i++) {
            const uint8_t pow = std::pow(2, i);
            p.fLevels.push_back(levelsId & pow);
        }
        p.fAllowed = p.fAllowed || enabled;
        file.skipBytes(11);
        if(p.fAllowed) {
            const auto name = eBuilding::sNameForBuilding(p.fType);
            printf("%s", name.c_str());
            for(const bool l : p.fLevels) {
                printf(l ? " b" : " w");
            }
            printf("\n");
        }
    }
    printf("\n");
}

void applyPyramidsToEpisode(const std::vector<ePakPyramid>& pyramids,
                            eEpisode& ep, const eCityId cid) {
    auto& a = ep.fAvailableBuildings[cid];
    for(const auto& p : pyramids) {
        if(!p.fAllowed) continue;
        a.allowPyramid(p.fType, p.fLevels);
    }
}

eEpisodeGoalType pakIdToEpisodeGoalType(const uint8_t id) {
    if(id == 0) return eEpisodeGoalType::population;
    else if(id == 1) return eEpisodeGoalType::treasury;
    else if(id == 2) return eEpisodeGoalType::sanctuary;
    else if(id == 3) return eEpisodeGoalType::support;
    else if(id == 4) return eEpisodeGoalType::quest;
    else if(id == 5) return eEpisodeGoalType::slay;
    else if(id == 6) return eEpisodeGoalType::yearlyProduction;
    else if(id == 7) return eEpisodeGoalType::rule;
    else if(id == 8) return eEpisodeGoalType::yearlyProfit;
    else if(id == 9) return eEpisodeGoalType::housing;
    else if(id == 10) return eEpisodeGoalType::tradingPartners;
    else if(id == 14) return eEpisodeGoalType::setAsideGoods;
    else if(id == 15) return eEpisodeGoalType::pyramid;
    else if(id == 16) return eEpisodeGoalType::hippodrome;
    printf("Invalid episode goal id %i\n", id);
    return eEpisodeGoalType::population;
}

eBannerType pakIdToBannerType(const uint16_t id) {
    if(id == 11) return eBannerType::rockThrower;
    else if(id == 12) return eBannerType::hoplite;
    else if(id == 13) return eBannerType::horseman;
    else if(id == 57) return eBannerType::trireme;
    printf("Invalid banner type id %i\n", id);
    return eBannerType::rockThrower;
}

eWorldMap pakIdToWorldMap(const uint8_t id) {
    if(id == 1) return eWorldMap::greece1;
    if(id == 2) return eWorldMap::greece2;
    if(id == 3) return eWorldMap::greece3;
    if(id == 4) return eWorldMap::greece4;
    if(id == 5) return eWorldMap::greece5;
    if(id == 6) return eWorldMap::greece6;
    if(id == 7) return eWorldMap::greece7;
    if(id == 8 || id == 9 || id == 10) return eWorldMap::greece8;

    printf("Invalid world map type id %i\n", id);
    return eWorldMap::greece1;
}

void readEpisodeGoal(eEpisode& ep, ZeusFile& file) {
    const bool newVersion = file.isNewVersion();
    const uint8_t typeId = file.readUByte();
    file.skipBytes(3);
    const uint16_t value1 = file.readUShort();
    file.skipBytes(2);
    const uint16_t value2 = file.readUShort();
    file.skipBytes(2);
    const uint8_t value3 = file.readUByte();
    file.skipBytes(3);
    const uint8_t value4 = file.readUByte();
    file.skipBytes(59);

    const auto goalType = pakIdToEpisodeGoalType(typeId);
    printf("%s %i %i %i %i\n", eEpisodeGoal::sText(goalType).c_str(),
           value1, value2, value3, value4);
    const auto goal = std::make_shared<eEpisodeGoal>();
    goal->fType = goalType;
    switch(goalType) {
    case eEpisodeGoalType::population:
    case eEpisodeGoalType::treasury:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::pyramid: {
        if(value2 == 0) {
            goal->fEnumInt1 = -1;
            goal->fRequiredCount = value4;
        } else {
            auto type = pakIdToPyramidType(value2);
            const bool isToGod = ePyramid::sIsToGod(type);
            if(isToGod) {
                bool valid;
                const auto god = pakIdToGodType(value1, valid);
                type = ePyramid::sSwitchGod(type, god);
            }
            goal->fEnumInt1 = static_cast<int>(type);
            goal->fRequiredCount = 100;
        }
    } break;
    case eEpisodeGoalType::hippodrome: {
        goal->fRequiredCount = value1;
    } break;
    case eEpisodeGoalType::sanctuary: {
        bool valid;
        const auto godType = pakIdToGodType(value1, valid);
        if(valid) {
            goal->fEnumInt1 = static_cast<int>(godType);
            goal->fRequiredCount = 100;
        } else {
            goal->fEnumInt1 = -1;
            goal->fRequiredCount = value3;
        }
    } break;
    case eEpisodeGoalType::support: {
        const auto btype = pakIdToBannerType(value1);
        goal->fEnumInt1 = static_cast<int>(btype);
        goal->fRequiredCount = value2;
    } break;
    case eEpisodeGoalType::quest:
        break;
    case eEpisodeGoalType::slay:
        // value1 is id?
        break;
    case eEpisodeGoalType::yearlyProduction: {
        goal->fRequiredCount = value2;
        const auto type = ePakHelpers::pakResourceByteToType(value1, newVersion);
        goal->fEnumInt1 = static_cast<int>(type);
    } break;
    case eEpisodeGoalType::rule:
        break;
    case eEpisodeGoalType::yearlyProfit:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::housing: {
        if(value1 < 9) {
            goal->fEnumInt1 = 0;
            goal->fEnumInt2 = value1 - 2;
        } else {
            goal->fEnumInt1 = 1;
            goal->fEnumInt2 = value1 - 10;
        }
        goal->fRequiredCount = value2;
    } break;
    case eEpisodeGoalType::tradingPartners:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::setAsideGoods: {
        const auto type = ePakHelpers::pakResourceByteToType(value1, newVersion);
        goal->fEnumInt1 = static_cast<int>(type);
        goal->fRequiredCount = value2;
    } break;
    case eEpisodeGoalType::surviveUntil:
    case eEpisodeGoalType::completeBefore:
        break;
    }

    printf("%s\n", goal->text(false, false).c_str());
    ep.fGoals.push_back(goal);
}

void readEpisodeText(eEpisode& ep, ZeusFile& file) {
    const uint16_t introId = file.readUShort();
    file.skipBytes(10);
    const uint16_t completeId = file.readUShort();
    if(introId != 0 && introId != 65535) {
        ep.fIntroId = introId;
        const auto intro = eLanguage::zeusMM(introId);
        ep.fTitle = intro.first;
        ep.fIntroduction = intro.second;
        eCampaign::sReplaceSpecial(ep.fIntroduction);
    }
    if(completeId != 0 && completeId != 65535) {
        ep.fCompleteId = completeId;
        const auto complete = eLanguage::zeusMM(completeId);
        ep.fComplete = complete.second;
        eCampaign::sReplaceSpecial(ep.fComplete);
    }
}

uint16_t toUShort(const uint8_t b1, const uint8_t b2) {
    return (uint16_t)(b1 | ((b2) << 8));
}

#include "pak/pkwareinputstream.h"
void eCampaign::readPak(const std::string& name,
                        const std::string& path) {
    mIsPak = true;
    mName = name;
    ZeusFile file(path);
    file.readVersion();
    const bool newVersion = file.isNewVersion();
    file.readAtlantean();
    printf("v%i a%i\n", file.isNewVersion() ? 1 : 0, file.isAtlantean() ? 1 : 0);
    mParentBoard = e::make_shared<eGameBoard>();
    mParentBoard->setWorldBoard(&mWorldBoard);

    file.seek(0);
    file.getNumMaps();
    eCityId parentCid;
    file.loadBoard(*mParentBoard, *this, parentCid);

    file.seek(8);
    const uint8_t nParentEps = file.readUByte();
    file.seek(12); // and 16?
    const uint8_t nColonyEps = file.readUByte();
    const bool atlantean = file.isAtlantean();

    file.seek(35648);
    const uint16_t briefId = file.readUShort();
    if(briefId != 0 && briefId != 65535) {
        mBriefId = briefId;
        const auto brief = eLanguage::zeusMM(briefId);
        mTitle = brief.first;
        mIntroduction = brief.second;
    }

    file.seek(35652);
    const uint16_t completeId = file.readUShort();
    if(completeId != 0 && completeId != 65535) {
        mCompleteId = completeId;
        mComplete = eLanguage::zeusMM(completeId).second;
    }

    std::vector<ePakGod> friendlyGods;
    friendlyGods.resize(6);
    std::vector<ePakPyramid> pyramids;

    for(int i = 0; i < nParentEps; i++) {
        printf("parent episode %i:\n\n", i);

        const auto ep = addParentCityEpisode();
        file.seek(104 + i*356);
        const auto nextByte = file.readUByte();
        const bool nextColony = nextByte == 1;
        ep->fNextEpisode = nextColony ? eEpisodeType::colony :
                                        eEpisodeType::parentCity;
        file.seek(8174 + i*2032); // mint
        readEpisodeAllowedBuildings(*ep, file, atlantean, parentCid);
        file.seek(8752 + i*2032);
        readEpisodeText(*ep, file);
        file.seek(9100 + i*2032);
        readEpisodeResources(*ep, file, parentCid);

        const int epInc = newVersion ? 300 : 224;

        if(i == 0) {
            file.seek(35784 + i*epInc);
            for(int i = 0; i < 6; i++) {
                auto& v = friendlyGods[i];
                const uint8_t godId = file.readUByte();
                v.fType = pakIdToGodType(godId, v.fValid);
                file.skipBytes(3);
            }

            if(atlantean) {
                file.seek(35960 + i*epInc);
                readEpisodeAllowedPyramids(file, pyramids);
            }
        }

        file.seek(35944 + i*epInc);
        readEpisodeAllowedSanctuaries(*ep, file, friendlyGods, parentCid);
        if(atlantean) {
            file.seek(35972 + i*epInc);
            readEpisodeEnabledPyramids(file, pyramids);
            applyPyramidsToEpisode(pyramids, *ep, parentCid);
        }

        file.seek(799297 + i*4);
        const uint8_t nEvents = file.readUByte();
        file.seek(38874 + i*18600);
        readEpisodeEvents(*ep, file, nEvents, parentCid);

        if(newVersion) {
            file.seek(838331 + i*4);
        } else {
            file.seek(837263 + i*4);
        }
        const uint8_t nGoals = file.readUByte();
        if(newVersion) {
            file.seek(838371 + i*456);
        } else {
            file.seek(837303 + i*456);
        }
        for(int j = 0; j < nGoals; j++) {
            readEpisodeGoal(*ep, file);
        }
        printf("\n");
    }

    file.seek(7140);
    const int16_t startDate = file.readShort();
    file.seek(7172);
    const uint16_t initialFunds = file.readUShort();

    setInitialFunds(ePlayerId::player0, initialFunds);

    setDate(eDate{1, eMonth::january, startDate});

    for(int i = 0; i < 4; i++) {
        printf("colony episode %i:\n\n", i);
        auto& board = mColonyBoards.emplace_back();
        board = e::make_shared<eGameBoard>();
        board->setWorldBoard(&mWorldBoard);
        eCityId colonyCid;
        const bool r = file.loadBoard(*board, *this, colonyCid);
        if(r) {
            const auto c = board->addCityToBoard(colonyCid);
            c->setAtlantean(atlantean);
            board->addPlayerToBoard(ePlayerId::player0);
        } else {
            board->initialize(1, 1);
            continue;
        }

        const auto ep = std::make_shared<eColonyEpisode>();
        ep->fBoard = board.get();
        ep->fWorldBoard = &mWorldBoard;
        ep->fCity = mWorldBoard.cityWithId(colonyCid);
        mColonyEpisodes.push_back(ep);

        file.seek(28494 + i*2032); // mint
        readEpisodeAllowedBuildings(*ep, file, atlantean, colonyCid);
        file.seek(29072 + i*2032);
        readEpisodeText(*ep, file);
        file.seek(29420 + i*2032);
        readEpisodeResources(*ep, file, colonyCid);

        const int epInc = newVersion ? 300 : 224;

        if(newVersion) {
            file.seek(38944 + i*epInc);
        } else {
            file.seek(38184 + i*epInc);
        }
        readEpisodeAllowedSanctuaries(*ep, file, friendlyGods, colonyCid);

        if(atlantean) {
            std::vector<ePakPyramid> pyramids;
            file.seek(38960 + i*epInc);
            readEpisodeAllowedPyramids(file, pyramids);
            file.seek(38972 + i*epInc);
            readEpisodeEnabledPyramids(file, pyramids);
            applyPyramidsToEpisode(pyramids, *ep, colonyCid);
        }

        if(newVersion) {
            file.seek(836491 + i*4);
        } else {
            file.seek(835423 + i*4);
        }
        const uint8_t nGoals = file.readUByte();
        if(newVersion) {
            file.seek(836507 + i*456);
        } else {
            file.seek(835439 + i*456);
        }
        for(int j = 0; j < nGoals; j++) {
            readEpisodeGoal(*ep, file);
        }
        printf("\n");
    }

    loadStrings();
}
