#include "ecampaign.h"

#include <fstream>

#include <filesystem>
#include <regex>

#include "evectorhelpers.h"
#include "egamedir.h"

#include "pak/zeusfile.h"

eCampaign::eCampaign() {
    const auto types = eResourceTypeHelpers::extractResourceTypes(
                           eResourceType::allBasic);
    for(const auto type : types) {
        mPrices[type] = eResourceTypeHelpers::defaultPrice(type);
    }
}

void eCampaign::initialize(const std::string& name) {
    mName = name;

    mParentBoard = e::make_shared<eGameBoard>();
    mParentBoard->initialize(100, 100);
    mParentBoard->setWorldBoard(&mWorldBoard);

    for(int i = 0; i < 4; i++) {
        auto& board = mColonyBoards.emplace_back();
        board = e::make_shared<eGameBoard>();
        board->initialize(1, 1);
        board->setWorldBoard(&mWorldBoard);

        const auto e = std::make_shared<eColonyEpisode>();
        e->fBoard = board.get();
        e->fWorldBoard = &mWorldBoard;
        mColonyEpisodes.push_back(e);
    }

    addParentCityEpisode();
}

int eCampaign::initialFunds(const ePlayerId pid) const {
    const auto it = mDrachmas.find(pid);
    if(it == mDrachmas.end()) return 0;
    return it->second;
}

void eCampaign::setInitialFunds(const ePlayerId pid, const int f) {
    mDrachmas[pid] = f;
}

bool eCampaign::sLoadStrings(const std::string& path, eMap& map) {
    std::ifstream file(path);
    if(!file.good()) {
        printf("File missing %s\n", path.c_str());
        return false;
    }
    std::string line;
    std::string key;
    std::string value;
    while(std::getline(file, line)) {
        if(line.empty()) continue;
        if(line.front() == '\r') continue;
        if(line.front() == '\t') continue;
        if(line.front() == ';') continue;
        if(key.empty()) {
            const auto keyEnd1 = line.find(' ');
            const auto keyEnd2 = line.find('=');
            const auto keyEnd = std::min(keyEnd1, keyEnd2);
            if(keyEnd == std::string::npos) continue;
            key = line.substr(0, keyEnd);
        }

        unsigned long valueStart = 0;
        if(value.empty()) {
            valueStart = line.find('"');
            if(valueStart == std::string::npos) continue;
            valueStart += 1;
        }
        bool foundEnd = true;
        auto valueEnd = line.find('"', valueStart);
        if(valueEnd == std::string::npos) {
            valueEnd = line.size();
            foundEnd = false;
        }
        const auto valueLen = valueEnd - valueStart;
        value = value + line.substr(valueStart, valueLen);

        if(foundEnd) {
            value = std::regex_replace(value, std::regex("^@L"), "");
            value = std::regex_replace(value, std::regex("@L"), "\n");
            value = std::regex_replace(value, std::regex("^@P"), "   ");
            value = std::regex_replace(value, std::regex("@P"), "\n\n   ");
            map[key] = value;
            key = "";
            value = "";
        }
    }
    return true;
}

bool eCampaign::loadStrings() {
    const auto baseDir = eGameDir::adventuresDir();
    const auto aDir = baseDir + mName + "/";
    const auto txtFile = aDir + mName + ".txt";
    std::map<std::string, std::string> map;
    const bool r = sLoadStrings(txtFile, map);
    if(!r) return false;

    mTitle = map["Adventure_Title"];
    mIntroduction = map["Adventure_Introduction"];
    mComplete = map["Adventure_Complete"];

    int id = 1;
    for(auto& e : mParentCityEpisodes) {
        const auto ids = std::to_string(id++);
        const auto titleK = "Parent_Episode_" + ids + "_Title";
        const auto introK = "Parent_Episode_" + ids + "_Introduction";
        const auto completeK = "Parent_Episode_" + ids + "_Complete";
        e->fTitle = map[titleK];
        e->fIntroduction = map[introK];
        e->fComplete = map[completeK];
    }
    id = 1;
    for(auto& e : mColonyEpisodes) {
        const auto ids = std::to_string(id++);
        const auto titleK = "Colony_" + ids + "_Title";
        const auto introK = "Colony_" + ids + "_Introduction";
        const auto completeK = "Colony_" + ids + "_Complete";
        const auto selectionK = "Colony_" + ids + "_Selection";
        e->fTitle = map[titleK];
        e->fIntroduction = map[introK];
        e->fComplete = map[completeK];
        e->fSelection = map[selectionK];
    }
    return true;
}

