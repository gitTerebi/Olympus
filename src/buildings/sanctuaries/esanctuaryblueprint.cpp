#include "esanctuaryblueprint.h"

#include <fstream>
#include <regex>

#include "game-dir.h"

eSanctBlueprints eSanctBlueprints::instance;

static int stairsId180(const int id) {
    if(id == 0) return 4;  if(id == 1) return 5;
    if(id == 2) return 6;  if(id == 3) return 7;
    if(id == 4) return 0;  if(id == 5) return 1;
    if(id == 6) return 2;  if(id == 7) return 3;
    if(id == 8) return 10; if(id == 9) return 11;
    if(id == 10) return 8; if(id == 11) return 9;
    return id;
}

static int statueId180(const int id) {
    if(id == 0) return 1; if(id == 1) return 3;
    if(id == 2) return 0; if(id == 3) return 2;
    return id;
}

static int templeId180(const int id) {
    if(id == 0) return 2; if(id == 1) return 3;
    if(id == 2) return 0; if(id == 3) return 1;
    return id;
}

eSanctEle sanctEleFlip180(eSanctEle ele, int bpW, int bpH) {
    if(ele.fType == eSanctEleType::stairs)
        ele.fId = stairsId180(ele.fId);
    else if(ele.fType == eSanctEleType::defaultStatue ||
            ele.fType == eSanctEleType::aphroditeStatue ||
            ele.fType == eSanctEleType::apolloStatue ||
            ele.fType == eSanctEleType::aresStatue ||
            ele.fType == eSanctEleType::artemisStatue ||
            ele.fType == eSanctEleType::athenaStatue ||
            ele.fType == eSanctEleType::atlasStatue ||
            ele.fType == eSanctEleType::demeterStatue ||
            ele.fType == eSanctEleType::dionysusStatue ||
            ele.fType == eSanctEleType::hadesStatue ||
            ele.fType == eSanctEleType::hephaestusStatue ||
            ele.fType == eSanctEleType::heraStatue ||
            ele.fType == eSanctEleType::hermesStatue ||
            ele.fType == eSanctEleType::poseidonStatue ||
            ele.fType == eSanctEleType::zeusStatue ||
            ele.fType == eSanctEleType::monument)
        ele.fId = statueId180(ele.fId);
    else if(ele.fType == eSanctEleType::sanctuary)
        ele.fId = templeId180(ele.fId);
    ele.fX = bpW - 1 - ele.fX;
    ele.fY = bpH - 1 - ele.fY;
    return ele;
}

bool loadLineBP(const int x,
                std::vector<eSanctEle>& bp,
                const std::string& line) {
    const std::string rgxStr = "([0-9]+)_([a-z]+)([0-9]*)";
    std::regex rgx(rgxStr);
    const auto begin =
            std::sregex_iterator(line.begin(), line.end(), rgx);
    const auto end = std::sregex_iterator();
    int y = 0;
    for(auto i = begin; i != end; i++) {
        const auto is = (*i).str();
        std::regex rgx(rgxStr);
        std::smatch matches;
        std::regex_search(is, matches, rgx);

        const auto altm = matches[1];
        const auto altStr = altm.str();
        const int alt = std::stoi(altStr);

        const auto codem = matches[2];
        const auto codeStr = codem.str();

        const auto idm = matches[3];
        auto idStr = idm.str();

        eSanctEle ele;
        ele.fA = alt;
        ele.fX = x;
        ele.fY = y++;

        if(codeStr == "s") {
            ele.fType = eSanctEleType::stairs;
        } else if(codeStr == "y") {
            ele.fType = eSanctEleType::defaultStatue;
        } else if(codeStr == "b") {
            ele.fType = eSanctEleType::sanctuary;
        } else if(codeStr == "m") {
            ele.fType = eSanctEleType::monument;
        } else if(codeStr == "w") {
            ele.fType = eSanctEleType::woman;
        } else if(codeStr == "a") {
            ele.fType = eSanctEleType::altar;
        } else if(codeStr == "c") {
            ele.fType = eSanctEleType::copper;
        } else if(codeStr == "ss") {
            ele.fType = eSanctEleType::silver;
        } else if(codeStr == "ot") {
            ele.fType = eSanctEleType::oliveTree;
        } else if(codeStr == "ort") {
            ele.fType = eSanctEleType::orangeTree;
        } else if(codeStr == "v") {
            ele.fType = eSanctEleType::vine;
        } else if(codeStr == "t") {
            ele.fType = eSanctEleType::tile;
        } else if(codeStr == "tw") {
            ele.fType = eSanctEleType::tile;
            ele.fWarrior = true;
        } else { // x
            ele.fType = eSanctEleType::none;
        }

        if(codeStr == "y" && idStr.size() > 1) {
            const auto godIdStr = idStr.substr(1, idStr.size() - 1);
            idStr = idStr.substr(0, 1);
            const int godId = std::stoi(godIdStr);
            const auto aphEle = eSanctEleType::aphroditeStatue;
            const int id0 = static_cast<int>(aphEle);
            const auto god = static_cast<eSanctEleType>(id0 + godId);
            ele.fType = god;
        }
        ele.fId = idStr.empty() ? 0 : std::stoi(idStr);

        bp.push_back(ele);
    }
    return true;
}

