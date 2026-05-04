#include "erepair.h"

#include "egamewidget.h"

#include "buildings/allbuildings.h"
#include "buildings/eruins.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "audio/esounds.h"

#include "evectorhelpers.h"
#include "ebuildingstoerase.h"
#include "engine/edifficulty.h"

#include "fileIO/ebuildingreader.h"
#include "fileIO/ereadstream.h"

static const std::set<eBuildingType> sRepairableTypes = {
    eBuildingType::commonHouse,
    eBuildingType::maintenanceOffice,
    eBuildingType::wheatFarm,
    eBuildingType::carrotsFarm,
    eBuildingType::growersLodge,
    eBuildingType::onionsFarm,
    eBuildingType::dairy,
    eBuildingType::cardingShed,
    eBuildingType::huntingLodge,
    eBuildingType::corral,
    eBuildingType::mint,
    eBuildingType::foundry,
    eBuildingType::timberMill,
    eBuildingType::masonryShop,
    eBuildingType::refinery,
    eBuildingType::blackMarbleWorkshop,
    eBuildingType::winery,
    eBuildingType::olivePress,
    eBuildingType::sculptureStudio,
    eBuildingType::artisansGuild,
    eBuildingType::armory,
    eBuildingType::horseRanch,
    eBuildingType::horseRanchEnclosure,
    eBuildingType::chariotFactory,
    eBuildingType::wall,
    eBuildingType::tower,
    eBuildingType::gatehouse,
    eBuildingType::podium,
    eBuildingType::college,
    eBuildingType::theater,
    eBuildingType::dramaSchool,
    eBuildingType::gymnasium,
    eBuildingType::bibliotheke,
    eBuildingType::observatory,
    eBuildingType::university,
    eBuildingType::laboratory,
    eBuildingType::inventorsWorkshop,
    eBuildingType::museum,
    eBuildingType::granary,
    eBuildingType::warehouse,
    eBuildingType::tradePost,
    eBuildingType::pier,
    eBuildingType::fishery,
    eBuildingType::urchinQuay,
    eBuildingType::triremeWharf,
    eBuildingType::fountain,
    eBuildingType::hospital,
    eBuildingType::watchPost,
    eBuildingType::taxOffice,
    eBuildingType::stadium,
    eBuildingType::park,
    eBuildingType::doricColumn,
    eBuildingType::ionicColumn,
    eBuildingType::corinthianColumn,
    eBuildingType::avenue,
    eBuildingType::bench,
    eBuildingType::flowerGarden,
    eBuildingType::gazebo,
    eBuildingType::hedgeMaze,
    eBuildingType::fishPond,
    eBuildingType::waterPark,
    eBuildingType::birdBath,
    eBuildingType::shortObelisk,
    eBuildingType::tallObelisk,
    eBuildingType::shellGarden,
    eBuildingType::sundial,
    eBuildingType::dolphinSculpture,
    eBuildingType::orrery,
    eBuildingType::spring,
    eBuildingType::topiary,
    eBuildingType::baths,
    eBuildingType::stoneCircle,
    eBuildingType::commonAgora,
    eBuildingType::grandAgora,
};

struct sRepairGroup {
    eBuildingType wasType;
    int ox, oy, ow, oh;
    std::vector<eRuins*> tiles;
    eRuins* originRuins = nullptr; // the one with snapshot
    int cost = 0;
};

static stdsptr<eBuilding> restoreFromSnapshot(
        const std::vector<uint8_t>& data, eGameBoard& board) {
    if(data.empty()) return nullptr;
    eReadSource source(const_cast<void*>(static_cast<const void*>(data.data())));
    eReadStream src(source);
    src.readFormat();
    eBuildingType type;
    src >> type;
    const auto b = eBuildingReader::sRead(board, type, src);
    src.handlePostFuncs();

    return b;
}

static std::vector<sRepairGroup> collectRepairGroups(
        eGameBoard& board, const ePlayerId ppid,
        const int minX, const int minY,
        const int maxX, const int maxY) {
    std::vector<sRepairGroup> groups;
    std::set<std::tuple<int,int,int,int>> processed;
    const auto diff = board.difficulty(ppid);

    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            const auto tile = board.tile(x, y);
            if(!tile) continue;
            const auto b = tile->underBuilding();
            if(!b || b->type() != eBuildingType::ruins) continue;
            const auto ruins = static_cast<eRuins*>(b);
            if(ruins->isOnFire()) continue;
            const auto wasType = ruins->wasType();
            if(!sRepairableTypes.count(wasType)) continue;

            const int ox = ruins->originX();
            const int oy = ruins->originY();
            const int ow = ruins->originW();
            const int oh = ruins->originH();
            auto key = std::make_tuple(ox, oy, ow, oh);
            if(processed.count(key)) continue;
            processed.insert(key);

            sRepairGroup g;
            g.wasType = wasType;
            g.ox = ox; g.oy = oy; g.ow = ow; g.oh = oh;
            bool canRepair = true;
            for(int rx = ox; rx < ox+ow && canRepair; rx++) {
                for(int ry = oy; ry < oy+oh && canRepair; ry++) {
                    const auto rt = board.tile(rx, ry);
                    if(!rt) { canRepair = false; break; }
                    if(!rt->characters().empty()) { canRepair = false; break; }
                    const auto rb = rt->underBuilding();
                    if(!rb || rb->type() != eBuildingType::ruins) {
                        const auto terrain = rt->terrain();
                        const bool buildable = static_cast<bool>(eTerrain::buildable & terrain);
                        if(buildable) { canRepair = false; break; }
                        continue;
                    }
                    const auto rr = static_cast<eRuins*>(rb);
                    if(rr->isOnFire()) { canRepair = false; break; }
                    if(rr->originX() != ox || rr->originY() != oy) { canRepair = false; break; }
                    g.tiles.push_back(rr);
                    if(rr->hasSavedBuilding()) g.originRuins = rr;
                }
            }
            if(!canRepair || g.tiles.empty()) continue;
            g.cost = eDifficultyHelpers::buildingCost(diff, wasType) * 1.10;
            groups.push_back(std::move(g));
        }
    }
    return groups;
}