bool eCampaign::writeStrings(const std::string& path) const {
    std::ofstream file(path);
    if(!file.good()) {
        printf("File missing %s\n", path.c_str());
        return false;
    }

    const auto titleStr = mTitle.empty() ? mName : mTitle;
    file << "Adventure_Title=\"" + titleStr + "\"\n";
    file << "Adventure_Introduction=\"" + mIntroduction + "\"\n";
    file << "Adventure_Complete=\"" + mComplete + "\"\n";
    file << '\n';
    for(int i = 1; i < 11; i++) {
        const eParentCityEpisode* e = nullptr;
        const int iMax = mParentCityEpisodes.size();
        if(i < iMax) {
            e = mParentCityEpisodes[i].get();
        }
        const auto titleStr = e ? e->fTitle : "";
        const auto introStr = e ? e->fIntroduction : "";
        const auto completeStr = e ? e->fComplete : "";
        const auto is = std::to_string(i);
        file << "Parent_Episode_" + is + "_Title=\"" + titleStr + "\"\n";
        file << "Parent_Episode_" + is + "_Introduction=\"" + introStr + "\"\n";
        file << "Parent_Episode_" + is + "_Complete=\"" + completeStr + "\"\n";
    }
    file << '\n';
    for(int i = 1; i < 5; i++) {
        const eColonyEpisode* e = nullptr;
        const int iMax = mColonyEpisodes.size();
        if(i < iMax) {
            e = mColonyEpisodes[i].get();
        }
        const auto titleStr = e ? e->fTitle : "";
        const auto introStr = e ? e->fIntroduction : "";
        const auto completeStr = e ? e->fComplete : "";
        const auto selectionStr = e ? e->fSelection : "";
        const auto is = std::to_string(i);
        file << "Colony_" + is + "_Title=\"" + titleStr + "\"\n";
        file << "Colony_" + is + "_Introduction=\"" + introStr + "\"\n";
        file << "Colony_" + is + "_Complete=\"" + completeStr + "\"\n";
        file << "Colony_" + is + "_Selection=\"" + selectionStr + "\"\n";
    }
    return true;
}

bool eCampaign::sReadGlossary(const std::string& name,
                              eCampaignGlossary& glossary) {
    glossary.fIsPak = false;
    const auto baseDir = eGameDir::adventuresDir();
    const auto aDir = baseDir + name + "/";
    const auto txtFile = aDir + name + ".txt";
    std::map<std::string, std::string> map;
    const bool r = sLoadStrings(txtFile, map);
    if(!r) return false;
    glossary.fFolderName = name;
    glossary.fTitle = map["Adventure_Title"];
    glossary.fIntroduction = map["Adventure_Introduction"];
    glossary.fComplete = map["Adventure_Complete"];

    const auto pakFile = aDir + name + ".epak";
    std::ifstream file(pakFile, std::ios::in | std::ios::binary);
    if(!file) return false;
    eReadSource source(&file);
    eReadStream src(source);
    src.readFormat();
    src >> glossary.fBitmap;
    file.close();
    return true;
}

