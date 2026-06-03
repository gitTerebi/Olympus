#include "repair.h"

#include "game-widget.h"

#include "buildings/allbuildings.h"
#include "buildings/eruins.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "audio/sounds.h"

#include "evectorhelpers.h"
#include "ebuildingstoerase.h"
#include "engine/difficulty.h"

#include "fileIO/building-reader.h"
#include "fileIO/ereadstream.h"
#include "fileIO/esavearchive.h"

#include <algorithm>
#include <cstdint>

static const std::set<eBuildingType> sRepairableTypes = {
    eBuildingType::commonHouse,
    eBuildingType::eliteHousing,
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
    eBuildingType::palace,
};

struct sRepairGroup
{
    eBuildingType wasType;
    int ox, oy, ow, oh;
    std::vector<eRuins *> tiles;
    eRuins *originRuins = nullptr;
    int cost = 0;
};

static void placeBuildingOnTiles(GameBoard &board,
                                 const stdsptr<eBuilding> &b,
                                 const SDL_Rect &rect)
{
    const int cx = rect.x + rect.w / 2;
    const int cy = rect.y + rect.h / 2;
    b->setCenterTile(board.tile(cx, cy));
    b->setTileRect(rect);
    for (int x = rect.x; x < rect.x + rect.w; x++)
    {
        for (int y = rect.y; y < rect.y + rect.h; y++)
        {
            const auto t = board.tile(x, y);
            if (t)
            {
                t->setUnderBuilding(b);
                b->addUnderBuilding(t);
            }
        }
    }
    if (b->dead())
        b->setHP(eBuilding::sMaxHp(b->type()));
    board.registerBuilding(b.get());
}

// Replace a bundle-restored pier (pier-type TradePost + its ePier) with a
// freshly constructed pair, copying the saved trade settings across. The
// restored pair links up fine but never resumes spawning trade boats, so a
// clean rebuild via the normal construction path is more reliable than
// reviving whatever the snapshot drops.
static void rebuildPierFresh(std::vector<stdsptr<eBuilding>> &buildings,
                             GameBoard &board)
{
    TradePost *oldPost = nullptr;
    ePier *oldPier = nullptr;
    for (const auto &b : buildings)
    {
        if (const auto tp = dynamic_cast<TradePost *>(b.get()))
        {
            if (tp->tpType() == eTradePostType::pier) oldPost = tp;
        }
        else if (const auto p = dynamic_cast<ePier *>(b.get()))
        {
            oldPier = p;
        }
    }
    if (!oldPost || !oldPier) return;

    // capture saved settings + footprints before tearing the old pair down
    const auto postRect = oldPost->tileRect();
    const auto pierRect = oldPier->tileRect();
    const auto o = oldPost->orientation();
    const auto cid = oldPost->cityId();
    auto &worldCity = oldPost->city();
    eResourceType imports, exports, empty, cartGet, cartAccept, cartDontAccept;
    oldPost->getOrders(imports, exports, empty, cartGet,
                       cartAccept, cartDontAccept);
    const auto savedMaxCount = oldPost->maxCount();

    // TradePost::erase also erases its unpack building (the pier), so a
    // single call tears the whole pair down.
    oldPost->erase();

    std::vector<stdsptr<eBuilding>> kept;
    kept.reserve(buildings.size());
    for (const auto &b : buildings)
    {
        if (b.get() != oldPost && b.get() != oldPier)
            kept.push_back(b);
    }
    buildings = std::move(kept);

    const auto pier = e::make_shared<ePier>(board, o, cid);
    placeBuildingOnTiles(board, pier, pierRect);

    const auto post = e::make_shared<TradePost>(
        board, worldCity, cid, eTradePostType::pier);
    post->setOrientation(o);
    post->setUnpackBuilding(pier.get());
    placeBuildingOnTiles(board, post, postRect);
    pier->setTradePost(post.get());
    post->setOrders(imports, exports, empty, cartGet,
                    cartAccept, cartDontAccept);
    post->setMaxCount(savedMaxCount);

    buildings.push_back(pier);
    buildings.push_back(post);
}

struct sRepairByteVecRef
{
    std::vector<uint8_t> &fVec;
};

static eWriteStream &operator<<(eWriteStream &dst,
                                const sRepairByteVecRef &ref)
{
    const int32_t sz = static_cast<int32_t>(ref.fVec.size());
    dst.write(&sz, sizeof(sz));
    if (sz > 0)
        dst.write(ref.fVec.data(), sz);
    return dst;
}

static eReadStream &operator>>(eReadStream &src, sRepairByteVecRef &ref)
{
    int32_t sz;
    src.read(&sz, sizeof(sz));
    ref.fVec.clear();
    if (sz < 0)
        return src;
    if (sz > 0)
    {
        ref.fVec.resize(sz);
        src.read(ref.fVec.data(), sz);
    }
    return src;
}