bool loadBP(eSanctBlueprint& bp,
            const std::string& path) {
    std::ifstream file(path);
    if(!file.good()) {
        printf("File missing %s\n", path.c_str());
        return false;
    }
    std::string str;
    int x = 0;
    int s = -1;
    while(std::getline(file, str)) {
        auto& lineBP = bp.fTiles.emplace_back();
        loadLineBP(x++, lineBP, str);
        const int lineBPs = lineBP.size();
        if(s == -1) {
            s = lineBPs;
        } else if(s != lineBPs) {
            printf("Invalid sanctuary %s\n", path.c_str());
            return false;
        }
    }
    if(bp.fTiles.empty()) {
        printf("Invalid sanctuary %s\n", path.c_str());
        return false;
    }
    bp.fW = bp.fTiles.size();
    bp.fH = s;
    return true;
}

eSanctEle rotate(const eSanctEle& src) {
    eSanctEle result = src;
    result.fX = src.fY;
    result.fY = src.fX;
    if(result.fType == eSanctEleType::stairs) {
        int& id = result.fId;
        if(id == 1) id = 7;
        else if(id == 5) id = 3;
        else if(id == 3) id = 5;
        else if(id == 7) id = 1;
        else if(id == 2) id = 6;
        else if(id == 4) id = 4;
        else if(id == 8) id = 8;
        else if(id == 11) id = 9;
        else if(id == 10) id = 10;
        else if(id == 9) id = 11;
        else if(id == 6) id = 2;
        else if(id == 0) id = 0;
    } else if(result.fType == eSanctEleType::defaultStatue ||
              result.fType == eSanctEleType::aphroditeStatue ||
              result.fType == eSanctEleType::apolloStatue ||
              result.fType == eSanctEleType::aresStatue ||
              result.fType == eSanctEleType::artemisStatue ||
              result.fType == eSanctEleType::athenaStatue ||
              result.fType == eSanctEleType::atlasStatue ||
              result.fType == eSanctEleType::demeterStatue ||
              result.fType == eSanctEleType::dionysusStatue ||
              result.fType == eSanctEleType::hadesStatue ||
              result.fType == eSanctEleType::hephaestusStatue ||
              result.fType == eSanctEleType::heraStatue ||
              result.fType == eSanctEleType::hermesStatue ||
              result.fType == eSanctEleType::poseidonStatue ||
              result.fType == eSanctEleType::zeusStatue ||
              result.fType == eSanctEleType::monument) {
        int& id = result.fId;
        if(id == 1) id = 2;
        else if(id == 2) id = 1;
        else if(id == 3) id = 0;
        else if(id == 0) id = 3;
    } else if(result.fType == eSanctEleType::sanctuary) {
        int& id = result.fId;
        if(id == 1) id = 0;
        else if(id == 0) id = 1;
        else if(id == 2) id = 3;
        else if(id == 3) id = 2;
    }

    return result;
}

eSanctBlueprint rotate(const eSanctBlueprint& src) {
    eSanctBlueprint result;
    result.fH = src.fW;
    result.fW = src.fH;
    for(int y = 0; y < src.fH; y++) {
        std::vector<eSanctEle> v;
        for(int x = 0; x < src.fW; x++) {
            const auto& row = src.fTiles[x];
            const auto& col = row[y];
            v.push_back(rotate(col));
        }
        result.fTiles.push_back(v);
    }
    return result;
}

