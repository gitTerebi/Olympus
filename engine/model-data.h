#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

enum class Difficulty;

struct FigureStats {
    int type = 0;
    int hp = 0;
    int att = 0;
    int arm = 0;
    int avsm = 0;
    int ma = 0;
    int mr = 0;
    int mrf = 0;
    int spd = 0;
    int frq = 0;
};

struct MonsterBehavior {
    std::array<int, 9> cols{};
};

struct HouseReq {
    int desLow = 0;     // col a: devolve below
    int desHigh = 0;    // col b: evolve above
    int culture = 0;    // col c: culture pts to evolve
    int horses = 0;     // col h: horses needed
    int wine = 0;       // col l: wine needed
    int armor = 0;      // col m: armor needed
    int crimeInc = 0;    // col o: crime risk increment
    int crimeBase = 0;   // col p: crime risk base
    int diseaseRisk = 0; // col u: disease risk increment
    int capacity = 0;   // col r: population capacity
    int taxMult = 0;    // col s: tax rate mult
    bool valid = false;
};

struct BuildingModelStats {
    int cost = 0;
    int desirability = 0;
    int desirabilityStepTiles = 0;
    int desirabilityStepSize = 0;
    int desirabilityRange = 0;
    int employment = 0;
    int fireRisk = 0;
    int damageRisk = 0;
    int resourceUsed = 0;
    int riskReducer = 0;
};

struct DifficultySet {
    std::unordered_map<std::string, FigureStats> figures;
    std::vector<FigureStats> enemies;
    std::unordered_map<std::string, MonsterBehavior> monsters;
    std::unordered_map<std::string, BuildingModelStats> buildings;
    std::array<HouseReq, 7> commonHouses{};   // Hut..Townhouse
    std::array<HouseReq, 4> eliteHouses{};    // Residence..Estate
};

class ModelData {
public:
    static ModelData& instance();

    bool load();
    bool loaded() const { return mLoaded; }

    const FigureStats* figure(Difficulty d, const std::string& name) const;
    const FigureStats* enemyFigure(Difficulty d, int id) const;
    const MonsterBehavior* monster(Difficulty d, const std::string& name) const;
    const BuildingModelStats* building(Difficulty d, const std::string& name) const;
    const HouseReq* houseReq(Difficulty d, int level, bool elite) const;

private:
    ModelData() = default;

    bool loadFigureFile(const std::string& path, DifficultySet& out);
    bool loadMonsterFile(const std::string& path, DifficultySet& out);
    bool loadZeusFile(const std::string& path, DifficultySet& out);

    static std::string modelDir();
    static int diffIndex(Difficulty d);

    bool mLoaded = false;
    std::array<DifficultySet, 5> mSets;
};

#endif
