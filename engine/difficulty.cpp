#include "difficulty.h"

#include <algorithm>

#include "characters/echaracterbase.h"
#include "model-data.h"

std::string DifficultyHelpers::name(const Difficulty diff)
{
    switch (diff)
    {
    case Difficulty::beginner:
        return "beginner";
    case Difficulty::mortal:
        return "mortal";
    case Difficulty::hero:
        return "hero";
    case Difficulty::titan:
        return "titan";
    case Difficulty::olympian:
        return "olympian";
    }
}

int costBase(const eBuildingType type)
{
    switch (type)
    {
    case eBuildingType::none:
        return 0;
    case eBuildingType::erase:
        return 1;
    case eBuildingType::road:
    case eBuildingType::roadblock:
        return 2;
    case eBuildingType::bridge:
        return 5;
    case eBuildingType::commonHouse:
        return 10;
    case eBuildingType::eliteHousing:
        return 100;

    case eBuildingType::vine:
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
        return 3;

    case eBuildingType::goat:
    case eBuildingType::sheep:
        return 4;
    case eBuildingType::cattle:
        return 8;

    case eBuildingType::carrotsFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::wheatFarm:
        return 20;
    case eBuildingType::cardingShed:
        return 16;
    case eBuildingType::dairy:
        return 16;
    case eBuildingType::fishery:
        return 30;
    case eBuildingType::growersLodge:
        return 25;
    case eBuildingType::orangeTendersLodge:
        return 20;
    case eBuildingType::huntingLodge:
        return 20;
    case eBuildingType::corral:
        return 75;
    case eBuildingType::urchinQuay:
        return 30;

    case eBuildingType::foundry:
        return 60;
    case eBuildingType::masonryShop:
        return 75;
    case eBuildingType::refinery:
        return 65;
    case eBuildingType::blackMarbleWorkshop:
        return 75;
    case eBuildingType::mint:
        return 100;
    case eBuildingType::olivePress:
        return 45;
    case eBuildingType::sculptureStudio:
        return 100;
    case eBuildingType::timberMill:
        return 35;
    case eBuildingType::winery:
        return 45;

    case eBuildingType::wall:
        return 2;
    case eBuildingType::gatehouse:
        return 20;
    case eBuildingType::tower:
        return 50;

    case eBuildingType::granary:
        return 50;
    case eBuildingType::warehouse:
        return 25;

    case eBuildingType::tradePost:
        return 100;
    case eBuildingType::pier:
        return 100;

    case eBuildingType::commonAgora:
        return 25;
    case eBuildingType::grandAgora:
        return 50;
    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::wineVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::horseTrainer:
    case eBuildingType::chariotVendor:
        return 10;

    case eBuildingType::fountain:
        return 16;
    case eBuildingType::hospital:
        return 35;
    case eBuildingType::maintenanceOffice:
        return 10;
    case eBuildingType::watchPost:
        return 20;

    case eBuildingType::palace:
        return 125;
    case eBuildingType::taxOffice:
        return 25;

    case eBuildingType::college:
        return 30;
    case eBuildingType::dramaSchool:
        return 16;
    case eBuildingType::gymnasium:
        return 30;
    case eBuildingType::stadium:
        return 200;
    case eBuildingType::podium:
        return 15;
    case eBuildingType::theater:
        return 60;

    case eBuildingType::bibliotheke:
        return 18;
    case eBuildingType::observatory:
        return 75;
    case eBuildingType::university:
        return 30;
    case eBuildingType::laboratory:
        return 65;
    case eBuildingType::inventorsWorkshop:
        return 40;
    case eBuildingType::museum:
        return 225;

    case eBuildingType::artisansGuild:
        return 35;

    case eBuildingType::armory:
        return 40;
    case eBuildingType::triremeWharf:
        return 75;
    case eBuildingType::horseRanch:
        return 100;

    case eBuildingType::park:
        return 6;
    case eBuildingType::doricColumn:
    case eBuildingType::ionicColumn:
    case eBuildingType::corinthianColumn:
        return 8;
    case eBuildingType::avenue:
        return 10;

    case eBuildingType::bench:
        return 6;
    case eBuildingType::flowerGarden:
        return 20;
    case eBuildingType::gazebo:
        return 24;
    case eBuildingType::hedgeMaze:
        return 40;
    case eBuildingType::fishPond:
        return 60;

    case eBuildingType::waterPark:
        return 16;

    case eBuildingType::birdBath:
        return 6;
    case eBuildingType::shortObelisk:
        return 10;
    case eBuildingType::tallObelisk:
        return 12;
    case eBuildingType::shellGarden:
        return 20;
    case eBuildingType::sundial:
        return 20;
    case eBuildingType::dolphinSculpture:
        return 40;
    case eBuildingType::orrery:
        return 40;
    case eBuildingType::spring:
        return 40;
    case eBuildingType::topiary:
        return 38;
    case eBuildingType::baths:
        return 60;
    case eBuildingType::stoneCircle:
        return 60;

    case eBuildingType::modestPyramid:
        return 0;
    case eBuildingType::pyramid:
        return 0;
    case eBuildingType::greatPyramid:
        return 0;
    case eBuildingType::majesticPyramid:
        return 0;

    case eBuildingType::smallMonumentToTheSky:
        return 0;
    case eBuildingType::monumentToTheSky:
        return 0;
    case eBuildingType::grandMonumentToTheSky:
        return 0;

    case eBuildingType::minorShrineAphrodite:
    case eBuildingType::minorShrineApollo:
    case eBuildingType::minorShrineAres:
    case eBuildingType::minorShrineArtemis:
    case eBuildingType::minorShrineAthena:
    case eBuildingType::minorShrineAtlas:
    case eBuildingType::minorShrineDemeter:
    case eBuildingType::minorShrineDionysus:
    case eBuildingType::minorShrineHades:
    case eBuildingType::minorShrineHephaestus:
    case eBuildingType::minorShrineHera:
    case eBuildingType::minorShrineHermes:
    case eBuildingType::minorShrinePoseidon:
    case eBuildingType::minorShrineZeus:
        return 0;
    case eBuildingType::shrineAphrodite:
    case eBuildingType::shrineApollo:
    case eBuildingType::shrineAres:
    case eBuildingType::shrineArtemis:
    case eBuildingType::shrineAthena:
    case eBuildingType::shrineAtlas:
    case eBuildingType::shrineDemeter:
    case eBuildingType::shrineDionysus:
    case eBuildingType::shrineHades:
    case eBuildingType::shrineHephaestus:
    case eBuildingType::shrineHera:
    case eBuildingType::shrineHermes:
    case eBuildingType::shrinePoseidon:
    case eBuildingType::shrineZeus:
        return 0;
    case eBuildingType::majorShrineAphrodite:
    case eBuildingType::majorShrineApollo:
    case eBuildingType::majorShrineAres:
    case eBuildingType::majorShrineArtemis:
    case eBuildingType::majorShrineAthena:
    case eBuildingType::majorShrineAtlas:
    case eBuildingType::majorShrineDemeter:
    case eBuildingType::majorShrineDionysus:
    case eBuildingType::majorShrineHades:
    case eBuildingType::majorShrineHephaestus:
    case eBuildingType::majorShrineHera:
    case eBuildingType::majorShrineHermes:
    case eBuildingType::majorShrinePoseidon:
    case eBuildingType::majorShrineZeus:
        return 0;

    case eBuildingType::pyramidOfThePantheon:
        return 0;
    case eBuildingType::altarOfOlympus:
        return 0;
    case eBuildingType::templeOfOlympus:
        return 0;
    case eBuildingType::observatoryKosmika:
        return 0;
    case eBuildingType::museumAtlantika:
        return 0;

    case eBuildingType::templeZeus:
        return 2920;
    case eBuildingType::templePoseidon:
        return 2480;
    case eBuildingType::templeHades:
        return 1320;
    case eBuildingType::templeHera:
        return 1760;
    case eBuildingType::templeDemeter:
        return 840;
    case eBuildingType::templeAthena:
        return 2160;
    case eBuildingType::templeArtemis:
        return 1520;
    case eBuildingType::templeApollo:
        return 920;
    case eBuildingType::templeAtlas:
        return 1760;
    case eBuildingType::templeAres:
        return 960;
    case eBuildingType::templeHephaestus:
        return 760;
    case eBuildingType::templeAphrodite:
        return 720;
    case eBuildingType::templeHermes:
        return 640;
    case eBuildingType::templeDionysus:
        return 400;

    case eBuildingType::chariotFactory:
        return 110;

    case eBuildingType::agoraSpace:
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::horseRanchEnclosure:
    case eBuildingType::commemorative:
    case eBuildingType::palaceTile:
    case eBuildingType::godMonument:
    case eBuildingType::godMonumentTile:

    case eBuildingType::temple:
    case eBuildingType::templeTile:
    case eBuildingType::templeStatue:
    case eBuildingType::templeMonument:
    case eBuildingType::templeAltar:

    case eBuildingType::achillesHall:
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall:

    case eBuildingType::pyramidPart:
    case eBuildingType::pyramidWall:
    case eBuildingType::pyramidTop:
    case eBuildingType::pyramidStatue:
    case eBuildingType::pyramidMonument:
    case eBuildingType::pyramidTile:
    case eBuildingType::pyramidAltar:
    case eBuildingType::pyramidTemple:
    case eBuildingType::pyramidObservatory:
    case eBuildingType::pyramidMuseum:
        return 0;

    case eBuildingType::hippodromePiece:
        return 30;
    case eBuildingType::crosswalk:
        return 25;
    }
}