void eSanctBlueprints::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;
    const std::string dir = GameDir::exeDir() + "../Sanctuaries/";

    loadBP(fZeusW, dir + "zeus.txt");
    fZeusH = rotate(fZeusW);
    fZeusW2 = rotate(fZeusH);
    fZeusH2 = rotate(fZeusW2);

    loadBP(fAresW, dir + "ares.txt");
    fAresH = rotate(fAresW);
    fAresW2 = rotate(fAresH);
    fAresH2 = rotate(fAresW2);

    loadBP(fAphroditeW, dir + "aphrodite.txt");
    fAphroditeH = rotate(fAphroditeW);
    fAphroditeW2 = rotate(fAphroditeH);
    fAphroditeH2 = rotate(fAphroditeW2);

    loadBP(fApolloW, dir + "apollo.txt");
    fApolloH = rotate(fApolloW);
    fApolloW2 = rotate(fApolloH);
    fApolloH2 = rotate(fApolloW2);

    loadBP(fAthenaW, dir + "athena.txt");
    fAthenaH = rotate(fAthenaW);
    fAthenaW2 = rotate(fAthenaH);
    fAthenaH2 = rotate(fAthenaW2);

    loadBP(fAtlasW, dir + "atlas.txt");
    fAtlasH = rotate(fAtlasW);
    fAtlasW2 = rotate(fAtlasH);
    fAtlasH2 = rotate(fAtlasW2);

    loadBP(fArtemisW, dir + "artemis.txt");
    fArtemisH = rotate(fArtemisW);
    fArtemisW2 = rotate(fArtemisH);
    fArtemisH2 = rotate(fArtemisW2);

    loadBP(fDemeterW, dir + "demeter.txt");
    fDemeterH = rotate(fDemeterW);
    fDemeterW2 = rotate(fDemeterH);
    fDemeterH2 = rotate(fDemeterW2);

    loadBP(fDionysusW, dir + "dionysus.txt");
    fDionysusH = rotate(fDionysusW);
    fDionysusW2 = rotate(fDionysusH);
    fDionysusH2 = rotate(fDionysusW2);

    loadBP(fHadesW, dir + "hades.txt");
    fHadesH = rotate(fHadesW);
    fHadesW2 = rotate(fHadesH);
    fHadesH2 = rotate(fHadesW2);

    loadBP(fHephaestusW, dir + "hephaestus.txt");
    fHephaestusH = rotate(fHephaestusW);
    fHephaestusW2 = rotate(fHephaestusH);
    fHephaestusH2 = rotate(fHephaestusW2);

    loadBP(fHeraW, dir + "hera.txt");
    fHeraH = rotate(fHeraW);
    fHeraW2 = rotate(fHeraH);
    fHeraH2 = rotate(fHeraW2);

    loadBP(fHermesW, dir + "hermes.txt");
    fHermesH = rotate(fHermesW);
    fHermesW2 = rotate(fHermesH);
    fHermesH2 = rotate(fHermesW2);

    loadBP(fPoseidonW, dir + "poseidon.txt");
    fPoseidonH = rotate(fPoseidonW);
    fPoseidonW2 = rotate(fPoseidonH);
    fPoseidonH2 = rotate(fPoseidonW2);
}

const eSanctBlueprint* eSanctBlueprints::sSanctuaryBlueprint(
        const eBuildingType type, const int rotateId) {
    const auto& i = eSanctBlueprints::instance;
    const int r = rotateId % 4;
#define SANC_BP(W, H) \
    if(r == 0) return &i.W; \
    if(r == 1) return &i.H; \
    if(r == 2) return &i.W##2; \
    return &i.H##2;
    switch(type) {
    case eBuildingType::templeZeus:      { SANC_BP(fZeusW,      fZeusH)      } break;
    case eBuildingType::templeAres:      { SANC_BP(fAresW,      fAresH)      } break;
    case eBuildingType::templeAphrodite: { SANC_BP(fAphroditeW, fAphroditeH) } break;
    case eBuildingType::templeApollo:    { SANC_BP(fApolloW,    fApolloH)    } break;
    case eBuildingType::templeAthena:    { SANC_BP(fAthenaW,    fAthenaH)    } break;
    case eBuildingType::templeAtlas:     { SANC_BP(fAtlasW,     fAtlasH)     } break;
    case eBuildingType::templeArtemis:   { SANC_BP(fArtemisW,   fArtemisH)   } break;
    case eBuildingType::templeDemeter:   { SANC_BP(fDemeterW,   fDemeterH)   } break;
    case eBuildingType::templeDionysus:  { SANC_BP(fDionysusW,  fDionysusH)  } break;
    case eBuildingType::templeHades:     { SANC_BP(fHadesW,     fHadesH)     } break;
    case eBuildingType::templeHephaestus:{ SANC_BP(fHephaestusW,fHephaestusH)} break;
    case eBuildingType::templeHera:      { SANC_BP(fHeraW,      fHeraH)      } break;
    case eBuildingType::templeHermes:    { SANC_BP(fHermesW,    fHermesH)    } break;
    case eBuildingType::templePoseidon:  { SANC_BP(fPoseidonW,  fPoseidonH)  } break;
    default: return nullptr;
    }
#undef SANC_BP
    return nullptr;
}

void eSanctBlueprints::load() {
    instance.loadImpl();
}

bool eSanctBlueprints::loaded() {
    return instance.mLoaded;
}
