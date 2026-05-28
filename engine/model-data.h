#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

enum class eDifficulty;

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

struct DifficultySet {
    std::unordered_map<std::string, FigureStats> figures;
    std::vector<FigureStats> enemies;
    std::unordered_map<std::string, MonsterBehavior> monsters;
};

class ModelData {
public:
    static ModelData& instance();

    bool load();
    bool loaded() const { return mLoaded; }

    const FigureStats* figure(eDifficulty d, const std::string& name) const;
    const FigureStats* enemyFigure(eDifficulty d, int id) const;
    const MonsterBehavior* monster(eDifficulty d, const std::string& name) const;

private:
    ModelData() = default;

    bool loadFigureFile(const std::string& path, DifficultySet& out);
    bool loadMonsterFile(const std::string& path, DifficultySet& out);

    static std::string modelDir();
    static int diffIndex(eDifficulty d);

    bool mLoaded = false;
    std::array<DifficultySet, 5> mSets;
};

#endif
