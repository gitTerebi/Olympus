#include "building-writer.h"

#include "buildings/allbuildings.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

void BuildingArchive::save(const eBuilding* const b,
                            eSaveArchive& ar) {
    const auto type = b->type();
    if(type == eBuildingType::palace) {
        const auto p = static_cast<const ePalace*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            bool rotated = p->rotated();
            it.field("cityId", cid);
            it.field("rotated", rotated);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::palaceTile) {
        const auto p = static_cast<const ePalaceTile*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            bool other = p->other();
            it.field("cityId", cid);
            it.field("other", other);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    {
        bool simple = false;
        switch(type) {
        case eBuildingType::commonHouse:
        case eBuildingType::eliteHousing:
        case eBuildingType::road:
        case eBuildingType::roadblock:
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
        case eBuildingType::taxOffice:
        case eBuildingType::mint:
        case eBuildingType::foundry:
        case eBuildingType::refinery:
        case eBuildingType::blackMarbleWorkshop:
        case eBuildingType::timberMill:
        case eBuildingType::masonryShop:
        case eBuildingType::huntingLodge:
        case eBuildingType::corral:
        case eBuildingType::dairy:
        case eBuildingType::cardingShed:
        case eBuildingType::wheatFarm:
        case eBuildingType::onionsFarm:
        case eBuildingType::carrotsFarm:
        case eBuildingType::granary:
        case eBuildingType::warehouse:
        case eBuildingType::wall:
        case eBuildingType::tower:
        case eBuildingType::armory:
        case eBuildingType::olivePress:
        case eBuildingType::winery:
        case eBuildingType::sculptureStudio:
        case eBuildingType::artisansGuild:
        case eBuildingType::chariotFactory:
        case eBuildingType::park:
        case eBuildingType::doricColumn:
        case eBuildingType::ionicColumn:
        case eBuildingType::corinthianColumn:
        case eBuildingType::avenue:
        case eBuildingType::bench:
        case eBuildingType::flowerGarden:
        case eBuildingType::gazebo:
        case eBuildingType::hedgeMaze:
        case eBuildingType::fishPond:
        case eBuildingType::waterPark:
        case eBuildingType::birdBath:
        case eBuildingType::shortObelisk:
        case eBuildingType::tallObelisk:
        case eBuildingType::shellGarden:
        case eBuildingType::sundial:
        case eBuildingType::dolphinSculpture:
        case eBuildingType::spring:
        case eBuildingType::orrery:
        case eBuildingType::topiary:
        case eBuildingType::baths:
        case eBuildingType::stoneCircle:
        case eBuildingType::oliveTree:
        case eBuildingType::vine:
        case eBuildingType::orangeTree:
        case eBuildingType::sheep:
        case eBuildingType::goat:
        case eBuildingType::cattle:
        case eBuildingType::growersLodge:
        case eBuildingType::orangeTendersLodge:
            simple = true;
            break;
        default:
            break;
        }
        if(simple) {
            ar.archiveField("factory", [&](eSaveArchive& it) {
                eCityId cid = b->cityId();
                it.field("cityId", cid);
            });
            ar.archiveField("state", [&](eSaveArchive& it) {
                const_cast<eBuilding*>(b)->serialize(it);
            });
            return;
        }
    }
    if(type == eBuildingType::stadium) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            bool rotated = static_cast<const eStadium*>(b)->rotated();
            it.field("cityId", cid);
            it.field("rotated", rotated);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::gatehouse) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            bool rotated = static_cast<const eGatehouse*>(b)->rotated();
            it.field("cityId", cid);
            it.field("rotated", rotated);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::pier) {
        const auto p = static_cast<const ePier*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eDiagonalOrientation o = p->orientation();
            it.field("cityId", cid);
            it.field("orientation", o);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::tradePost) {
        const auto tp = static_cast<const TradePost*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eTradePostType tpt = tp->tpType();
            eDiagonalOrientation o = tp->orientation();
            eCityId ctid = tp->city().cityId();
            it.field("cityId", cid);
            it.field("tpType", tpt);
            it.field("orientation", o);
            it.field("worldCityId", ctid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::horseRanch) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::horseRanchEnclosure) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::foodVendor ||
       type == eBuildingType::fleeceVendor ||
       type == eBuildingType::oilVendor ||
       type == eBuildingType::wineVendor ||
       type == eBuildingType::armsVendor ||
       type == eBuildingType::horseTrainer ||
       type == eBuildingType::chariotVendor) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::urchinQuay ||
       type == eBuildingType::fishery ||
       type == eBuildingType::triremeWharf) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            switch(type) {
            case eBuildingType::urchinQuay:
                o = static_cast<const eUrchinQuay*>(b)->orientation(); break;
            case eBuildingType::fishery:
                o = static_cast<const eFishery*>(b)->orientation(); break;
            case eBuildingType::triremeWharf:
                o = static_cast<const eTriremeWharf*>(b)->orientation(); break;
            default: break;
            }
            it.field("cityId", cid);
            it.field("orientation", o);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::commonAgora ||
       type == eBuildingType::grandAgora) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eAgoraOrientation o = (type == eBuildingType::commonAgora) ?
                static_cast<const eCommonAgora*>(b)->orientation() :
                static_cast<const eGrandAgora*>(b)->orientation();
            it.field("cityId", cid);
            it.field("orientation", o);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::agoraSpace) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        return;
    }
    if(type == eBuildingType::commemorative) {
        const auto p = static_cast<const eCommemorative*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            int id = p->id();
            it.field("cityId", cid);
            it.field("id", id);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::godMonument) {
        const auto gm = static_cast<const eGodMonument*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eGodType gtype = gm->god();
            eGodQuestId qid = gm->id();
            it.field("cityId", cid);
            it.field("god", gtype);
            it.field("questId", qid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::godMonumentTile) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(eBuilding::sHeroHall(type)) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(eBuilding::sPyramidBuilding(type) &&
       type != eBuildingType::pyramidWall &&
       type != eBuildingType::pyramidTop &&
       type != eBuildingType::pyramidTile &&
       type != eBuildingType::pyramidPart &&
       type != eBuildingType::pyramidAltar &&
       type != eBuildingType::pyramidStatue &&
       type != eBuildingType::pyramidMonument &&
       type != eBuildingType::pyramidTemple &&
       type != eBuildingType::pyramidObservatory &&
       type != eBuildingType::pyramidMuseum) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    {
        const bool isPyramidElem =
            type == eBuildingType::pyramidWall ||
            type == eBuildingType::pyramidTop ||
            type == eBuildingType::pyramidTile ||
            type == eBuildingType::pyramidPart ||
            type == eBuildingType::pyramidAltar ||
            type == eBuildingType::pyramidStatue ||
            type == eBuildingType::pyramidMonument ||
            type == eBuildingType::pyramidTemple ||
            type == eBuildingType::pyramidObservatory ||
            type == eBuildingType::pyramidMuseum;
        if(isPyramidElem) {
            const auto se = static_cast<const ePyramidElement*>(b);
            ar.archiveField("factory", [&](eSaveArchive& it) {
                eCityId cid = b->cityId();
                it.field("cityId", cid);
                int elevation = se->elevation();
                if(type == eBuildingType::pyramidWall) {
                    const auto s = static_cast<const ePyramidWall*>(b);
                    eOrientation o = s->orientation();
                    int special = s->special();
                    it.field("orientation", o);
                    it.field("elevation", elevation);
                    it.field("special", special);
                } else if(type == eBuildingType::pyramidTile) {
                    const auto s = static_cast<const ePyramidTile*>(b);
                    int subType = s->type();
                    it.field("elevation", elevation);
                    it.field("subType", subType);
                } else if(type == eBuildingType::pyramidStatue) {
                    const auto s = static_cast<const ePyramidStatue*>(b);
                    eGodType godType = s->type();
                    int gid = s->id();
                    it.field("elevation", elevation);
                    it.field("godType", godType);
                    it.field("godId", gid);
                } else if(type == eBuildingType::pyramidMonument) {
                    const auto s = static_cast<const ePyramidMonument*>(b);
                    eGodType godType = s->type();
                    int gid = s->id();
                    it.field("elevation", elevation);
                    it.field("godType", godType);
                    it.field("godId", gid);
                } else {
                    it.field("elevation", elevation);
                }
                std::vector<eSanctCost> costs = se->costs();
                it.arrayField("costs", costs, [](eSaveArchive& ia, eSanctCost& c) {
                    c.serialize(ia);
                });
            });
            ar.archiveField("state", [&](eSaveArchive& it) {
                const_cast<eBuilding*>(b)->serialize(it);
            });
            return;
        }
    }
    if(type == eBuildingType::ruins ||
       type == eBuildingType::placeholder ||
       type == eBuildingType::hippodromePiece) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(eBuilding::sSanctuaryBuilding(type) &&
       type != eBuildingType::templeStatue &&
       type != eBuildingType::templeMonument &&
       type != eBuildingType::templeAltar &&
       type != eBuildingType::temple &&
       type != eBuildingType::templeTile) {
        const auto s = static_cast<const eSanctuary*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            int sw = s->spanW();
            int sh = s->spanH();
            it.field("cityId", cid);
            it.field("spanW", sw);
            it.field("spanH", sh);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::templeStatue) {
        const auto s = static_cast<const eTempleStatueBuilding*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eGodType godType = s->godType();
            int id = s->id();
            it.field("cityId", cid);
            it.field("godType", godType);
            it.field("id", id);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::templeMonument) {
        const auto s = static_cast<const eTempleMonumentBuilding*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            eGodType godType = s->godType();
            int id = s->id();
            it.field("cityId", cid);
            it.field("godType", godType);
            it.field("id", id);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::templeAltar) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::temple) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            it.field("cityId", cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
    if(type == eBuildingType::templeTile) {
        const auto s = static_cast<const eTempleTileBuilding*>(b);
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid = b->cityId();
            int id = s->id();
            it.field("cityId", cid);
            it.field("id", id);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            const_cast<eBuilding*>(b)->serialize(it);
        });
        return;
    }
}
