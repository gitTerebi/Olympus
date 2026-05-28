#include "repair.h"

#include "game-widget.h"

#include "buildings/allbuildings.h"
#include "buildings/eruins.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "audio/sounds.h"

#include "evectorhelpers.h"
#include "ebuildingstoerase.h"
#include "engine/edifficulty.h"

#include "fileIO/building-reader.h"
#include "fileIO/ereadstream.h"
#include "fileIO/esavearchive.h"

#include <algorithm>

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

struct sRepairGroup
{
    eBuildingType wasType;
    int ox, oy, ow, oh;
    std::vector<eRuins *> tiles;
    eRuins *originRuins = nullptr;
    int cost = 0;
};

static std::vector<stdsptr<eBuilding>> restoreFromBundle(
    const std::vector<uint8_t> &data, eGameBoard &board)
{
    std::vector<stdsptr<eBuilding>> buildings;
    if (data.empty())
        return buildings;

    eReadSource source(const_cast<void *>(static_cast<const void *>(data.data())));
    eReadStream src(source);
    src.readFormat();
    std::vector<std::pair<eBuilding *, int>> oldBuildingIds;
    for (const auto b : board.buildings())
    {
        if (!b)
            continue;
        oldBuildingIds.push_back({b, b->ioID()});
        b->setIOID(-1);
    }
    std::vector<std::pair<eCharacter *, int>> oldCharacterIds;
    for (const auto c : board.characters())
    {
        if (!c)
            continue;
        oldCharacterIds.push_back({c, c->ioID()});
        c->setIOID(-1);
    }
    eSaveArchive ar(src);
    int marker;
    ar.field("bundleMarker", marker);
    if (marker == -1)
    {
        int n;
        ar.field("buildingCount", n);
        for (int i = 0; i < n; i++)
        {
            int size;
            ar.field("snapshotSize", size);
            if (size == 0 || size > data.size())
                continue;
            std::vector<uint8_t> snapshot;
            snapshot.reserve(size);
            for (size_t j = 0; j < size; j++)
            {
                int byte;
                ar.field("snapshotByte", byte);
                snapshot.push_back(static_cast<uint8_t>(byte));
            }
            eReadSource bs(const_cast<void *>(
                static_cast<const void *>(snapshot.data())));
            eReadStream bsrc(bs);
            bsrc.readFormat();
            eBuildingType type;
            eSaveArchive bar(bsrc);
            bar.field("buildingType", type);
            const auto b = BuildingArchive::load(board, type, bar);
            if (!b)
                continue;
            buildings.push_back(b);
        }
        src.handlePostFuncs();
    }
    for (const auto &b : buildings)
        b->setIOID(-1);
    for (const auto &p : oldBuildingIds)
        p.first->setIOID(p.second);
    for (const auto &p : oldCharacterIds)
        p.first->setIOID(p.second);
    for (const auto &b : buildings)
    {
        if (const auto r = dynamic_cast<eHorseRanch *>(b.get()))
        {
            if (!r->enclosure())
            {
                for (const auto &bb : buildings)
                {
                    const auto e = dynamic_cast<eHorseRanchEnclosure *>(bb.get());
                    if (e)
                    {
                        r->setEnclosure(e);
                        if (!e->ranch()) e->setRanch(r);
                        break;
                    }
                }
            }
        }
        else if (const auto e = dynamic_cast<eHorseRanchEnclosure *>(b.get()))
        {
            if (!e->ranch())
            {
                for (const auto &bb : buildings)
                {
                    const auto r = dynamic_cast<eHorseRanch *>(bb.get());
                    if (r)
                    {
                        e->setRanch(r);
                        if (!r->enclosure()) r->setEnclosure(e);
                        break;
                    }
                }
            }
        }
    }
    return buildings;
}

static bool containsRuins(const std::vector<eRuins *> &tiles,
                          const eRuins *const ruins)
{
    return std::find(tiles.begin(), tiles.end(), ruins) != tiles.end();
}

static bool addBundleRuins(eGameBoard &board, sRepairGroup &g)
{
    if (!g.originRuins || !g.originRuins->hasRestoreBundle())
        return true;
    const auto &bundle = g.originRuins->restoreBundle();
    for (int x = 0; x < board.width(); x++)
    {
        for (int y = 0; y < board.height(); y++)
        {
            const auto tile = board.tile(x, y);
            if (!tile)
                continue;
            const auto b = tile->underBuilding();
            if (!b || b->type() != eBuildingType::ruins)
                continue;
            const auto ruins = static_cast<eRuins *>(b);
            if (!ruins->hasRestoreBundle() ||
                ruins->restoreBundle() != bundle ||
                containsRuins(g.tiles, ruins))
                continue;
            if (ruins->isOnFire() || !tile->characters().empty())
                return false;
            g.tiles.push_back(ruins);
        }
    }
    return true;
}

