#include "widgets/game-widget.h"

#include "textures/game-textures.h"
#include "buildings/allbuildings.h"
#include "widgets/etilepainter.h"
#include "engine/game-board.h"
#include "engine/stamps/estamptool.h"

void GameWidget::paintStampPreview(eTilePainter &tp,
                                    const TerrainTextures &trrTexs,
                                    const BuildingTextures &builTexs,
                                    int worldTileX, int worldTileY,
                                    ePlayerId ppid)
{
    GameTextures::loadCommonHouse();
    GameTextures::loadFoodVendor();
    GameTextures::loadFleeceVendor();
    GameTextures::loadOilVendor();
    GameTextures::loadGranary();
    GameTextures::loadMaintenanceOffice();
    GameTextures::loadPark();
    GameTextures::loadAgora();

    const auto doDrawXY = [&](int buildingTileX, int buildingTileY, double &drawX, double &drawY,
                              int tileSpanW, int tileSpanH, int altitude)
    {
        drawXY(buildingTileX, buildingTileY, drawX, drawY, tileSpanW, tileSpanH, altitude);
    };

    const auto doDrawTex = [&](double drawX, double drawY,
                               eBuildingType buildingType, int tileSpanW, bool canBuild)
    {
        stdsptr<eTexture> tex;
        const auto firstTex = [](const eTextureCollection &coll) -> stdsptr<eTexture>
        {
            if (coll.size() <= 0)
                return nullptr;
            return coll.getTexture(0);
        };
        switch (buildingType)
        {
        case eBuildingType::road:
            tex = trrTexs.fRoad.getTexture(12);
            break;
        case eBuildingType::roadblock:
            GameTextures::loadRoadblock();
            tex = builTexs.fRoadblock;
            break;
        case eBuildingType::commonHouse:
            if (builTexs.fCommonHouse.empty() || builTexs.fCommonHouse[0].size() == 0)
                return;
            tex = builTexs.fCommonHouse[0].getTexture(0);
            break;
        case eBuildingType::eliteHousing:
            GameTextures::loadEliteHouse();
            if (builTexs.fEliteHouse.empty() || builTexs.fEliteHouse[0].size() == 0)
                return;
            tex = builTexs.fEliteHouse[0].getTexture(0);
            break;
        case eBuildingType::gymnasium:
            GameTextures::loadGymnasium();
            tex = builTexs.fGymnasium;
            break;
        case eBuildingType::college:
            GameTextures::loadCollege();
            tex = builTexs.fCollege;
            break;
        case eBuildingType::dramaSchool:
            GameTextures::loadDramaSchool();
            tex = builTexs.fDramaSchool;
            break;
        case eBuildingType::podium:
            GameTextures::loadPodium();
            tex = builTexs.fPodium;
            break;
        case eBuildingType::theater:
            GameTextures::loadTheater();
            tex = builTexs.fTheater;
            break;
        case eBuildingType::stadium:
            GameTextures::loadStadium();
            tex = builTexs.fStadium1H;
            break;
        case eBuildingType::bibliotheke:
            GameTextures::loadBibliotheke();
            tex = builTexs.fBibliotheke;
            break;
        case eBuildingType::observatory:
            GameTextures::loadObservatory();
            tex = builTexs.fObservatory;
            break;
        case eBuildingType::university:
            GameTextures::loadUniversity();
            tex = builTexs.fUniversity;
            break;
        case eBuildingType::laboratory:
            GameTextures::loadLaboratory();
            tex = builTexs.fLaboratory;
            break;
        case eBuildingType::inventorsWorkshop:
            GameTextures::loadInventorsWorkshop();
            tex = builTexs.fInventorsWorkshop;
            break;
        case eBuildingType::museum:
            GameTextures::loadMuseum();
            tex = builTexs.fMuseum;
            break;
        case eBuildingType::fountain:
            GameTextures::loadFountain();
            tex = builTexs.fFountain;
            break;
        case eBuildingType::hospital:
            GameTextures::loadHospital();
            tex = builTexs.fHospital;
            break;
        case eBuildingType::oliveTree:
            GameTextures::loadOliveTree();
            tex = firstTex(builTexs.fOliveTree);
            break;
        case eBuildingType::vine:
            GameTextures::loadVine();
            tex = firstTex(builTexs.fVine);
            break;
        case eBuildingType::orangeTree:
            GameTextures::loadPlantation();
            tex = firstTex(builTexs.fOrangeTree);
            break;
        case eBuildingType::wheatFarm:
            GameTextures::loadPlantation();
            tex = builTexs.fPlantation;
            break;
        case eBuildingType::carrotsFarm:
            GameTextures::loadPlantation();
            tex = builTexs.fPlantation;
            break;
        case eBuildingType::onionsFarm:
            GameTextures::loadPlantation();
            tex = builTexs.fPlantation;
            break;
        case eBuildingType::huntingLodge:
            GameTextures::loadHuntingLodge();
            tex = builTexs.fHuntingLodge;
            break;
        case eBuildingType::fishery:
            GameTextures::loadFishery();
            tex = firstTex(builTexs.fFishery);
            break;
        case eBuildingType::urchinQuay:
            GameTextures::loadUrchinQuay();
            tex = firstTex(builTexs.fUrchinQuay);
            break;
        case eBuildingType::cardingShed:
            GameTextures::loadCardingShed();
            tex = builTexs.fCardingShed;
            break;
        case eBuildingType::dairy:
            GameTextures::loadDairy();
            tex = builTexs.fDairy;
            break;
        case eBuildingType::growersLodge:
            GameTextures::loadGrowersLodge();
            tex = builTexs.fGrowersLodge;
            break;
        case eBuildingType::orangeTendersLodge:
            GameTextures::loadOrangeTendersLodge();
            tex = builTexs.fOrangeTendersLodge;
            break;
        case eBuildingType::corral:
            GameTextures::loadCorral();
            tex = builTexs.fCorral;
            break;
        case eBuildingType::tradePost:
            GameTextures::loadTradingPost();
            tex = builTexs.fTradingPost;
            break;
        case eBuildingType::pier:
            GameTextures::loadPier();
            tex = firstTex(builTexs.fPier1);
            break;
        case eBuildingType::foodVendor:
            tex = builTexs.fFoodVendor;
            break;
        case eBuildingType::fleeceVendor:
            tex = builTexs.fFleeceVendor;
            break;
        case eBuildingType::oilVendor:
            tex = builTexs.fOilVendor;
            break;
        case eBuildingType::wineVendor:
            GameTextures::loadWineVendorBuilding();
            tex = builTexs.fWineVendor;
            break;
        case eBuildingType::armsVendor:
            GameTextures::loadArmsVendor();
            tex = builTexs.fArmsVendor;
            break;
        case eBuildingType::horseTrainer:
            GameTextures::loadHorseVendor();
            tex = builTexs.fHorseTrainer;
            break;
        case eBuildingType::chariotVendor:
            GameTextures::loadChariotVendor();
            tex = builTexs.fChariotVendor;
            break;
        case eBuildingType::timberMill:
            GameTextures::loadTimberMill();
            tex = builTexs.fTimberMill;
            break;
        case eBuildingType::masonryShop:
            GameTextures::loadMasonryShop();
            tex = builTexs.fMasonryShop;
            break;
        case eBuildingType::mint:
            GameTextures::loadMint();
            tex = builTexs.fMint;
            break;
        case eBuildingType::foundry:
            GameTextures::loadFoundry();
            tex = builTexs.fFoundry;
            break;
        case eBuildingType::olivePress:
            GameTextures::loadOlivePress();
            tex = builTexs.fOlivePress;
            break;
        case eBuildingType::winery:
            GameTextures::loadWinery();
            tex = builTexs.fWinery;
            break;
        case eBuildingType::sculptureStudio:
            GameTextures::loadSculptureStudio();
            tex = builTexs.fSculptureStudio;
            break;
        case eBuildingType::artisansGuild:
            GameTextures::loadArtisansGuild();
            tex = builTexs.fArtisansGuild;
            break;
        case eBuildingType::wall:
            GameTextures::loadWall();
            tex = firstTex(builTexs.fWall);
            break;
        case eBuildingType::tower:
            GameTextures::loadGatehouseAndTower();
            tex = builTexs.fTower;
            break;
        case eBuildingType::triremeWharf:
            GameTextures::loadTriremeWharf();
            tex = firstTex(builTexs.fTriremeWharf);
            break;
        case eBuildingType::horseRanch:
            GameTextures::loadHorseRanch();
            tex = builTexs.fHorseRanch;
            break;
        case eBuildingType::horseRanchEnclosure:
            GameTextures::loadHorseRanch();
            tex = builTexs.fHorseRanchEnclosure;
            break;
        case eBuildingType::chariotFactory:
            GameTextures::loadChariotFactory();
            tex = builTexs.fChariotFactory;
            break;
        case eBuildingType::armory:
            GameTextures::loadArmory();
            tex = builTexs.fArmory;
            break;
        case eBuildingType::maintenanceOffice:
            GameTextures::loadMaintenanceOffice();
            tex = builTexs.fMaintenanceOffice;
            break;
        case eBuildingType::taxOffice:
            GameTextures::loadTaxOffice();
            tex = builTexs.fTaxOffice;
            break;
        case eBuildingType::watchPost:
            GameTextures::loadWatchpost();
            tex = builTexs.fWatchPost;
            break;
        case eBuildingType::palace:
            GameTextures::loadPalace();
            tex = builTexs.fPalace1H;
            break;
        case eBuildingType::park:
            tex = builTexs.fPark.getTexture(0);
            break;
        case eBuildingType::doricColumn:
            GameTextures::loadColumns();
            tex = builTexs.fDoricColumn;
            break;
        case eBuildingType::ionicColumn:
            GameTextures::loadColumns();
            tex = builTexs.fIonicColumn;
            break;
        case eBuildingType::corinthianColumn:
            GameTextures::loadColumns();
            tex = builTexs.fCorinthianColumn;
            break;
        case eBuildingType::avenue:
            GameTextures::loadAvenue();
            tex = !builTexs.fAvenue.empty() ? firstTex(builTexs.fAvenue[0]) : nullptr;
            break;
        case eBuildingType::commemorative:
            GameTextures::loadCommemorative();
            tex = firstTex(builTexs.fCommemorative);
            break;
        case eBuildingType::bench:
            GameTextures::loadBench();
            tex = builTexs.fBench;
            break;
        case eBuildingType::flowerGarden:
            GameTextures::loadFlowerGarden();
            tex = builTexs.fFlowerGarden;
            break;
        case eBuildingType::gazebo:
            GameTextures::loadGazebo();
            tex = builTexs.fGazebo;
            break;
        case eBuildingType::hedgeMaze:
            GameTextures::loadHedgeMaze();
            tex = builTexs.fHedgeMaze;
            break;
        case eBuildingType::fishPond:
            GameTextures::loadFishPond();
            tex = builTexs.fFishPond;
            break;
        case eBuildingType::waterPark:
            GameTextures::loadWaterPark();
            tex = builTexs.fWaterPark1;
            break;
        case eBuildingType::birdBath:
            GameTextures::loadBirdBath();
            tex = builTexs.fBirdBath;
            break;
        case eBuildingType::shortObelisk:
            GameTextures::loadShortObelisk();
            tex = builTexs.fShortObelisk;
            break;
        case eBuildingType::tallObelisk:
            GameTextures::loadTallObelisk();
            tex = builTexs.fTallObelisk;
            break;
        case eBuildingType::shellGarden:
            GameTextures::loadShellGarden();
            tex = builTexs.fShellGarden;
            break;
        case eBuildingType::sundial:
            GameTextures::loadSundial();
            tex = builTexs.fSundial;
            break;
        case eBuildingType::dolphinSculpture:
            GameTextures::loadDolphinSculpture();
            tex = builTexs.fDolphinSculpture;
            break;
        case eBuildingType::orrery:
            GameTextures::loadOrrery();
            tex = builTexs.fOrrery;
            break;
        case eBuildingType::spring:
            GameTextures::loadSpring();
            tex = firstTex(builTexs.fSpring);
            break;
        case eBuildingType::topiary:
            GameTextures::loadTopiary();
            tex = builTexs.fTopiary;
            break;
        case eBuildingType::baths:
            GameTextures::loadBaths();
            tex = builTexs.fBaths;
            break;
        case eBuildingType::stoneCircle:
            GameTextures::loadStoneCircle();
            tex = builTexs.fStoneCircle;
            break;
        case eBuildingType::refinery:
            GameTextures::loadRefinery();
            tex = builTexs.fRefinery;
            break;
        case eBuildingType::blackMarbleWorkshop:
            tex = builTexs.fBlackMarbleWorkshop;
            break;
        case eBuildingType::commonAgora:
            tex = builTexs.fAgora.getTexture(0);
            break;
        case eBuildingType::granary:
            GameTextures::loadGranary();
            tex = builTexs.fGranary;
            break;
        case eBuildingType::warehouse:
            tex = builTexs.fWarehouseDoor;
            break;
        default:
            break;
        }
        const bool fallback = !tex;
        if (fallback)
            tex = trrTexs.fBuildingBase;
        if (!tex) return;
        if (fallback)
            tex->setColorMod(140, 140, 140);
        else if (!canBuild)
            tex->setColorMod(255, 0, 0);
        tex->setAlpha(120);
        tp.drawTexture(drawX, drawY, tex, eAlignment::top);
        tex->clearAlphaMod();
        if (fallback || !canBuild)
            tex->clearColorMod();
    };
    const auto doDrawAgora = [&](const int ax, const int ay, const int id)
    {
        const auto o = static_cast<eAgoraOrientation>(id);
        const bool horizontal = o == eAgoraOrientation::bottomLeft ||
                                o == eAgoraOrientation::topRight;
        const int w = horizontal ? 6 : 3;
        const int h = horizontal ? 3 : 6;
        const auto isRoad = [&](const int x, const int y)
        {
            switch (o)
            {
            case eAgoraOrientation::bottomLeft:
                return y == ay;
            case eAgoraOrientation::topRight:
                return y == ay + h - 1;
            case eAgoraOrientation::bottomRight:
                return x == ax;
            case eAgoraOrientation::topLeft:
                return x == ax + w - 1;
            }
        };
        for (int y = ay; y < ay + h; y++)
        {
            for (int x = ax; x < ax + w; x++)
            {
                const auto tile = mBoard->tile(x, y);
                if (!tile)
                    continue;
                double drawX;
                double drawY;
                drawXY(x, y, drawX, drawY, 1, 1, tile->altitude());
                stdsptr<eTexture> tex;
                if (isRoad(x, y))
                {
                    tex = builTexs.fAgoraRoad.getTexture(tile->seed() %
                                                         builTexs.fAgoraRoad.size());
                }
                else
                {
                    tex = builTexs.fAgora.getTexture(tile->seed() %
                                                     builTexs.fAgora.size());
                }
                if (!tex)
                    continue;
                tex->setColorMod(0, 255, 0);
                tex->setAlpha(120);
                tp.drawTexture(drawX, drawY, tex, eAlignment::top);
                tex->clearAlphaMod();
                tex->clearColorMod();
            }
        }
    };
    const auto doDrawStampAgora = [&](const eStampBuildCommand &cmd)
    {
        if (cmd.agoraRoads.empty())
            return;

        const auto drawCell = [&](const int x, const int y, const bool road)
        {
            const auto tile = mBoard->tile(x, y);
            if (!tile)
                return;
            double drawX;
            double drawY;
            drawXY(x, y, drawX, drawY, 1, 1, tile->altitude());
            stdsptr<eTexture> tex;
            if (road)
            {
                tex = builTexs.fAgoraRoad.getTexture(tile->seed() %
                                                     builTexs.fAgoraRoad.size());
            }
            else
            {
                tex = builTexs.fAgora.getTexture(tile->seed() %
                                                 builTexs.fAgora.size());
            }
            if (!tex)
                return;
            tex->setColorMod(0, 255, 0);
            tex->setAlpha(120);
            tp.drawTexture(drawX, drawY, tex, eAlignment::top);
            tex->clearAlphaMod();
            tex->clearColorMod();
        };
        for (const auto &road : cmd.agoraRoads)
            drawCell(worldTileX + road.first, worldTileY + road.second, true);
    };

    mStampTool->paintPreview(worldTileX, worldTileY, mBoard, mEditorMode, mViewedCityId, ppid,
                             doDrawXY, doDrawTex, doDrawAgora,
                             doDrawStampAgora);
}
