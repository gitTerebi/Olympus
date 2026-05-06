#include "estampblueprint.h"

#include "buildings/ebuilding.h"

#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

namespace {
bool typeFromString(const std::string& name, eBuildingType& type) {
#define E_STAMP_TYPE(TYPE) {#TYPE, eBuildingType::TYPE},
    static const std::unordered_map<std::string, eBuildingType> types = {
        E_STAMP_TYPE(none)
        E_STAMP_TYPE(erase)
        E_STAMP_TYPE(road)
        E_STAMP_TYPE(roadblock)
        E_STAMP_TYPE(commonHouse)
        E_STAMP_TYPE(eliteHousing)
        E_STAMP_TYPE(gymnasium)
        E_STAMP_TYPE(college)
        E_STAMP_TYPE(dramaSchool)
        E_STAMP_TYPE(podium)
        E_STAMP_TYPE(theater)
        E_STAMP_TYPE(stadium)
        E_STAMP_TYPE(bibliotheke)
        E_STAMP_TYPE(observatory)
        E_STAMP_TYPE(university)
        E_STAMP_TYPE(laboratory)
        E_STAMP_TYPE(inventorsWorkshop)
        E_STAMP_TYPE(museum)
        E_STAMP_TYPE(fountain)
        E_STAMP_TYPE(hospital)
        E_STAMP_TYPE(goat)
        E_STAMP_TYPE(sheep)
        E_STAMP_TYPE(cattle)
        E_STAMP_TYPE(oliveTree)
        E_STAMP_TYPE(vine)
        E_STAMP_TYPE(orangeTree)
        E_STAMP_TYPE(wheatFarm)
        E_STAMP_TYPE(carrotsFarm)
        E_STAMP_TYPE(onionsFarm)
        E_STAMP_TYPE(huntingLodge)
        E_STAMP_TYPE(fishery)
        E_STAMP_TYPE(urchinQuay)
        E_STAMP_TYPE(cardingShed)
        E_STAMP_TYPE(dairy)
        E_STAMP_TYPE(growersLodge)
        E_STAMP_TYPE(orangeTendersLodge)
        E_STAMP_TYPE(corral)
        E_STAMP_TYPE(granary)
        E_STAMP_TYPE(warehouse)
        E_STAMP_TYPE(tradePost)
        E_STAMP_TYPE(pier)
        E_STAMP_TYPE(commonAgora)
        E_STAMP_TYPE(grandAgora)
        E_STAMP_TYPE(agoraSpace)
        E_STAMP_TYPE(foodVendor)
        E_STAMP_TYPE(fleeceVendor)
        E_STAMP_TYPE(oilVendor)
        E_STAMP_TYPE(wineVendor)
        E_STAMP_TYPE(armsVendor)
        E_STAMP_TYPE(horseTrainer)
        E_STAMP_TYPE(chariotVendor)
        E_STAMP_TYPE(timberMill)
        E_STAMP_TYPE(masonryShop)
        E_STAMP_TYPE(mint)
        E_STAMP_TYPE(foundry)
        E_STAMP_TYPE(olivePress)
        E_STAMP_TYPE(winery)
        E_STAMP_TYPE(sculptureStudio)
        E_STAMP_TYPE(artisansGuild)
        E_STAMP_TYPE(wall)
        E_STAMP_TYPE(tower)
        E_STAMP_TYPE(gatehouse)
        E_STAMP_TYPE(triremeWharf)
        E_STAMP_TYPE(horseRanch)
        E_STAMP_TYPE(horseRanchEnclosure)
        E_STAMP_TYPE(chariotFactory)
        E_STAMP_TYPE(armory)
        E_STAMP_TYPE(maintenanceOffice)
        E_STAMP_TYPE(taxOffice)
        E_STAMP_TYPE(watchPost)
        E_STAMP_TYPE(palace)
        E_STAMP_TYPE(palaceTile)
        E_STAMP_TYPE(bridge)
        E_STAMP_TYPE(park)
        E_STAMP_TYPE(doricColumn)
        E_STAMP_TYPE(ionicColumn)
        E_STAMP_TYPE(corinthianColumn)
        E_STAMP_TYPE(avenue)
        E_STAMP_TYPE(commemorative)
        E_STAMP_TYPE(godMonument)
        E_STAMP_TYPE(godMonumentTile)
        E_STAMP_TYPE(bench)
        E_STAMP_TYPE(flowerGarden)
        E_STAMP_TYPE(gazebo)
        E_STAMP_TYPE(hedgeMaze)
        E_STAMP_TYPE(fishPond)
        E_STAMP_TYPE(waterPark)
        E_STAMP_TYPE(birdBath)
        E_STAMP_TYPE(shortObelisk)
        E_STAMP_TYPE(tallObelisk)
        E_STAMP_TYPE(shellGarden)
        E_STAMP_TYPE(sundial)
        E_STAMP_TYPE(dolphinSculpture)
        E_STAMP_TYPE(orrery)
        E_STAMP_TYPE(spring)
        E_STAMP_TYPE(topiary)
        E_STAMP_TYPE(baths)
        E_STAMP_TYPE(stoneCircle)
        E_STAMP_TYPE(templeAphrodite)
        E_STAMP_TYPE(templeApollo)
        E_STAMP_TYPE(templeAres)
        E_STAMP_TYPE(templeArtemis)
        E_STAMP_TYPE(templeAthena)
        E_STAMP_TYPE(templeAtlas)
        E_STAMP_TYPE(templeDemeter)
        E_STAMP_TYPE(templeDionysus)
        E_STAMP_TYPE(templeHades)
        E_STAMP_TYPE(templeHephaestus)
        E_STAMP_TYPE(templeHera)
        E_STAMP_TYPE(templeHermes)
        E_STAMP_TYPE(templePoseidon)
        E_STAMP_TYPE(templeZeus)
        E_STAMP_TYPE(temple)
        E_STAMP_TYPE(templeTile)
        E_STAMP_TYPE(templeStatue)
        E_STAMP_TYPE(templeMonument)
        E_STAMP_TYPE(templeAltar)
        E_STAMP_TYPE(achillesHall)
        E_STAMP_TYPE(atalantaHall)
        E_STAMP_TYPE(bellerophonHall)
        E_STAMP_TYPE(herculesHall)
        E_STAMP_TYPE(jasonHall)
        E_STAMP_TYPE(odysseusHall)
        E_STAMP_TYPE(perseusHall)
        E_STAMP_TYPE(theseusHall)
        E_STAMP_TYPE(ruins)
        E_STAMP_TYPE(placeholder)
        E_STAMP_TYPE(refinery)
        E_STAMP_TYPE(blackMarbleWorkshop)
        E_STAMP_TYPE(modestPyramid)
        E_STAMP_TYPE(pyramid)
        E_STAMP_TYPE(greatPyramid)
        E_STAMP_TYPE(majesticPyramid)
        E_STAMP_TYPE(smallMonumentToTheSky)
        E_STAMP_TYPE(monumentToTheSky)
        E_STAMP_TYPE(grandMonumentToTheSky)
        E_STAMP_TYPE(minorShrineAphrodite)
        E_STAMP_TYPE(minorShrineApollo)
        E_STAMP_TYPE(minorShrineAres)
        E_STAMP_TYPE(minorShrineArtemis)
        E_STAMP_TYPE(minorShrineAthena)
        E_STAMP_TYPE(minorShrineAtlas)
        E_STAMP_TYPE(minorShrineDemeter)
        E_STAMP_TYPE(minorShrineDionysus)
        E_STAMP_TYPE(minorShrineHades)
        E_STAMP_TYPE(minorShrineHephaestus)
        E_STAMP_TYPE(minorShrineHera)
        E_STAMP_TYPE(minorShrineHermes)
        E_STAMP_TYPE(minorShrinePoseidon)
        E_STAMP_TYPE(minorShrineZeus)
        E_STAMP_TYPE(shrineAphrodite)
        E_STAMP_TYPE(shrineApollo)
        E_STAMP_TYPE(shrineAres)
        E_STAMP_TYPE(shrineArtemis)
        E_STAMP_TYPE(shrineAthena)
        E_STAMP_TYPE(shrineAtlas)
        E_STAMP_TYPE(shrineDemeter)
        E_STAMP_TYPE(shrineDionysus)
        E_STAMP_TYPE(shrineHades)
        E_STAMP_TYPE(shrineHephaestus)
        E_STAMP_TYPE(shrineHera)
        E_STAMP_TYPE(shrineHermes)
        E_STAMP_TYPE(shrinePoseidon)
        E_STAMP_TYPE(shrineZeus)
        E_STAMP_TYPE(majorShrineAphrodite)
        E_STAMP_TYPE(majorShrineApollo)
        E_STAMP_TYPE(majorShrineAres)
        E_STAMP_TYPE(majorShrineArtemis)
        E_STAMP_TYPE(majorShrineAthena)
        E_STAMP_TYPE(majorShrineAtlas)
        E_STAMP_TYPE(majorShrineDemeter)
        E_STAMP_TYPE(majorShrineDionysus)
        E_STAMP_TYPE(majorShrineHades)
        E_STAMP_TYPE(majorShrineHephaestus)
        E_STAMP_TYPE(majorShrineHera)
        E_STAMP_TYPE(majorShrineHermes)
        E_STAMP_TYPE(majorShrinePoseidon)
        E_STAMP_TYPE(majorShrineZeus)
        E_STAMP_TYPE(pyramidOfThePantheon)
        E_STAMP_TYPE(altarOfOlympus)
        E_STAMP_TYPE(templeOfOlympus)
        E_STAMP_TYPE(observatoryKosmika)
        E_STAMP_TYPE(museumAtlantika)
        E_STAMP_TYPE(pyramidPart)
        E_STAMP_TYPE(pyramidWall)
        E_STAMP_TYPE(pyramidTop)
        E_STAMP_TYPE(pyramidStatue)
        E_STAMP_TYPE(pyramidMonument)
        E_STAMP_TYPE(pyramidTile)
        E_STAMP_TYPE(pyramidAltar)
        E_STAMP_TYPE(pyramidTemple)
        E_STAMP_TYPE(pyramidObservatory)
        E_STAMP_TYPE(pyramidMuseum)
        E_STAMP_TYPE(hippodromePiece)
        E_STAMP_TYPE(crosswalk)
    };
#undef E_STAMP_TYPE
    const auto it = types.find(name);
    if(it == types.end()) return false;
    type = it->second;
    return true;
}
}

bool eReadStampBlueprint(const std::string& path,
                         std::vector<eStampElement>& blueprint) {
    std::ifstream file(path);
    if(!file.good()) return false;
    blueprint.clear();

    const std::regex lineRegex(
        R"(eBuildingType::([A-Za-z0-9_]+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)(?:\s*,\s*(-?\d+))?)");
    std::string line;
    while(std::getline(file, line)) {
        const auto comment = line.find("//");
        if(comment != std::string::npos) line = line.substr(0, comment);
        const bool blank = line.find_first_not_of(" \t\r\n,{}") == std::string::npos;
        if(blank) continue;

        std::smatch match;
        if(!std::regex_search(line, match, lineRegex)) return false;
        eBuildingType type;
        if(!typeFromString(match[1].str(), type)) return false;
        const int dx = std::stoi(match[2].str());
        const int dy = std::stoi(match[3].str());
        const int id = match[4].matched ? std::stoi(match[4].str()) : -1;
        blueprint.push_back({type, dx, dy, id});
    }
    return true;
}