static std::vector<sRepairGroup> collectRepairGroups(
    eGameBoard &board, const ePlayerId ppid,
    const int minX, const int minY,
    const int maxX, const int maxY)
{
    std::vector<sRepairGroup> groups;
    std::set<std::tuple<int, int, int, int>> processed;
    const auto diff = board.difficulty(ppid);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            const auto tile = board.tile(x, y);
            if (!tile)
                continue;
            const auto b = tile->underBuilding();
            if (!b || b->type() != eBuildingType::ruins)
                continue;
            const auto ruins = static_cast<eRuins *>(b);
            if (ruins->isOnFire())
                continue;
            const auto wasType = ruins->wasType();
            if (!sRepairableTypes.count(wasType))
                continue;

            const int ox = ruins->originX();
            const int oy = ruins->originY();
            const int ow = ruins->originW();
            const int oh = ruins->originH();
            auto key = std::make_tuple(ox, oy, ow, oh);
            if (processed.count(key))
                continue;
            processed.insert(key);

            sRepairGroup g;
            g.wasType = wasType;
            g.ox = ox;
            g.oy = oy;
            g.ow = ow;
            g.oh = oh;
            bool canRepair = true;
            for (int rx = ox; rx < ox + ow && canRepair; rx++)
            {
                for (int ry = oy; ry < oy + oh && canRepair; ry++)
                {
                    const auto rt = board.tile(rx, ry);
                    if (!rt)
                    {
                        canRepair = false;
                        break;
                    }
                    if (!rt->characters().empty())
                    {
                        canRepair = false;
                        break;
                    }
                    const auto rb = rt->underBuilding();
                    if (!rb)
                    {
                        const auto terrain = rt->terrain();
                        const bool buildable = static_cast<bool>(eTerrain::buildable & terrain);
                        if (!buildable)
                            canRepair = false;
                        continue;
                    }
                    if (rb->type() != eBuildingType::ruins)
                    {
                        canRepair = false;
                        break;
                    }
                    const auto rr = static_cast<eRuins *>(rb);
                    if (rr->originX() != ox || rr->originY() != oy)
                    {
                        canRepair = false;
                        break;
                    }
                    if (rr->isOnFire())
                    {
                        canRepair = false;
                        break;
                    }
                    g.tiles.push_back(rr);
                    if (rr->hasRestoreBundle())
                        g.originRuins = rr;
                }
            }
            if (!canRepair || g.tiles.empty())
                continue;
            if (wasType != eBuildingType::commonHouse &&
                (!g.originRuins || !g.originRuins->hasRestoreBundle()))
                continue;
            canRepair = addBundleRuins(board, g);
            if (!canRepair)
                continue;
            g.cost = eDifficultyHelpers::buildingCost(diff, wasType) * 1.10;
            groups.push_back(std::move(g));
        }
    }
    return groups;
}

void handleRepair(eGameBoard &board, GameWidget *const widget,
                  const int minX, const int minY,
                  const int maxX, const int maxY,
                  const eCityId cid,
                  const bool editorMode)
{
    const auto ppid = board.personPlayer();

    auto groups = collectRepairGroups(board, ppid, minX, minY, maxX, maxY);
    int totalCost = 0;
    for (const auto &g : groups)
        totalCost += g.cost;

    if (totalCost > 0)
    {
        const auto title = "Repair buildings";
        const auto text = "Repair cost " + std::to_string(totalCost) + " drachmas (10% fee). Proceed?";
        const auto acceptA = [groups, totalCost, ppid, cid, &board, editorMode]()
        {
            for (const auto &g : groups)
            {
                bool hasUnit = false;
                for (int rx = g.ox; rx < g.ox + g.ow && !hasUnit; rx++)
                    for (int ry = g.oy; ry < g.oy + g.oh && !hasUnit; ry++)
                    {
                        const auto rt = board.tile(rx, ry);
                        if (rt && !rt->characters().empty())
                            hasUnit = true;
                    }
                    
                if (hasUnit)
                    continue;

                if (g.wasType == eBuildingType::commonHouse)
                {
                    for (auto rr : g.tiles)
                        rr->erase();
                    const auto cid2 = board.tile(g.ox, g.oy) ? board.tile(g.ox, g.oy)->cityId() : cid;
                    board.buildBase(g.ox, g.oy, g.ox + g.ow - 1, g.oy + g.oh - 1, [&board, cid2]()
                                    { return e::make_shared<eSmallHouse>(board, cid2); }, ppid, cid2, true, false, true);
                }
                else if (g.originRuins && g.originRuins->hasRestoreBundle())
                {
                    const auto bundle = g.originRuins->restoreBundle();
                    for (auto rr : g.tiles)
                        rr->erase();
                    const auto restored = restoreFromBundle(
                        bundle, board);
                    (void)restored;
                    continue;
                }
            }

            if (!editorMode)
                board.incDrachmas(ppid, -totalCost, eFinanceTarget::construction);
            board.scheduleTerrainUpdate();
        };
        widget->showQuestion(title, text, acceptA);
    }
}