void handleRepair(eGameBoard& board, eGameWidget* const widget,
                  const int minX, const int minY,
                  const int maxX, const int maxY,
                  const eCityId cid,
                  const bool editorMode) {
    const auto ppid = board.personPlayer();

    auto groups = collectRepairGroups(board, ppid, minX, minY, maxX, maxY);
    int totalCost = 0;
    for(const auto& g : groups) totalCost += g.cost;
    

    if(totalCost > 0) {
        const auto title = "Repair buildings";
        const auto text = "Repair will cost " + std::to_string(totalCost) + " drachmas (10% fee). Proceed?";
        const auto acceptA = [groups, totalCost, ppid, cid, &board, editorMode]() {
            for(const auto& g : groups) {
                bool hasUnit = false;
                for(int rx = g.ox; rx < g.ox + g.ow && !hasUnit; rx++)
                    for(int ry = g.oy; ry < g.oy + g.oh && !hasUnit; ry++) {
                        const auto rt = board.tile(rx, ry);
                        if(rt && !rt->characters().empty()) hasUnit = true;
                    }
                if(hasUnit) continue;

                //for(auto rr : g.tiles) rr->erase(); causes pier restore to die

                printf("Repairing %p %d\n",
                    (void*)g.originRuins,
                    g.originRuins ? g.originRuins->hasSavedBuilding() : 0);
                
                if(g.wasType == eBuildingType::commonHouse) {
                    for(auto rr : g.tiles) rr->erase();
                    const auto cid2 = board.tile(g.ox, g.oy) ? board.tile(g.ox, g.oy)->cityId() : cid;
                    board.buildBase(g.ox, g.oy, g.ox + g.ow - 1, g.oy + g.oh - 1,
                        [&board, cid2]() { return e::make_shared<eSmallHouse>(board, cid2); },
                        ppid, cid2, true, false, true);
                } else if(g.originRuins && g.originRuins->hasSavedBuilding()) {
                    stdsptr<eBuilding> pierRestored;
                    if(g.originRuins->hasSavedPier()) {
                        pierRestored = restoreFromSnapshot(g.originRuins->savedPier(), board);
                        if(!pierRestored) continue;
                    }

                    const auto restored = restoreFromSnapshot(g.originRuins->savedBuilding(), board);

                    if(!restored) {continue;}

                    if(pierRestored) {
                        const auto& pierRect = g.originRuins->savedPierRect();
                        pierRestored->setTileRect(pierRect);
                        eTile* ct = nullptr;
                        for(int px = pierRect.x; px < pierRect.x + pierRect.w; px++) {
                            for(int py = pierRect.y; py < pierRect.y + pierRect.h; py++) {
                                const auto pt = board.tile(px, py);
                                if(!pt) continue;
                                if(!ct) ct = pt;
                                pt->setUnderBuilding(pierRestored);
                                pierRestored->addUnderBuilding(pt);
                            }
                        }
                        if(ct) pierRestored->setCenterTile(ct);
                        if(const auto tp = dynamic_cast<eTradePost*>(restored.get()))
                            tp->setUnpackBuilding(pierRestored.get());
                        if(const auto pier = dynamic_cast<ePier*>(pierRestored.get()))
                            pier->setTradePost(restored.get());
                    }

                    // wire restored building to its land tiles
                    // const int maxX = g.ox + g.ow - 1;
                    // const int maxY = g.oy + g.oh - 1;
                    // const int cx = (g.ox + maxX) / 2;
                    // const int cy = (g.oy + maxY) / 2;
                    // const auto ct = board.tile(cx, cy);
                    // if(ct) restored->setCenterTile(ct);
                    // restored->setTileRect({g.ox, g.oy, g.ow, g.oh});
                    // for(int rx = g.ox; rx <= maxX; rx++) {
                    //     for(int ry = g.oy; ry <= maxY; ry++) {
                    //         const auto rt = board.tile(rx, ry);
                    //         if(!rt) continue;
                    //         rt->setUnderBuilding(restored);
                    //         restored->addUnderBuilding(rt);
                    //     }
                    // }
                }
            }

            if(!editorMode) board.incDrachmas(ppid, -totalCost, eFinanceTarget::construction);
            board.scheduleTerrainUpdate();
        };
        widget->showQuestion(title, text, acceptA);
    }
}