double DifficultyHelpers::costMultiplier(const Difficulty diff)
{
    switch (diff)
    {
    case Difficulty::beginner:
        return 1.0;
    case Difficulty::mortal:
        return 1.5;
    case Difficulty::hero:
        return 2.0;
    case Difficulty::titan:
        return 2.5;
    case Difficulty::olympian:
        return 3.0;
    }
    return 1;
}

int DifficultyHelpers::buildingCost(
    const Difficulty diff,
    const eBuildingType type)
{
    const int base = costBase(type);
    const int min = static_cast<int>(eBuildingType::templeAphrodite);
    const int max = static_cast<int>(eBuildingType::templeZeus);
    const int bi = static_cast<int>(type);
    if (bi >= min && bi <= max)
        return base;
    const double mult = DifficultyHelpers::costMultiplier(diff);
    return std::round(mult * base);
}

int fireRiskBase(const eBuildingType type)
{
    switch (type)
    {
    case eBuildingType::commonHouse:
        return 8;
    case eBuildingType::eliteHousing:
        return 8;

    case eBuildingType::carrotsFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::wheatFarm:
        return 5;
    case eBuildingType::cardingShed:
        return 5;
    case eBuildingType::dairy:
        return 5;
    case eBuildingType::fishery:
        return 5;
    case eBuildingType::growersLodge:
        return 5;
    case eBuildingType::orangeTendersLodge:
        return 8;
    case eBuildingType::huntingLodge:
        return 8;
    case eBuildingType::corral:
        return 8;
    case eBuildingType::urchinQuay:
        return 5;

    case eBuildingType::sculptureStudio:
        return 1;
    case eBuildingType::timberMill:
        return 10;

    case eBuildingType::granary:
        return 8;
    case eBuildingType::warehouse:
        return 5;

    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::wineVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::horseTrainer:
    case eBuildingType::chariotVendor:
        return 6;

    case eBuildingType::bibliotheke:
        return 5;
    case eBuildingType::inventorsWorkshop:
    case eBuildingType::laboratory:
        return 8;
    case eBuildingType::museum:
        return 4;

    case eBuildingType::hospital:
        return 5;
    case eBuildingType::maintenanceOffice:
        return 3;

    case eBuildingType::palace:
        return 3;
    case eBuildingType::taxOffice:
        return 5;

    case eBuildingType::artisansGuild:
        return 5;

    case eBuildingType::armory:
        return 8;
    case eBuildingType::triremeWharf:
        return 5;
    case eBuildingType::horseRanch:
        return 5;
    case eBuildingType::chariotFactory:
        return 6;

    default:
        return 0;
    }
}