void eCampaign::read(eReadStream& src) {
    src >> mBitmap;
    std::string name;
    src >> name;
    if(mName.empty()) mName = name;
    src >> mCurrentParentEpisode;
    src >> mCurrentColonyEpisode;
    src >> mCurrentEpisodeType;
    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            ePlayerId pid;
            src >> pid;
            src >> mDrachmas[pid];
        }
    }
    mDate.read(src);
    for(auto& p : mPrices) {
        src >> p.second;
    }
    src >> mDifficulty;
    mWorldBoard.read(src);
    mParentBoard = e::make_shared<eGameBoard>();
    mParentBoard->setWorldBoard(&mWorldBoard);
    mParentBoard->read(src);

    {
        int ne;
        src >> ne;
        for(int i = 0; i < ne; i++) {
            int e;
            src >> e;
            mPlayedColonyEpisodes.push_back(e);
        }
    }

    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            auto& b = mColonyBoards.emplace_back();
            const bool finished = colonyEpisodeFinished(i);
            b = e::make_shared<eGameBoard>();
            b->setWorldBoard(&mWorldBoard);
            if(finished) continue;
            b->read(src);
        }
    }

    {
        int ne;
        src >> ne;
        for(int i = 0; i < ne; i++) {
            const auto e = std::make_shared<eParentCityEpisode>();
            e->fBoard = mParentBoard.get();
            e->fWorldBoard = &mWorldBoard;
            e->read(src);
            mParentCityEpisodes.push_back(e);
        }
    }

    {
        int ne;
        src >> ne;
        for(int i = 0; i < ne; i++) {
            const auto e = std::make_shared<eColonyEpisode>();
            e->fBoard = mColonyBoards[i].get();
            e->fWorldBoard = &mWorldBoard;
            e->read(src);
            mColonyEpisodes.push_back(e);
        }
    }

    {
        int ne;
        src >> ne;
        for(int i = 0; i < ne; i++) {
            eSetAside set;
            set.read(src, &mWorldBoard);
            mForColony.push_back(set);
        }
    }
    {
        int ne;
        src >> ne;
        for(int i = 0; i < ne; i++) {
            eSetAside set;
            set.read(src, &mWorldBoard);
            mForParent.push_back(set);
        }
    }
}

void eCampaign::write(eWriteStream& dst) const {
    dst << mBitmap;
    dst << mName;
    dst << mCurrentParentEpisode;
    dst << mCurrentColonyEpisode;
    dst << mCurrentEpisodeType;
    dst << mDrachmas.size();
    for(const auto& d : mDrachmas) {
        dst << d.first;
        dst << d.second;
    }
    mDate.write(dst);
    for(const auto& p : mPrices) {
        dst << p.second;
    }
    dst << mDifficulty;
    mWorldBoard.write(dst);
    mParentBoard->write(dst);

    dst << mPlayedColonyEpisodes.size();
    for(const int e : mPlayedColonyEpisodes) {
        dst << e;
    }

    dst << mColonyBoards.size();
    int cid = 0;
    for(const auto& b : mColonyBoards) {
        const bool finished = colonyEpisodeFinished(cid++);
        if(finished) continue;
        b->write(dst);
    }

    dst << mParentCityEpisodes.size();
    for(const auto& e : mParentCityEpisodes) {
        e->write(dst);
    }

    dst << mColonyEpisodes.size();
    for(const auto& e : mColonyEpisodes) {
        e->write(dst);
    }

    dst << mForColony.size();
    for(const auto& e : mForColony) {
        e.write(dst);
    }

    dst << mForParent.size();
    for(const auto& e : mForParent) {
        e.write(dst);
    }
}

eWorldMap pakMapIdToMap(const uint8_t mapId) {
    if(mapId == 1) {
        return eWorldMap::greece1;
    } else if(mapId == 2) {
        return eWorldMap::greece2;
    } else if(mapId == 3) {
        return eWorldMap::greece3;
    } else if(mapId == 4) {
        return eWorldMap::greece4;
    } else if(mapId == 5) {
        return eWorldMap::greece5;
    } else if(mapId == 6) {
        return eWorldMap::greece6;
    } else if(mapId == 7) {
        return eWorldMap::greece7;
    } else if(mapId == 8 || mapId == 9 || mapId == 10) {
        return eWorldMap::greece8;
    } else if(mapId == 11) {
        return eWorldMap::poseidon1;
    } else if(mapId == 12) {
        return eWorldMap::poseidon2;
    } else if(mapId == 13) {
        return eWorldMap::poseidon3;
    } else if(mapId == 14) {
        return eWorldMap::poseidon4;
    }
    printf("Unknown pak map id %i\n", mapId);
    return eWorldMap::greece1;
}