static bool byteVecField(eSaveArchive &ar, const char *const name,
                         std::vector<uint8_t> &v)
{
    sRepairByteVecRef ref{v};
    return ar.field(name, ref);
}

static std::vector<stdsptr<eBuilding>> restoreFromBundle(
    const std::vector<uint8_t> &data, GameBoard &board)
{
    std::vector<stdsptr<eBuilding>> buildings;
    if (data.empty())
        return buildings;

    eReadSource source(const_cast<void *>(static_cast<const void *>(data.data())), data.size());
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
    {
        eSaveArchive ar(src);
        int marker;
        ar.field("bundleMarker", marker);
        if (marker == -1)
        {
            int n;
            ar.field("buildingCount", n);
            for (int i = 0; i < n; i++)
            {
                std::vector<uint8_t> snapshot;
                byteVecField(ar, "buildingSnapshot", snapshot);
                const int size = static_cast<int>(snapshot.size());
                if (size == 0 || size > (int)data.size())
                {
                    continue;
                }
                eReadSource bs(const_cast<void *>(
                    static_cast<const void *>(snapshot.data())), snapshot.size());
                eReadStream bsrc(bs);
                bsrc.readFormat();
                eBuildingType type;
                stdsptr<eBuilding> b;
                {
                    eSaveArchive bar(bsrc);
                    bar.field("buildingType", type);
                    b = BuildingArchive::load(board, type, bar);
                }
                if (!b)
                {
                    continue;
                }
                buildings.push_back(b);
            }
        }
        ar.stopReadingFields();
    }
    src.handlePostFuncs();
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
                    const auto e = dynamic_cast<HorseRanchEnclosure *>(bb.get());
                    if (e)
                    {
                        r->setEnclosure(e);
                        if (!e->ranch()) e->setRanch(r);
                        break;
                    }
                }
            }
        }
        else if (const auto e = dynamic_cast<HorseRanchEnclosure *>(b.get()))
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
        else if (const auto tp = dynamic_cast<TradePost *>(b.get()))
        {
            if (tp->tpType() == eTradePostType::pier &&
                !dynamic_cast<ePier *>(tp->unpackBuilding()))
            {
                for (const auto &bb : buildings)
                {
                    const auto pier = dynamic_cast<ePier *>(bb.get());
                    if (pier)
                    {
                        tp->setUnpackBuilding(pier);
                        if (!pier->tradePost()) pier->setTradePost(tp);
                        break;
                    }
                }
            }
        }
        else if (const auto pier = dynamic_cast<ePier *>(b.get()))
        {
            if (!pier->tradePost())
            {
                for (const auto &bb : buildings)
                {
                    const auto tp = dynamic_cast<TradePost *>(bb.get());
                    if (tp && tp->tpType() == eTradePostType::pier)
                    {
                        pier->setTradePost(tp);
                        if (!dynamic_cast<ePier *>(tp->unpackBuilding()))
                            tp->setUnpackBuilding(pier);
                        break;
                    }
                }
            }
        }
    }

    // The bundle-restored pier never resumes trade traffic (boats stop
    // spawning). Rather than chase whatever state the snapshot loses, throw
    // the restored pier pair away and build a fresh one the same way the
    // player build does, copying the saved settings across.
    rebuildPierFresh(buildings, board);

    return buildings;
}

static bool containsRuins(const std::vector<eRuins *> &tiles,
                          const eRuins *const ruins)
{
    return std::find(tiles.begin(), tiles.end(), ruins) != tiles.end();
}