double fireRiskMultiplier(const Difficulty diff)
{
    switch (diff)
    {
    case Difficulty::beginner:
        return 1.0;
    case Difficulty::mortal:
        return 1.5;
    case Difficulty::hero:
        return 1.875;
    case Difficulty::titan:
        return 2.25;
    case Difficulty::olympian:
        return 2.5;
    }
    return 1;
}

int DifficultyHelpers::fireRisk(
    const Difficulty diff,
    const eBuildingType type)
{
    const double mult = fireRiskMultiplier(diff);
    const int base = fireRiskBase(type);
    return std::round(mult * base);
}

int DifficultyHelpers::houseDiseaseRisk(const Difficulty diff,
                                         const int level, const bool elite)
{
    const auto r = ModelData::instance().houseReq(diff, level, elite);
    if (!r) return 0;
    return r->diseaseRisk;
}

int DifficultyHelpers::houseCrimeRisk(const Difficulty diff,
                                       const int level, const bool elite)
{
    const auto r = ModelData::instance().houseReq(diff, level, elite);
    if (!r) return 0;
    return r->crimeBase + r->crimeInc;
}

int damageRiskBase(const eBuildingType type)
{
    switch (type)
    {
    case eBuildingType::carrotsFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::wheatFarm:
        return 5;

    case eBuildingType::fishery:
        return 5;
    case eBuildingType::huntingLodge:
        return 5;
    case eBuildingType::corral:
        return 5;
    case eBuildingType::urchinQuay:
        return 5;

    case eBuildingType::foundry:
        return 10;
    case eBuildingType::masonryShop:
        return 10;
    case eBuildingType::refinery:
        return 10;
    case eBuildingType::blackMarbleWorkshop:
        return 10;
    case eBuildingType::mint:
        return 10;
    case eBuildingType::olivePress:
        return 8;
    case eBuildingType::sculptureStudio:
        return 2;
    case eBuildingType::winery:
        return 5;

    case eBuildingType::granary:
        return 8;

    case eBuildingType::fountain:
        return 8;
    case eBuildingType::hospital:
        return 5;
    case eBuildingType::watchPost:
        return 5;

    case eBuildingType::palace:
        return 5;

    case eBuildingType::college:
        return 6;
    case eBuildingType::dramaSchool:
        return 8;
    case eBuildingType::gymnasium:
        return 5;
    case eBuildingType::stadium:
        return 8;
    case eBuildingType::podium:
        return 5;
    case eBuildingType::theater:
        return 8;

    case eBuildingType::museum:
        return 8;
    case eBuildingType::university:
        return 6;
    case eBuildingType::observatory:
        return 8;

    case eBuildingType::triremeWharf:
        return 5;
    case eBuildingType::chariotFactory:
        return 6;

    default:
        return 0;
    }
}

