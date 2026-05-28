#include "model-data.h"

#include "edifficulty.h"
#include "egamedir.h"

#include <cstdio>
#include <fstream>
#include <sstream>

ModelData& ModelData::instance() {
    static ModelData s;
    return s;
}

std::string ModelData::modelDir() {
    return eGameDir::exeDir() + "../../Model/";
}

int ModelData::diffIndex(eDifficulty d) {
    switch(d) {
    case eDifficulty::beginner: return 0;
    case eDifficulty::mortal:   return 1;
    case eDifficulty::hero:     return 2;
    case eDifficulty::titan:    return 3;
    case eDifficulty::olympian: return 4;
    }
    return 2;
}

static std::vector<std::string> splitCsv(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    for(char c : s) {
        if(c == ',') { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

static std::string trim(std::string s) {
    while(!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n')) s.pop_back();
    size_t i = 0;
    while(i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    return s.substr(i);
}

static bool parseRow(const std::string& line, std::string& name, std::vector<int>& nums) {
    const auto open = line.find('{');
    const auto close = line.find('}');
    if(open == std::string::npos || close == std::string::npos || close <= open) return false;
    const std::string prefix = line.substr(0, open);
    const auto parts = splitCsv(prefix);
    if(parts.size() < 2) return false;
    name = trim(parts[parts.size() - 2]);
    const std::string inner = line.substr(open + 1, close - open - 1);
    const auto innerParts = splitCsv(inner);
    nums.clear();
    for(auto& p : innerParts) {
        auto t = trim(p);
        if(t.empty()) continue;
        try { nums.push_back(std::stoi(t)); } catch(...) {}
    }
    return !name.empty();
}

bool ModelData::loadFigureFile(const std::string& path, DifficultySet& out) {
    std::ifstream f(path);
    if(!f.is_open()) return false;
    std::string line;
    bool inEnemies = false;
    while(std::getline(f, line)) {
        if(line.find("ALL ENEMIES") != std::string::npos) { inEnemies = true; continue; }
        if(line.find("End of model data") != std::string::npos) break;
        std::string name;
        std::vector<int> nums;
        if(!parseRow(line, name, nums)) continue;
        if(nums.size() < 9) continue;
        FigureStats s;
        s.type = nums[0];
        s.hp   = nums[1];
        s.att  = nums[2];
        s.arm  = nums[3];
        s.avsm = nums[4];
        s.ma   = nums[5];
        s.mr   = nums[6];
        s.mrf  = nums[7];
        s.spd  = nums.size() > 8 ? nums[8] : 0;
        s.frq  = nums.size() > 9 ? nums[9] : 0;
        if(inEnemies) out.enemies.push_back(s);
        else out.figures[name] = s;
    }
    return true;
}

bool ModelData::loadMonsterFile(const std::string& path, DifficultySet& out) {
    std::ifstream f(path);
    if(!f.is_open()) return false;
    std::string line;
    while(std::getline(f, line)) {
        const auto open = line.find('{');
        const auto close = line.find('}');
        if(open == std::string::npos || close == std::string::npos) continue;
        const auto firstComma = line.find(',');
        if(firstComma == std::string::npos) continue;
        std::string name = trim(line.substr(0, firstComma));
        const std::string inner = line.substr(open + 1, close - open - 1);
        const auto parts = splitCsv(inner);
        MonsterBehavior mb;
        int idx = 0;
        for(auto& p : parts) {
            auto t = trim(p);
            if(t.empty()) continue;
            try { mb.cols[idx++] = std::stoi(t); } catch(...) {}
            if(idx >= 9) break;
        }
        if(!name.empty()) out.monsters[name] = mb;
    }
    return true;
}

bool ModelData::load() {
    if(mLoaded) return true;
    const std::string dir = modelDir();
    const std::array<std::string, 5> figNames{
        "veryeasy", "easy", "normal", "hard", "impossible"
    };
    const std::array<std::string, 5> monNames{
        "Veryeasy", "Easy", "Normal", "Hard", "VeryHard"
    };
    bool ok = true;
    for(int i = 0; i < 5; i++) {
        const std::string fp = dir + "Figure_model_" + figNames[i] + ".txt";
        if(!loadFigureFile(fp, mSets[i])) {
            fprintf(stderr, "ModelData: missing %s\n", fp.c_str());
            ok = false;
        }
        // Monster behavior columns are not mapped yet.
        // Keep figure model loading independent until those fields are understood.
        // const std::string mp = dir + "monster_behavior_" + monNames[i] + ".txt";
        // loadMonsterFile(mp, mSets[i]);
    }
    mLoaded = ok;
    return ok;
}

const FigureStats* ModelData::figure(eDifficulty d, const std::string& name) const {
    const auto& set = mSets[diffIndex(d)];
    auto it = set.figures.find(name);
    if(it == set.figures.end()) return nullptr;
    return &it->second;
}

const FigureStats* ModelData::enemyFigure(eDifficulty d, const int id) const {
    const auto& set = mSets[diffIndex(d)];
    if(id < 0 || id >= static_cast<int>(set.enemies.size())) return nullptr;
    return &set.enemies[id];
}

const MonsterBehavior* ModelData::monster(eDifficulty d, const std::string& name) const {
    const auto& set = mSets[diffIndex(d)];
    auto it = set.monsters.find(name);
    if(it == set.monsters.end()) return nullptr;
    return &it->second;
}