eResourceType pakResourceByteToType(
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
    } else if(!newVersion && byte == 14) {
        return eResourceType::horse;
    } else if(!newVersion && byte == 15) {
        return eResourceType::armor;
    } else if(!newVersion && byte == 16) {
        return eResourceType::sculpture;
    } else if(!newVersion && byte == 17) {
        return eResourceType::oliveOil;
    } else if(!newVersion && byte == 18) {
        return eResourceType::wine;
    } else if(newVersion && byte == 16) {
        return eResourceType::blackMarble;
    } else if(newVersion && byte == 17) {
        return eResourceType::orichalc;
    } else if(newVersion && byte == 18) {
        return eResourceType::armor;
    } else if(newVersion && byte == 19) {
        return eResourceType::sculpture;
    } else if(newVersion && byte == 20) {
        return eResourceType::oliveOil;
    } else if(newVersion && byte == 21) {
        return eResourceType::wine;
    }

    printf("Invalid resource byte %i\n", byte);
    return eResourceType::none;
}

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
    valid = false;
    return eGodType::zeus;
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

void readEpisodeGoal(eEpisode& ep, ZeusFile& file) {
    const bool newVersion = file.isNewVersion();
    const uint8_t typeId = file.readUByte();
    file.skipBytes(3);
    const uint16_t value1 = file.readUShort();
    file.skipBytes(2);
    const uint16_t value2 = file.readUShort();
    file.skipBytes(2);
    const uint8_t value3 = file.readUByte();
    file.skipBytes(63);

    const auto goalType = pakIdToEpisodeGoalType(typeId);
    printf("%s %i %i %i\n", eEpisodeGoal::sText(goalType).c_str(),
           value1, value2, value3);
    const auto goal = std::make_shared<eEpisodeGoal>();
    goal->fType = goalType;
    switch(goalType) {
    case eEpisodeGoalType::population:
    case eEpisodeGoalType::treasury:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::sanctuary: {
        bool valid;
        const auto godType = pakIdToGodType(value1, valid);
        if(valid) goal->fEnumInt1 = static_cast<int>(godType);
        else goal->fRequiredCount = value3;
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
        const auto type = pakCityResourceByteToType(value1, newVersion);
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
        const auto type = pakResourceByteToType(value1, newVersion);
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

void eCampaign::readPak(const std::string& path) {
    ZeusFile file(path);
    file.readVersion();
    const bool newVersion = file.isNewVersion();
    file.readAtlantean();
    printf("v%i a%i\n", file.isNewVersion() ? 1 : 0, file.isAtlantean() ? 1 : 0);
    mParentBoard = e::make_shared<eGameBoard>();
    mParentBoard->setWorldBoard(&mWorldBoard);

    file.seek(0);
    file.getNumMaps();
    const auto cid = eCityId::city0;
    file.loadBoard(*mParentBoard, cid);

    file.seek(8);
    const uint8_t nParentEps = file.readUByte();
    file.seek(12); // and 16?
    const uint8_t nColonyEps = file.readUByte();
    const bool atlantean = file.isAtlantean();

    {
        const auto c = std::make_shared<eWorldCity>(
                           eCityType::parentCity,
                           cid, "Athens", 0.5, 0.5);
        mWorldBoard.addCity(c);
        mWorldBoard.moveCityToPlayer(cid, ePlayerId::player0);
        mWorldBoard.setPlayerTeam(ePlayerId::player0, eTeamId::team0);
        for(int i = 0; i < nColonyEps; i++) {
            const auto cid = static_cast<eCityId>(i + 1);
            const auto name = "Colony " + std::to_string(i);
            const auto c = std::make_shared<eWorldCity>(
                               eCityType::colony,
                               cid, name, i*0.2, 0.75);
            mWorldBoard.addCity(c);
            mWorldBoard.moveCityToPlayer(cid, ePlayerId::player0);
        }
    }
    {
        const auto c = mParentBoard->addCityToBoard(cid);
        c->setAtlantean(atlantean);
    }

    std::vector<ePakGod> friendlyGods;
    friendlyGods.resize(6);

    for(int i = 0; i < nParentEps; i++) {
        printf("parent episode %i:\n\n", i);

        const auto ep = addParentCityEpisode();
        file.seek(104 + i*356);
        const auto nextByte = file.readUByte();
        const bool nextColony = nextByte == 1;
        ep->fNextEpisode = nextColony ? eEpisodeType::colony :
                                        eEpisodeType::parentCity;
        file.seek(8174 + i*2032); // mint
        readEpisodeAllowedBuildings(*ep, file, atlantean, cid);
        file.seek(9100 + i*2032);
        readEpisodeResources(*ep, file, cid);

        const int epInc = newVersion ? 300 : 224;

        if(i == 0) {
            file.seek(35784 + i*epInc);
            for(int i = 0; i < 6; i++) {
                auto& v = friendlyGods[i];
                const uint8_t godId = file.readUByte();
                v.fType = pakIdToGodType(godId, v.fValid);
                file.skipBytes(3);
            }
        }

        file.seek(35944 + i*epInc);
        readEpisodeAllowedSanctuaries(*ep, file, friendlyGods, cid);

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
        const auto cid = static_cast<eCityId>(i + 1);
        auto& board = mColonyBoards.emplace_back();
        board = e::make_shared<eGameBoard>();
        board->setWorldBoard(&mWorldBoard);
        const bool r = file.loadBoard(*board, cid);
        if(r) {
            const auto c = board->addCityToBoard(cid);
            c->setAtlantean(atlantean);
        } else {
            board->initialize(1, 1);
            continue;
        }

        const auto ep = std::make_shared<eColonyEpisode>();
        ep->fBoard = board.get();
        ep->fWorldBoard = &mWorldBoard;
        ep->fCity = mWorldBoard.cityWithId(cid);
        mColonyEpisodes.push_back(ep);

        file.seek(28494 + i*2032); // mint
        readEpisodeAllowedBuildings(*ep, file, atlantean, cid);
        file.seek(29420 + i*2032);
        readEpisodeResources(*ep, file, cid);

        const int epInc = newVersion ? 300 : 224;

        if(newVersion) {
            file.seek(38944 + i*epInc);
        } else {
            file.seek(38184 + i*epInc);
        }
        readEpisodeAllowedSanctuaries(*ep, file, friendlyGods, cid);

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
}

bool eCampaign::load(const std::string& name) {
    mName = name;
    const auto baseDir = eGameDir::adventuresDir();
    const auto aDir = baseDir + mName + "/";

    const auto pakFile = aDir + mName + ".epak";
    std::ifstream file(pakFile, std::ios::in | std::ios::binary);
    if(!file) return false;
    eReadSource source(&file);
    eReadStream src(source);
    src.readFormat();
    const auto& format = src.format();
    const int version = src.formatVersion();
    if(format != "eZeus.epak") {
        printf("Invalid file '%s' format '%s', expected 'eZeus.epak'.\n",
               pakFile.c_str(), format.c_str());
        return false;
    }
    if(version > eFileFormat::version) {
        printf("Attempting to read '%s' format '%s' version '%i' newer than the executable.\n",
               pakFile.c_str(), format.c_str(), version);
    }
    read(src);
    src.handlePostFuncs();
    file.close();

    const auto txtFile = aDir + mName + ".txt";
    loadStrings();
    return true;
}

bool eCampaign::save() const {
    const auto baseDir = eGameDir::adventuresDir();
    const auto aDir = baseDir + mName + "/";
    std::filesystem::create_directories(aDir);
    const auto txtFile = aDir + mName + ".txt";
    if(!std::filesystem::exists(txtFile)) writeStrings(txtFile);

    const auto pakFile = aDir + mName + ".epak";
    std::ofstream file(pakFile, std::ios::out | std::ios::binary |
                       std::ios::trunc);
    if(!file) return false;
    eWriteTarget target(&file);
    eWriteStream dst(target);
    dst.writeFormat("eZeus.epak");
    write(dst);
    file.close();
    return true;
}

eEpisode* eCampaign::currentEpisode() const {
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        return mColonyEpisodes[mCurrentColonyEpisode].get();
    } else {
        return mParentCityEpisodes[mCurrentParentEpisode].get();
    }
}

void eCampaign::setCurrentColonyEpisode(const int e) {
    mCurrentColonyEpisode = e;
}

void eCampaign::startEpisode() {
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        mPlayedColonyEpisodes.push_back(mCurrentColonyEpisode);
        mWorldBoard.activateColony(mCurrentColonyEpisode);
        mWorldBoard.setColonyAsCurrentCity(mCurrentColonyEpisode);
    } else { // parentCity
        mWorldBoard.setParentAsCurrentCity();
    }
    const auto e = currentEpisode();
    e->fDrachmas = mDrachmas;
    e->fStartDate = mDate;
    e->fWageMultiplier = mWageMultiplier;
    e->fPrices = mPrices;
    const auto board = e->fBoard;
    const auto citiesOnBoard = board->citiesOnBoard();
    mWorldBoard.setCitiesOnBoard(citiesOnBoard);
    const bool pcol = mPreviousEpisodeType == eEpisodeType::colony;
    const auto col = pcol ? lastPlayedColony() : nullptr;
    board->startEpisode(e, col);
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        for(const auto& g : mForColony) {
            board->planGiftFrom(g.fFrom, g.fRes, g.fCount, 180);
        }
        mForColony.clear();
    } else {
        for(const auto& g : mForParent) {
            board->planGiftFrom(g.fFrom, g.fRes, g.fCount, 180);
        }
        mForParent.clear();
    }
}

void eCampaign::episodeFinished() {
    const auto e = currentEpisode();
    const auto board = e->fBoard;
    for(auto& d : mDrachmas) {
        d.second = board->drachmas(d.first);
    }
    mDate = board->date();
    mWageMultiplier = board->wageMultiplier();
    mPrices = board->prices();
    mPreviousEpisodeType = mCurrentEpisodeType;
    const auto& gls = e->fGoals;
    for(const auto& g : gls) {
        const auto type = g->fType;
        if(type != eEpisodeGoalType::setAsideGoods) continue;
        const auto res = static_cast<eResourceType>(g->fEnumInt1);
        const int count = g->fRequiredCount;
        const auto from = mWorldBoard.currentCity();
        setAside(res, count, from);
    }
    if(mCurrentEpisodeType == eEpisodeType::parentCity) {
        const auto ee = static_cast<eParentCityEpisode*>(e);
        auto n = ee->fNextEpisode;
        if(n == eEpisodeType::parentCity) {
            mCurrentParentEpisode++;
        } else {
            const auto rem  = remainingColonies();
            if(rem.size() == 0) {
                mCurrentParentEpisode++;
                n = eEpisodeType::parentCity;
            }
        }
        mCurrentEpisodeType = n;
    } else {
        mCurrentParentEpisode++;
        mCurrentEpisodeType = eEpisodeType::parentCity;
    }
}

bool eCampaign::finished() const {
    const int n = mParentCityEpisodes.size();
    return mCurrentParentEpisode >= n;
}

std::vector<eColonyEpisode*> eCampaign::remainingColonies() const {
    std::vector<eColonyEpisode*> result;
    for(int i = 0; i < 4; i++) {
        const bool p = eVectorHelpers::contains(mPlayedColonyEpisodes, i);
        if(p) continue;
        const auto& ep = mColonyEpisodes[i];
        if(!ep->fCity) continue;
        result.push_back(ep.get());
    }
    return result;
}

std::vector<int> eCampaign::colonyEpisodesLeft() const {
    std::vector<int> result;
    const int iMax = mColonyEpisodes.size();
    for(int i = 0; i < iMax; i++) {
        const bool p = eVectorHelpers::contains(mPlayedColonyEpisodes, i);
        if(p) continue;
        result.push_back(i);
    }
    return result;
}

stdsptr<eParentCityEpisode> eCampaign::addParentCityEpisode() {
    const auto e = std::make_shared<eParentCityEpisode>();
    e->fBoard = mParentBoard.get();
    e->fWorldBoard = &mWorldBoard;
    mParentCityEpisodes.push_back(e);
    return e;
}

stdsptr<eParentCityEpisode> eCampaign::insertParentCityEpisode(const int id) {
    const auto e = std::make_shared<eParentCityEpisode>();
    mParentCityEpisodes.insert(mParentCityEpisodes.begin() + id, e);
    return e;
}

void eCampaign::deleteParentCityEpisode(const int id) {
    mParentCityEpisodes.erase(mParentCityEpisodes.begin() + id);
}

void eCampaign::setVictoryParentCityEpisode(const int id) {
    for(int i = mParentCityEpisodes.size() - 1; i > id; i--) {
        deleteParentCityEpisode(id);
    }
}

void eCampaign::copyParentCityEpisodeSettings(const int from, const int to) {
    const auto f = mParentCityEpisodes[from];
    const auto t = mParentCityEpisodes[to];
    copyEpisodeSettings(f.get(), t.get());
}

void eCampaign::copyEpisodeSettings(eEpisode* const from,
                                    eEpisode* const to) {
    to->clear();
    const size_t size = 1000000;
    void* mem = malloc(size);
    {
        mWorldBoard.setIOIDs();
        eWriteTarget target(mem);
        eWriteStream dst(target);
        dst.writeFormat("eZeus");
        from->write(dst);
    }
    {
        eReadSource source(mem);
        eReadStream src(source);
        src.readFormat();
        to->read(src);
        src.handlePostFuncs();
    }

    free(mem);
}

void eCampaign::setDifficulty(const eDifficulty d) {
    mDifficulty = d;
    mParentBoard->setDifficulty(d);
    for(const auto& b : mColonyBoards) {
        b->setDifficulty(d);
    }
}

void eCampaign::setEditorMode(const bool e) {
    mWorldBoard.setEditorMode(e);
    mParentBoard->setEditorMode(e);
    for(const auto& c : mColonyBoards) {
        c->setEditorMode(e);
    }
}

void eCampaign::setAside(const eResourceType res, const int count,
                         const stdsptr<eWorldCity>& from) {
    eSetAside set;
    set.fRes = res;
    set.fCount = count;
    set.fFrom = from;
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        mForParent.push_back(set);
    } else {
        mForColony.push_back(set);
    }
}

stdsptr<eWorldCity> eCampaign::lastPlayedColony() const {
    if(mPlayedColonyEpisodes.empty()) return nullptr;
    const int i = mPlayedColonyEpisodes.back();
    return mColonyEpisodes[i]->fCity;
}

bool eCampaign::colonyEpisodeFinished(const int id) const {
    const bool current = mCurrentColonyEpisode == id &&
                         mCurrentEpisodeType == eEpisodeType::colony;
    if(current) return false;
    const bool finished = eVectorHelpers::contains(
                              mPlayedColonyEpisodes, id);
    return finished;
}

void eCampaign::copyColonyEpisodeSettings(const int from, const int to) {
    const auto f = mColonyEpisodes[from];
    const auto t = mColonyEpisodes[to];
    copyEpisodeSettings(f.get(), t.get());
}
