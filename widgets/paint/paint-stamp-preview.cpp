#include "widgets/game-widget.h"

#include "textures/egametextures.h"
#include "buildings/allbuildings.h"
#include "widgets/etilepainter.h"
#include "engine/game-board.h"
#include "engine/stamps/estamptool.h"

void GameWidget::paintStampPreview(eTilePainter &tp,
                                    const eTerrainTextures &trrTexs,
                                    const eBuildingTextures &builTexs,
                                    int tx, int ty,
                                    ePlayerId ppid)
{
    eGameTextures::loadCommonHouse();
    eGameTextures::loadFoodVendor();
    eGameTextures::loadFleeceVendor();
    eGameTextures::loadOilVendor();
    eGameTextures::loadGranary();
    eGameTextures::loadMaintenanceOffice();
    eGameTextures::loadPark();
    eGameTextures::loadAgora();

    const auto doDrawXY = [&](int bx, int by, double &rx, double &ry,
                              int sw, int sh, int alt)
    {
        drawXY(bx, by, rx, ry, sw, sh, alt);
    };

    const auto doDrawTex = [&](double rx, double ry,
                               eBuildingType type, int sw, bool can)
    {
        stdsptr<eTexture> tex;
        const auto firstTex = [](const eTextureCollection &coll) -> stdsptr<eTexture>
        {
            if (coll.size() <= 0)
                return nullptr;
            return coll.getTexture(0);
        };
        switch (type)
        {
        case eBuildingType::road:
            tex = trrTexs.fRoad.getTexture(12);
            break;
        case eBuildingType::roadblock:
            eGameTextures::loadRoadblock();
            tex = builTexs.fRoadblock;
            break;
        case eBuildingType::commonHouse:
            if (builTexs.fCommonHouse.empty() || builTexs.fCommonHouse[0].size() == 0)
                return;
            tex = builTexs.fCommonHouse[0].getTexture(0);
            break;
        case eBuildingType::eliteHousing:
            eGameTextures::loadEliteHouse();
            if (builTexs.fEliteHouse.empty() || builTexs.fEliteHouse[0].size() == 0)
                return;
            tex = builTexs.fEliteHouse[0].getTexture(0);
            break;
        case eBuildingType::gymnasium:
            eGameTextures::loadGymnasium();
            tex = builTexs.fGymnasium;
            break;
        case eBuildingType::college:
            eGameTextures::loadCollege();
            tex = builTexs.fCollege;
            break;
        case eBuildingType::dramaSchool:
            eGameTextures::loadDramaSchool();
            tex = builTexs.fDramaSchool;
            break;
        case eBuildingType::podium:
            eGameTextures::loadPodium();
            tex = builTexs.fPodium;
            break;
        case eBuildingType::theater:
            eGameTextures::loadTheater();
            tex = builTexs.fTheater;
            break;
        case eBuildingType::stadium:
            eGameTextures::loadStadium();
            tex = builTexs.fStadium1H;
            break;
        case eBuildingType::bibliotheke:
            eGameTextures::loadBibliotheke();
            tex = builTexs.fBibliotheke;
            break;
        case eBuildingType::observatory:
            eGameTextures::loadObservatory();
            tex = builTexs.fObservatory;
            break;
        case eBuildingType::university:
            eGameTextures::loadUniversity();
            tex = builTexs.fUniversity;
            break;
        case eBuildingType::laboratory:
            eGameTextures::loadLaboratory();
            tex = builTexs.fLaboratory;
            break;
        case eBuildingType::inventorsWorkshop:
            eGameTextures::loadInventorsWorkshop();
            tex = builTexs.fInventorsWorkshop;
            break;
        case eBuildingType::museum:
            eGameTextures::loadMuseum();
            tex = builTexs.fMuseum;
            break;
        case eBuildingType::fountain:
            eGameTextures::loadFountain();
            tex = builTexs.fFountain;
            break;
        case eBuildingType::hospital:
            eGameTextures::loadHospital();
            tex = builTexs.fHospital;
            break;
        case eBuildingType::oliveTree:
            eGameTextures::loadOliveTree();
            tex = firstTex(builTexs.fOliveTree);
            break;
        case eBuildingType::vine:
            eGameTextures::loadVine();
            tex = firstTex(builTexs.fVine);
            break;
        case eBuildingType::orangeTree:
            eGameTextures::loadPlantation();
            tex = firstTex(builTexs.fOrangeTree);
            break;
        case eBuildingType::wheatFarm:
            eGameTextures::loadPlantation();
            tex = builTexs.fPlantation;
            break;
        case eBuildingType::carrotsFarm:
            eGameTextures::loadPlantation();
            tex = builTexs.fPlantation;
            break;
        case eBuildingType::onionsFarm:
            eGameTextures::loadPlantation();
            tex = builTexs.fPlantation;
            break;
        case eBuildingType::huntingLodge:
            eGameTextures::loadHuntingLodge();
            tex = builTexs.fHuntingLodge;
            break;
        case eBuildingType::fishery:
            eGameTextures::loadFishery();
            tex = firstTex(builTexs.fFishery);
            break;
        case eBuildingType::urchinQuay:
            eGameTextures::loadUrchinQuay();
            tex = firstTex(builTexs.fUrchinQuay);
            break;
        case eBuildingType::cardingShed:
            eGameTextures::loadCardingShed();
            tex = builTexs.fCardingShed;
            break;
        case eBuildingType::dairy:
            eGameTextures::loadDairy();
            tex = builTexs.fDairy;
            break;
        case eBuildingType::growersLodge:
            eGameTextures::loadGrowersLodge();
            tex = builTexs.fGrowersLodge;
            break;
        case eBuildingType::orangeTendersLodge:
            eGameTextures::loadOrangeTendersLodge();
            tex = builTexs.fOrangeTendersLodge;
            break;
        case eBuildingType::corral:
            eGameTextures::loadCorral();
            tex = builTexs.fCorral;
            break;
        case eBuildingType::tradePost:
            eGameTextures::loadTradingPost();
            tex = builTexs.fTradingPost;
            break;
        case eBuildingType::pier:
            eGameTextures::loadPier();
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
            eGameTextures::loadWineVendorBuilding();
            tex = builTexs.fWineVendor;
            break;
        case eBuildingType::armsVendor:
            eGameTextures::loadArmsVendor();
            tex = builTexs.fArmsVendor;
            break;
        case eBuildingType::horseTrainer:
            eGameTextures::loadHorseVendor();
            tex = builTexs.fHorseTrainer;
            break;
        case eBuildingType::chariotVendor:
            eGameTextures::loadChariotVendor();
            tex = builTexs.fChariotVendor;
            break;
        case eBuildingType::timberMill:
            eGameTextures::loadTimberMill();
            tex = builTexs.fTimberMill;
            break;
        case eBuildingType::masonryShop:
            eGameTextures::loadMasonryShop();
            tex = builTexs.fMasonryShop;
            break;
        case eBuildingType::mint:
            eGameTextures::loadMint();
            tex = builTexs.fMint;
            break;
        case eBuildingType::foundry:
            eGameTextures::loadFoundry();
            tex = builTexs.fFoundry;
            break;
        case eBuildingType::olivePress:
            eGameTextures::loadOlivePress();
            tex = builTexs.fOlivePress;
            break;
        case eBuildingType::winery:
            eGameTextures::loadWinery();
            tex = builTexs.fWinery;
            break;
        case eBuildingType::sculptureStudio:
            eGameTextures::loadSculptureStudio();
            tex = builTexs.fSculptureStudio;
            break;
        case eBuildingType::artisansGuild:
            eGameTextures::loadArtisansGuild();
            tex = builTexs.fArtisansGuild;
            break;
        case eBuildingType::wall:
            eGameTextures::loadWall();
            tex = firstTex(builTexs.fWall);
            break;
        case eBuildingType::tower:
            eGameTextures::loadGatehouseAndTower();
            tex = builTexs.fTower;
            break;
        case eBuildingType::triremeWharf:
            eGameTextures::loadTriremeWharf();
            tex = firstTex(builTexs.fTriremeWharf);
            break;
        case eBuildingType::horseRanch:
            eGameTextures::loadHorseRanch();
            tex = builTexs.fHorseRanch;
            break;
        case eBuildingType::horseRanchEnclosure:
            eGameTextures::loadHorseRanch();
            tex = builTexs.fHorseRanchEnclosure;
            break;
        case eBuildingType::chariotFactory:
            eGameTextures::loadChariotFactory();
            tex = builTexs.fChariotFactory;
            break;
        case eBuildingType::armory:
            eGameTextures::loadArmory();
            tex = builTexs.fArmory;
            break;
        case eBuildingType::maintenanceOffice:
            eGameTextures::loadMaintenanceOffice();
            tex = builTexs.fMaintenanceOffice;
            break;
        case eBuildingType::taxOffice:
            eGameTextures::loadTaxOffice();
            tex = builTexs.fTaxOffice;
            break;
        case eBuildingType::watchPost:
            eGameTextures::loadWatchpost();
            tex = builTexs.fWatchPost;
            break;
        case eBuildingType::palace:
            eGameTextures::loadPalace();
            tex = builTexs.fPalace1H;
            break;
        case eBuildingType::park:
            tex = builTexs.fPark.getTexture(0);
            break;
        case eBuildingType::doricColumn:
            eGameTextures::loadColumns();
            tex = builTexs.fDoricColumn;
            break;
        case eBuildingType::ionicColumn:
            eGameTextures::loadColumns();
            tex = builTexs.fIonicColumn;
            break;
        case eBuildingType::corinthianColumn:
            eGameTextures::loadColumns();
            tex = builTexs.fCorinthianColumn;
            break;
        case eBuildingType::avenue:
            eGameTextures::loadAvenue();
            tex = !builTexs.fAvenue.empty() ? firstTex(builTexs.fAvenue[0]) : nullptr;
            break;
        case eBuildingType::commemorative:
            eGameTextures::loadCommemorative();
            tex = firstTex(builTexs.fCommemorative);
            break;
        case eBuildingType::bench:
            eGameTextures::loadBench();
            tex = builTexs.fBench;
            break;
        case eBuildingType::flowerGarden:
            eGameTextures::loadFlowerGarden();
            tex = builTexs.fFlowerGarden;
            break;
        case eBuildingType::gazebo:
            eGameTextures::loadGazebo();
            tex = builTexs.fGazebo;
            break;
        case eBuildingType::hedgeMaze:
            eGameTextures::loadHedgeMaze();
            tex = builTexs.fHedgeMaze;
            break;
        case eBuildingType::fishPond:
            eGameTextures::loadFishPond();
            tex = builTexs.fFishPond;
            break;
        case eBuildingType::waterPark:
            eGameTextures::loadWaterPark();
            tex = builTexs.fWaterPark1;
            break;
        case eBuildingType::birdBath:
            eGameTextures::loadBirdBath();
            tex = builTexs.fBirdBath;
            break;
        case eBuildingType::shortObelisk:
            eGameTextures::loadShortObelisk();
            tex = builTexs.fShortObelisk;
            break;
        case eBuildingType::tallObelisk:
            eGameTextures::loadTallObelisk();
            tex = builTexs.fTallObelisk;
            break;
        case eBuildingType::shellGarden:
            eGameTextures::loadShellGarden();
            tex = builTexs.fShellGarden;
            break;
        case eBuildingType::sundial:
            eGameTextures::loadSundial();
            tex = builTexs.fSundial;
            break;
        case eBuildingType::dolphinSculpture:
            eGameTextures::loadDolphinSculpture();
            tex = builTexs.fDolphinSculpture;
            break;
        case eBuildingType::orrery:
            eGameTextures::loadOrrery();
            tex = builTexs.fOrrery;
            break;
        case eBuildingType::spring:
            eGameTextures::loadSpring();
            tex = firstTex(builTexs.fSpring);
            break;
        case eBuildingType::topiary:
            eGameTextures::loadTopiary();
            tex = builTexs.fTopiary;
            break;
        case eBuildingType::baths:
            eGameTextures::loadBaths();
            tex = builTexs.fBaths;
            break;
        case eBuildingType::stoneCircle:
            eGameTextures::loadStoneCircle();
            tex = builTexs.fStoneCircle;
            break;
        case eBuildingType::refinery:
            eGameTextures::loadRefinery();
            tex = builTexs.fRefinery;
            break;
        case eBuildingType::blackMarbleWorkshop:
            tex = builTexs.fBlackMarbleWorkshop;
            break;
        case eBuildingType::commonAgora:
            tex = builTexs.fAgora.getTexture(0);
            break;
        case eBuildingType::granary:
            eGameTextures::loadGranary();
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
        else if (!can)
            tex->setColorMod(255, 0, 0);
        tex->setAlpha(120);
        tp.drawTexture(rx, ry, tex, eAlignment::top);
        tex->clearAlphaMod();
        if (fallback || !can)
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
                double rx;
                double ry;
                drawXY(x, y, rx, ry, 1, 1, tile->altitude());
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
                tp.drawTexture(rx, ry, tex, eAlignment::top);
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
            double rx;
            double ry;
            drawXY(x, y, rx, ry, 1, 1, tile->altitude());
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
            tp.drawTexture(rx, ry, tex, eAlignment::top);
            tex->clearAlphaMod();
            tex->clearColorMod();
        };
        for (const auto &road : cmd.agoraRoads)
            drawCell(tx + road.first, ty + road.second, true);
    };

    mStampTool->paintPreview(tx, ty, mBoard, mEditorMode, mViewedCityId, ppid,
                             doDrawXY, doDrawTex, doDrawAgora,
                             doDrawStampAgora);
}