static bool addBundleRuins(GameBoard &board, sRepairGroup &g)
{
    if (!g.originRuins || !g.originRuins->hasRestoreBundle())
        return true;
    const auto &bundle = g.originRuins->restoreBundle();
    for (int dx = 0; dx < board.width(); dx++)
    {
        for (int dy = 0; dy < board.height(); dy++)
        {
            const auto tile = board.dtile(dx, dy);
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

static bool hasUsableOrigin(GameBoard &board,
                            const int ox, const int oy,
                            const int ow, const int oh)
{
    if (ow < 1 || oh < 1)
        return false;
    for (int rx = ox; rx < ox + ow; rx++)
        for (int ry = oy; ry < oy + oh; ry++)
            if (board.tile(rx, ry))
                return true;
    return false;
}

static bool isHousingPlotType(const eBuildingType type)
{
    return type == eBuildingType::commonHouse ||
           type == eBuildingType::eliteHousing;
}

static double repairFee(const Difficulty d)
{
    switch (d) {
    case Difficulty::beginner: return 1.0;
    case Difficulty::mortal:   return 1.25;
    case Difficulty::hero:     return 1.5;
    case Difficulty::titan:    return 1.75;
    case Difficulty::olympian: return 2.0;
    }
    return 1.0;
}

static std::vector<sRepairGroup> collectRepairGroups(
    GameBoard &board, const ePlayerId ppid,
    const int minX, const int minY,
    const int maxX, const int maxY)
{
    std::vector<sRepairGroup> groups;
    std::set<std::tuple<int, int, int, int>> processed;
    std::set<eRuins *> processedRuins;
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
            if (processedRuins.count(ruins))
                continue;
            const auto wasType = ruins->wasType();
            if (!sRepairableTypes.count(wasType))
                continue;

            const int ox = ruins->originX();
            const int oy = ruins->originY();
            const int ow = ruins->originW();
            const int oh = ruins->originH();
            const bool badOrigin = !hasUsableOrigin(board, ox, oy, ow, oh);
            if (badOrigin && ruins->hasRestoreBundle())
            {
                sRepairGroup g;
                g.wasType = wasType;
                g.originRuins = ruins;
                g.ox = 1000000;
                g.oy = 1000000;
                int right = -1;
                int bottom = -1;
                const auto &bundle = ruins->restoreBundle();
                bool canRepair = true;
                for (int dx = 0; dx < board.width(); dx++)
                {
                    for (int dy = 0; dy < board.height(); dy++)
                    {
                        const auto bt = board.dtile(dx, dy);
                        if (!bt)
                            continue;
                        const auto bb = bt->underBuilding();
                        if (!bb || bb->type() != eBuildingType::ruins)
                            continue;
                        const auto br = static_cast<eRuins *>(bb);
                        if (!br->hasRestoreBundle() ||
                            br->restoreBundle() != bundle)
                            continue;
                        if (br->isOnFire() || !bt->characters().empty())
                        {
                            canRepair = false;
                            break;
                        }
                        processedRuins.insert(br);
                        g.tiles.push_back(br);
                        g.ox = std::min(g.ox, bt->x());
                        g.oy = std::min(g.oy, bt->y());
                        right = std::max(right, bt->x() + 1);
                        bottom = std::max(bottom, bt->y() + 1);
                    }
                    if (!canRepair)
                        break;
                }
                if (!canRepair || g.tiles.empty())
                    continue;
                g.ow = right - g.ox;
                g.oh = bottom - g.oy;
                g.cost = DifficultyHelpers::buildingCost(diff, wasType) * repairFee(diff);
                groups.push_back(std::move(g));
                continue;
            }
            if (badOrigin)
                continue;
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
                        {
                            canRepair = false;
                        }
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
                    processedRuins.insert(rr);
                    if (rr->hasRestoreBundle())
                        g.originRuins = rr;
                }
            }
            if (!canRepair || g.tiles.empty())
                continue;
            if (!isHousingPlotType(wasType) &&
                (!g.originRuins || !g.originRuins->hasRestoreBundle()))
                continue;
            canRepair = addBundleRuins(board, g);
            if (!canRepair)
                continue;
            g.cost = DifficultyHelpers::buildingCost(diff, wasType) * repairFee(diff);
            groups.push_back(std::move(g));
        }
    }
    return groups;
}

void handleRepair(GameBoard &board, GameWidget *const widget,
                  const int minX, const int minY,
                  const int maxX, const int maxY,
                  const eCityId cid,
                  const bool editorMode)
{
    const auto ppid = board.personPlayer();
    const auto diff = board.difficulty(ppid);

    auto groups = collectRepairGroups(board, ppid, minX, minY, maxX, maxY);
    int totalCost = 0;
    for (const auto &g : groups)
        totalCost += g.cost;

    if (totalCost > 0)
    {
        const auto title = "Repair buildings";
        const int feePercent = static_cast<int>((repairFee(diff) - 1.0) * 100 + 0.5);
        const auto text = "Repair cost " + std::to_string(totalCost) + " drachmas (" + std::to_string(feePercent) + "% fee). Proceed?";
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

                if (isHousingPlotType(g.wasType))
                {
                    for (auto rr : g.tiles)
                        rr->erase();
                    const auto cid2 = board.tile(g.ox, g.oy) ? board.tile(g.ox, g.oy)->cityId() : cid;
                    if (g.wasType == eBuildingType::eliteHousing)
                    {
                        board.buildBase(g.ox, g.oy, g.ox + g.ow - 1, g.oy + g.oh - 1, [&board, cid2]()
                                        { return e::make_shared<EliteHousing>(board, cid2); }, ppid, cid2, true, false, true);
                    }
                    else
                    {
                        board.buildBase(g.ox, g.oy, g.ox + g.ow - 1, g.oy + g.oh - 1, [&board, cid2]()
                                        { return e::make_shared<SmallHouse>(board, cid2); }, ppid, cid2, true, false, true);
                    }
                }
                else if (g.originRuins && g.originRuins->hasRestoreBundle())
                {
                    if (!hasUsableOrigin(board, g.ox, g.oy, g.ow, g.oh))
                    {
                        continue;
                    }
                    const auto bundle = g.originRuins->restoreBundle();
                    for (auto rr : g.tiles)
                        rr->erase();
                    const auto restored = restoreFromBundle(
                        bundle, board);
                    for (const auto &rb : restored)
                        rb->resetRisks();
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