double damageRiskMultiplier(const Difficulty diff)
{
    switch (diff)
    {
    case Difficulty::beginner:
        return 1.0;
    case Difficulty::mortal:
        return 1.25;
    case Difficulty::hero:
        return 1.5;
    case Difficulty::titan:
        return 2.0;
    case Difficulty::olympian:
        return 2.5;
    }
    return 1;
}

int DifficultyHelpers::damageRisk(
    const Difficulty diff,
    const eBuildingType type)
{
    const double mult = damageRiskMultiplier(diff);
    const int base = damageRiskBase(type);
    return std::round(mult * base);
}

int DifficultyHelpers::taxMultiplier(
    const Difficulty diff,
    const eBuildingType type,
    const int level)
{
    const bool elite = (type == eBuildingType::eliteHousing);
    if(type != eBuildingType::commonHouse && !elite) return 0;
    const int lvl = elite ? std::clamp(level, 0, 3) : std::clamp(level, 0, 6);
    if(const auto r = ModelData::instance().houseReq(diff, lvl, elite)) {
        return r->taxMult;
    }
    return 0;
}

int DifficultyHelpers::taxSentiment(
    const Difficulty diff,
    const eTaxRate taxRate)
{
    const int id = static_cast<int>(taxRate);
    switch (diff)
    {
    case Difficulty::beginner:
    {
        const int vals[]{7, 5, 3, 2, 0, -1, -2};
        return vals[id];
    }
    case Difficulty::mortal:
    {
        const int vals[]{4, 3, 1, 0, 0, -2, -4};
        return vals[id];
    }
    case Difficulty::hero:
    {
        const int vals[]{3, 2, 1, 0, -1, -3, -5};
        return vals[id];
    }
    case Difficulty::titan:
    {
        const int vals[]{2, 1, 0, 0, -2, -4, -6};
        return vals[id];
    }
    case Difficulty::olympian:
    {
        const int vals[]{1, 1, 0, 0, -3, -5, -7};
        return vals[id];
    }
    }
}

