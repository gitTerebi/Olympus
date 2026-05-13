#include "ebuildingwriter.h"

#include "buildings/allbuildings.h"
#include "engine/e-game-board.h"

void writePyramidElementCost(
        const ePyramidElement* const e,
        eWriteStream& dst) {
    const auto& cost = e->costs();
    dst << cost.size();
    for(const auto& c : cost) {
        c.write(dst);
    }
}

void eBuildingWriter::sWrite(const eBuilding* const b,
                             eWriteStream& dst) {
    const auto cid = b->cityId();
    dst << cid;
    const auto type = b->type();
    switch(type) {
    case eBuildingType::road:
    case eBuildingType::roadblock:
        break;
    case eBuildingType::commonAgora: {
        const auto ca = static_cast<const eCommonAgora*>(b);
        dst << ca->orientation();
    } break;
    case eBuildingType::grandAgora: {
        const auto ga = static_cast<const eGrandAgora*>(b);
        dst << ga->orientation();
    } break;
    case eBuildingType::agoraSpace:
        return;
    case eBuildingType::commonHouse:
    case eBuildingType::gymnasium:
    case eBuildingType::podium:
    case eBuildingType::fountain:
    case eBuildingType::watchPost:
    case eBuildingType::maintenanceOffice:
    case eBuildingType::college:
    case eBuildingType::dramaSchool:
    case eBuildingType::theater:
    case eBuildingType::hospital:
    case eBuildingType::bibliotheke:
    case eBuildingType::observatory:
    case eBuildingType::university:
    case eBuildingType::laboratory:
    case eBuildingType::inventorsWorkshop:
    case eBuildingType::museum:
        break;
    case eBuildingType::stadium: {
        const auto s = static_cast<const eStadium*>(b);
        dst << s->rotated();
    } break;
    case eBuildingType::palace: {
        const auto p = static_cast<const ePalace*>(b);
        dst << p->rotated();
    } break;
    case eBuildingType::palaceTile: {
        const auto p = static_cast<const ePalaceTile*>(b);
        dst << p->other();
        dst.writeBuilding(p->palace());
    } break;
    case eBuildingType::eliteHousing:
    case eBuildingType::taxOffice:
    case eBuildingType::mint:
    case eBuildingType::foundry:
    case eBuildingType::timberMill:
    case eBuildingType::refinery:
    case eBuildingType::blackMarbleWorkshop:
    case eBuildingType::masonryShop:
    case eBuildingType::oliveTree:
    case eBuildingType::vine:
    case eBuildingType::orangeTree:
    case eBuildingType::huntingLodge:
    case eBuildingType::corral:
    case eBuildingType::chariotFactory:
        break;
    case eBuildingType::urchinQuay: {
        const auto p = static_cast<const eUrchinQuay*>(b);
        dst << p->orientation();
    } break;
    case eBuildingType::fishery: {
        const auto p = static_cast<const eFishery*>(b);
        dst << p->orientation();
    } break;
    case eBuildingType::triremeWharf: {
        const auto p = static_cast<const eTriremeWharf*>(b);
        dst << p->orientation();
    } break;
    case eBuildingType::pier: {
        const auto p = static_cast<const ePier*>(b);
        dst << p->orientation();
        const auto tp = p->tradePost();
        dst.writeBuilding(tp);
    } break;
    case eBuildingType::tradePost: {
        const auto tp = static_cast<const eTradePost*>(b);
        const auto tpt = tp->tpType();
        dst << tpt;
        dst << tp->orientation();
        const auto& c = tp->city();
        dst << c.cityId();

        if(tpt == eTradePostType::pier) {
            const auto ub = tp->unpackBuilding();
            dst.writeBuilding(ub);
        }
    } break;
    case eBuildingType::dairy:
    case eBuildingType::cardingShed:
    case eBuildingType::wheatFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::carrotsFarm:
    case eBuildingType::growersLodge:
    case eBuildingType::orangeTendersLodge:
    case eBuildingType::granary:
    case eBuildingType::warehouse:
    case eBuildingType::wall:
    case eBuildingType::tower:
        break;
    case eBuildingType::gatehouse: {
        const auto p = static_cast<const eGatehouse*>(b);
        dst << p->rotated();
    } break;
    case eBuildingType::armory:
        break;

    case eBuildingType::horseRanch: {
        const auto hr = static_cast<const eHorseRanch*>(b);
        const auto hre = hr->enclosure();
        dst.writeBuilding(hre);
    } break;
    case eBuildingType::horseRanchEnclosure: {
        const auto hre = static_cast<const eHorseRanchEnclosure*>(b);
        const auto hr = hre->ranch();
        dst.writeBuilding(hr);
    } break;

    case eBuildingType::olivePress:
    case eBuildingType::winery:
    case eBuildingType::sculptureStudio:
    case eBuildingType::artisansGuild:
        break;

    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::wineVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::horseTrainer:
    case eBuildingType::chariotVendor: {
        const auto v = static_cast<const eVendor*>(b);
        const auto a = v->agora();
        dst.writeBuilding(a);
        const int sid = v->agoraSpaceId();
        dst << sid;
    } break;

    case eBuildingType::park:
    case eBuildingType::doricColumn:
    case eBuildingType::ionicColumn:
    case eBuildingType::corinthianColumn:
    case eBuildingType::avenue:
        break;

    case eBuildingType::commemorative: {
        const auto p = static_cast<const eCommemorative*>(b);
        dst << p->id();
    } break;

    case eBuildingType::godMonument: {
        const auto gm = static_cast<const eGodMonument*>(b);
        dst << gm->god();
        dst << gm->id();
    } break;
    case eBuildingType::godMonumentTile: {
        const auto gmt = static_cast<const eGodMonumentTile*>(b);
        dst.writeBuilding(gmt->monument());
    } break;

    case eBuildingType::bench:
    case eBuildingType::flowerGarden:
    case eBuildingType::gazebo:
    case eBuildingType::hedgeMaze:
    case eBuildingType::fishPond:
    case eBuildingType::orrery:
        break;

    case eBuildingType::waterPark: {
        const auto p = static_cast<const eWaterPark*>(b);
        dst << p->id();
    } break;

    case eBuildingType::birdBath:
    case eBuildingType::shortObelisk:
    case eBuildingType::tallObelisk:
    case eBuildingType::shellGarden:
    case eBuildingType::sundial:
    case eBuildingType::dolphinSculpture:
    case eBuildingType::spring:
    case eBuildingType::topiary:
    case eBuildingType::baths:
    case eBuildingType::stoneCircle:
        break;

    case eBuildingType::templeAphrodite:
    case eBuildingType::templeApollo:
    case eBuildingType::templeAres:
    case eBuildingType::templeArtemis:
    case eBuildingType::templeAthena:
    case eBuildingType::templeAtlas:
    case eBuildingType::templeDemeter:
    case eBuildingType::templeDionysus:
    case eBuildingType::templeHades:
    case eBuildingType::templeHephaestus:
    case eBuildingType::templeHera:
    case eBuildingType::templeHermes:
    case eBuildingType::templePoseidon:
    case eBuildingType::templeZeus: {
        const auto s = static_cast<const eSanctuary*>(b);
        dst << s->spanW();
        dst << s->spanH();
    } break;
    case eBuildingType::templeStatue: {
        const auto s = static_cast<const eTempleStatueBuilding*>(b);
        dst << s->godType();
        dst << s->id();
        const auto sanct = s->monument();
        dst << sanct->ioID();
    } break;
    case eBuildingType::templeMonument: {
        const auto s = static_cast<const eTempleMonumentBuilding*>(b);
        dst << s->godType();
        dst << s->id();
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::templeAltar: {
        const auto s = static_cast<const eTempleAltarBuilding*>(b);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::temple: {
        const auto s = static_cast<const eTempleBuilding*>(b);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::templeTile: {
        const auto s = static_cast<const eTempleTileBuilding*>(b);
        dst << s->id();
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;

    case eBuildingType::modestPyramid:
    case eBuildingType::pyramid:
    case eBuildingType::greatPyramid:
    case eBuildingType::majesticPyramid:

    case eBuildingType::smallMonumentToTheSky:
    case eBuildingType::monumentToTheSky:
    case eBuildingType::grandMonumentToTheSky:

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

    case eBuildingType::pyramidOfThePantheon:
    case eBuildingType::altarOfOlympus:
    case eBuildingType::templeOfOlympus:
    case eBuildingType::observatoryKosmika:
    case eBuildingType::museumAtlantika:
        break;
    case eBuildingType::pyramidWall: {
        const auto s = static_cast<const ePyramidWall*>(b);
        dst << s->orientation();
        dst << s->elevation();
        dst << s->special();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidTop: {
        const auto s = static_cast<const ePyramidTop*>(b);
        dst << s->elevation();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidTile: {
        const auto s = static_cast<const ePyramidTile*>(b);
        dst << s->elevation();
        dst << s->type();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidPart: {
        const auto s = static_cast<const ePyramidBuildingPart*>(b);
        dst << s->elevation();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidAltar: {
        const auto s = static_cast<const ePyramidAltar*>(b);
        dst << s->elevation();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidStatue: {
        const auto s = static_cast<const ePyramidStatue*>(b);
        dst << s->elevation();
        dst << s->type();
        dst << s->id();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidMonument: {
        const auto s = static_cast<const ePyramidMonument*>(b);
        dst << s->elevation();
        dst << s->type();
        dst << s->id();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidTemple: {
        const auto s = static_cast<const ePyramidTemple*>(b);
        dst << s->elevation();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidObservatory: {
        const auto s = static_cast<const ePyramidObservatory*>(b);
        dst << s->elevation();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;
    case eBuildingType::pyramidMuseum: {
        const auto s = static_cast<const ePyramidMuseum*>(b);
        dst << s->elevation();
        writePyramidElementCost(s, dst);
        const auto sanct = s->monument();
        dst.writeBuilding(sanct);
    } break;

    case eBuildingType::ruins:
    case eBuildingType::placeholder:
        break;

    case eBuildingType::achillesHall:
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall:
        break;

    case eBuildingType::hippodromePiece:
        break;

    case eBuildingType::none:
    case eBuildingType::erase:
    case eBuildingType::bridge:
    case eBuildingType::crosswalk:
        return;
    }
    b->write(dst);
}

static void writePyramidElementCostJson(const ePyramidElement* e, eJsonArchive& ar) {
    const auto& costs = e->costs();
    int n = static_cast<int>(costs.size());
    ar.field("costCount", n);
    for(int i = 0; i < n; i++) {
        const auto& c = costs[i];
        const std::string p = "cost." + std::to_string(i) + ".";
        int wood = c.fWood, marble = c.fMarble, sculpt = c.fSculpture,
            oric = c.fOrichalc, bm = c.fBlackMarble;
        ar.field((p + "wood").c_str(), wood);
        ar.field((p + "marble").c_str(), marble);
        ar.field((p + "sculpture").c_str(), sculpt);
        ar.field((p + "orichalc").c_str(), oric);
        ar.field((p + "blackMarble").c_str(), bm);
    }
}

void eBuildingWriter::sWriteJson(const eBuilding* b, eJsonArchive& ar) {
    auto cid = b->cityId();
    ar.field("cityId", cid);
    const auto type = b->type();
    auto& board = const_cast<eBuilding*>(b)->getBoard();

    switch(type) {
    case eBuildingType::road:
    case eBuildingType::roadblock:
        break;
    case eBuildingType::commonAgora: {
        const auto ca = static_cast<const eCommonAgora*>(b);
        auto o = ca->orientation(); ar.field("orientation", o);
    } break;
    case eBuildingType::grandAgora: {
        const auto ga = static_cast<const eGrandAgora*>(b);
        auto o = ga->orientation(); ar.field("orientation", o);
    } break;
    case eBuildingType::agoraSpace:
        return;
    case eBuildingType::commonHouse:
    case eBuildingType::gymnasium:
    case eBuildingType::podium:
    case eBuildingType::fountain:
    case eBuildingType::watchPost:
    case eBuildingType::maintenanceOffice:
    case eBuildingType::college:
    case eBuildingType::dramaSchool:
    case eBuildingType::theater:
    case eBuildingType::hospital:
    case eBuildingType::bibliotheke:
    case eBuildingType::observatory:
    case eBuildingType::university:
    case eBuildingType::laboratory:
    case eBuildingType::inventorsWorkshop:
    case eBuildingType::museum:
        break;
    case eBuildingType::stadium: {
        const auto s = static_cast<const eStadium*>(b);
        auto r = s->rotated(); ar.field("rotated", r);
    } break;
    case eBuildingType::palace: {
        const auto p = static_cast<const ePalace*>(b);
        auto r = p->rotated(); ar.field("rotated", r);
    } break;
    case eBuildingType::palaceTile: {
        const auto p = static_cast<const ePalaceTile*>(b);
        auto o = p->other(); ar.field("other", o);
        eBuilding* palace = p->palace();
        ar.buildingRef("palace", palace, board);
    } break;
    case eBuildingType::eliteHousing:
    case eBuildingType::taxOffice:
    case eBuildingType::mint:
    case eBuildingType::foundry:
    case eBuildingType::timberMill:
    case eBuildingType::refinery:
    case eBuildingType::blackMarbleWorkshop:
    case eBuildingType::masonryShop:
    case eBuildingType::oliveTree:
    case eBuildingType::vine:
    case eBuildingType::orangeTree:
    case eBuildingType::huntingLodge:
    case eBuildingType::corral:
    case eBuildingType::chariotFactory:
        break;
    case eBuildingType::urchinQuay: {
        const auto p = static_cast<const eUrchinQuay*>(b);
        auto o = p->orientation(); ar.field("orientation", o);
    } break;
    case eBuildingType::fishery: {
        const auto p = static_cast<const eFishery*>(b);
        auto o = p->orientation(); ar.field("orientation", o);
    } break;
    case eBuildingType::triremeWharf: {
        const auto p = static_cast<const eTriremeWharf*>(b);
        auto o = p->orientation(); ar.field("orientation", o);
    } break;
    case eBuildingType::pier: {
        const auto p = static_cast<const ePier*>(b);
        auto o = p->orientation(); ar.field("orientation", o);
        eBuilding* tp = p->tradePost();
        ar.buildingRef("tradePost", tp, board);
    } break;
    case eBuildingType::tradePost: {
        const auto tp = static_cast<const eTradePost*>(b);
        auto tpt = tp->tpType(); ar.field("tpType", tpt);
        auto o = tp->orientation(); ar.field("orientation", o);
        auto ctid = tp->city().cityId(); ar.field("cityId2", ctid);
        if(tpt == eTradePostType::pier) {
            eBuilding* ub = tp->unpackBuilding();
            ar.buildingRef("unpackBuilding", ub, board);
        }
    } break;
    case eBuildingType::dairy:
    case eBuildingType::cardingShed:
    case eBuildingType::wheatFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::carrotsFarm:
    case eBuildingType::growersLodge:
    case eBuildingType::orangeTendersLodge:
    case eBuildingType::granary:
    case eBuildingType::warehouse:
    case eBuildingType::wall:
    case eBuildingType::tower:
        break;
    case eBuildingType::gatehouse: {
        const auto p = static_cast<const eGatehouse*>(b);
        auto r = p->rotated(); ar.field("rotated", r);
    } break;
    case eBuildingType::armory:
        break;
    case eBuildingType::horseRanch: {
        const auto hr = static_cast<const eHorseRanch*>(b);
        eBuilding* hre = hr->enclosure();
        ar.buildingRef("enclosure", hre, board);
    } break;
    case eBuildingType::horseRanchEnclosure: {
        const auto hre = static_cast<const eHorseRanchEnclosure*>(b);
        eBuilding* hr = hre->ranch();
        ar.buildingRef("ranch", hr, board);
    } break;
    case eBuildingType::olivePress:
    case eBuildingType::winery:
    case eBuildingType::sculptureStudio:
    case eBuildingType::artisansGuild:
        break;
    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::wineVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::horseTrainer:
    case eBuildingType::chariotVendor: {
        const auto v = static_cast<const eVendor*>(b);
        eBuilding* a = v->agora();
        ar.buildingRef("agora", a, board);
        int sid = v->agoraSpaceId(); ar.field("agoraSpaceId", sid);
    } break;
    case eBuildingType::park:
    case eBuildingType::doricColumn:
    case eBuildingType::ionicColumn:
    case eBuildingType::corinthianColumn:
    case eBuildingType::avenue:
        break;
    case eBuildingType::commemorative: {
        const auto p = static_cast<const eCommemorative*>(b);
        auto id = p->id(); ar.field("id", id);
    } break;
    case eBuildingType::godMonument: {
        const auto gm = static_cast<const eGodMonument*>(b);
        auto god = gm->god(); ar.field("god", god);
        auto id = gm->id(); ar.field("id", id);
    } break;
    case eBuildingType::godMonumentTile: {
        const auto gmt = static_cast<const eGodMonumentTile*>(b);
        eBuilding* mon = gmt->monument();
        ar.buildingRef("monument", mon, board);
    } break;
    case eBuildingType::bench:
    case eBuildingType::flowerGarden:
    case eBuildingType::gazebo:
    case eBuildingType::hedgeMaze:
    case eBuildingType::fishPond:
    case eBuildingType::orrery:
        break;
    case eBuildingType::waterPark: {
        const auto p = static_cast<const eWaterPark*>(b);
        auto id = p->id(); ar.field("id", id);
    } break;
    case eBuildingType::birdBath:
    case eBuildingType::shortObelisk:
    case eBuildingType::tallObelisk:
    case eBuildingType::shellGarden:
    case eBuildingType::sundial:
    case eBuildingType::dolphinSculpture:
    case eBuildingType::spring:
    case eBuildingType::topiary:
    case eBuildingType::baths:
    case eBuildingType::stoneCircle:
        break;
    case eBuildingType::templeAphrodite:
    case eBuildingType::templeApollo:
    case eBuildingType::templeAres:
    case eBuildingType::templeArtemis:
    case eBuildingType::templeAthena:
    case eBuildingType::templeAtlas:
    case eBuildingType::templeDemeter:
    case eBuildingType::templeDionysus:
    case eBuildingType::templeHades:
    case eBuildingType::templeHephaestus:
    case eBuildingType::templeHera:
    case eBuildingType::templeHermes:
    case eBuildingType::templePoseidon:
    case eBuildingType::templeZeus: {
        const auto s = static_cast<const eSanctuary*>(b);
        auto sw = s->spanW(); ar.field("spanW", sw);
        auto sh = s->spanH(); ar.field("spanH", sh);
    } break;
    case eBuildingType::templeStatue: {
        const auto s = static_cast<const eTempleStatueBuilding*>(b);
        auto gt = s->godType(); ar.field("godType", gt);
        auto id = s->id(); ar.field("id", id);
        int sanctIoid = s->monument() ? s->monument()->ioID() : -1;
        ar.field("sanctIoid", sanctIoid);
    } break;
    case eBuildingType::templeMonument: {
        const auto s = static_cast<const eTempleMonumentBuilding*>(b);
        auto gt = s->godType(); ar.field("godType", gt);
        auto id = s->id(); ar.field("id", id);
        eBuilding* sanct = s->monument();
        ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::templeAltar: {
        const auto s = static_cast<const eTempleAltarBuilding*>(b);
        eBuilding* sanct = s->monument();
        ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::temple: {
        const auto s = static_cast<const eTempleBuilding*>(b);
        eBuilding* sanct = s->monument();
        ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::templeTile: {
        const auto s = static_cast<const eTempleTileBuilding*>(b);
        auto id = s->id(); ar.field("id", id);
        eBuilding* sanct = s->monument();
        ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::modestPyramid:
    case eBuildingType::pyramid:
    case eBuildingType::greatPyramid:
    case eBuildingType::majesticPyramid:
    case eBuildingType::smallMonumentToTheSky:
    case eBuildingType::monumentToTheSky:
    case eBuildingType::grandMonumentToTheSky:
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
    case eBuildingType::pyramidOfThePantheon:
    case eBuildingType::altarOfOlympus:
    case eBuildingType::templeOfOlympus:
    case eBuildingType::observatoryKosmika:
    case eBuildingType::museumAtlantika:
        break;
    case eBuildingType::pyramidWall: {
        const auto s = static_cast<const ePyramidWall*>(b);
        auto o = s->orientation(); ar.field("orientation", o);
        auto elev = s->elevation(); ar.field("elevation", elev);
        auto spec = s->special(); ar.field("special", spec);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidTop: {
        const auto s = static_cast<const ePyramidTop*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidTile: {
        const auto s = static_cast<const ePyramidTile*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        auto pt = s->type(); ar.field("pyramidTileType", pt);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidPart: {
        const auto s = static_cast<const ePyramidBuildingPart*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidAltar: {
        const auto s = static_cast<const ePyramidAltar*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidStatue: {
        const auto s = static_cast<const ePyramidStatue*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        auto pt = s->type(); ar.field("pyramidStatueType", pt);
        auto id = s->id(); ar.field("id", id);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidMonument: {
        const auto s = static_cast<const ePyramidMonument*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        auto pt = s->type(); ar.field("pyramidMonumentType", pt);
        auto id = s->id(); ar.field("id", id);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidTemple: {
        const auto s = static_cast<const ePyramidTemple*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidObservatory: {
        const auto s = static_cast<const ePyramidObservatory*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::pyramidMuseum: {
        const auto s = static_cast<const ePyramidMuseum*>(b);
        auto elev = s->elevation(); ar.field("elevation", elev);
        writePyramidElementCostJson(s, ar);
        eBuilding* sanct = s->monument(); ar.buildingRef("monument", sanct, board);
    } break;
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::achillesHall:
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall:
    case eBuildingType::hippodromePiece:
        break;
    case eBuildingType::none:
    case eBuildingType::erase:
    case eBuildingType::bridge:
    case eBuildingType::crosswalk:
        return;
    }
    const_cast<eBuilding*>(b)->serializeJson(ar);
}
