#include "ecampaign.h"

#include <fstream>

#include <filesystem>

#include "evectorhelpers.h"
#include "egamedir.h"
#include "elanguage.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

namespace {
const int currentSaveVersion = 1;
}

eCampaign::eCampaign() {
    const auto types = eResourceTypeHelpers::extractResourceTypes(
                           eResourceType::allBasic);
    for(const auto type : types) {
        mPrices[type] = eResourceTypeHelpers::defaultPrice(type);
    }
}

void eCampaign::initialize(const std::string& name) {
    mName = name;

    mParentBoard = e::make_shared<eGameBoard>(mWorldBoard);
    mParentBoard->initialize(100, 100);

    for(int i = 0; i < 4; i++) {
        auto& board = mColonyBoards.emplace_back();
        board = e::make_shared<eGameBoard>(mWorldBoard);
        board->initialize(1, 1);

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

int eCampaign::audioFilesId() const {
    if(mBriefId == 493) { // hercules labors
        return 1;
    } else if(mBriefId == 494) { // the voyages of jason
        return 2;
    } else if(mBriefId == 495) { // perseus and medusa
        return 3;
    } else if(mBriefId == 496) { // the trojan war
        return 4;
    } else if(mBriefId == 497) { // athens through the ages
        return 5;
    } else if(mBriefId == 498) { // zeus and europa
        return 6;
    } else if(mBriefId == 499) { // the peloponnesian war
        return 7;
    }
    return 0;
}

std::string eCampaign::audioFilesBasePath() const {
    {
        auto name = mPakFilename;
        if(name.size() > 4) {
            const auto baseDir = eGameDir::path("Audio/Voice/Campaign/");
            name = name.substr(0, name.length() - 4);
            const auto basePath = baseDir + name + "_";
            std::ifstream file(basePath + "A_v.mp3");
            if(file.good()) return basePath;
        }
    }
    {
        const auto baseDir = mIsPak ? eGameDir::pakAdventuresDir() :
                                      eGameDir::adventuresDir();
        const auto aDir = baseDir + mName + "/";
        const auto basePath = aDir + mName + "_";
        std::ifstream file(basePath + "A_v.mp3");
        if(file.good()) return basePath;
    }
    return "";
}

std::string eCampaign::currentEpisodeAudioFilePath(const bool intro) const {
    const int id = audioFilesId();
    if(id) {
        const auto baseDir = eGameDir::path("Audio/Voice/Campaign/");
        int subId;
        if(mCurrentEpisodeType == eEpisodeType::colony) {
            subId = 11;
        } else {
            subId = 1;
        }
        subId += currentEpisodeId();
        auto path = baseDir + "C" + std::to_string(id) +
                    "_E" + std::to_string(subId);
        if(intro) path += "_i.mp3";
        else path += "_v.mp3";
        return path;
    } else {
        auto path = audioFilesBasePath();
        if(mCurrentEpisodeType == eEpisodeType::colony) {
            path += "C";
        } else {
            path += "P";
        }
        const int n = currentEpisodeId();
        path += std::to_string(n + 1);
        if(intro) path += "_i.mp3";
        else path += "_v.mp3";
        return path;
    }
}

std::string eCampaign::adventureVictoryAudioFilePath() const {
    const int id = audioFilesId();
    if(id) {
        const auto baseDir = eGameDir::path("Audio/Voice/Campaign/");
        const auto path = baseDir + "C" + std::to_string(id) + "_v.mp3";
        return path;
    } else {
        auto path = audioFilesBasePath();
        path += "A_v.mp3";
        return path;
    }
}

bool eCampaign::sLoadStrings(const std::string& path, eMap& map) {
    std::ifstream file(path);
    if(!file.good()) {
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

        size_t valueStart = 0;
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
            map[key] = value;
            key = "";
            value = "";
        }
    }
    return true;
}

bool eCampaign::loadStrings() {
    const auto baseDir = mIsPak ? eGameDir::pakAdventuresDir() :
                                  eGameDir::adventuresDir();
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
        if(i - 1 < iMax) {
            e = mParentCityEpisodes[i - 1].get();
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
        if(i - 1 < iMax) {
            e = mColonyEpisodes[i - 1].get();
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

void eCampaign::loadNumbers() {
    const auto baseDir = mIsPak ? eGameDir::pakAdventuresDir() :
                                  eGameDir::adventuresDir();
    const auto aDir = baseDir + mName + "/";
    const auto numFile = aDir + "numbers.txt";
    std::ifstream file(numFile);
    if(file.good()) {
        eNumbers::sLoad(numFile);
    } else {
        eNumbers::sLoad();
    }
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
    if(file) {
        eReadSource source(&file);
        eReadStream src(source);
        src.readFormat();
        eSaveArchive ar(src);
        ar.field("bitmap", glossary.fBitmap);
        file.close();
    }
    return true;
}

void eCampaign::serialize(eSaveArchive& ar) {
    ar.field("bitmap", mBitmap);
    ar.field("isPak", mIsPak);
    if(mIsPak) ar.field("pakFilename", mPakFilename);

    if(ar.reading()) {
        std::string name;
        ar.field("name", name);
        if(mName.empty()) mName = name;
    } else {
        ar.field("name", mName);
    }

    ar.field("currentParentEpisode", mCurrentParentEpisode);
    ar.field("currentColonyEpisode", mCurrentColonyEpisode);
    ar.field("currentEpisodeType", mCurrentEpisodeType);

    // drachmas map<ePlayerId, int>
    {
        int nc = static_cast<int>(mDrachmas.size());
        ar.field("drachmasCount", nc);
        if(ar.reading()) {
            for(int i = 0; i < nc; i++) {
                ePlayerId pid;
                int val = 0;
                ar.archiveField(("drachmas." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("playerId", pid);
                        itemAr.field("drachmas", val);
                    });
                mDrachmas[pid] = val;
            }
        } else {
            int i = 0;
            for(auto& d : mDrachmas) {
                ePlayerId pid = d.first;
                int val = d.second;
                ar.archiveField(("drachmas." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("playerId", pid);
                        itemAr.field("drachmas", val);
                    });
            }
        }
    }

    ar.dateField("date", mDate);

    for(auto& p : mPrices) {
        ar.field(("price." + std::to_string(static_cast<int>(p.first))).c_str(),
                 p.second);
    }

    ar.field("difficulty", mDifficulty);

    ar.archiveField("worldBoard",
        [this](eSaveArchive& itemAr) { mWorldBoard.serialize(itemAr); });

    if(ar.reading()) {
        mParentBoard = e::make_shared<eGameBoard>(mWorldBoard);
    }
    ar.archiveField("parentBoard",
        [this](eSaveArchive& itemAr) { mParentBoard->serialize(itemAr); });

    ar.arrayField("playedColonyEpisodes", mPlayedColonyEpisodes,
        [](eSaveArchive& itemAr, int& v) { itemAr.field("episode", v); });

    // colony boards — finished ones not written, placeholder kept
    {
        int nc = static_cast<int>(mColonyBoards.size());
        ar.field("colonyBoards.count", nc);
        if(ar.reading()) mColonyBoards.resize(nc);
        for(int i = 0; i < nc; i++) {
            const bool finished = colonyEpisodeFinished(i);
            if(ar.reading() && !mColonyBoards[i]) {
                mColonyBoards[i] = e::make_shared<eGameBoard>(mWorldBoard);
            }
            if(finished) continue;
            ar.archiveField(("colonyBoard." + std::to_string(i)).c_str(),
                [this, i](eSaveArchive& itemAr) { mColonyBoards[i]->serialize(itemAr); });
        }
    }

    ar.arrayField("parentCityEpisodes", mParentCityEpisodes,
        [this](eSaveArchive& itemAr, stdsptr<eParentCityEpisode>& e) {
            if(itemAr.reading() && !e) {
                e = std::make_shared<eParentCityEpisode>();
                e->fBoard = mParentBoard.get();
                e->fWorldBoard = &mWorldBoard;
            }
            e->serialize(itemAr);
        });

    // colonyEpisodes — need index for fBoard wiring on read
    {
        int nc = static_cast<int>(mColonyEpisodes.size());
        ar.field("colonyEpisodes.count", nc);
        if(ar.reading()) mColonyEpisodes.resize(nc);
        for(int i = 0; i < nc; i++) {
            if(ar.reading() && !mColonyEpisodes[i]) {
                mColonyEpisodes[i] = std::make_shared<eColonyEpisode>();
                mColonyEpisodes[i]->fBoard = mColonyBoards[i].get();
                mColonyEpisodes[i]->fWorldBoard = &mWorldBoard;
            }
            ar.archiveField(("colonyEpisode." + std::to_string(i)).c_str(),
                [this, i](eSaveArchive& itemAr) {
                    mColonyEpisodes[i]->serialize(itemAr);
                });
        }
    }

    ar.arrayField("forColony", mForColony,
        [this](eSaveArchive& itemAr, stdsptr<eSetAside>& s) {
            if(itemAr.reading() && !s) s = std::make_shared<eSetAside>();
            s->serialize(itemAr, &mWorldBoard);
        });

    ar.arrayField("forParent", mForParent,
        [this](eSaveArchive& itemAr, stdsptr<eSetAside>& s) {
            if(itemAr.reading() && !s) s = std::make_shared<eSetAside>();
            s->serialize(itemAr, &mWorldBoard);
        });

    ar.field("briefId", mBriefId);
    ar.field("completeId", mCompleteId);
}


bool eCampaign::load(const std::string& name) {
    mName = name;
    const auto baseDir = eGameDir::adventuresDir();
    const auto aDir = baseDir + mName + "/";

    const auto pakFile = aDir + mName + ".epak";
    std::ifstream file(pakFile, std::ios::in | std::ios::binary);
    if(!file) {
        initialize(name);
        loadStrings();
        loadNumbers();
        return true;
    }

    eReadSource source(&file);
    eReadStream src(source);
    src.readFormat();
    const auto& format = src.format();
    if(format != "eZeus.epak") {
        printf("Invalid file '%s' format '%s', expected 'eZeus.epak'.\n",
               pakFile.c_str(), format.c_str());
        return false;
    }
    eSaveArchive ar(src);
    serialize(ar);
    src.handlePostFuncs();
    file.close();

    loadStrings();
    loadNumbers();
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
    eSaveArchive ar(dst);
    const_cast<eCampaign*>(this)->serialize(ar);
    file.close();
    return true;
}

eEpisode* eCampaign::currentEpisode() const {
    if(!hasCurrentEpisode()) return nullptr;
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        return mColonyEpisodes[mCurrentColonyEpisode].get();
    } else {
        return mParentCityEpisodes[mCurrentParentEpisode].get();
    }
}

bool eCampaign::hasCurrentEpisode() const {
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        return mCurrentColonyEpisode >= 0 &&
               mCurrentColonyEpisode < int(mColonyEpisodes.size());
    } else {
        return mCurrentParentEpisode >= 0 &&
               mCurrentParentEpisode < int(mParentCityEpisodes.size());
    }
}

void eCampaign::printCurrentEpisodeDebug() const {
    printf("Campaign episode state: type=%d parent=%d/%d colony=%d/%d.\n",
           static_cast<int>(mCurrentEpisodeType),
           mCurrentParentEpisode, int(mParentCityEpisodes.size()),
           mCurrentColonyEpisode, int(mColonyEpisodes.size()));
}

void eCampaign::setCurrentColonyEpisode(const int e) {
    mCurrentColonyEpisode = e;
}

void eCampaign::setCurrentParentEpisode(const int e) {
    mCurrentEpisodeType = eEpisodeType::parentCity;
    mCurrentParentEpisode = e;
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
            board->planGiftFrom(g->fFrom, g->fRes, g->fCount, 180);
        }
        mForColony.clear();
    } else {
        for(const auto& g : mForParent) {
            board->planGiftFrom(g->fFrom, g->fRes, g->fCount, 180);
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

int eCampaign::currentEpisodeId() const {
    if(mCurrentEpisodeType == eEpisodeType::colony) {
        return mCurrentColonyEpisode;
    } else {
        return mCurrentParentEpisode;
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
    std::vector<char> mem;
    {
        mWorldBoard.setIOIDs();
        eWriteTarget target(&mem);
        eWriteStream dst(target);
        dst.writeFormat("eZeus");
        eSaveArchive ar(dst);
        from->serialize(ar);
    }
    {
        eReadSource source(mem.data());
        eReadStream src(source);
        src.readFormat();
        eSaveArchive ar(src);
        to->serialize(ar);
        src.handlePostFuncs();
    }
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
    const auto set = std::make_shared<eSetAside>();
    set->fRes = res;
    set->fCount = count;
    set->fFrom = from;
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