double DifficultyHelpers::workerFrac(
    const Difficulty diff, const eWageRate wageRate)
{
    switch (wageRate)
    {
    case eWageRate::none:
    {
        switch (diff)
        {
        case Difficulty::beginner:
            return 0.42;
        case Difficulty::mortal:
            return 0.37;
        case Difficulty::hero:
            return 0.32;
        case Difficulty::titan:
            return 0.29;
        case Difficulty::olympian:
            return 0.27;
        }
    }
    case eWageRate::veryLow:
    {
        switch (diff)
        {
        case Difficulty::beginner:
            return 0.46;
        case Difficulty::mortal:
            return 0.41;
        case Difficulty::hero:
            return 0.36;
        case Difficulty::titan:
            return 0.33;
        case Difficulty::olympian:
            return 0.31;
        }
    }
    case eWageRate::low:
    {
        switch (diff)
        {
        case Difficulty::beginner:
            return 0.49;
        case Difficulty::mortal:
            return 0.44;
        case Difficulty::hero:
            return 0.39;
        case Difficulty::titan:
            return 0.36;
        case Difficulty::olympian:
            return 0.34;
        }
    }
    case eWageRate::normal:
    {
        switch (diff)
        {
        case Difficulty::beginner:
            return 0.52;
        case Difficulty::mortal:
            return 0.47;
        case Difficulty::hero:
            return 0.42;
        case Difficulty::titan:
            return 0.39;
        case Difficulty::olympian:
            return 0.37;
        }
    }
    case eWageRate::high:
    {
        switch (diff)
        {
        case Difficulty::beginner:
            return 0.55;
        case Difficulty::mortal:
            return 0.50;
        case Difficulty::hero:
            return 0.45;
        case Difficulty::titan:
            return 0.42;
        case Difficulty::olympian:
            return 0.4;
        }
    }
    case eWageRate::veryHigh:
    {
        switch (diff)
        {
        case Difficulty::beginner:
            return 0.57;
        case Difficulty::mortal:
            return 0.52;
        case Difficulty::hero:
            return 0.47;
        case Difficulty::titan:
            return 0.44;
        case Difficulty::olympian:
            return 0.42;
        }
    }
    }
    return 0;
}

double soliderBribeMultiplier(const Difficulty diff)
{
    switch (diff)
    {
    case Difficulty::beginner:
        return 0.25;
    case Difficulty::mortal:
        return 0.5;
    case Difficulty::hero:
        return 1;
    case Difficulty::titan:
        return 1.25;
    case Difficulty::olympian:
        return 1.5;
    }
    return 1;
}

int DifficultyHelpers::soliderBribe(const Difficulty diff,
                                     const eCharacterType type)
{
    const double mult = soliderBribeMultiplier(diff);
    int base = 0;
    switch (type)
    {
    case eCharacterType::rockThrower:
        base = 25;
        break;
    case eCharacterType::hoplite:
        base = 25;
        break;
    case eCharacterType::horseman:
        base = 50;
        break;
    default:
        return 25;
    }
    return static_cast<int>(mult * base);
}

DifficultyHelpers::eHouseLevelReq
DifficultyHelpers::houseLevelReq(const Difficulty diff,
                                  const bool elite,
                                  const int level)
{
    if(const auto mr = ModelData::instance().houseReq(diff, level, elite)) {
        eHouseLevelReq r;
        r.fAppD = mr->desLow;
        r.fAppE = mr->desHigh;
        r.fEnt  = mr->culture;
        return r;
    }
    return {0, 0, 0};
}
